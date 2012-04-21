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
 * Initialize external RAM
 */

void SPI1_Config(void){
	volatile uint32_t dummy;

	dummy = dummy;                                   /* avoid warning */

    LPC_SC->PCONP |= (1 << 10);                      /* Enable power to SSPI0 block */


    CS_RAM1_HIGH();
    CS_RAM1_SET_OUTPUT();

    CS_RAM2_HIGH();
    CS_RAM2_SET_OUTPUT();

    CS_RAM3_HIGH();
    CS_RAM3_SET_OUTPUT();

    CS_RAM4_HIGH();
    CS_RAM4_SET_OUTPUT();

	/* P0.6 SSEL, P0.7 SCK, P0.8 MISO, P0.9 MOSI are SSP pins. */
	LPC_PINCON->PINSEL0 &= ~( (2UL<<14) | (2UL<<16) | (2UL<<18) );		/* P0.15  cleared */

	LPC_PINCON->PINSEL0 |=  ( (2UL<<14) | (2UL<<16) | (2UL<<18));		/* P0.15 SCK0 */

	/* PCLK_SSP0=CCLK */
	LPC_SC->PCLKSEL0 &= ~(3<<20);                    /* PCLKSP0 = CCLK/4 (25MHz) */
	LPC_SC->PCLKSEL0 |=  (1<<20);                    /* PCLKSP0 = CCLK   (100MHz) */

	LPC_SSP1->CR0  = ( 7<<0 );                       /* 8Bit, CPOL=0, CPHA=0         */
	LPC_SSP1->CR1  = ( 1<<1 );                       /* SSP0 enable, master          */

	SPI1_SetSpeed (6);						/* PCLK/50 */

	/* wait for busy gone */
	while( LPC_SSP1->SR & ( 1 << SSPSR_BSY ) );

	/* drain SPI RX FIFO */
	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )
	{
		dummy = LPC_SSP1->DR;
	}
}

uint8_t SPI1_Write(uint8_t byte_s)
{
	uint8_t byte_r;

	while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
	LPC_SSP1->DR = byte_s;

	while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */

	while( !( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) ) );	/* Wait untill the Rx FIFO is not empty */
	byte_r = LPC_SSP1->DR;

	return byte_r;                                      /* Return received value */
}

void SPI1_FIFO_write(uint8_t byte_s)
{
	while ( !(LPC_SSP1->SR & (1 << SSPSR_TNF)) ); 	    /* Wait if FIFO full */
	LPC_SSP1->DR = byte_s;
}

void SPI1_SetSpeed(uint8_t speed)
{
	speed &= 0xFE;
	if ( speed < 2  ) {
		speed = 2 ;
	}
	LPC_SSP0->CPSR = speed;
}

void RAM_init(void) {
	uint8_t sts;

	SPI1_Config();

	delay_ms(2);

	/*--- Config external RAM no. 1 ---*/
	CS_RAM1_LOW();
	SPI1_Write(WRSR); 				/* Write STATUS register  */
	SPI1_Write(SEQ_MODE | HOLD_OFF); /* Set sequential mode and disable HOLD pin */
	CS_RAM1_HIGH();

	delay_ms(2);

	CS_RAM1_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM1_HIGH();

	/*--- Config external RAM no. 2 ---*/
	CS_RAM2_LOW();
	SPI1_Write(WRSR); 				/* Write STATUS register  */
	SPI1_Write(SEQ_MODE | HOLD_OFF); /* Set sequential mode and disable HOLD pin */
	CS_RAM2_HIGH();

	delay_ms(2);

	CS_RAM2_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM2_HIGH();

	/*--- Config external RAM no. 3 ---*/
	CS_RAM3_LOW();
	SPI1_Write(WRSR); 				/* Write STATUS register  */
	SPI1_Write(SEQ_MODE | HOLD_OFF); /* Set sequential mode and disable HOLD pin */
	CS_RAM3_HIGH();

	delay_ms(2);

	CS_RAM3_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM3_HIGH();

	/*--- Config external RAM no. 4 ---*/
	CS_RAM4_LOW();
	SPI1_Write(WRSR); 				/* Write STATUS register  */
	SPI1_Write(SEQ_MODE | HOLD_OFF); /* Set sequential mode and disable HOLD pin */
	CS_RAM4_HIGH();

	delay_ms(2);

	CS_RAM4_LOW();
	SPI1_Write(RDSR); 				/* Read STATUS register  */
	sts = SPI1_Write(DUMMY_BYTE);
	CS_RAM4_HIGH();
}

void RAM_test(void) {

	uint16_t cnt;
	uint8_t buf[32], a=4, buf2[32];

	CS_RAM1_LOW();
	SPI1_Write(WRITE);
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
	for (cnt = 0; cnt < 32; cnt++) {
		buf[cnt] = SPI1_Write(DUMMY_BYTE);
	}
	CS_RAM1_HIGH();

	CS_RAM2_LOW();
	SPI1_Write(WRITE);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		SPI1_Write(cnt);
	}
	CS_RAM2_HIGH();

	delay_ms(2);

	CS_RAM2_LOW();
	SPI1_Write(READ);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 32; cnt++) {
		buf[cnt] = SPI1_Write(DUMMY_BYTE);
	}
	CS_RAM2_HIGH();

	/* zerowanie bufora */
	for (cnt = 0; cnt < 32; cnt++) {
		buf[cnt] = 0;
	}
//	delay_ms(2);
	/* transfer DMA - odczyt z SPI */
while(1){
	CS_RAM2_LOW();
	SPI1_Write(READ);
	SPI1_Write(0x00);
	SPI1_Write(0x00);

	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) ){
		a = LPC_SSP1->DR;
	}
	StartSpi1RxDmaTransfer(&buf[0], 21);
	StartSpi1TxDmaDummyTransfer(&a, 21);
//	for (cnt = 0; cnt < 20; cnt++) {
//		SPI1_FIFO_write(DUMMY_BYTE);
//		delay_ms(100);
	//	LED_Toggle(0);
	//}
	while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
	CS_RAM2_HIGH();
	delay_ms(1000);
	cnt=0;
	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) ){
		buf2[cnt] = LPC_SSP1->DR;
		cnt++;
	}
}
	while(1);

//	CS_RAM2_LOW();
//	DMA_Config();
////	while(!(LPC_GPDMA->DMACIntStat & 0x01)); //czekaj na koniec transferu DMA
//	while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
//	CS_RAM2_HIGH();
//	LPC_GPDMACH0->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */
//	LPC_GPDMA->DMACIntErrClr |= 0xFF;
//	LPC_GPDMA->DMACIntTCClear |= 0xFF;
//	NVIC_SetPriority(DMA_IRQn, 5);
//	NVIC_EnableIRQ(DMA_IRQn);
//	//wyczyć bufor RX
//
//	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )
//	{
//		buf[0] = LPC_SSP1->DR;
//	}
//
//	CS_RAM2_LOW();
//	SPI1_Write(READ);
//	SPI1_Write(0x00);
//	SPI1_Write(0x00);
//	for (cnt = 0; cnt < 13; cnt++) {
//		buf[cnt] = SPI1_Write(DUMMY_BYTE);
//	}
//	CS_RAM2_HIGH();
}

//void DMA_Config(void){
//	uint8_t dane[] = {0x02, 0x00, 0x00, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
//	uint8_t TransferSize = 16;
//	LPC_SC->PCONP |= (1<<29); 					/* Power on DMA module */
//	LPC_SSP1->DMACR |=(1<<1);					/* Enable DMA for the SSP1 transmit FIFO */
//	LPC_GPDMA->DMACConfig |= (1<<0);			/* Enable DMA Controller */
//
//	LPC_GPDMA->DMACIntErrClr |= 0xFF;
//	LPC_GPDMA->DMACIntTCClear |= 0xFF;
//
//	LPC_GPDMACH0->DMACCLLI = 0;								/* Clear Linked List - don't use */
//	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)&dane[0];			/* Set source address */
//	LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR;	/* Set destination address */
//	LPC_GPDMACH0->DMACCControl = TransferSize 			/* Set transfer size */
//								| (1<<12)				/* Source burst size - 4 */
//								| (1<<15)				/* Destination burst size - 4 */
//								| (0<<18)				/* Source transfer width - Byte (8-bit) */
//								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
//								| (1<<26)				/* Source address is incremented */
//								| (0<<31);				/* Enable Interrupt */
//
//	LPC_GPDMACH0->DMACCConfig |= (2<<6)		/* SSP1 Tx as DMA request peripheral */
//							|(1<<11)		/* Memory to peripheral transfer */
//							|(1<<14)		/* Enable error interrupt */
//							|(1<<15);		/* Enable interrupt */
//
//	LPC_GPDMACH0->DMACCConfig |= 1;	/* Enable DMA channel */
////	NVIC_SetPriority(DMA_IRQn, 5);
////	NVIC_EnableIRQ(DMA_IRQn);
//
//	LPC_GPDMA->DMACSoftSReq |= (1<<2);		/* Transfer request */
//
//}


/**
 * Puts samples to RAM buffer
 *
 * @param s data to buffer
 * @patam len length of data
 */
void RAM_bufputs(char *data, uint16_t len) {
	uint32_t head;
	uint16_t written_data;
	if(len==0){
		return;
	}
	if (xSemaphoreTake(xSPI1_Mutex, 2000/portTICK_RATE_MS) == pdTRUE) {
		head = RAM_bufhead;

		if(head<RAM_CHIPSIZE){
			if(RAM_CHIPSIZE-head >= len){							/* enough space in first ram? */
				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & (head >> 8));				/* Send high address */
				SPI1_FIFO_write(0xFF & head);						/* Send low address */
				StartSpi1TxDmaTransfer(data, len);						/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					UART_PrintStr("No semph ch0 1\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}else{
				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & (head >> 8));				/* Send high address */
				SPI1_FIFO_write(0xFF & head);						/* Send low address */
				StartSpi1TxDmaTransfer(data, RAM_CHIPSIZE-head);			/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();									/* Deselect ram chip */
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();
					UART_PrintStr("No semph ch0 2\r\n");
				}
				written_data = (RAM_CHIPSIZE-head);
				head = RAM_CHIPSIZE;								/* Set head at start of second RAM */

				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));				/* Send low address */
				StartSpi1TxDmaTransfer(data+written_data, len-written_data);	/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					UART_PrintStr("No semph ch0 3\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}
		}else if ((RAM_CHIPSIZE <= head) && (head < 2 * RAM_CHIPSIZE)) {
			if(2*RAM_CHIPSIZE-head >= len){							/* enough space in second ram? */
				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));				/* Send low address */
				StartSpi1TxDmaTransfer(data, len);						/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					UART_PrintStr("No semph ch0 4\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}else{
				CS_RAM2_LOW();										/* Select second ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));				/* Send low address */
				StartSpi1TxDmaTransfer(data, 2*RAM_CHIPSIZE-head);			/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect ram chip */
					UART_PrintStr("No semph ch0 5\r\n");
				}
				written_data = (2*RAM_CHIPSIZE-head);
				head = 2*RAM_CHIPSIZE;								/* Set head at start of third RAM */

				CS_RAM3_LOW();										/* Select third ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 2*RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - 2*RAM_CHIPSIZE));			/* Send low address */
				StartSpi1TxDmaTransfer(data+written_data, len-written_data);	/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					UART_PrintStr("No semph ch0 6\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}
		}else if ((2 * RAM_CHIPSIZE <= head) && (head < 3 * RAM_CHIPSIZE)){
			if(3*RAM_CHIPSIZE-head >= len){
				CS_RAM3_LOW();										/* Select third ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 2*RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - 2*RAM_CHIPSIZE));			/* Send low address */
				StartSpi1TxDmaTransfer(data, len);						/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					UART_PrintStr("No semph ch0 7\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}else{
				CS_RAM3_LOW();										/* Select third ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 2*RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - 2*RAM_CHIPSIZE));			/* Send low address */
				StartSpi1TxDmaTransfer(data, 3*RAM_CHIPSIZE-head);			/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect ram chip */
					UART_PrintStr("No semph ch0 8\r\n");
				}
				written_data = (3*RAM_CHIPSIZE-head);
				head = 3*RAM_CHIPSIZE;								/* Set head at start of third RAM */

				CS_RAM4_LOW();										/* Select fourth ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 3*RAM_CHIPSIZE) >> 8));		/* Send high address - można dać 0 */
				SPI1_FIFO_write(0xFF & (head - 3*RAM_CHIPSIZE));			/* Send low address - można dać 0 */
				StartSpi1TxDmaTransfer(data+written_data, len-written_data);	/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+(len-written_data);
					UART_PrintStr("No semph ch0 9\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}
		}else if (3 * RAM_CHIPSIZE <= head){
			if(4*RAM_CHIPSIZE-head >= len){
				CS_RAM4_LOW();										/* Select fourth ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 3*RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - 3*RAM_CHIPSIZE));			/* Send low address */
				StartSpi1TxDmaTransfer(data, len);						/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					if(RAM_bufhead >= 4*RAM_CHIPSIZE)
						RAM_bufhead = 0;
					xSemaphoreGive(xSPI1_Mutex);
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
					RAM_bufhead = head+len;
					if(RAM_bufhead >= 4*RAM_CHIPSIZE)
						RAM_bufhead = 0;
					UART_PrintStr("No semph ch0 10\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}else{
				CS_RAM4_LOW();										/* Select fourth ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & ((head - 3*RAM_CHIPSIZE) >> 8));		/* Send high address */
				SPI1_FIFO_write(0xFF & (head - 3*RAM_CHIPSIZE));			/* Send low address */
				StartSpi1TxDmaTransfer(data, 4*RAM_CHIPSIZE-head);			/* Start DMA transfer */
				if(xSemaphoreTake(xDMAch0_Semaphore, 1000/portTICK_RATE_MS) == pdTRUE){	/* Wait for transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
				}else{
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect ram chip */
					UART_PrintStr("No semph ch0 11\r\n");
				}
				written_data = (4*RAM_CHIPSIZE-head);
				head = 0;								/* Set head at start of third RAM */

				CS_RAM1_LOW();										/* Select first ram chip */
				SPI1_FIFO_write(WRITE);								/* Send write command */
				SPI1_FIFO_write(0xFF & (head >> 8));				/* Send high address - można dać 0 */
				SPI1_FIFO_write(0xFF & head);						/* Send low address - można dać 0 */
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
					UART_PrintStr("No semph ch0 12\r\n");
					xSemaphoreGive(xSPI1_Mutex);
				}
			}
		}
	}else{
		UART_PrintStr("bufput mutex timeout\r\n");
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

		if(tail < RAM_CHIPSIZE){		/* End of circular buffer is in first RAM */
			if(RAM_CHIPSIZE-tail >= len){
				CS_RAM1_LOW();				/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
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
			}else{
				CS_RAM1_LOW();				/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();
				}

				read_data = RAM_CHIPSIZE - tail;

				CS_RAM2_LOW();					/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);
				SPI1_Write(0x00);
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);		/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();									/* Deselect second ram chip */
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}
		}else if((RAM_CHIPSIZE <= tail ) && (tail < 2*RAM_CHIPSIZE) ){		/* End of circular buffer is in first RAM */
			if(2*RAM_CHIPSIZE-tail >= len){
				CS_RAM2_LOW();				/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len+1);					/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len+1);			/* initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){		/*Wait for DMA transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}else{
				CS_RAM2_LOW();				/* Select second ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, 2*RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, 2*RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM2_HIGH();
				}

				read_data = 2*RAM_CHIPSIZE - tail;

				CS_RAM3_LOW();					/* Select third ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);
				SPI1_Write(0x00);
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);		/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();									/* Deselect second ram chip */
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}
		}else if((2*RAM_CHIPSIZE <= tail ) && (tail < 3*RAM_CHIPSIZE) ){		/* End of circular buffer is in first RAM */
			if(3*RAM_CHIPSIZE-tail >= len){
				CS_RAM3_LOW();				/* Select third ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2*RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2*RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len+1);					/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len+1);			/* initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){		/*Wait for DMA transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}else{
				CS_RAM3_LOW();				/* Select third ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2*RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2*RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, 3*RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, 3*RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();
				}

				read_data = 3*RAM_CHIPSIZE - tail;

				CS_RAM4_LOW();					/* Select fourth ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);
				SPI1_Write(0x00);
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);		/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();									/* Deselect second ram chip */
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}
		}else if(3*RAM_CHIPSIZE <= tail){		/* End of circular buffer is in first RAM */
			if(4*RAM_CHIPSIZE-tail >= len){
				CS_RAM4_LOW();				/* Select fourth ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3*RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3*RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len+1);					/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len+1);			/* initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){		/*Wait for DMA transfer complete */
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM4_HIGH();
					RAM_buftail = tail + len;						/* Update tail of circular buffer */
					if(RAM_buftail >= 4*RAM_CHIPSIZE){
						RAM_buftail = 0;
					}
					xSemaphoreGive(xSPI1_Mutex);					/* Release SSP1 resource */
				}
			}else{
				CS_RAM3_LOW();				/* Select fourth ram chip */
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3*RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3*RAM_CHIPSIZE));
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, 4*RAM_CHIPSIZE-tail+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, 4*RAM_CHIPSIZE-tail+1);	/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM3_HIGH();
				}

				read_data = 4*RAM_CHIPSIZE - tail;

				CS_RAM1_LOW();					/* Select first ram chip */
				SPI1_Write(READ);
				SPI1_Write(0x00);
				SPI1_Write(0x00);
				while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Make sure that SSP1 Rx FIFO is empty */
					dummy = LPC_SSP1->DR;
				StartSpi1RxDmaTransfer(buf, len-read_data+1);			/* Initialize DMA transfer SSP1 Rx FIFO -> buf */
				StartSpi1TxDmaDummyTransfer(&dummy, len-read_data+1);		/* Initialize DMA transfer dummy byte -> SSP Tx FIFO */
				if(xSemaphoreTake(xDMAch2_Semaphore, portMAX_DELAY) == pdTRUE){
					while ( LPC_SSP1->SR & (1 << SSPSR_BSY) );		/* Wait for transfer to finish */
					CS_RAM1_HIGH();									/* Deselect first ram chip */
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
