
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

//#include "GLCD.h"
#include "GUI.h"
#include "PROGBAR.h"

#include "shoutcast.h"
#include "stations.h"

#if LWIP_NETCONN

extern xTaskHandle xVsTskHandle;
extern xSemaphoreHandle xDhcpCmplSemaphore_1;
extern xSemaphoreHandle xSPI1_Mutex;
xSemaphoreHandle xDMAch0_Semaphore = NULL;
xSemaphoreHandle xDMAch2_Semaphore = NULL;

//__DATA(RAM2) char mybuf[6144];
__SECTION(bss,RAM2) char mybuf[6144];

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
	u8_t cnt, buffering_progress;
	u16_t CpBytes, MetaDataLen, BufLen, MDLpos, LeftMetaDataFrame;
	char *ptr, *ptr_tmp;
	int HeaderLen, DataCounter=0;
	char flaga = 0, dummy;
	uint32_t buffered_byte;
	PROGBAR_Handle BuffProgBar;
	enum state eState = MODE1;





	while(LPC_UART2->LSR & 0x01){		//wyczysc Rx FIFO
		dummy=LPC_UART2->RBR;
	}
	printf("Wybierz stację radiową\r\n");
	UART_PrintStr("a - ZET AAC+ 32bps\r\n");
	UART_PrintStr("b - EuropaFM AAC+ Romiania 32kbps\r\n");
	UART_PrintStr("c - idobi Radio MP3 128kbps\r\n");
	UART_PrintStr("d - RMF MAXXX AAC+ 48kbps\r\n");
	UART_PrintStr("e - Alex Jones - Infowars.com MP3 32kbps\r\n");
	UART_PrintStr("f - TechnoBase.FM MP3 128kbps\r\n");
	UART_PrintStr("g - RMF AAC+ 48bps - test\r\n");
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
		IP4_ADDR(&ipaddrserv, 192, 168, 195, 87);	// rmf test
		portserv = 9000;
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
	case 'k':

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
			case 'g':		// rmf test
				rc1 = netconn_write(NetConn, string8p1, sizeof(string8p1)-1, NETCONN_COPY);
				rc1 = netconn_write(NetConn, string8p2, sizeof(string8p2)-1, NETCONN_COPY);
				rc1 = netconn_write(NetConn, string8p3, sizeof(string8p3)-1, NETCONN_COPY);
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
			vTaskDelay(4000/portTICK_RATE_MS);

			while((netconn_recv(NetConn, &inbuf)) == ERR_OK){
				BufLen = netbuf_len(inbuf);
				CpBytes = netbuf_copy(inbuf, mybuf, BufLen);
				netbuf_delete(inbuf);

				/* Znajdź kod statusu żądania HTTP */
				ptr = strstr(mybuf, "ICY");
				ptr_tmp = strstr(mybuf, "\r\n");
				if((ptr != NULL) && (ptr_tmp != NULL)){
					do{
						UART_PrintChar(*ptr);
					}while(ptr++ != ptr_tmp);
					UART_PrintStr("\r\n");
				}
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
					GUI_SetFont(&GUI_Font16B_ASCII);
					GUI_DispStringHCenterAt(RadioInf.Name,160, 15);
					//					GLCD_DisplayString(0,0,RadioInf.Name);	/* Title on LCD */
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
			/* Utwórz pasek postępu */
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font8x16);
			GUI_DispStringHCenterAt("Buffering...",160, 80);
			BuffProgBar = PROGBAR_Create(110, 100, 100, 20, WM_CF_SHOW);
			PROGBAR_SetValue(BuffProgBar, 0);
			WM_Paint(BuffProgBar);

			while((rc3 = netconn_recv(NetConn, &inbuf)) == ERR_OK){

				BufLen = netbuf_len(inbuf);
				if(BufLen > 6144){
					printf("ERROR3\r\n");
				}
				CpBytes= netbuf_copy(inbuf, mybuf, BufLen);	/* Skopiuj odebrane dane */
				netbuf_delete(inbuf);						/* Zwolnij bufor */

				if(RAM_buflen() < 32 && flaga == PLAY){					/* Bufor jest pusty - zatrzymaj zadanie VS */
					printf("BUFF EMPTY\r\n");
//					xSemaphoreTake(xSPI1_Mutex, portMAX_DELAY);			/* poczekaj aż VS zwolni SPI1 */
					vTaskSuspend(xVsTskHandle);
//					xSemaphoreGive(xSPI1_Mutex);						/* Oddaj mutex */
					flaga = STOP;
					resetbuff();						/* Resetuj bufor */

					/* Utwórz pasek postępu */
					GUI_SetFont(&GUI_Font8x16);
					GUI_DispStringHCenterAt("Buffering...",160, 80);
					BuffProgBar = PROGBAR_Create(110, 100, 100, 20, WM_CF_SHOW);
					PROGBAR_SetBarColor(BuffProgBar, 0, GUI_DARKGRAY);
					PROGBAR_SetBarColor(BuffProgBar, 1, GUI_LIGHTGRAY);
					PROGBAR_SetValue(BuffProgBar, 0);
					WM_Paint(BuffProgBar);
				}

				DataCounter += CpBytes;

				/* W odebranych danych są meta-dane */
				if(DataCounter > RadioInf.MetaInt){
					switch(eState){
					case MODE1:
						MDLpos = CpBytes-(DataCounter-RadioInf.MetaInt);				/* Pozycja MetaDataLength */
						MetaDataLen = 16*mybuf[CpBytes-(DataCounter-RadioInf.MetaInt)];	/* Długosć MetaDanych */
						if (MetaDataLen > 0){							/* czy ramka nie jest pusta? */
							if((CpBytes - MDLpos - 1) >= MetaDataLen){	/* czy cała ramka została odebrana? */
								ptr = strstr((mybuf+MDLpos + 1), "'");		/* 27 = ascii "'" */
								if(ptr != NULL){
									ptr_tmp = strstr(ptr+1, "';");				/* koniec pola StreamTitle */
									if(ptr_tmp != NULL){
										cnt = 0;
										while(++ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
											UART_PrintChar(*ptr);
											RadioInf.Title[cnt] = *ptr;
											cnt++;
										}
										RadioInf.Title[cnt] = '\0';				/* NULL na końcu tytułu */
										UART_PrintStr("\r\n");

										GUI_SetFont(&GUI_Font10S_ASCII);
										GUI_ClearRect(0, 38, 360, 52);
										GUI_DispStringHCenterAt(RadioInf.Title,160, 45);
									}else{
										/* Niestandarowy format zapisu zarmi danych ? */
										printf("MODE1 - no steream title end\r\n");
									}
								}else{
									/* Niestandarowy format zapisu zarmi danych ? */
									printf("MODE1 - no stream title start\r\n");
								}
							}else{										/* Odebrany został tylko kawałek ramki */
								LeftMetaDataFrame = MetaDataLen - (CpBytes - MDLpos - 1);	/* Jeszcze brakuje tyle bajtów ramki */
								printf("CpBytes = %d\r\n", CpBytes);
								printf("MDLpos = %d\r\n", MDLpos);
								printf("MetaDataLen = %d\r\n", MetaDataLen);
								printf("LeftMetaDataFrame = %d\r\n", LeftMetaDataFrame);
								ptr = strstr((mybuf+MDLpos + 1), "'");	/* 27 = ascii "'" */
								if(ptr != NULL){						/* Jest początek tytułu */
									ptr_tmp = strstr(ptr+1, "';");		/* Jest koniec */
									if(ptr_tmp != NULL){
										cnt = 0;
										while(++ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
											UART_PrintChar(*ptr);
											RadioInf.Title[cnt] = *ptr;
											cnt++;
										}
										RadioInf.Title[cnt] = '\0';				/* NULL na końcu tytułu */
										UART_PrintStr("\r\n");
										GUI_SetFont(&GUI_Font10S_ASCII);
										GUI_ClearRect(0, 30, 320, 60);
										GUI_DispStringHCenterAt(RadioInf.Title,160, 45);
										eState = MODE3;					/* Przyją jeszcze zera wupełniajce ramkę metadyanych */
									}else{
										cnt = 0;
										while(ptr++ != &mybuf[CpBytes-1] && cnt < TITLE_MAX_LEN-1){
											UART_PrintChar(*ptr);
											RadioInf.Title[cnt] = *ptr;
											cnt++;
										}
										eState = MODE2;					/* Przyjdzie jeszcze reszta tytułu */
									}

									while(RAM_buffree() < MDLpos){			/* Sprawdź czy jest miejsce w buforze vs */
										vTaskDelay(5/portTICK_RATE_MS);
									}
									RAM_bufputs(mybuf, MDLpos);		/* Dane przed meta danymi */

									DataCounter = RadioInf.MetaInt;

									break;
								}else{									/* Niema nawet początku tytułu */

									while(RAM_buffree() < MDLpos){			/* Sprawdź czy jest miejsce w buforze vs */
										vTaskDelay(5/portTICK_RATE_MS);
									}
									RAM_bufputs(mybuf, MDLpos);		/* Dane przed meta danymi */

									DataCounter = RadioInf.MetaInt;
									eState = MODE4;
									break;
								}
							}
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
						break;

					case MODE2:
						ptr_tmp = strstr(mybuf, "';");			/* Koniec pola StreamTitle */
						if(ptr_tmp != NULL){
							ptr_tmp = strstr(mybuf, ";");		/* Może jest tylko srednik */
						}
						if(ptr_tmp != NULL){
							ptr = mybuf;
							while(ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
								UART_PrintChar(*ptr);
								RadioInf.Title[cnt] = *ptr;
								ptr++;
								cnt++;
							}
							RadioInf.Title[cnt] = '\0';				/* NULL na końcu tytułu */
							UART_PrintStr("\r\n");

							GUI_SetFont(&GUI_Font10S_ASCII);
							GUI_ClearRect(0, 30, 320, 60);
							GUI_DispStringHCenterAt(RadioInf.Title,160, 45);
						}else{
							/* Niestandarowy format zapisu zarmi danych ? */
							printf("MODE2 - no steram title end\r\n");
						}

						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){			/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufputs((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));		/* Dane po metadanych */

						DataCounter = CpBytes - LeftMetaDataFrame;
						eState = MODE1;
						break;
					case MODE3:
						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){			/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufputs((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));		/* Dane po metadanych */

						DataCounter = CpBytes - LeftMetaDataFrame;
						eState = MODE1;
						break;
					case MODE4:
						ptr = strstr((mybuf+MDLpos + 1), "'");		/* 27 = ascii "'" */
						if(ptr != NULL){
							ptr_tmp = strstr(ptr+1, "';");				/* koniec pola StreamTitle */
							if(ptr_tmp != NULL){
								cnt = 0;
								while(++ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
									UART_PrintChar(*ptr);
									RadioInf.Title[cnt] = *ptr;
									cnt++;
								}
								RadioInf.Title[cnt] = '\0';				/* NULL na końcu tytułu */
								UART_PrintStr("\r\n");

								GUI_SetFont(&GUI_Font10S_ASCII);
								GUI_ClearRect(0, 30, 320, 60);
								GUI_DispStringHCenterAt(RadioInf.Title,160, 45);
							}else{
								/* Niestandarowy format zapisu zarmi danych ? */
							}
						}else{
							/* Niestandarowy format zapisu zarmi danych ? */
						}

						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){			/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufputs((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));		/* Dane po metadanych */

						DataCounter = CpBytes - LeftMetaDataFrame;
						eState = MODE1;
						break;
					default:
						break;
					}
				}
				/* W odebranych danych niema meta-danych */
				else{
					while(RAM_buffree() < CpBytes){			/* Sprawdź czy jest miejsce w buforze vs */
						vTaskDelay(5/portTICK_RATE_MS);
					}
					RAM_bufputs(mybuf, CpBytes);
				}

				if(flaga == STOP){
					buffered_byte = RAM_buflen();
					buffering_progress = (buffered_byte*100)/(BUFF_TRESH);

					if(buffering_progress>100) buffering_progress = 100;

					PROGBAR_SetValue(BuffProgBar, buffering_progress);
					WM_Paint(BuffProgBar);
					if(RAM_buflen()>BUFF_TRESH && flaga == STOP){	/* Bufor jest pełny */
						vTaskResume(xVsTskHandle);				/* Uruchom zadanie dekodera */
						flaga = PLAY;								/* Ustaw flagę uruchomienia zadania dekodera */
						/*Usuń pasek postępu */
						PROGBAR_Delete(BuffProgBar);
						GUI_ClearRect(0, 80, 320, 120);
					}
				}

				LED_Toggle(1);
			}
			printf("Recv error: ");
			PrintERR(rc3);

			LED_On(0);
			rc4=netconn_close(NetConn);
			if(rc4 != ERR_OK){
				printf("Ncon_close error: ");
				PrintERR(rc4);
			}
			printf("netcon closed\r\n");
			netconn_delete(NetConn);
			printf("netcon deleted\r\n");
			vTaskDelay(1000/portTICK_RATE_MS);
			goto reconect;
		}
	}
	while(1) vTaskDelay(100/portTICK_RATE_MS);			/* Tutaj aplikacja nie powinna dojsc */
}

void PrintERR(err_t rc){
	switch(rc){
	case ERR_OK:
		printf("ERR_OK - No error, everything OK.\r\n");
		break;
	case ERR_MEM:
		printf("ERR_MEM - Out of memory error.\r\n");
		break;
	case ERR_BUF:
		printf("ERR_BUF - Buffer error.\r\n");
		break;
	case ERR_TIMEOUT:
		printf("ERR_TIMEOUT - Timeout.\r\n");
		break;
	case ERR_RTE:
		printf("ERR_RTE - Routing problem.\r\n");
		break;
	case ERR_INPROGRESS:
		printf("ERR_INPROGRESS - Operation in progress.\r\n");
		break;
	case ERR_VAL:
		printf("ERR_VAL - Illegal value.\r\n");
		break;
	case ERR_WOULDBLOCK:
		printf("ERR_WOULDBLOCK - Operation would block.\r\n");
		break;
	case ERR_USE:
		printf("ERR_USE - Address in use.\r\n");
		break;
	case ERR_ISCONN:
		printf("ERR_ISCONN - Already connected.\r\n");
		break;
	case ERR_ABRT:
		printf("ERR_ABRT - Connection aborted.\r\n");
		break;
	case ERR_RST:
		printf("ERR_RST - Connection reset.\r\n");  /* Server is refuse to be connect */
		break;
	case ERR_CLSD:
		printf("ERR_CLSD - Connection closed.\r\n");
		break;
	case ERR_CONN:
		printf("ERR_CONN - Not connected.\r\n");
		break;
	case ERR_ARG:
		printf("ERR_ARG - Illegal argument.\r\n");
		break;
	case ERR_IF:
		printf("ERR_IF - Low-level netif error.\r\n");
		break;
	default:
		break;

	}
}

#endif /* LWIP_NETCONN*/
