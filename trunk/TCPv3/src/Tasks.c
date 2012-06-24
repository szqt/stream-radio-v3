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
#include "external_RAM.h"

#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"
#include "TouchPanel.h"
#include "calibration.h"




/*--- GLOBALS ---*/
xSemaphoreHandle xDMAch1_Semaphore = NULL;
xSemaphoreHandle xButton_pushed_Semaphore = NULL;
extern xSemaphoreHandle xSPI0_Mutex;	// SDI + SCI
extern xSemaphoreHandle xSPI1_Mutex;	// pamieć FRAM + Touch Panel
extern xSemaphoreHandle xDhcpCmplSemaphore_1;	// Semafor informujący o powodzeniu DHCP

/* Uchwyty do zadan */
extern xTaskHandle xVsTskHandle;
extern xTaskHandle xHeartbeatTskHandle;
extern xTaskHandle xShoutcastTaskHandle;
extern xTaskHandle xETHTsk;
extern sys_thread_t xLWIPTskHandler;
extern xTaskHandle xTouchPanelTskHandle;
extern xTaskHandle xWinMngTskHandle;

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
		vTaskDelay(20/portTICK_RATE_MS);
	}
}

void vTouchPanelTask(void * pvParameters) {

	TP_Init();

	while (1) {
		if (xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY) == pdTRUE) {
			SPI1_SetSpeed(SPI_2MHz);
			GUI_TOUCH_Exec();
			SPI1_SetSpeed(SPI_16MHz);
			xSemaphoreGive(xSPI1_Mutex);
		}
		vTaskDelay(20/portTICK_RATE_MS);
	}
}

void vWinMngTask(void * pvParameters){
	while(1){
		WM_Exec();
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void vHeartbeatTask (void * pvParameters){
//	static uint8_t last_vol = 0;
//	uint8_t new_vol;
//	uint8_t licznik=0;
	char buf[120];//, buf2[300];
	unsigned portBASE_TYPE Shoutcast, Vs, Heartbeat, lwIP, ETH, TP;
	BUTTON_Handle hOK_Button;

	vSemaphoreCreateBinary(xButton_pushed_Semaphore);
	if(xDMAch1_Semaphore != NULL){
		xSemaphoreTake(xButton_pushed_Semaphore, 0);
	}else{
		// The semaphore was not created
	}

	BUTTON_Config();	//INT0 Button as source of interrupt

	GUI_Init();
	_ExecCalibration();
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();



	if (xSemaphoreTake(xDhcpCmplSemaphore_1, portMAX_DELAY) == pdTRUE) {
		hOK_Button = BUTTON_Create(70, 85, 180, 70, 12, WM_CF_SHOW);
		GUI_SetFont(&GUI_Font32_ASCII);
		BUTTON_SetText(hOK_Button, "OK");
	}
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
		if(GUI_WaitKey()==12){
			BUTTON_Delete(hOK_Button);
			GUI_ClearRect(0, 80, 319, 170);
			vTaskResume(xShoutcastTaskHandle);		// po kalibracji odblokuj zadanie SHOUTcast
		}
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
			TP = uxTaskGetStackHighWaterMark(xTouchPanelTskHandle);

			printf("\r\n--------Tasks stack watermark-------\r\n");
			printf("Hearbeat:   %d\r\n", Heartbeat);
			printf("Shoutcast:  %d\r\n", Shoutcast);
			printf("VS:         %d\r\n", Vs);
			printf("lwIP:       %d\r\n", lwIP);
			printf("ETH:        %d\r\n", ETH);
			printf("TP          %d\r\n", TP);
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
