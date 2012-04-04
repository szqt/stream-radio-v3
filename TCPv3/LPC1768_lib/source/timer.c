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

void ConfigTimer1(void){
	LPC_SC->PCONP |= (1U<<2);

	LPC_SC->PCLKSEL0 &= ~(3U<<4);	//PCLK_TIMER1 = CCLK/4 = 100MHz/4

	LPC_TIM1->TCR = 0x02;			/* reset timer */
	LPC_TIM1->MR1 = 0xFFFFFFFF;
	LPC_TIM1->PR = 1000; 			/* Prescaler, f=25kHz */
//	LPC_TIM1->MCR |= (1U<<3)		/* Interrupt on match */
//					|(1<<4);		/* Reset on match */

	LPC_TIM1->MCR |= (1U<<4);		/* Reset on match */
	LPC_TIM1->TCR = 0x01;		/* start timer */
//	NVIC_SetPriority(TIMER1_IRQn, 4);
//	NVIC_EnableIRQ(TIMER1_IRQn);
}

//void TIMER1_IRQHandler(void){
//
//}
uint32_t GetTimTimer1(void){
	return LPC_TIM1->TC;
}
