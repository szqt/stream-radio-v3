/*
 * LED.c
 *
 *  Created on: 06-02-2012
 *      Author: ja
 */

#include "LPC17xx.h"
#include "gpio.h"

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
void LED_Config(void) {

	LPC_SC->PCONP |= (1 <<15);	//włącz GPIO
	LPC_GPIO2->FIOMASK = 0;
	LPC_GPIO2->FIODIR |= 0x000000FF;	// P2.0 jako wyjcie
	LPC_GPIO2->FIOPIN &= ~0x000000FF;	// diody zgaszone
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
void LED_On (uint8_t led) {

  LPC_GPIO2->FIOPIN |=  (1<<led);                  /* Turn On  LED */
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
void LED_Off (uint8_t led) {

  LPC_GPIO2->FIOPIN &= ~(1<<led);                  /* Turn Off LED */
}

/*------------------------------------------------------------------------------
  Toggle LEDs
 *------------------------------------------------------------------------------*/
void LED_Toggle (uint8_t led) {

  LPC_GPIO2->FIOPIN ^= (1<<led);                  /* Toggle LED */
}

/*------------------------------------------------------------------------------
  configer INT0 interrupt
 *------------------------------------------------------------------------------*/
void BUTTON_Config(void) {

	LPC_GPIO2->FIODIR &= ~(1<<10) ;		// P2.10 jako wejcie
	LPC_GPIOINT->IO2IntEnF |= (1<<10);	// źródło przerwania

	NVIC_EnableIRQ(EINT3_IRQn);
}

