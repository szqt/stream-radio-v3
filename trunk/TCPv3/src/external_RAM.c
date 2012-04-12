/*
 * external_RAM.c
 *
 *  Created on: 23-03-2012
 *      Author: ja
 */
#include <cr_section_macros.h>
#include "LPC17xx.h"
#include "external_RAM.h"
#include "vs1053.h"
#include "vs1053.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "semphr.h"

//extern OS_MutexID SPI_Mutex;
extern xSemaphoreHandle xSPI1_Mutex;

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
	while ( !(LPC_SSP1->SR & (1 << SSPSR_TNF)) ); 	    /* Wait for FIFO not full */
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
	uint8_t buf[32];

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

	CS_RAM2_LOW();
	DMA_Config();
	while(!(LPC_GPDMA->DMACIntStat & 0x01)); //czekaj na koniec transferu DMA
	while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
	CS_RAM2_HIGH();

	//wyczyć bufor RX

	while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )
	{
		buf[0] = LPC_SSP1->DR;
	}

	CS_RAM2_LOW();
	SPI1_Write(READ);
	SPI1_Write(0x00);
	SPI1_Write(0x00);
	for (cnt = 0; cnt < 13; cnt++) {
		buf[cnt] = SPI1_Write(DUMMY_BYTE);
	}
	CS_RAM2_HIGH();
}

void DMA_Config(void){
	uint8_t dane[] = {0x02, 0x00, 0x00, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
	uint8_t TransferSize = 16;
	LPC_SC->PCONP |= (1<<29); 					/* Power on DMA module */
	LPC_SSP1->DMACR |=(1<<1);					/* Enable DMA for the SSP1 transmit FIFO */
	LPC_GPDMA->DMACConfig |= (1<<0);			/* Enable DMA Controller */

	LPC_GPDMA->DMACIntErrClr |= 0xFF;
	LPC_GPDMA->DMACIntTCClear |= 0xFF;

	LPC_GPDMACH0->DMACCLLI = 0;								/* Clear Linked List - don't use */
	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)&dane[0];			/* Set source address */
	LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR;	/* Set destination address */
	LPC_GPDMACH0->DMACCControl = TransferSize 			/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<26)				/* Source address is incremented */
								| (1<<31);				/* Enable Interrupt */

	LPC_GPDMACH0->DMACCConfig |= (2<<6)		/* SSP1 Tx as DMA request peripheral */
							|(1<<11)		/* Memory to peripheral transfer */
							|(1<<14)		/* Enable error interrupt */
							|(1<<15);		/* Enable interrupt */

	LPC_GPDMACH0->DMACCConfig |= 1;	/* Enable DMA channel */
//	NVIC_SetPriority(DMA_IRQn, 5);
//	NVIC_EnableIRQ(DMA_IRQn);

	LPC_GPDMA->DMACSoftSReq |= (1<<2);		/* Transfer request */

}
void
void DMA_IRQHandler(void){
	uint32_t status;
	LPC_GPDMACH0->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */

	status = LPC_GPDMA->DMACIntTCStat;
	if((status & 0xFF) > 0){
		LPC_GPDMA->DMACIntTCClear = status;
		//terminal count interrupt request
	}
	status = LPC_GPDMA->DMACRawIntErrStat;
	if ((status & 0xFF)>0){
		LPC_GPDMA->DMACIntErrClr = status;
		//error interrupt request
	}

}
void RAM_bufputs(char *s, uint16_t len) {
	uint32_t head;
	if (xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY) == pdTRUE) {
		head = RAM_bufhead;

		if(head<RAM_CHIPSIZE){
			if(RAM_CHIPSIZE-head >= len){
				//transfer
			}else{
				//transfer do dwóch pamieci
			}
		}else if ((RAM_CHIPSIZE <= head) && (head < 2 * RAM_CHIPSIZE)) {
			if(2*RAM_CHIPSIZE-head >= len){
				//transfer
			}else{

			}
		}else if ((2 * RAM_CHIPSIZE <= head) && (head < 3 * RAM_CHIPSIZE)){
			if(3*RAM_CHIPSIZE-head >= len){
				//transfer
			}else{

			}
		}else if (3 * RAM_CHIPSIZE <= head){
			if(4*RAM_CHIPSIZE-head >= len){
				//transfer
			}else{

			}
		}
	}
}
/**
 * Puts samples to RAM buffer
 *
 * @param s data to buffer
 * @patam len length of data
 */
//__SECTION(data,RAM2)
void RAM_bufputs(char *s, uint16_t len) {
	uint32_t head;

//	CoEnterMutexSection(SPI_Mutex);
	if (xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY) == pdTRUE) {
		while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
		head = RAM_bufhead;

		if (head < RAM_CHIPSIZE) { // zapis do pierwszej pamięci
			CS_RAM1_LOW();
			SPI1_FIFO_write(WRITE);
			SPI1_FIFO_write(0xFF & (head >> 8));
			SPI1_FIFO_write(0xFF & head);

			while ((len != 0) && (head < RAM_CHIPSIZE)) {
				SPI1_FIFO_write(*s++);
				head++;
				len--;
			}
			while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
			CS_RAM1_HIGH();

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do drugiej pamięci
				CS_RAM2_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));

				while ((len != 0) && (head < (2 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM2_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 3 pamięci
				CS_RAM3_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 2 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 2 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (3 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM3_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 4 pamięci
				CS_RAM4_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 3 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 3 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (4 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM4_HIGH();

				if (head >= RAM_BUFSIZE) { //jezeli powodem wyjcia w while było niespełnienie
					head = 0; //warunku head < 2*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
				}
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane się nie zmieciły to zapisz początek pierwszej pamieci
				CS_RAM1_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & (head >> 8));
				SPI1_FIFO_write(0xFF & head);

				while ((len != 0) && (head < RAM_CHIPSIZE)) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM1_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

		} else if ((RAM_CHIPSIZE <= head) && (head < 2 * RAM_CHIPSIZE)) { // jestemy w obszarze 2 pamięci

			CS_RAM2_LOW(); // zapis do 2 pamięci
			SPI1_FIFO_write(WRITE);
			SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8)); //
			SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));

			while ((len != 0) && (head < (2 * RAM_CHIPSIZE))) {
				SPI1_FIFO_write(*s++);
				head++;
				len--;
			}
			while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
			CS_RAM2_HIGH();

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 3 pamięci
				CS_RAM3_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 2 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 2 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (3 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM3_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 4 pamięci
				CS_RAM4_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 3 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 3 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (4 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM4_HIGH();

				if (head >= RAM_BUFSIZE) { //jezeli powodem wyjcia w while było niespełnienie
					head = 0; //warunku head < 2*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
				}
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane się nie zmieciły to zapisz początek 1 pamieci
				CS_RAM1_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & (head >> 8));
				SPI1_FIFO_write(0xFF & head);

				while ((len != 0) && (head < RAM_CHIPSIZE)) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM1_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 2 pamięci
				CS_RAM2_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));

				while ((len != 0) && (head < (2 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM2_HIGH();
				/* tutaj program nie powinien dojsc */
				if (head >= RAM_BUFSIZE) //jezeli powodem wyjcia w while było niespełnienie
					head = 0; //warunku head < 2*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		} else if ((2 * RAM_CHIPSIZE <= head) && (head < 3 * RAM_CHIPSIZE)) {

			CS_RAM3_LOW(); // zapis do 3 pamięci
			SPI1_FIFO_write(WRITE);
			SPI1_FIFO_write(0xFF & ((head - 2 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
			SPI1_FIFO_write(0xFF & (head - 2 * RAM_CHIPSIZE));

			while ((len != 0) && (head < (3 * RAM_CHIPSIZE))) {
				SPI1_FIFO_write(*s++);
				head++;
				len--;
			}
			while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
			CS_RAM3_HIGH();

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 4 pamięci
				CS_RAM4_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 3 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 3 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (4 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM4_HIGH();

				if (head >= RAM_BUFSIZE) { //jezeli powodem wyjcia w while było niespełnienie
					head = 0; //warunku head < 2*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
				}
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane się nie zmieciły to zapisz początek 1 pamieci
				CS_RAM1_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & (head >> 8));
				SPI1_FIFO_write(0xFF & head);

				while ((len != 0) && (head < RAM_CHIPSIZE)) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM1_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 2 pamięci
				CS_RAM2_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));

				while ((len != 0) && (head < (2 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM2_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 3 pamięci

				CS_RAM3_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 2 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 2 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (3 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM3_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

		} else if (3 * RAM_CHIPSIZE <= head) {

			CS_RAM4_LOW();
			SPI1_FIFO_write(WRITE);
			SPI1_FIFO_write(0xFF & ((head - 3 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
			SPI1_FIFO_write(0xFF & (head - 3 * RAM_CHIPSIZE));

			while ((len != 0) && (head < (4 * RAM_CHIPSIZE))) {
				SPI1_FIFO_write(*s++);
				head++;
				len--;
			}
			while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
			CS_RAM4_HIGH();

			if (head >= RAM_BUFSIZE) { //jezeli powodem wyjcia w while było niespełnienie
				head = 0; //warunku head < 4*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
			}

			if (len > 0) { // jeżeli dane się nie zmieciły to zapisz początek 1 pamieci
				CS_RAM1_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & (head >> 8));
				SPI1_FIFO_write(0xFF & head);

				while ((len != 0) && (head < RAM_CHIPSIZE)) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM1_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 2 pamięci

				CS_RAM2_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - RAM_CHIPSIZE));

				while ((len != 0) && (head < (2 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM2_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 3 pamięci

				CS_RAM3_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 2 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 2 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (3 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM3_HIGH();
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { // jeżeli dane sie nie zmieciły to zapisz do 4 pamięci

				CS_RAM4_LOW();
				SPI1_FIFO_write(WRITE);
				SPI1_FIFO_write(0xFF & ((head - 3 * RAM_CHIPSIZE) >> 8)); // adres powinien wtuaj wynosic 0
				SPI1_FIFO_write(0xFF & (head - 3 * RAM_CHIPSIZE));

				while ((len != 0) && (head < (4 * RAM_CHIPSIZE))) {
					SPI1_FIFO_write(*s++);
					head++;
					len--;
				}
				while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
				CS_RAM4_HIGH();

				if (head >= RAM_BUFSIZE) { //jezeli powodem wyjcia w while było niespełnienie
					head = 0; //warunku head < 2*RAM_CHIPSIZE to trezba przeskoczyć na początek bufora
				}
			} else {
				RAM_bufhead = head;
				xSemaphoreGive(xSPI1_Mutex);
//				CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		}

		RAM_bufhead = head;
		xSemaphoreGive(xSPI1_Mutex);
//		CoLeaveMutexSection(SPI_Mutex);
	}
	return;
}

/**
 * Gets samples to audio buffer
 *
 * @param buf buffer
 * @patam len length of data to get
 */
void RAM_bufget(uint8_t *buf, uint16_t len) {
	uint32_t tail;
	uint8_t dummy;

	if (xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY) == pdTRUE) {
		while ( LPC_SSP1->SR & (1 << SSPSR_BSY) ); 	        /* Wait for transfer to finish */
		while( LPC_SSP1->SR & ( 1 << SSPSR_RNE ) )			/* Celan Rx FIFO */
		{
			dummy = LPC_SSP1->DR;
		}

		tail = RAM_buftail;
		if (tail < RAM_CHIPSIZE) { // odczyt 1 pamięci
			CS_RAM1_LOW();
			SPI1_Write(READ);
			SPI1_Write(0xFF & (tail >> 8));
			SPI1_Write(0xFF & tail);
			while ((len != 0) && (tail < RAM_CHIPSIZE)) {
				*buf++ = SPI1_Write(DUMMY_BYTE);
				tail++;
				len--;
			}
			CS_RAM1_HIGH();

			if (len > 0) { //odczyt 2 pamięci
				CS_RAM2_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while ((len != 0) && (tail < (2 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM2_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 3 pamięci
				CS_RAM3_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (3 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM3_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
			if (len > 0) { //odczyt 4 pamięci
				CS_RAM4_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (4 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM4_HIGH();

				if (tail >= RAM_BUFSIZE) {
					tail = 0;
				}
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt do 1 pamięci
				CS_RAM1_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
				while ((len != 0) && (tail < RAM_CHIPSIZE)) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM1_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		} else if ((RAM_CHIPSIZE <= tail) && (tail < 2 * RAM_CHIPSIZE)) { // odczyt 2 pamieci

			CS_RAM2_LOW();
			SPI1_Write(READ);
			SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
			SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
			while ((len != 0) && (tail < (2 * RAM_CHIPSIZE))) {
				*buf++ = SPI1_Write(DUMMY_BYTE);
				tail++;
				len--;
			}
			CS_RAM2_HIGH();

			if (len > 0) { //odczyt 3 pamięci
				CS_RAM3_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (3 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM3_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
			if (len > 0) { //odczyt 4 pamięci
				CS_RAM4_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (4 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM4_HIGH();

				if (tail >= RAM_BUFSIZE) {
					tail = 0;
				}
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
			if (len > 0) { //odczyt 1 pamięci
				CS_RAM1_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
				while ((len != 0) && (tail < RAM_CHIPSIZE)) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM1_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 2 pamięci
				CS_RAM2_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while ((len != 0) && (tail < (2 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM2_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		} else if ((2 * RAM_CHIPSIZE <= tail) && (tail < 3 * RAM_CHIPSIZE)) { //odczyt 3 pamieci
			CS_RAM3_LOW();
			SPI1_Write(READ);
			SPI1_Write(0xFF & ((tail - 2 * RAM_CHIPSIZE) >> 8));
			SPI1_Write(0xFF & (tail - 2 * RAM_CHIPSIZE));
			while ((len != 0) && (tail < (3 * RAM_CHIPSIZE))) {
				*buf++ = SPI1_Write(DUMMY_BYTE);
				tail++;
				len--;
			}
			CS_RAM3_HIGH();

			if (len > 0) { //odczyt 4 pamięci
				CS_RAM4_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (4 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM4_HIGH();

				if (tail >= RAM_BUFSIZE) {
					tail = 0;
				}
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 1 pamięci
				CS_RAM1_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
				while ((len != 0) && (tail < RAM_CHIPSIZE)) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM1_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 2 pamięci
				CS_RAM2_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while ((len != 0) && (tail < (2 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM2_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 3 pamięci
				CS_RAM3_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (3 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM3_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		} else if (3 * RAM_CHIPSIZE <= tail) { // odczyt z 4 pamięci

			CS_RAM4_LOW();
			SPI1_Write(READ);
			SPI1_Write(0xFF & ((tail - 3 * RAM_CHIPSIZE) >> 8));
			SPI1_Write(0xFF & (tail - 3 * RAM_CHIPSIZE));
			while ((len != 0) && (tail < (4 * RAM_CHIPSIZE))) {
				*buf++ = SPI1_Write(DUMMY_BYTE);
				tail++;
				len--;
			}
			CS_RAM4_HIGH();

			if (tail >= RAM_BUFSIZE) {
				tail = 0;
			}
			if (len > 0) { //odczyt 1 pamięci
				CS_RAM1_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & (tail >> 8));
				SPI1_Write(0xFF & tail);
				while ((len != 0) && (tail < RAM_CHIPSIZE)) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM1_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 2 pamięci
				CS_RAM2_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - RAM_CHIPSIZE));
				while ((len != 0) && (tail < (2 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM2_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 3 pamięci
				CS_RAM3_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 2 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 2 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (3 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM3_HIGH();
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}

			if (len > 0) { //odczyt 4 pamięci
				CS_RAM4_LOW();
				SPI1_Write(READ);
				SPI1_Write(0xFF & ((tail - 3 * RAM_CHIPSIZE) >> 8));
				SPI1_Write(0xFF & (tail - 3 * RAM_CHIPSIZE));
				while ((len != 0) && (tail < (4 * RAM_CHIPSIZE))) {
					*buf++ = SPI1_Write(DUMMY_BYTE);
					tail++;
					len--;
				}
				CS_RAM4_HIGH();

				if (tail >= RAM_BUFSIZE) {
					tail = 0;
				}
			} else {
				RAM_buftail = tail;
				xSemaphoreGive(xSPI1_Mutex);
				//			CoLeaveMutexSection(SPI_Mutex);
				return;
			}
		}
		RAM_buftail = tail;
		xSemaphoreGive(xSPI1_Mutex);
		//	CoLeaveMutexSection(SPI_Mutex);
	}
	return;
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
