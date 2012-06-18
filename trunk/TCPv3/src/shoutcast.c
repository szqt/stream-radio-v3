
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

void disp_menu(char *request_header, char *host_name, uint16_t *port);
void pars_URL(char *URL, char *request_header, char *host_name, uint16_t *port);

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

	char request_header[REQUEST_HEADER_MAX_LEN];
	char host_name[HOST_NAME_MAX_LEN];

	int HeaderLen, DataCounter=0;
	char flaga = 0;
	uint32_t buffered_byte;
	PROGBAR_Handle BuffProgBar;
	enum state eState = MODE1;




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

		disp_menu(request_header, host_name, &portserv);

		printf("Host name: %s\r\n", host_name);
		printf("Port number: %d\r\n", portserv);
//		printf("Reguest: %s\r\n", request_header);

		printf("DNS .....\r\n");
		if((rc1 = netconn_gethostbyname(host_name, &ipaddrserv)) == ERR_OK){
			printf("IP: ");
			printf("%d.",0xff&(ipaddrserv.addr));
			printf("%d.",0xff&(ipaddrserv.addr>>8));
			printf("%d.",0xff&(ipaddrserv.addr>>16));
			printf("%d\r\n",0xff&(ipaddrserv.addr>>24));
		}else{
			PrintERR(rc1);
		}

reconect:

		NetConn = netconn_new(NETCONN_TCP);
		netconn_set_recvtimeout(NetConn, 30000);

		if(NetConn == NULL){
			/*No memory for new connection? */
			printf("No mem for new con\r\n");
		}
		while((rc1 = netconn_bind(NetConn, NULL, 3250)) != ERR_OK){	/* Adres IP i port local host'a */
			printf("Ncon_bind error: ");
			PrintERR(rc1);
			vTaskDelay(10000/portTICK_RATE_MS); 		// wait 10 sec
		}
		printf("netcon binded\r\n");

		rc2 = netconn_connect(NetConn, &ipaddrserv, portserv);		/* Adres IP i port serwera */

		if(rc2 != ERR_OK){
			printf("Ncon_connect error: ");
			PrintERR(rc2);
			LED_On(0);
			netconn_delete(NetConn);
			goto reconect;
		}else{
			rc1 = netconn_write(NetConn, request_header, strlen(request_header), NETCONN_COPY);
			if(rc1 != ERR_OK){
				printf("Ncon_write error: ");
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
					RAM_bufput(mybuf, DataCounter);		/* Wrzuć te dane do bufora VS */
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
					GUI_DispStringHCenterAt("Buff empty - buffering...",160, 80);
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
						if (MetaDataLen > 0){											/* czy ramka nie jest pusta? */
							if((CpBytes - MDLpos - 1) >= MetaDataLen){					/* czy cała ramka została odebrana? */
								ptr = strstr((mybuf+MDLpos + 1), "'");					/* 27 = ascii "'" */
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
										RadioInf.Title[cnt] = '\0';		/* NULL na końcu tytułu */
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

									while(RAM_buffree() < MDLpos){		/* Sprawdź czy jest miejsce w buforze vs */
										vTaskDelay(5/portTICK_RATE_MS);
									}
									RAM_bufput(mybuf, MDLpos);			/* Dane przed meta danymi */

									DataCounter = RadioInf.MetaInt;

									break;
								}else{									/* Niema nawet początku tytułu */

									while(RAM_buffree() < MDLpos){		/* Sprawdź czy jest miejsce w buforze vs */
										vTaskDelay(5/portTICK_RATE_MS);
									}
									RAM_bufput(mybuf, MDLpos);			/* Dane przed meta danymi */

									DataCounter = RadioInf.MetaInt;
									eState = MODE4;						/* Tersć tytułu jeszcze nie pszyszła - zakładamy że w kolejnym pakiecie będzie cały tytuł*/
									break;
								}
							}
						}
						DataCounter=DataCounter-RadioInf.MetaInt-MetaDataLen-1;			/* Ilosć danych audio przed meta-danymi */

						while(RAM_buffree() < MDLpos){		/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufput(mybuf, MDLpos);			/* Dane przed meta danymi */

						while(RAM_buffree() < (CpBytes - MDLpos - MetaDataLen - 1)){	/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufput(mybuf+MDLpos+MetaDataLen+1, (CpBytes - MDLpos - MetaDataLen - 1));		/* Dane po meta danych */
						break;

					case MODE2:
						ptr_tmp = strstr(mybuf, "';");		/* Koniec pola StreamTitle */
						if(ptr_tmp != NULL){
							ptr_tmp = strstr(mybuf, ";");	/* Może jest tylko srednik */
						}
						if(ptr_tmp != NULL){
							ptr = mybuf;
							while(ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
								UART_PrintChar(*ptr);
								RadioInf.Title[cnt] = *ptr;
								ptr++;
								cnt++;
							}
							RadioInf.Title[cnt] = '\0';		/* NULL na końcu tytułu */
							UART_PrintStr("\r\n");

							GUI_SetFont(&GUI_Font10S_ASCII);
							GUI_ClearRect(0, 30, 320, 60);
							GUI_DispStringHCenterAt(RadioInf.Title,160, 45);
						}else{
							/* Niestandarowy format zapisu zarmi danych ? */
							printf("MODE2 - no steram title end\r\n");
						}

						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){	/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufput((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));	/* Dane po metadanych */

						DataCounter = CpBytes - LeftMetaDataFrame;
						eState = MODE1;
						break;
					case MODE3:
						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){	/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufput((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));	/* Dane po metadanych */

						DataCounter = CpBytes - LeftMetaDataFrame;
						eState = MODE1;
						break;
					case MODE4:
						ptr = strstr((mybuf+MDLpos + 1), "'");		/* 27 = ascii "'" */
						if(ptr != NULL){
							ptr_tmp = strstr(ptr+1, "';");			/* koniec pola StreamTitle */
							if(ptr_tmp != NULL){
								cnt = 0;
								while(++ptr != ptr_tmp && cnt < TITLE_MAX_LEN-1){
									UART_PrintChar(*ptr);
									RadioInf.Title[cnt] = *ptr;
									cnt++;
								}
								RadioInf.Title[cnt] = '\0';			/* NULL na końcu tytułu */
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

						while(RAM_buffree() < (CpBytes - LeftMetaDataFrame)){	/* Sprawdź czy jest miejsce w buforze vs */
							vTaskDelay(5/portTICK_RATE_MS);
						}
						RAM_bufput((mybuf + LeftMetaDataFrame), (CpBytes - LeftMetaDataFrame));	/* Dane po metadanych */

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
					RAM_bufput(mybuf, CpBytes);
				}

				if(flaga == STOP){
					buffered_byte = RAM_buflen();
					buffering_progress = (buffered_byte*100)/(BUFF_TRESH);

					if(buffering_progress>100) buffering_progress = 100;

					PROGBAR_SetValue(BuffProgBar, buffering_progress);
					WM_Paint(BuffProgBar);
					if(RAM_buflen()>BUFF_TRESH && flaga == STOP){	/* Bufor jest pełny */
						vTaskResume(xVsTskHandle);					/* Uruchom zadanie dekodera */
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

void disp_menu(char *request_header, char *host_name, uint16_t *port){

	char pls[200];
	char dummy;
	uint8_t cnt;

	while(LPC_UART2->LSR & 0x01){		//wyczysc Rx FIFO
		dummy=LPC_UART2->RBR;
	}
	printf("Wybierz stacje radiowa\r\n");
	printf("a - ZET AAC+ 32bps\r\n");
	printf("b - EuropaFM AAC+ Romiania 32kbps\r\n");
	printf("c - idobi Radio MP3 128kbps\r\n");
	printf("d - RMF MAXXX AAC+ 48kbps\r\n");
	printf("e - Alex Jones - Infowars.com MP3 32kbps\r\n");
	printf("f - TechnoBase.FM MP3 128kbps\r\n");
	printf("g - French Kiss FM MP3 128kbps\r\n");
	printf("h - RMF AAC+ 48bps\r\n");
	printf("i - SKY.FM FM MP3 96kbps\r\n");
	printf("j - 181.FM MP3 128kbps\r\n");
	printf("k - Eska Rock MP3 128kbps\r\n");
	printf("l - Radio PLUS 192kbps\r\n");
	printf("m - Wlasny URL\r\n");

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
		strcpy(pls, string1);
		break;
	case 'b':
		strcpy(pls, string2);
		break;
	case 'c':
		strcpy(pls, string3);
		break;
	case 'd':
		strcpy(pls, string4);
		break;
	case 'e':
		strcpy(pls, string5);
		break;
	case 'f':
		strcpy(pls, string6);
		break;
	case 'g':
		strcpy(pls, string7);
		break;
	case 'h':
		strcpy(pls, string8);
		break;
	case 'i':
		strcpy(pls, string9);
		break;
	case 'j':
		strcpy(pls, string10);
		break;
	case 'k':
		strcpy(pls, string11);
		break;
	case 'l':
		strcpy(pls, string12);
		break;
	case 'm':
		printf("Podaj URL stacji:\r\n");
		scanf("%s", pls);
		printf("Podany URL: %s\r\n", pls);
		break;
	default:
		strcpy(pls, string8);
		break;
	}
	pars_URL(pls, request_header, host_name, port);
}

void pars_URL(char *URL, char *request_header, char *host_name, uint16_t *port){
	char *url_start, *url_end;
	char directory[DIR_MAX_LEN];
	uint8_t cnt;

	/* find host name */
	url_start = strstr(URL, "http");		// http://urladdr.com
	url_start += 7;
	url_end = strstr(url_start, ":");

	cnt = 0;
	do{
		host_name[cnt] = *url_start;
		cnt++;
	}while((++url_start != url_end) && (cnt < HOST_NAME_MAX_LEN-1));
	host_name[cnt] = '\0';

	/* find directory path */
	url_start = strstr(url_end, "/");
	if(url_start == NULL){
		directory[0] = '/';
		directory[1] = 0;
	}else{
		cnt = 0;
		do{
			directory[cnt] = *url_start;
			cnt++;
		}while((++url_start != NULL) && (cnt < DIR_MAX_LEN-1));
		directory[cnt] = '\0';
	}

	/* find port number */
	url_end++;		// pointer to port number
	*port = atoi(url_end);

	/* create request header */
	strcpy(request_header, "GET ");
	strcat(request_header, directory);
	strcat(request_header, " HTTP/1.0\r\nHost: ");
	strcat(request_header, host_name);
	strcat(request_header, "\r\n");
	strcat(request_header, "User-Agent: ");
	strcat(request_header, USER_NAME);
	strcat(request_header, "\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n");
}


#endif /* LWIP_NETCONN*/
