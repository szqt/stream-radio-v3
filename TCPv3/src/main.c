#include <cr_section_macros.h>
#include <NXP/crp.h>

#include "stdint.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "system_lpc17xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "ethernetif.h"
#include "lwip/debug.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
//#include "netif/loopif.h"

#include "lwip/dhcp.h"

#include "uart.h"
#include "gpio.h"
#include "main.h"
#include "vs1053.h"
#include "timer.h"
#include "adc.h"
#include "external_RAM.h"


extern u8_t IPradio_init(void);
extern u8_t HTTP_init(void);
void eth_lwip_init(void);

void vIamLiveTask (void * pvParameters);
void vVsTask(void * pvParameters);
void vDHCP_TimerCallback(xTimerHandle pxTimer);

xTaskHandle xVsTskHandle;
xTaskHandle xIamLiveHandle;
extern xTaskHandle xShoutcastTaskHandle;
extern xTaskHandle xETHTsk;
extern sys_thread_t xLWIPTskHandler;
//OS_STK IamLive_stk[100];
//OS_STK vs_task_stk[100];
//OS_TID VS_TSK_ID;
//OS_TID LIVE_TSK_ID;

xSemaphoreHandle xSPI0_Mutex = NULL;
xSemaphoreHandle xSPI1_Mutex = NULL;
//OS_MutexID SPI_Mutex;
//OS_MutexID SPI0_Mutex;


struct netif *lpc17xx_netif;
struct netif *loop_netif;

#if DHCP_ON
	u16_t dhcpcnt = 0;
	u8_t dhcpflag = 0;
	u32_t ipadres[4];
	//OS_TCID timer;
#define DHCP_TIM_ID		1
	xTimerHandle xDHCP_Timer;
	xSemaphoreHandle xDhcpCmplSemaphore_1 = NULL;
	xSemaphoreHandle xDhcpCmplSemaphore_2 = NULL;
//	OS_EventID semDhcpCmpl;
//	OS_EventID semDhcpCmpl_2;
#endif

int main()
{
	uint8_t k, l;
	DelayTimer_Config();
	LED_Config();
	BUTTON_Config();
	UART2_Config(115200);
	ADC_Config();
	vs_init();
	RAM_init();
	RAM_test();
	for(k=5; k>0; k--){
		l=2/(k-1);
	}

	eth_lwip_init();
#if DHCP_ON
	xDHCP_Timer = xTimerCreate((signed char *) "DHCP_TIM", 500/portTICK_RATE_MS, pdTRUE, ( void * )DHCP_TIM_ID, vDHCP_TimerCallback);
	if( xTimerStart(xDHCP_Timer, 0 ) != pdPASS ){
		// The timer could not be set into the Active state.
	}
#endif

	IPradio_init();
//	HTTP_init();
	xSPI0_Mutex = xSemaphoreCreateMutex();
    if(xSPI0_Mutex == NULL){
        // The mutex was not created.
    }
	xSPI1_Mutex = xSemaphoreCreateMutex();
    if(xSPI1_Mutex == NULL){
        // The mutex was not created.
    }
	xTaskCreate( vVsTask, ( signed portCHAR * ) "VsTsk", 200, NULL, 4, &xVsTskHandle );
	xTaskCreate( vIamLiveTask, ( signed portCHAR * ) "IaLTsk", 400, NULL, 3, &xIamLiveHandle );

	vTaskSuspend(xVsTskHandle);

	vTaskStartScheduler();
	while(1);
}

void eth_lwip_init(void)
{
	struct ip_addr ipaddr, netmask, gw;

	tcpip_init(NULL, NULL);

	lpc17xx_netif = mem_malloc(sizeof(struct netif));

#if DHCP_ON
	IP4_ADDR(&gw, 0,0,0,0);
	IP4_ADDR(&ipaddr, 0,0,0,0);
	IP4_ADDR(&netmask, 0,0,0,0);

#else
	IP4_ADDR(&gw, 192,168,192,1);
	IP4_ADDR(&ipaddr, 192,168,196,124);
	IP4_ADDR(&netmask, 255,255,240,0);
#endif

	netif_set_default(netif_add(lpc17xx_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
	netif_set_up(lpc17xx_netif);

#if DHCP_ON
	dhcp_start(lpc17xx_netif);					/* Uruchom klienta DHCP */
	vSemaphoreCreateBinary(xDhcpCmplSemaphore_1);
	if(xDhcpCmplSemaphore_1 != NULL){
		xSemaphoreTake(xDhcpCmplSemaphore_1, 0);
	}else{
		// The semaphore was not created
	}
	vSemaphoreCreateBinary(xDhcpCmplSemaphore_2);
	if(xDhcpCmplSemaphore_1 != NULL){
		xSemaphoreTake(xDhcpCmplSemaphore_2, 0);
	}else{
		// The semaphore was not created
	}
#endif

}

#if DHCP_ON
/*funkcja co 500ms*/
void vDHCP_TimerCallback(xTimerHandle pxTimer){
	dhcpcnt++;
	dhcp_fine_tmr();

	if(dhcpcnt>=DHCP_COARSE_TIMER_SECS*2){
		dhcp_coarse_tmr();
		dhcpcnt=0;

	}
	if(lpc17xx_netif->ip_addr.addr != 0 && dhcpflag == 0){
//		ipadres[0]=0xff&(lpc17xx_netif->ip_addr.addr>>24); //od prawej
//		ipadres[1]=0xff&(lpc17xx_netif->ip_addr.addr>>16);
//		ipadres[2]=0xff&(lpc17xx_netif->ip_addr.addr>>8);
//		ipadres[3]=0xff&(lpc17xx_netif->ip_addr.addr);	// do lewej
		LED_On(5);
		xSemaphoreGive(xDhcpCmplSemaphore_1);
//		xSemaphoreGive(xDhcpCmplSemaphore_2);
		dhcpflag = 1;
	}
}
#endif

void vIamLiveTask (void * pvParameters){
//	static uint8_t last_vol = 0;
//	uint8_t new_vol;
//	uint8_t licznik=0;
	char buf[120];
	unsigned portBASE_TYPE Shoutcast, Vs, IamLive, TCP, ETHinp;
	while(1){

//		new_vol = ((LPC_ADC->ADDR5>>8) & 0x00FF);
//		if((new_vol > last_vol+4) || (new_vol < last_vol - 4)){
//			if (xSemaphoreTake(xSPI0_Mutex, portMAX_DELAY) == pdTRUE) {
//				vs_set_volume(new_vol);
//				xSemaphoreGive(xSPI0_Mutex);
//			}
//			last_vol = new_vol;
//			LED_Toggle(3);
//		}
//		licznik++;
//		if(licznik >=40){
//			UART_PrintNum(RAM_buflen());
//			UART_PrintStr("\r\n");
//			licznik = 0;
//		}

		vTaskGetRunTimeStats((signed char*)buf);
		UART_PrintBuf (buf, strlen(buf));

		IamLive = uxTaskGetStackHighWaterMark(NULL);
		Shoutcast = uxTaskGetStackHighWaterMark(xShoutcastTaskHandle);
		Vs = uxTaskGetStackHighWaterMark(xVsTskHandle);
		TCP = uxTaskGetStackHighWaterMark(xLWIPTskHandler);
		ETHinp = uxTaskGetStackHighWaterMark(xETHTsk);
		UART_PrintStr("ILive: ");
		UART_PrintNum(IamLive);
		UART_PrintStr("\r\n");

		UART_PrintStr("shout: ");
		UART_PrintNum(Shoutcast);
		UART_PrintStr("\r\n");

		UART_PrintStr("VS: ");
		UART_PrintNum(Vs);
		UART_PrintStr("\r\n");

		UART_PrintStr("LWIP: ");
		UART_PrintNum(TCP);
		UART_PrintStr("\r\n");

		UART_PrintStr("LWIP: ");
		UART_PrintNum(TCP);
		UART_PrintStr("\r\n");

		UART_PrintStr("ETH: ");
		UART_PrintNum(ETHinp);
		UART_PrintStr("\r\n");

		LED_Toggle(2);
		vTaskDelay(4000/portTICK_RATE_MS);
//		CoTickDelay(100);
	}
}

void vVsTask(void * pvParameters) {
	while (1) {
		if (xSemaphoreTake(xSPI0_Mutex, portMAX_DELAY) == pdTRUE) {
			VS_feed();
			xSemaphoreGive(xSPI0_Mutex);
		}
		vTaskDelay(15/portTICK_RATE_MS);
//		CoTickDelay(15);
	}
}
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ){
	while(1);
}
