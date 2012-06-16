/*
 * external_RAM.c
 *
 *  Created on: 23-03-2012
 *      Author: ja
 */
#include <cr_section_macros.h>
#include "LPC17xx.h"
#include "external_RAM.h"
#include "DMA.h"
#include "vs1053.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "gpio.h"
#include "uart.h"

extern xSemaphoreHandle xSPI1_Mutex;
extern xSemaphoreHandle xDMAch0_Semaphore;
extern xSemaphoreHandle xDMAch2_Semaphore;
/*--- GLOBALS ---*/
volatile uint32_t RAM_bufhead = 0, /* Next to write */
RAM_buftail = 0; /* Next to read */

/**
 * Configure SPI1
 * MSTR
 * CPHA = 0, CPOL = 0
 * CLK = 16,6MHz
 */
void SPI1_Config(void){
	volatile uint32_t dummy;

	dummy = dummy;						/* avoid warning */

    LPC_SC->PCONP |= (1 << 10);			/* Enable power to SSPI1 block */

	/* P0.6 SSEL, P0.7 SCK, P0.8 MISO, P0.9 MOSI are SSP pins. */
	LPC_PINCON->PINSEL0 &= ~( (2UL<<14) | (2UL<<16) | (2UL<<18) );		/* P0.15  cleared */
	LPC_PINCON->PINSEL0 |=  ( (2UL<<14) | (2UL<<16) | (2UL<<18));		/* P0.15 SCK0 */

	/* PCLK_SSP0=CCLK */
	LPC_SC->PCLKSEL0 &= ~(3<<20);
	LPC_SC->PCLKSEL0 |=  (1<<20);					/* PCLKSP0 = CCLK   (100MHz) */

	LPC_SSP1->CR0  = (7<<0);						/* 8Bit, CPOL=0, CPHA=0         */
	LPC_SSP1->CR1  = (1<<1);						/* SSP0 enable, master          */

	/* Set SPI Clock speed */
	SPI1_SetSpeed (SPI_33MHz);						/* PCLK/6 */

	while( LPC_SSP1->SR & (1 << SSPSR_BSY));		/* wait for busy gone */

	/* drain SPI RX FIFO */
	while( LPC_SSP1->SR & (1 << SSPSR_RNE))
		dummy = LPC_SSP1->DR;

}
/*
 * Send one byte via SPI1
 * It first wait for previous transfer complete
 *
 * @param byte_s byte to send
 * @return received byte
 */
uint8_t SPI1_Write(uint8_t byte_s)
{
	uint8_t byte_r;

	while ( LPC_SSP1->SR & (1 << SSPSR_BSY));			/* Wait for transfer to finish */
	LPC_SSP1->DR = byte_s;

	while ( LPC_SSP1->SR & (1 << SSPSR_BSY));			/* Wait for transfer to finish */

	while( !( LPC_SSP1->SR & (1 << SSPSR_RNE )));		/* Wait untill the Rx FIFO is not empty */
	byte_r = LPC_SSP1->DR;

	return byte_r;                                      /* Return received value */
}

/*
 * Write one byte to SPI1 Transmit FIFO
 * It wait if FIFO is full
 *
 * @param byte_s byte to send
 */
void SPI1_FIFO_write(uint8_t byte_s)
{
	while ( !(LPC_SSP1->SR & (1 << SSPSR_TNF)) ); 	    /* Wait if FIFO full */
	LPC_SSP1->DR = byte_s;
}

/*
 * Set SPI1 clk speed
 *
 * @param clk prescaler
 */
void SPI1_SetSpeed(uint8_t speed)
{
	speed &= 0xFE;
	if (speed < 2 ) {
		speed = 2 ;
	}
	LPC_SSP0->CPSR = speed;
}

/*
 * Initialize external RAM memory
 *
 */
void RAM_Init(void) {
	uint8_t sts;

	CS_RAM1_HIGH();			/* Set high CS1 pin */
    CS_RAM1_SET_OUTPUT();	/* Set output CS1 pin */

    CS_RAM2_HIGH();			/* Set high CS2 pin */
    CS_RAM2_SET_OUTPUT();	/* Set output CS2 pin */

    SPI1_Config();

	delay_ms(1);

	/*--- Config external RAM no. 1 ---*/
	CS_RAM1_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM1_HIGH();

	/*--- Config external RAM no. 2 ---*/
	CS_RAM2_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM2_HIGH();
}
/*
 * Use for test RAM memory
 */
void RAM_test(void) {

	uint16_t cnt;
	uint8_t buf[32], buf2[32];

	CS_RAM1_LOW();
	SPI1_Write(WREN);
	CS_RAM1_HIGH();
	CS_RAM1_LOW();
	SPI1_Write(WRITE);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		SPI1_Write(cnt);
	}
	CS_RAM1_HIGH();

	delay_ms(2);

	CS_RAM1_LOW();
	SPI1_Write(READ);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		buf[cnt] = SPI1_Write(DUMMY_BYTE);
	}
	CS_RAM1_HIGH();

	CS_RAM2_LOW();
	SPI1_Write(WREN);
	CS_RAM2_HIGH();
	CS_RAM2_LOW();
	SPI1_Write(WRITE);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		SPI1_Write(cnt+4);
	}
	CS_RAM2_HIGH();

	delay_ms(2);

	CS_RAM2_LOW();
	SPI1_Write(READ);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		buf2[cnt] = SPI1_Write(DUMMY_BYTE);
	}
	CS_RAM2_HIGH();
}

/**
 * Puts samples to RAM buffer
 *
 * @param s data to buffer
 * @patam len length of data
 */
void RAM_bufputs(char *data_in, uint16_t len_in) {
	char *data;
	uint32_t head;
	uint16_t written_data, len;

	data = data_in;

	while(len_in > 0){

		if(len_in > 4095){			/* Max DMA transfer size is 4095 byte */
			len = 4095;				/* If len_in is bigger than 4095 we must do several DMA transfer */
			len_in = len_in - 4095;
		}else{
			len = len_in;
			len_in = 0;
		}

		if (xSemaphoreTake(xSPI1_Mutex, 2000/portTICK_RATE_MS) == pdTRUE) {
			head = RAM_bufhead;

			if(head<RAM_CHIPSIZE){
				if(RAM_CHIPSIZE-head >= len){							/* enough space in first ram? */
					CS_RAM1_LOW();										/* Select first ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM1_HIGH();										/* Deselect ram chip */
					CS_RAM1_LOW();										/* Select first ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0xFF & (head >> 16));				/* Send first byte of address */
					SPI1_FIFO_write(0xFF & (head >> 8));				/* Send second byte of address */
					SPI1_FIFO_write(0xFF & head);						/* Send third byte of address */
					StartSpi1TxDmaTransfer(data, len);					/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 500/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+len;
						xSemaphoreGive(xSPI1_Mutex);
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+len;
						UART_PrintStr("No semph ch0 1\r\n");
						DMA_Config();									/* Reconfigure DMA */
						xSemaphoreGive(xSPI1_Mutex);
					}
				}else{													/* We must write some data to first and second ram chip */
					CS_RAM1_LOW();										/* Select first ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM1_HIGH();										/* Deselect ram chip */
					CS_RAM1_LOW();										/* Select first ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0xFF & (head >> 16));				/* Send first byte of address */
					SPI1_FIFO_write(0xFF & (head >> 8));				/* Send second byte of address */
					SPI1_FIFO_write(0xFF & head);						/* Send third byte of address */
					StartSpi1TxDmaTransfer(data, RAM_CHIPSIZE-head);			/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 500/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();									/* Deselect ram chip */
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();
						UART_PrintStr("No semph ch0 2\r\n");
						DMA_Config();									/* Reconfigure DMA */
					}
					written_data = (RAM_CHIPSIZE-head);
					head = RAM_CHIPSIZE;								/* Set head at start of second RAM */

					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM2_HIGH();										/* Deselect ram chip */
					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0x00);								/* Send first byte of address <----------------- adr = 0*/
					SPI1_FIFO_write(0x00);								/* Send second byte of address */
					SPI1_FIFO_write(0x00);								/* Send third byte of address */
					StartSpi1TxDmaTransfer(data+written_data, len-written_data);	/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 500/portTICK_RATE_MS) == pdTRUE){
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+(len-written_data);
						xSemaphoreGive(xSPI1_Mutex);
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+(len-written_data);
						UART_PrintStr("No semph ch0 3\r\n");
						DMA_Config();									/* Reconfigure DMA */
						xSemaphoreGive(xSPI1_Mutex);
					}
				}
			}else {
				if(2*RAM_CHIPSIZE-head >= len){							/* enough space in second ram? */
					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM2_HIGH();										/* Deselect ram chip */
					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 16));		/* Send first byte of address */
					SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8));		/* Send second byte of address */
					SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));				/* Send third byte of address */
					StartSpi1TxDmaTransfer(data, len);						/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 500/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+len;
						if(RAM_bufhead >= 2*RAM_CHIPSIZE){
							RAM_bufhead = 0;
						}
						xSemaphoreGive(xSPI1_Mutex);
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+len;
						UART_PrintStr("No semph ch0 4\r\n");
						DMA_Config();									/* Reconfigure DMA */
						xSemaphoreGive(xSPI1_Mutex);
					}
				}else{													/* We must write some data to second and first ram chip */
					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM2_HIGH();										/* Deselect ram chip */
					CS_RAM2_LOW();										/* Select second ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 16));		/* Send first byte of address */
					SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8));		/* Send second byte of address */
					SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));				/* Send third byte of address */
					StartSpi1TxDmaTransfer(data, 2*RAM_CHIPSIZE-head);			/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM2_HIGH();									/* Deselect ram chip */
						UART_PrintStr("No semph ch0 5\r\n");
						DMA_Config();									/* Reconfigure DMA */
					}
					written_data = (2*RAM_CHIPSIZE-head);
					head = 0;										/* Set head at start of first RAM */

					CS_RAM1_LOW();										/* Select first ram chip */
					SPI1_Write(WREN);									/* Enable writing to memory */
					CS_RAM1_HIGH();										/* Deselect ram chip */
					CS_RAM1_LOW();										/* Select third ram chip */
					SPI1_FIFO_write(WRITE);								/* Send write command */
					SPI1_FIFO_write(0x00);								/* Send first byte of address */
					SPI1_FIFO_write(0x00);								/* Send second byte of address */
					SPI1_FIFO_write(0x00);								/* Send third byte of address */
					StartSpi1TxDmaTransfer(data+written_data, len-written_data);	/* Start DMA transfer */
					if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+(len-written_data);
						xSemaphoreGive(xSPI1_Mutex);
					}else{
						while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
						CS_RAM1_HIGH();									/* Deselect ram chip */
						RAM_bufhead = head+(len-written_data);
						UART_PrintStr("No semph ch0 6\r\n");
						DMA_Config();									/* Reconfigure DMA */
						xSemaphoreGive(xSPI1_Mutex);
					}
				}
			}
		}else{
			UART_PrintStr("bufput mutex timeout\r\n");
		}

		data = data_in + 4095;
	}
}

/**
 * Gets samples to audio buffer
 *
 * @param buf buffer
 * @patam len length of data to get (max value = 32*1024)
 */
void RAM_bufget(uint8_t *buf, uint16_t len){
	uint32_t tail;
	uint8_t dummy = 0xa5;
	uint16_t read_data;

	if(xSemaphoreTake(xSPI1_Mutex, 2000/portTICK_RATE_MS) == pdTRUE){
		tail = RAM_buftail;

		if(tail < RAM_CHIPSIZE){									/* End of circular buffer is in first RAM */
			if(RAM_CHIPSIZE-tail >= len){
				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF&(tail>>16));						/* Send first byte of address */
				SPI1_Write(0xFF & (tail >> 8));						/* Send second byte of address */
				SPI1_Write(0xFF & tail);							/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len+1);					/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len+1);			/* initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){		/*Wait for DMA transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}else{													/* We must read some data from first and second ram chip */
				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 16));					/* Send first byte of address */
				SPI1_Write(0xFF & (tail >> 8));						/* Send second byte of address */
				SPI1_Write(0xFF & tail);							/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();
				}

				read_data = RAM_CHIPSIZE - tail;

				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);									/* Send first byte of address */
				SPI1_Write(0x00);									/* Send second byte of address */
				SPI1_Write(0x00);									/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf <--------------------------- jest bug*/
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect second ram chip */
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}
		}else{														/* End of circular buffer is in second RAM */
			if(2*RAM_CHIPSIZE-tail >= len){
				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 16));	/* Send first byte of address */
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));	/* Send second byte of address */
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));			/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len+1);					/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len+1);			/* initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){		/*Wait for DMA transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					if(RAM_buftail >= 2*RAM_CHIPSIZE)
						RAM_buftail = 0;
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}else{													/* We must read some data from first and second ram chip */
				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 16));	/* Send first byte of address */
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));	/* Send second byte of address */
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));			/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, 2*RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, 2*RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();
				}

				read_data = 2*RAM_CHIPSIZE - tail;

				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);									/* Send first byte of address */
				SPI1_Write(0x00);									/* Send second byte of address */
				SPI1_Write(0x00);									/* Send third byte of address */
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);		/* Initialize DMA transfer SSP1 Rx FIFO -> buf <------------------------------- jest bug*/
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);		/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();									/* Deselect second ram chip */
					RAM_buftail = len-read_data;					/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}
		}
	}else{
		UART_PrintStr("bufget mutex timeout\r\n");
	}
}

/**
 * Check number of samples in audio buffer
 *
 * @return number of samples in audio buffer
 */
uint32_t RAM_buflen(void) {
	uint32_t head, tail;

	head = RAM_bufhead;
	tail = RAM_buftail;

	if (head > tail) {
		return (head - tail);
	} else if (head < tail) {
		return (RAM_BUFSIZE - (tail - head));
	}

	return 0;
}

/**
 * Check free space in RAM buffer
 *
 * @return free bytes in RAM buffer
 */
uint32_t RAM_buffree(void) {

	return (RAM_BUFSIZE - RAM_buflen() - 1);

}
/**
 * Reset audio buffer
 */
void resetbuff(void){
	RAM_buftail = 0;
	RAM_bufhead = 0;
}
