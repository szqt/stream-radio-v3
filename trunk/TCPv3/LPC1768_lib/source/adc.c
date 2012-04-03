/*
 * adc.c
 *
 *  Created on: 15-03-2012
 *      Author: ja
 */
#include "LPC17xx.h"
#include "adc.h"

void ADC_Config(void) {
	LPC_SC->PCONP |= (1U << 12); 	/* Power on ADC */
									/* PCLK_ADC = CCLK/4 = 25MHz*/
	LPC_PINCON->PINSEL3 |= (0b11<<30);
	LPC_PINCON->PINMODE3 |= (0b11<<30);		/* pulldown */

	LPC_ADC->ADCR |= (1 << 5) |		/* Select AD0.5 pin */
					(1 << 8) |		/* ADC clock = 25MHz/2 */
					(1<<21) |		/* enable ADC */
					(1<<16);		/* ciągła konwersja */

}
