/*
 * printuart.c
 *
 *  Created on: 03-03-2012
 *      Author: ja
 */

#include <stdio.h>
#include <stdarg.h>
#include "uart.h"

void printfuart(const char *pFormat, ...){

	char buf[100];
	int num;
	va_list ap;
	va_start(ap, pFormat);
	num = vsprintf(buf, pFormat, ap);
	va_end(ap);
	UART_PrintStr(buf);

}
