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
#include "LISTBOX.h"
#include "SCROLLBAR.h"
#include "TouchPanel.h"
#include "calibration.h"




/*--- GLOBALS ---*/
xSemaphoreHandle xDMAch1_Semaphore = NULL;
xSemaphoreHandle xButton_pushed_Semaphore = NULL;
xQueueHandle xListBoxQueue;
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
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void vWinMngTask(void * pvParameters){
	while(1){
		WM_Exec();
	}
}

void vHeartbeatTask (void * pvParameters){

	int Key, Entries, ySize;
	uint16_t click_cnt=0;
	int8_t Item_nb;
	BUTTON_Handle hOK_Button, hTest_Key;
	LISTBOX_Handle hStationListBox;
	static enum GuiState eGuiState = SELECT;
	static const GUI_ConstString StationList[] = {"ZET", "RMFFM", "RMFMAXXX", "SKY.FM", "ESKA ROCK", "TERMINAL", NULL};

	vSemaphoreCreateBinary(xButton_pushed_Semaphore);
	if(xDMAch1_Semaphore != NULL){
		xSemaphoreTake(xButton_pushed_Semaphore, 0);
	}else{
		// The semaphore was not created
	}
	BUTTON_Config();	//INT0 Button as source of interrupt
	xSemaphoreTake(xButton_pushed_Semaphore, 0);

	xListBoxQueue = xQueueCreate(2, sizeof(int8_t));


	GUI_Init();
	if(0){
		_ExecCalibration();
	}else{
		_DefaultCalibration();
	}

	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();



	if (xSemaphoreTake(xDhcpCmplSemaphore_1, portMAX_DELAY) == pdTRUE) {
		/* OK button */
		hOK_Button = BUTTON_CreateEx(120, 210, 80, 20, 0, WM_CF_SHOW, 0, 12);
		GUI_SetFont(&GUI_Font32_ASCII);
		BUTTON_SetText(hOK_Button, "OK");

		/* Cancel button */
		hTest_Key = BUTTON_CreateEx(120, 180, 80, 20, 0, WM_CF_SHOW, 0, 13);
		GUI_SetFont(&GUI_Font16B_ASCII);
		BUTTON_SetText(hTest_Key, "CLICK");
		WM_SetStayOnTop(hTest_Key, 1);

		/* Station list */
		Entries = 6;//countof(StationList) - 1;
		ySize = GUI_GetYDistOfFont(&GUI_Font16B_ASCII)*Entries;
		hStationListBox = LISTBOX_CreateEx(100, 10, 120, ySize, 0, WM_CF_SHOW, 0, 5, StationList);
		SCROLLBAR_CreateAttached(hStationListBox, SCROLLBAR_CF_VERTICAL);

	}

	while(1){
		Key = GUI_GetKey();
		//top = WM_GetStayOnTop(hTest_Key);

		switch(eGuiState){
		case SELECT:
			switch (Key){
			case 12:
				Item_nb = LISTBOX_GetSel(hStationListBox);
				if(Item_nb >= 0){
					if(xQueueSendToBack(xListBoxQueue, &Item_nb, 50/portTICK_RATE_MS) == pdPASS){
						/* OK button delete */
						BUTTON_Delete(hOK_Button);
						GUI_SetBkColor(GUI_BLUE);
						GUI_ClearRect(120, 210, 200, 230);
						GUI_ClearKeyBuffer();

						/* Listbox delete */
						LISTBOX_Delete(hStationListBox);
						GUI_ClearRect(100, 10, 220, ySize+10);

						eGuiState = PLAYING;
					}
				}
				vTaskResume(xShoutcastTaskHandle);
				break;
			case 13:
				click_cnt++;
				GUI_SetFont(&GUI_Font16B_ASCII);
				Item_nb = LISTBOX_GetSel(hStationListBox);
				GUI_DispStringAt("CNT = ", 0, 210);
				GUI_DispDecSpace(Item_nb, 3);
				break;
			default:
				break;
			}

			break;
		case PLAYING:
			switch (Key){
			case 13:
				click_cnt++;
				GUI_SetFont(&GUI_Font16B_ASCII);
//				Item_nb = LISTBOX_GetSel(hStationListBox);
				GUI_DispStringAt("CNT = ", 0, 210);
				GUI_DispDecSpace(123, 3);
				break;
			default:
				break;
			}
		}
		vTaskDelay(20/portTICK_RATE_MS);
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

void SystemStats(void){

	char buf[120];//, buf2[300];
	unsigned portBASE_TYPE Shoutcast, Vs, Heartbeat, lwIP, ETH, TP, WM;
	if (pdTRUE == xSemaphoreTake(xButton_pushed_Semaphore, 0)) {
		printf("\r\n-----------Run time stats-----------\r\n");
		vTaskGetRunTimeStats((signed char*) buf);
		UART_PrintBuf(buf, strlen(buf));
		//		vTaskList((signed char*)buf2);
		//		UART_PrintBuf (buf2, strlen(buf2));
		printf("\r\n------------------------------------\r\n");

		//
		Heartbeat = uxTaskGetStackHighWaterMark(NULL);
		Shoutcast = uxTaskGetStackHighWaterMark(xShoutcastTaskHandle);
		Vs = uxTaskGetStackHighWaterMark(xVsTskHandle);
		lwIP = uxTaskGetStackHighWaterMark(xLWIPTskHandler);
		ETH = uxTaskGetStackHighWaterMark(xETHTsk);
		TP = uxTaskGetStackHighWaterMark(xTouchPanelTskHandle);
		WM = uxTaskGetStackHighWaterMark(xWinMngTskHandle);

		printf("\r\n--------Tasks stack watermark-------\r\n");
		printf("Hearbeat:   %d\r\n", Heartbeat);
		printf("Shoutcast:  %d\r\n", Shoutcast);
		printf("VS:         %d\r\n", Vs);
		printf("lwIP:       %d\r\n", lwIP);
		printf("ETH:        %d\r\n", ETH);
		printf("TouchP:     %d\r\n", TP);
		printf("WinMan:    %d\r\n", WM);
		printf("\r\n------------------------------------\r\n");
	}

	LED_Toggle(2);
}
