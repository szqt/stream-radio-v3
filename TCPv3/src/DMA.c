/*
 * DMA.c
 *
 *  Created on: 13-04-2012
 *      Author: ja
 */
#include "LPC17xx.h"
#include "DMA.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern xSemaphoreHandle xDMAch0_Semaphore;
extern xSemaphoreHandle xDMAch1_Semaphore;
extern xSemaphoreHandle xDMAch2_Semaphore;

void DMA_Config(void){
	uint8_t dana;
	uint8_t TransferSize = 16;
	NVIC_DisableIRQ(DMA_IRQn);
	LPC_SC->PCONP |= (1<<29); 					/* Power on DMA module */
	LPC_SSP1->DMACR |=(1<<1);					/* Enable DMA for the SSP1 transmit FIFO */
	LPC_SSP1->DMACR |=(1<<0);					/* Enable DMA for the SSP1 receive FIFO */
	LPC_SSP0->DMACR |=(1<<1);					/* Enable DMA for the SSP0 transmit FIFO */
	LPC_GPDMA->DMACConfig |= (1<<0);			/* Enable DMA Controller */

	LPC_GPDMA->DMACIntErrClr |= 0xFF;
	LPC_GPDMA->DMACIntTCClear |= 0xFF;

	/* DMA CHANNEL 0 - transfer to SPI external RAM - zmiana na kanał 4*/
	LPC_GPDMACH0->DMACCLLI = 0;								/* Clear Linked List - don't use */
	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)&dana;			/* Set source address */
	LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR;	/* Set destination address */
	LPC_GPDMACH0->DMACCControl = TransferSize 			/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<26)				/* Source address is incremented */
								| (0<<31);				/* Enable Interrupt */

	LPC_GPDMACH0->DMACCConfig |= (2<<6)		/* SSP1 Tx as DMA request peripheral */
							|(1<<11)		/* Memory to peripheral transfer */
							|(1<<14)		/* Enable error interrupt */
							|(1<<15);		/* Enable interrupt */

	/* DMA CHANNEL 1 - transfer to VS decoder via SPI (SSP0) */
	LPC_GPDMACH1->DMACCLLI = 0;								/* Clear Linked List - don't use */
	LPC_GPDMACH1->DMACCSrcAddr = (uint32_t)&dana;			/* Set source address */
	LPC_GPDMACH1->DMACCDestAddr = (uint32_t)&LPC_SSP0->DR;	/* Set destination address */
	LPC_GPDMACH1->DMACCControl = 32 					/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<26)				/* Source address is incremented */
								| (0<<31);				/* Enable Interrupt */

	LPC_GPDMACH1->DMACCConfig |= (0<<6)		/* SSP0 Tx as DMA request peripheral */
							|(1<<11)		/* Memory to peripheral transfer */
							|(1<<14)		/* Enable error interrupt */
							|(1<<15);		/* Enable interrupt */

	/* DMA CHANNEL 2 - transfer from SPI (external RAM) to memory */
	LPC_GPDMACH2->DMACCLLI = 0;								/* Clear Linked List - don't use */
	LPC_GPDMACH2->DMACCSrcAddr = (uint32_t)&LPC_SSP1->DR;			/* Set source address */
	LPC_GPDMACH2->DMACCDestAddr = (uint32_t)&dana;	/* Set destination address */
	LPC_GPDMACH2->DMACCControl = 0;				/* Reset register */
	LPC_GPDMACH2->DMACCControl = 0 				/* Set transfer size */
								| (0<<12)		/* Source burst size - 4 */
								| (1<<15)		/* Destination burst size - 4 */
								| (0<<18)		/* Source transfer width - Byte (8-bit) */
								| (0<<21)		/* Destination transfer width - Byte (8-bit) */
								| (1<<27)		/* Destination address is incremented */
								| (0<<31);		/* Enable Interrupt */

	LPC_GPDMACH2->DMACCConfig = 0;			/* Reset register */
	LPC_GPDMACH2->DMACCConfig |= (3<<1)		/* SSP1 Rx as DMA source request peripheral */
							|(2<<11)		/* Peripheral to memory transfer */
							|(1<<14)		/* Enable error interrupt */
							|(1<<15);		/* Enable interrupt */

	/* DMA CHANNEL 3 - transfer dummy to SPI SSP1 */
	LPC_GPDMACH3->DMACCLLI = 0;								/* Clear Linked List - don't use */
	LPC_GPDMACH3->DMACCSrcAddr = (uint32_t)&dana;			/* Set source address */
	LPC_GPDMACH3->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR;	/* Set destination address */
	LPC_GPDMACH3->DMACCControl = TransferSize 			/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (0<<26)				/* Source address is not incremented */
								| (0<<31);				/* Enable Interrupt */

	LPC_GPDMACH3->DMACCConfig |= (2<<6)		/* SSP1 Tx as DMA request peripheral */
							|(1<<11)		/* Memory to peripheral transfer */
							|(0<<14)		/* Enable error interrupt */
							|(0<<15);		/* Enable interrupt */


	NVIC_SetPriority(DMA_IRQn, 5);
	NVIC_EnableIRQ(DMA_IRQn);
}

void StartSpi1TxDmaTransfer(char *data, uint16_t TransferSize){

	LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)data;		/* Set source address */
	LPC_GPDMACH0->DMACCControl = (0x0FFF & TransferSize)	/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<26)				/* Source address is incremented */
								| (1<<31);				/* Enable Interrupt */
	LPC_GPDMACH0->DMACCConfig |= 1;			/* Enable DMA channel */
	LPC_GPDMA->DMACSoftSReq |= (1<<2);		/* Transfer request - SSP1 Tx*/
}

void StartSpi1RxDmaTransfer(uint8_t *buffer, uint16_t TransferSize){

	LPC_GPDMACH2->DMACCSrcAddr = (uint32_t)&LPC_SSP1->DR;	/* Set source address */
	LPC_GPDMACH2->DMACCDestAddr = (uint32_t)buffer;		/* Set destination address */
	LPC_GPDMACH2->DMACCControl = (0x0FFF & TransferSize)	/* Set transfer size */
								| (0<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<27)				/* Destination address is incremented */
								| (1<<31);				/* Enable Interrupt */
	LPC_GPDMACH2->DMACCConfig |= 1;			/* Enable DMA channel */
//	LPC_GPDMA->DMACSoftSReq |= (1<<3);		/* Transfer request - SSP1 Rx*/
}

void StartSpi1TxDmaDummyTransfer(uint8_t *data, uint16_t TransferSize){

	LPC_GPDMACH3->DMACCSrcAddr = (uint32_t)data;		/* Set source address */
	LPC_GPDMACH3->DMACCControl = (0x0FFF & TransferSize)	/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (0<<26)				/* Source address is not incremented */
								| (0<<31);				/* Enable Interrupt */
	LPC_GPDMACH3->DMACCConfig |= 1;			/* Enable DMA channel */
	LPC_GPDMA->DMACSoftSReq |= (1<<2);		/* Transfer request - SSP1 Tx*/
}

void StartSpi0TxDmaTransfer(uint8_t *buffer){

	LPC_GPDMACH1->DMACCSrcAddr = (uint32_t)buffer;		/* Set source address */
	LPC_GPDMACH1->DMACCControl = 32						/* Set transfer size */
								| (1<<12)				/* Source burst size - 4 */
								| (1<<15)				/* Destination burst size - 4 */
								| (0<<18)				/* Source transfer width - Byte (8-bit) */
								| (0<<21)				/* Destination transfer width - Byte (8-bit) */
								| (1<<26)				/* Source address is incremented */
								| (1<<31);				/* Enable Interrupt */
	LPC_GPDMACH1->DMACCConfig |= 1;			/* Enable DMA channel */
	LPC_GPDMA->DMACSoftSReq |= (1<<0);		/* Transfer request - SSP0 Tx */
}


void DMA_IRQHandler(void){
	uint32_t status;
	static signed portBASE_TYPE xHigherPriorityTaskWoken;

	status = LPC_GPDMA->DMACIntTCStat;
	if((status & 0xFF) != 0){
		LPC_GPDMA->DMACIntTCClear = status;
		if(status & 0x01){					/* Channel 0 int */
			LPC_GPDMACH0->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */
			xSemaphoreGiveFromISR( xDMAch0_Semaphore, &xHigherPriorityTaskWoken );
		}
		if(status & 0x02){					/* Channel 1 int */
			LPC_GPDMACH1->DMACCConfig &= ~(1<0);	/* Disable DMA channel 1 */
			xSemaphoreGiveFromISR( xDMAch1_Semaphore, &xHigherPriorityTaskWoken );
		}
		if(status & 0x04){					/* Channel 2 int */
			LPC_GPDMACH2->DMACCConfig &= ~(1<0);	/* Disable DMA channel 2 */
			xSemaphoreGiveFromISR( xDMAch2_Semaphore, &xHigherPriorityTaskWoken );;
		}
//		if(status & 0x10){					/* Channel 4 int */
//			LPC_GPDMACH4->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */
//			xSemaphoreGiveFromISR( xDMAch0_Semaphore, &xHigherPriorityTaskWoken );
//		}

	}
	status = LPC_GPDMA->DMACRawIntErrStat;
	if ((status & 0xFF) != 0){
		LPC_GPDMA->DMACIntErrClr = status;
		//error interrupt request
		if(status&0x01){					/* Channel 0 int */
			LPC_GPDMACH0->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */
			xSemaphoreGiveFromISR( xDMAch0_Semaphore, &xHigherPriorityTaskWoken );
		}
		if(status & 0x02){					/* Channel 1 int */
			LPC_GPDMACH1->DMACCConfig &= ~(1<0);	/* Disable DMA channel 1 */
			xSemaphoreGiveFromISR( xDMAch1_Semaphore, &xHigherPriorityTaskWoken );
		}
		if(status & 0x04){					/* Channel 2 int */
			LPC_GPDMACH2->DMACCConfig &= ~(1<0);	/* Disable DMA channel 2 */
			xSemaphoreGiveFromISR( xDMAch2_Semaphore, &xHigherPriorityTaskWoken );;
		}
//		if(status&0x10){					/* Channel 4 int */
//			LPC_GPDMACH4->DMACCConfig &= ~(1<0);	/* Disable DMA channel 0 */
//			xSemaphoreGiveFromISR( xDMAch0_Semaphore, &xHigherPriorityTaskWoken );
//		}
	}
	if (xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD();
	}
}
