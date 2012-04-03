/*
 * timer.c
 *
 *  Created on: 15-03-2012
 *      Author: ja
 */

#include "LPC17xx.h"
#include "timer.h"

void DelayTimer_Config(void){
	LPC_SC->PCONP |=  (1U<<1);

	//LPC_SC->PCLKSEL0		PCLK_TIMER0 = CCLK/4 = 100MHz/4
	LPC_TIM0->PR  = 0x00;                /* set prescaler to zero */
    LPC_TIM0->IR  = 0xff;                /* reset all interrrupts */
    LPC_TIM0->MCR = 0x04;                /* stop timer on match */
}

void delay_ms(uint16_t delay_ms){
	LPC_TIM0->TCR = 0x02;                /* reset timer */
	LPC_TIM0->MR0 = delay_ms * (25000000 / 1000-1);
	LPC_TIM0->TCR = 0x01;                /* start timer */

	while (LPC_TIM0->TCR & 0x01);
}
