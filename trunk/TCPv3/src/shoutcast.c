
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
#include "vs1053.h"

#if LWIP_NETCONN

void shoutcast(void *pdata);
void PrintERR(err_t rc);

#define sizeScast				600		//wykorzystuje ok 130 można dać 200
#define prioScast				4
#define STATION_NAME_MAX_LEN	100
#define TITLE_MAX_LEN			100

extern xTaskHandle xVsTskHandle;
extern xSemaphoreHandle xDhcpCmplSemaphore_1;
extern xSemaphoreHandle xSPI1_Mutex;
xSemaphoreHandle xDMAch0_Semaphore = NULL;
xSemaphoreHandle xDMAch2_Semaphore = NULL;
//xSemaphoreHandle xVS_stopped = NULL;
//__DATA(RAM2) char mybuf[6144];
__SECTION(bss,RAM2) char mybuf[6144];

struct{
	int MetaInt;					/* interwał meta-danych stacji */
	char Name[STATION_NAME_MAX_LEN];	/* pole nazwy stacji */

}RadioInf;
char Title[TITLE_MAX_LEN];\
xTaskHandle xShoutcastTaskHandle;
unsigned char IPradio_init(void)
{
//	xTaskHandle xShoutcastTaskHandle;

	xShoutcastTaskHandle = sys_thread_new("SCstTsk",
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
	u16_t portserv;
	err_t rc1, rc2, rc3, rc4;
	u8_t cnt;
	u16_t CpBytes, MetaDataLen, BufLen, MDLpos;
	char *ptr, *ptr_tmp;
	int HeaderLen, DataCounter=0;
	char flaga = 0, dummy;


	/* ZET AAC+ 32bps port 8050 */
	const char string1[] = "GET / HTTP/1.0\r\nHost: 89.149.227.111\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 89, 149, 227, 111);
//	portserv = 8050;

	/* EuropaFM AAC+ Romiania 32kbps port 7000*/
	const char string2[] = "GET / HTTP/1.0\r\nHost: 89.238.252.146\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 89, 238, 252, 146);
//	portserv = 7000;

	/* idobi Radio MP3 128kbps port 80 */
	const char string3[] = "GET / HTTP/1.0\r\nHost: 50.117.115.211\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 50, 117, 115, 211);
//	portserv = 80;

	/* RMF MAXXX AAC+ 48kbps port 9002 */
	const char string4[] = "GET / HTTP/1.0\r\nHost: 217.74.72.12\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 217, 74, 72, 12);
//	portserv = 9002;

	/* Alex Jones - Infowars.com MP3 32kbps port 80 */
	const char string5[] = "GET / HTTP/1.0\r\nHost: 50.7.241.126\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 50, 7, 241, 126);
//	portserv = 80;

	/* TechnoBase.FM MP3 128kbps port 80 */
	const char string6[] = "GET / HTTP/1.0\r\nHost: 85.17.26.74\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 85, 17, 26, 74);
//	portserv = 80;

	/* French Kiss FM MP3 128kbps port 80 */
	const char string7[] = "GET / HTTP/1.0\r\nHost: 80.190.234.235\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 80, 190, 234, 235);
//	portserv = 80;

	/* RMF AAC+ 48bps port 9000 */
	const char string8[] = "GET / HTTP/1.0\r\nHost: 217.74.72.10\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 217, 74, 72, 10); //port 9000
//	portserv = 9000;

	/* SKY.FM FM MP3 96kbps port 80 */
	const char string9[] = "GET /stream/1010 HTTP/1.0\r\nHost: scfire-dtc-aa04.stream.aol.com\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 207, 200, 96, 231); //
//	portserv = 80;

	/* 181.FM MP3 128kbps port 8002 */
	const char string10[] = "GET /stream/1022 HTTP/1.0\r\nHost: scfire-ntc-aa05.stream.aol.com\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
//	IP4_ADDR(&ipaddrserv, 207, 200, 96, 134);
//	portserv = 80;

	while(LPC_UART2->LSR & 0x01){		//wyczysc Rx FIFO
		dummy=LPC_UART2->RBR;
	}

	UART_PrintStr("a - ZET AAC+ 32bps\r\n");
	UART_PrintStr("b - EuropaFM AAC+ Romiania 32kbps\r\n");
	UART_PrintStr("c - idobi Radio MP3 128kbps\r\n");
	UART_PrintStr("d - RMF MAXXX AAC+ 48kbps\r\n");
	UART_PrintStr("e - Alex Jones - Infowars.com MP3 32kbps\r\n");
	UART_PrintStr("f - TechnoBase.FM MP3 128kbps\r\n");
	UART_PrintStr("g - French Kiss FM MP3 128kbps\r\n");
	UART_PrintStr("h - RMF AAC+ 48bps\r\n");
	UART_PrintStr("i - SKY.FM FM MP3 96kbps\r\n");
	UART_PrintStr("j - 181.FM MP3 128kbps\r\n");
	cnt = 200;
	while(!(LPC_UART2->LSR & 0x01)){			//czeka na znak
		vTaskDelay(100/portTICK_RATE_MS);
		cnt--;
		if(cnt == 0)
			break;							// timeout
	}
	if (cnt != 0){		// nie było timeoutu
		dummy = LPC_UART2->RBR;
	}else
		dummy = 'h';

	switch(dummy){
	case 'a':
		IP4_ADDR(&ipaddrserv, 89, 149, 227, 111);
		portserv = 8050;
		break;
	case 'b':
		IP4_ADDR(&ipaddrserv, 89, 238, 252, 146);
		portserv = 7000;
		break;
	case 'c':
		IP4_ADDR(&ipaddrserv, 50, 117, 115, 211);
		portserv = 80;
		break;
	case 'd':
		IP4_ADDR(&ipaddrserv, 217, 74, 72, 12);
		portserv = 9002;
		break;
	case 'e':
		IP4_ADDR(&ipaddrserv, 50, 7, 241, 126);
		portserv = 80;
		break;
	case 'f':
		IP4_ADDR(&ipaddrserv, 85, 17, 26, 74);
		portserv = 80;
		break;
	case 'g':
		IP4_ADDR(&ipaddrserv, 80, 190, 234, 235);
		portserv = 80;
		break;
	case 'h':
		IP4_ADDR(&ipaddrserv, 217, 74, 72, 10); //port 9000
		portserv = 9000;
		break;
	case 'i':
		IP4_ADDR(&ipaddrserv, 207, 200, 96, 231); //
		portserv = 80;
		break;
	case 'j':
		IP4_ADDR(&ipaddrserv, 207, 200, 96, 134);
		portserv = 80;
		break;
	default:
		IP4_ADDR(&ipaddrserv, 217, 74, 72, 10); //port 9000
		portserv = 9000;
		break;
	}

//	vSemaphoreCreateBinary(xVS_stopped);
//	if(xVS_stopped != NULL){
//		xSemaphoreGive(xVS_stopped);
//	}else{
//		// The semaphore was not created
//	}

	vSemaphoreCreateBinary(xDMAch0_Semaphore);
	if(xDMAch0_Semaphore != NULL){
		xSemaphoreTake(xDMAch0_Semaphore, 0);
	}else{
		// The semaphore was not created
	}

	vSemaphoreCreateBinary(xDMAch2_Semaphore);
	if(xDMAch2_Semaphore != NULL){
		xSemaphoreTake(xDMAch2_Semaphore, 0);
	}else{
		// The semaphore was not created
	}

	if (xSemaphoreTake(xDhcpCmplSemaphore_1, portMAX_DELAY) == pdTRUE) {

reconect:
		NetConn = netconn_new(NETCONN_TCP);
		netconn_set_recvtimeout(NetConn, 30000);

		if(NetConn == NULL){
			/*No memory for new connection? */
			UART_PrintStr("No mem for new con\r\n");
		}
		while((rc1 = netconn_bind(NetConn, NULL, 3250)) != ERR_OK){	/* Adres IP i port local host'a */
			UART_PrintStr("Ncon_bind error: ");
			PrintERR(rc1);
			vTaskDelay(10000/portTICK_RATE_MS); 		// wait 10 sec
		}
		UART_PrintStr("netcon binded\r\n");

		rc2 = netconn_connect(NetConn, &ipaddrserv, portserv);		/* Adres IP i port serwera */

		if(rc2 != ERR_OK){
			UART_PrintStr("Ncon_connect error: ");
			PrintERR(rc2);
			LED_On(0);
			netconn_delete(NetConn);
			goto reconect;
		}else{
			UART_PrintStr("netcon connected\r\n");
			LED_Off(0);

			switch(dummy){
			case 'a':
				rc1 = netconn_write(NetConn, string1, sizeof(string1), NETCONN_NOCOPY);
				break;
			case 'b':
				rc1 = netconn_write(NetConn, string2, sizeof(string2), NETCONN_NOCOPY);
				break;
			case 'c':
				rc1 = netconn_write(NetConn, string3, sizeof(string3), NETCONN_NOCOPY);
				break;
			case 'd':
				rc1 = netconn_write(NetConn, string4, sizeof(string4), NETCONN_NOCOPY);
				break;
			case 'e':
				rc1 = netconn_write(NetConn, string5, sizeof(string5), NETCONN_NOCOPY);
				break;
			case 'f':
				rc1 = netconn_write(NetConn, string6, sizeof(string6), NETCONN_NOCOPY);
				break;
			case 'g':
				rc1 = netconn_write(NetConn, string7, sizeof(string7), NETCONN_NOCOPY);
				break;
			case 'h':
				rc1 = netconn_write(NetConn, string8, sizeof(string8), NETCONN_NOCOPY);
				break;
			case 'i':
				rc1 = netconn_write(NetConn, string9, sizeof(string9), NETCONN_NOCOPY);
				break;
			case 'j':
				rc1 = netconn_write(NetConn, string10, sizeof(string10), NETCONN_NOCOPY);
				break;
			default:
				rc1 = netconn_write(NetConn, string8, sizeof(string8), NETCONN_NOCOPY);
				break;
			}
//			rc1 = netconn_write(NetConn, string, sizeof(string), NETCONN_NOCOPY);
			if(rc1 != ERR_OK){
				UART_PrintStr("Ncon_write error: ");
				PrintERR(rc1);
			}

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
					HeaderLen = ptr - mybuf + 4;			/* Oblicz długosc danych nagłówkoych w odebranej porcji danych */
					DataCounter = CpBytes - HeaderLen;		/* W pakiecie są pierwsze dane strumienia */
					RAM_bufputs(mybuf, DataCounter);		/* Wrzuć te dane do bufora VS */
					break;
				}
			}
			while((rc3 = netconn_recv(NetConn, &inbuf)) == ERR_OK){

				BufLen = netbuf_len(inbuf);
				if(BufLen > 6144){
					UART_PrintStr("ERROR3\r\n");
				}
				CpBytes= netbuf_copy(inbuf, mybuf, BufLen);	/* Skopiuj odebrane dane */
				netbuf_delete(inbuf);						/* Zwolnij bufor */

				if(RAM_buflen() == 0 && flaga == 1){					/* Bufor jest pusty - zatrzymaj zadanie VS */
					UART_PrintStr("BFF EMPTY\r\n");
//					xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY);			/* poczekaj aż VS zwolni SPI1 */
					vTaskSuspend(xVsTskHandle);
//					xSemaphoreGive(xSPI1_Mutex);						/* Oddaj mutex */

//					vs_write_reg(VS_MODE, SM_SDINEW | SM_RESET);

					flaga = 0;
					resetbuff();						/* Resetuj bufor */
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

					while(RAM_buffree() < MDLpos){			/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(5/portTICK_RATE_MS);
					}
					RAM_bufputs(mybuf, MDLpos);		/* Dane przed meta danymi */

					while(RAM_buffree() < (CpBytes - MDLpos - MetaDataLen - 1)){		/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(5/portTICK_RATE_MS);
					}
					RAM_bufputs(mybuf+MDLpos+MetaDataLen+1, (CpBytes - MDLpos - MetaDataLen - 1));		/* Dane po meta danych */
				}
				/* W odebranych danych niema meta-danych */
				else{
					while(RAM_buffree() < CpBytes){			/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(5/portTICK_RATE_MS);
					}
					RAM_bufputs(mybuf, CpBytes);
				}


				if(RAM_buflen()>120*1024 && flaga == 0){	/* Bufor jest pełny */
//					vs_write_reg(VS_MODE, SM_SDINEW | SM_RESET);
					vTaskResume(xVsTskHandle);				/* Uruchom zadanie dekodera */
					flaga=1;								/* Ustaw flagę uruchomienia zadania dekodera */
				}

				LED_Toggle(1);
			}
			UART_PrintStr("Recv error: ");
			PrintERR(rc3);

			LED_On(0);
			rc4=netconn_close(NetConn);
			if(rc4 != ERR_OK){
				UART_PrintStr("Ncon_close error: ");
				PrintERR(rc4);
			}
			UART_PrintStr("netcon closed\r\n");
			netconn_delete(NetConn);
			UART_PrintStr("netcon deleted\r\n");
			vTaskDelay(1000/portTICK_RATE_MS);
			goto reconect;
		}
	}
	while(1) vTaskDelay(100/portTICK_RATE_MS);			/* Tutaj aplikacja nie powinna dojsc */
}

void PrintERR(err_t rc){
	switch(rc){
	case ERR_OK:
		UART_PrintStr("ERR_OK - No error, everything OK.\r\n");
		break;
	case ERR_MEM:
		UART_PrintStr("ERR_MEM - Out of memory error.\r\n");
		break;
	case ERR_BUF:
		UART_PrintStr("ERR_BUF - Buffer error.\r\n");
		break;
	case ERR_TIMEOUT:
		UART_PrintStr("ERR_TIMEOUT - Timeout.\r\n");
		break;
	case ERR_RTE:
		UART_PrintStr("ERR_RTE - Routing problem.\r\n");
		break;
	case ERR_INPROGRESS:
		UART_PrintStr("ERR_INPROGRESS - Operation in progress.\r\n");
		break;
	case ERR_VAL:
		UART_PrintStr("ERR_VAL - Illegal value.\r\n");
		break;
	case ERR_WOULDBLOCK:
		UART_PrintStr("ERR_WOULDBLOCK - Operation would block.\r\n");
		break;
	case ERR_USE:
		UART_PrintStr("ERR_USE - Address in use.\r\n");
		break;
	case ERR_ISCONN:
		UART_PrintStr("ERR_ISCONN - Already connected.\r\n");
		break;
	case ERR_ABRT:
		UART_PrintStr("ERR_ABRT - Connection aborted.\r\n");
		break;
	case ERR_RST:
		UART_PrintStr("ERR_RST - Connection reset.\r\n");
		break;
	case ERR_CLSD:
		UART_PrintStr("ERR_CLSD - Connection closed.\r\n");
		break;
	case ERR_CONN:
		UART_PrintStr("ERR_CONN - Not connected.\r\n");
		break;
	case ERR_ARG:
		UART_PrintStr("ERR_ARG - Illegal argument.\r\n");
		break;
	case ERR_IF:
		UART_PrintStr("ERR_IF - Low-level netif error.\r\n");
		break;
	default:
		break;

	}
}

#endif /* LWIP_NETCONN*/
