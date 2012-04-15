/*
 * DMA.h
 *
 *  Created on: 13-04-2012
 *      Author: ja
 */

void 	DMA_Config(void);
void 	StartSpi1TxDmaTransfer(char *data, uint16_t TransferSize);
void 	StartSpi1TxDmaDummyTransfer(uint8_t *data, uint16_t TransferSize);
void 	StartSpi1RxDmaTransfer(uint8_t *buffer, uint16_t TransferSize);
void 	StartSpi0TxDmaTransfer(uint8_t *buffer);
