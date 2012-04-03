/*
 * gpio.h
 *
 *  Created on: 06-02-2012
 *      Author: ja
 */

#ifndef GPIO_H_
#define GPIO_H_

void LED_Config(void);
void LED_On (uint8_t led);
void LED_Off (uint8_t led);
void LED_Toggle (uint8_t led);
void BUTTON_Config(void);

#endif /* GPIO_H_ */
