/*
 * Tasks.c
 *
 *  Created on: 02-06-2012
 *      Author: ja
 */
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "lwip/sys.h"
#include "lwip/stats.h"

#include "Tasks.h"

#include "vs1053.h"
#include "gpio.h"
#include "uart.h"




/*--- GLOBALS ---*/
xSemaphoreHandle xDMAch1_Semaphore = NULL;
xSemaphoreHandle xButton_pushed_Semaphore = NULL;
extern xSemaphoreHandle xSPI0_Mutex;

/* Uchwyty do zadan */
extern xTaskHandle xVsTskHandle;
extern xTaskHandle xHeartbeatTskHandle;
extern xTaskHandle xShoutcastTaskHandle;
extern xTaskHandle xETHTsk;
extern sys_thread_t xLWIPTskHandler;

void vVsTask(void * pvParameters) {

	vSemaphoreCreateBinary(xDMAch1_Semaphore);
	if(xDMAch1_Semaphore != NULL){
		xSemaphoreTake(xDMAch1_Semaphore, 0);
	}else{
		// The semaphore was not created
	}

	while (1) {
		if (xSemaphoreTake(xSPI0_Mutex, portMAX_DELAY) == pdTRUE) {
			VS_feed();
			xSemaphoreGive(xSPI0_Mutex);
		}
		vTaskDelay(15/portTICK_RATE_MS);
	}
}

//void vGUITask(void * pvParameters) {
//	GUI_Init();
//	while (1) {
//		if (xSemaphoreTake(xSPI0_Mutex, portMAX_DELAY) == pdTRUE) {
//			VS_feed();
//			xSemaphoreGive(xSPI0_Mutex);
//		}
//		vTaskDelay(15/portTICK_RATE_MS);
//	}
//}

void vHeartbeatTask (void * pvParameters){
//	static uint8_t last_vol = 0;
//	uint8_t new_vol;
//	uint8_t licznik=0;
	char buf[120];//, buf2[300];
	unsigned portBASE_TYPE Shoutcast, Vs, Heartbeat, lwIP, ETH;

	vSemaphoreCreateBinary(xButton_pushed_Semaphore);
	if(xDMAch1_Semaphore != NULL){
		xSemaphoreTake(xButton_pushed_Semaphore, 0);
	}else{
		// The semaphore was not created
	}

	BUTTON_Config();	//INT0 Button as source of interrupt

	while(1){

//		new_vol = ((LPC_ADC->ADDR5>>8) & 0x00FF);
//		if((new_vol > last_vol+4) || (new_vol < last_vol - 4)){
//			if (xSemaphoreTake(xSPI0_Mutex, portMAX_DELAY) == pdTRUE) {
//				vs_set_volume(new_vol);
//				LED_Toggle(3);
//				xSemaphoreGive(xSPI0_Mutex);
//			}
//			last_vol = new_vol;
//
//		}
//		licznik++;
//		if(licznik >=40){
//			UART_PrintNum(RAM_buflen());
//			UART_PrintStr("\r\n");
//			licznik = 0;
//		}
		if (pdTRUE == xSemaphoreTake(xButton_pushed_Semaphore, 0)) {
			printf("\r\n--------Run time stats-------\r\n");
			vTaskGetRunTimeStats((signed char*) buf);
			UART_PrintBuf(buf, strlen(buf));
			//		vTaskList((signed char*)buf2);
			//		UART_PrintBuf (buf2, strlen(buf2));
			printf("\r\n-----------------------------\r\n");

			//
			Heartbeat = uxTaskGetStackHighWaterMark(NULL);
			Shoutcast = uxTaskGetStackHighWaterMark(xShoutcastTaskHandle);
			Vs = uxTaskGetStackHighWaterMark(xVsTskHandle);
			lwIP = uxTaskGetStackHighWaterMark(xLWIPTskHandler);
			ETH = uxTaskGetStackHighWaterMark(xETHTsk);

			printf("\r\n--------Tasks stack watermark-------\r\n");
			printf("Hearbeat:   %d\r\n", Heartbeat);
			printf("Shoutcast:  %d\r\n", Shoutcast);
			printf("VS:         %d\r\n", Vs);
			printf("lwIP:       %d\r\n", lwIP);
			printf("ETH:        %d\r\n", ETH);
			printf("\r\n------------------------------------\r\n");
		}

		LED_Toggle(2);
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

/*
 * Button INT0 interrupt handler
 */
void EINT3_IRQHandler(void){

	static signed portBASE_TYPE xHigherPriorityTaskWoken;

	LPC_GPIOINT->IO2IntClr |= (1<<10);	//kasuje flagę prerwania
	xSemaphoreGiveFromISR(xButton_pushed_Semaphore, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken == pdTRUE){
		portYIELD();
	}
}
