
#include <string.h>
#include <stdlib.h>
#include <cr_section_macros.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "LPC17xx.h"
#include "gpio.h"
#include "uart.h"
#include "external_RAM.h"
//#include "vs1053.h"

#if LWIP_NETCONN

void shoutcast(void *pdata);

#define sizeScast				400
#define prioScast				5
#define STATION_NAME_MAX_LEN	100
#define TITLE_MAX_LEN			100

//OS_STK stkScast[sizeScast];
//extern OS_TID VS_TSK_ID;
//extern OS_EventID semDhcpCmpl;
extern xTaskHandle xVsTskHandle;
extern xSemaphoreHandle xDhcpCmplSemaphore_1;

//__DATA(RAM2) char mybuf[6144];
__SECTION(bss,RAM2) char mybuf[6144];

struct{
	int MetaInt;					/* interwał meta-danych stacji */
	char Name[STATION_NAME_MAX_LEN];	/* pole nazwy stacji */

}RadioInf;
char Title[TITLE_MAX_LEN];
unsigned char IPradio_init(void)
{
	xTaskHandle xShoutcastTaskHandle;

	xShoutcastTaskHandle = sys_thread_new(NULL,
                       shoutcast,
                       NULL,
                       sizeScast,
                       prioScast);

  if(xShoutcastTaskHandle == SYS_THREAD_NULL) {
    return 1;
  }

  return 0;	
}


void shoutcast(void *pdata) {
	struct netconn *NetConn = NULL;
	struct netbuf *inbuf;
	struct ip_addr ipaddrserv;
	err_t rc1, rc2, rc3;
	u8_t cnt;
	u16_t CpBytes, MetaDataLen, BufLen, MDLpos;
	char *ptr, *ptr_tmp;
	int HeaderLen, DataCounter=0;
	char flaga = 0;
//	int32_t dropped=0;
//	uint64_t OStime;
//	uint32_t dOStime;

//	const char string[] = "GET / HTTP/1.0\r\nHost: 217.74.72.10\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 89.149.227.111\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 89.238.252.146\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 50.117.115.211\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 217.74.72.12\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 50.7.241.126\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
	const char string[] = "GET / HTTP/1.0\r\nHost: 85.17.26.74\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	const char string[] = "GET / HTTP/1.0\r\nHost: 88.190.234.235\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";

//	IP4_ADDR(&ipaddrserv, 217, 74, 72, 10); //port 9000		RMF AAC+ 48bps
//	IP4_ADDR(&ipaddrserv, 89, 149, 227, 111); //port 8050	ZET AAC+ 32bps
//	IP4_ADDR(&ipaddrserv, 89, 238, 252, 146); //port 7000	EuropaFM AAC+ Romiania 32kbps
//	IP4_ADDR(&ipaddrserv, 50, 117, 115, 211); //port 80		idobi Radio MP3 128kbps
//	IP4_ADDR(&ipaddrserv, 217, 74, 72, 12); //port 9002		RMF MAXXX AAC+ 48kbps
//	IP4_ADDR(&ipaddrserv, 50, 7, 241, 126); //port 80		Alex Jones - Infowars.com MP3 32kbps
	IP4_ADDR(&ipaddrserv, 85, 17, 26, 74); //port 80		TechnoBase.FM MP3 128kbps
//	IP4_ADDR(&ipaddrserv, 80, 190, 234, 235); //port 80		French Kiss FM MP3 128kbps

	if (xSemaphoreTake(xDhcpCmplSemaphore_1, portMAX_DELAY) == pdTRUE) {
reconect:

		NetConn = netconn_new(NETCONN_TCP);
		netconn_set_recvtimeout(NetConn, 30000);

		if(NetConn == NULL){
			/*No memory for new connection? */
			UART_PrintStr("No mem for new con\r\n");
		}
		rc1 = netconn_bind(NetConn, NULL, 3250);	//3250		/* Adres IP i port local host'a */
		UART_PrintStr("netcon binded\r\n");
		rc2 = netconn_connect(NetConn, &ipaddrserv, 80);		/* Adres IP i port serwera */
		UART_PrintStr("netcon connected\r\n");

		if(rc1 != ERR_OK || rc2 != ERR_OK){
			netconn_delete(NetConn);
			UART_PrintStr("connection error\r\n");
			if(rc1 != ERR_OK)
				UART_PrintStr("rc1\r\n");
			if(rc2 != ERR_OK)
				UART_PrintStr("rc2\r\n");
			LED_On(0);
			goto reconect;
		}
		else{
			LED_Off(0);
			netconn_write(NetConn, string, sizeof(string), NETCONN_NOCOPY);

			while((netconn_recv(NetConn, &inbuf)) == ERR_OK){
				BufLen = netbuf_len(inbuf);
				CpBytes = netbuf_copy(inbuf, mybuf, BufLen);
				netbuf_delete(inbuf);

				/* Znajdź wartosć interwału meta-danych */
				ptr = strstr(mybuf, "icy-metaint");
				if(ptr != NULL){
					ptr = ptr+12;				/* przestaw wskaźnik na pierwszy bajt MetaInt */
					RadioInf.MetaInt = atoi(ptr);
				}
				/* Znajdź nazwę stacji */
				ptr = strstr(mybuf, "icy-name");
				if(ptr != NULL){
					ptr = ptr+9;				/* przestaw wkaźnika na pierwszy bajt nazwy */
					ptr_tmp = RadioInf.Name;
					cnt = 0;
					while(*ptr != '\r' && *ptr != '\n' && cnt<STATION_NAME_MAX_LEN-1){
						*ptr_tmp++ = *ptr++;
						cnt++;
					}
					*ptr_tmp = '\0';

					UART_PrintStr(RadioInf.Name);
					UART_PrintStr("\r\n");
				}

				/* Znajdź koniec nagłówka */
				ptr = strstr(mybuf, "\r\n\r\n");
				if (ptr != NULL) {
					HeaderLen = ptr - mybuf + 4;		/* Oblicz długosc danych nagłówkoych w odebranej porcji danych */
					DataCounter = CpBytes - HeaderLen;	/* W pakiecie są pierwsze dane strumienia */
					RAM_bufputs(mybuf, DataCounter);		/* Wrzuć te dane do bufora VS */
					break;
				}

//				CoAwakeTask(VS_TSK_ID);
				vTaskResume(xVsTskHandle);
			}
			while((rc3 = netconn_recv(NetConn, &inbuf)) == ERR_OK){

				BufLen = netbuf_len(inbuf);
				if(BufLen > 6144){
					UART_PrintStr("ERROR3");
				}
				CpBytes= netbuf_copy(inbuf, mybuf, BufLen);

				netbuf_delete(inbuf);

				if(RAM_buflen()==0){
//					CoSuspendTask(VS_TSK_ID);
					vTaskSuspend(xVsTskHandle);
					flaga = 0;
				}

				DataCounter += CpBytes;

				/* W odebranych danych są meta-dane */
				if(DataCounter > RadioInf.MetaInt){
					MDLpos = CpBytes-(DataCounter-RadioInf.MetaInt);				/* Pozycja MetaDataLength */
					MetaDataLen = 16*mybuf[CpBytes-(DataCounter-RadioInf.MetaInt)];	/* Długoć MetaDanych */

					if (MetaDataLen > 0){
						ptr = strstr((mybuf+MDLpos + 1), "'");		/* 27 = ascii "'" */
						if(ptr != NULL){
							ptr_tmp = strstr(ptr+1, "';");				/* koniec pola StreamTitle */
							if(ptr_tmp != NULL){
								cnt=0;
								while(++ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
									UART_PrintChar(*ptr);
									Title[cnt] = *ptr;
									cnt++;
								}
								Title[cnt] = '\0';				/* NULL na końcu tytułu */
								UART_PrintStr("\r\n");
							}else
								UART_PrintStr("NUL 2\r\n");
						}else
							UART_PrintStr("NUL 1\r\n");
					}

					DataCounter=DataCounter-RadioInf.MetaInt-MetaDataLen-1;			/* Ilosć danych audio przed meta-danymi */

//					if(RAM_buffree() >= MDLpos){			/* Sprawdź czy jest miejsce w buforze vs */
//						RAM_bufputs(mybuf, MDLpos);		/* Dane przed meta danymi */
//					}
//					else{
//						UART_PrintStr("DROP ");
//						dropped += CpBytes;
//						UART_PrintNum(dropped);
//						UART_PrintStr("\r\n");
//					}
//
//					if(RAM_buffree() >= (CpBytes - MDLpos - MetaDataLen - 1)){		/* Sprawdź czy jest miejsce w buforze vs */
//						RAM_bufputs(mybuf+MDLpos+MetaDataLen+1, (CpBytes - MDLpos - MetaDataLen - 1));		/* Dane po meta danych */
//					}
//					else{
//						UART_PrintStr("DROP ");
//						dropped += CpBytes;
//						UART_PrintNum(dropped);
//						UART_PrintStr("\r\n");
//					}
//
					while(RAM_buffree() < MDLpos){			/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(40/portTICK_RATE_MS);
//						CoTickDelay(40);
					}
					RAM_bufputs(mybuf, MDLpos);		/* Dane przed meta danymi */

					while(RAM_buffree() < (CpBytes - MDLpos - MetaDataLen - 1)){		/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(40/portTICK_RATE_MS);
//						CoTickDelay(40);
					}
//					OStime = CoGetOSTime();
					RAM_bufputs(mybuf+MDLpos+MetaDataLen+1, (CpBytes - MDLpos - MetaDataLen - 1));		/* Dane po meta danych */
//					dOStime = CoGetOSTime() - OStime;
//					UART_PrintNum(dOStime);
//					UART_PrintStr("\r\n");

				}
				/* W odebranych danych niema meta-danych */
				else{
//					if(RAM_buffree() >= CpBytes){		/* Sprawdź czy jest miejsce w buforze vs */
//						RAM_bufputs(mybuf, CpBytes);
//					}
//					else{
//						UART_PrintStr("DROP ");
//						dropped += CpBytes;
//						UART_PrintNum(dropped);
//						UART_PrintStr("\r\n");
//					}
//				}

					while(RAM_buffree() < CpBytes){		/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(40/portTICK_RATE_MS);
//						CoTickDelay(40);
					}

					RAM_bufputs(mybuf, CpBytes);
				}


				if(RAM_buflen()>20*1024 && flaga == 0){
					vTaskResume(xVsTskHandle);
//					CoAwakeTask(VS_TSK_ID);
					flaga=1;
				}
				LED_Toggle(1);
//				CoTickDelay(50);
			}
			if(rc3 == ERR_TIMEOUT)
				UART_PrintStr("Timeout\r\n");
			else
				UART_PrintStr("recv error\r\n");
			LED_On(0);
			netconn_close(NetConn);
			UART_PrintStr("netcon closed\r\n");
			//netbuf_delete(inbuf);
			netconn_delete(NetConn);
			UART_PrintStr("netcon deleted\r\n");
			goto reconect;
		}
	}
	while(1) vTaskDelay(100/portTICK_RATE_MS); //CoTickDelay(100);
}

#endif /* LWIP_NETCONN*/
