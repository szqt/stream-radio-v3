/*
 * uart.c
 *
 *  Created on: 24-02-2012
 *      Author: ja
 */
#include <stdlib.h>

#include "LPC17xx.h"
#include "uart.h"

/*------------------------------------------------------------------------------
  configer UART2
 *------------------------------------------------------------------------------*/
__INLINE void UART2_Config(int Baudrate)
{
    volatile uint64_t DLest;
    uint32_t pclkdiv, pclk;
    const uint8_t MulVal=2;
    const uint8_t DivAddVal=1;
    const int SystemFrequency = 100000000UL;


    LPC_SC->PCONP |=  (1U<<24);                 // Turn on power to UART2
    LPC_PINCON->PINSEL0 |= (LPC_PINCON->PINSEL0 & ~0xf00000)|   //ensure being cleared
                                           (1U << 20)|                                                  //P0.10 = TXD2
                                           (1U << 22);                                                  //P0.11 = RXD2


    LPC_UART2->LCR =  (3U<<0)|          //8 bit
                                  (0U<<2)|                      //1 stop bit
                                  (0U<<3)|                      //no parity
                                  (1U<<7);                      //DLAB=1


    LPC_UART2->FDR = (MulVal<<4) | (DivAddVal<<0);                      //MulVal=2, DivAddVal=1


    pclkdiv = (LPC_SC->PCLKSEL1 >> 16) & 0x03; //select PCLK_UART2 in PCLKSEL1, bit 6 et 7


    // PCLK is 01 in
    switch ( pclkdiv ){
                case 0x00:
                default:
                        pclk = SystemFrequency/4;
                break;
                case 0x01:
                        pclk = SystemFrequency;
                break;
                case 0x02:
                        pclk = SystemFrequency/2;
                break;
                case 0x03:
                        pclk = SystemFrequency/8;
                break;
        }


    DLest = (pclk>>4)/((Baudrate*(MulVal+DivAddVal))/MulVal);           //Set baud rate
    LPC_UART2->DLM = DLest>>8;                  //BaudRateClock Lower 8 bits
    LPC_UART2->DLL = DLest%256;


    //LPC_UART2->FCR = 0x07;      // Enable and reset TX and RX FIFO
    LPC_UART2->LCR = (LPC_UART2->LCR) & (~0x80); //DLAB=0 to use TER
    //LPC_UART2->IER = (1<<1);  //THRE Interrupt enabled
    LPC_UART2->TER = (1<<7);    //Transmit Enable = 1
    //NVIC_EnableIRQ(UART2_IRQn);
    //NVIC_SetPriority(UART2_IRQn,)
}


void  UART_PrintChar (uint8_t ch)
{


   while (!(LPC_UART2->LSR & 0x20));
   LPC_UART2->THR  = ch;
}

void UART_PrintNum (int numb){
	char buffer[10];
	itoa(numb, buffer, 10);
	UART_PrintStr(buffer);
}

void  UART_PrintStr (char * str)
{


   while ((*str) != 0) {
      if (*str == '\n') {
         UART_PrintChar(*str++);
         UART_PrintChar('\r');
      } else {
         UART_PrintChar(*str++);
      }
   }
}


/*void  UART_Printf (const  uint8_t *format, ...)
{
    static  uint8_t  buffer[40 + 1];
            va_list     vArgs;


    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);
    UART_PrintStr((uint8_t *) buffer);
}
*/


void UART2_IRQHandler(){
        //UART2_SendByte(0x43);
        if( 0x02==((LPC_UART2->IIR)&0xf) ){
                LPC_UART2->THR = 0x43;
                //LED_On(1<<0);
        }
}
