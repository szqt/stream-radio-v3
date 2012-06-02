/*
 * uart.h
 *
 *  Created on: 24-02-2012
 *      Author: ja
 */

#ifndef UART_H_
#define UART_H_

#include "LPC17xx.h"

__INLINE void UART2_Config(int Baudrate);
void  	UART_PrintChar (uint8_t ch);
void  UART_Sendchar (char ch);
char 	UART_Getchar(void);
void 	UART_PrintNum (int numb);
void  	UART_PrintStr (char * str);
void  	UART_PrintBuf (char * str, uint16_t len);
void 	UART2_IRQHandler();

#endif /* UART_H_ */
