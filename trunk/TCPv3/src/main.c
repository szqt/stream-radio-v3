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
#include "DMA.h"
//#include "GLCD.h"

#include "GUI.h"

#include "Tasks.h"


extern u8_t IPradio_init(void);
extern u8_t HTTP_init(void);
void eth_lwip_init(void);

/* Uchwyty do zadan */
xTaskHandle xVsTskHandle;
xTaskHandle xHeartbeatTskHandle;
extern xTaskHandle xShoutcastTaskHandle;
extern xTaskHandle xETHTsk;
extern sys_thread_t xLWIPTskHandler;

/* Semafory do synchronizacji dostępu do interfejsów SPI 0 i 1 */
xSemaphoreHandle xSPI0_Mutex = NULL;
xSemaphoreHandle xSPI1_Mutex = NULL;


struct netif *lpc17xx_netif;
struct netif *loop_netif;

#if DHCP_ON
	u16_t dhcpcnt = 0;
	u8_t dhcpflag = 0;
	u32_t ipadres[4];
#define DHCP_TIM_ID		1
	xTimerHandle xDHCP_Timer;
	xSemaphoreHandle xDhcpCmplSemaphore_1 = NULL;
	xSemaphoreHandle xDhcpCmplSemaphore_2 = NULL;
#endif

int main()
{
	DelayTimer_Config();
//	GLCD_Init();
//	GLCD_Clear(Purple);




	GUI_Init();
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
//	GUI_DispString("Hello world!");




	LED_Config();
	BUTTON_Config();
	UART2_Config(115200);
	ADC_Config();
	VS_Init();
	RAM_Init();
	DMA_Config();
//	RAM_test();

	eth_lwip_init();
#if DHCP_ON
	xDHCP_Timer = xTimerCreate((signed char *) "DHCP_TIM", 500/portTICK_RATE_MS, pdTRUE, ( void * )DHCP_TIM_ID, vDHCP_TimerCallback);
	if( xTimerStart(xDHCP_Timer, 0 ) != pdPASS ){
		// The timer could not be set into the Active state.
	}
#endif

	xSPI0_Mutex = xSemaphoreCreateMutex();
    if(xSPI0_Mutex == NULL){
        // The mutex was not created.
    }
	xSPI1_Mutex = xSemaphoreCreateMutex();
    if(xSPI1_Mutex == NULL){
        // The mutex was not created.
    }
	if(IPradio_init() != 0){
		// Could not be initiated
	}
//	HTTP_init();
	xTaskCreate( vVsTask, ( signed portCHAR * ) "VsTsk", 100, NULL, 5, &xVsTskHandle );			//wykorzystuje ok 60
	xTaskCreate( vHeartbeatTask, ( signed portCHAR * ) "IaLTsk", 400, NULL, 3, &xHeartbeatTskHandle );

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
		xSemaphoreGive(xDhcpCmplSemaphore_2);
		dhcpflag = 1;
	}
}
#endif



void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ){
	printf("Stack overflow\n");
	printf("Task: %s", pcTaskName);
	while(1);
}

void vApplicationMallocFailedHook(void){
	while(1);
}
