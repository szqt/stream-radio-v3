/*
 * Tasks.c
 *
 *  Created on: 02-06-2012
 *      Author: ja
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "Tasks.h"

#include "vs1053.h"
#include "gpio.h"




/*--- GLOBALS ---*/
xSemaphoreHandle xDMAch1_Semaphore = NULL;
extern xSemaphoreHandle xSPI0_Mutex;

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

void vIamLiveTask (void * pvParameters){
//	static uint8_t last_vol = 0;
//	uint8_t new_vol;
//	uint8_t licznik=0;
//	char buf[120], buf2[300];
//	unsigned portBASE_TYPE Shoutcast, Vs, IamLive, TCP, ETHinp;
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

//		vTaskGetRunTimeStats((signed char*)buf);
//		UART_PrintBuf (buf, strlen(buf));
//		vTaskList((signed char*)buf2);
//		UART_PrintBuf (buf2, strlen(buf2));

//
//		IamLive = uxTaskGetStackHighWaterMark(NULL);
//		Shoutcast = uxTaskGetStackHighWaterMark(xShoutcastTaskHandle);
//		Vs = uxTaskGetStackHighWaterMark(xVsTskHandle);
//		TCP = uxTaskGetStackHighWaterMark(xLWIPTskHandler);
//		ETHinp = uxTaskGetStackHighWaterMark(xETHTsk);
//		UART_PrintStr("ILive: ");
//		UART_PrintNum(IamLive);
//		UART_PrintStr("\r\n");
//
//		UART_PrintStr("shout: ");
//		UART_PrintNum(Shoutcast);
//		UART_PrintStr("\r\n");
//
//		UART_PrintStr("VS: ");
//		UART_PrintNum(Vs);
//		UART_PrintStr("\r\n");
//
//		UART_PrintStr("LWIP: ");
//		UART_PrintNum(TCP);
//		UART_PrintStr("\r\n");
//
//		UART_PrintStr("LWIP: ");
//		UART_PrintNum(TCP);
//		UART_PrintStr("\r\n");
//
//		UART_PrintStr("ETH: ");
//		UART_PrintNum(ETHinp);
//		UART_PrintStr("\r\n");

		LED_Toggle(2);
		vTaskDelay(50/portTICK_RATE_MS);
	}
}
