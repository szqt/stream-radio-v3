#include <string.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "LPC17xx.h"
#include "gpio.h"
#include "uart.h"

#if LWIP_NETCONN

void http_server(void *pdata);
void http_server_serve(struct netconn *conn);
extern void PrintERR(err_t rc);

extern char Title[];

const static char http_html_hdr[] =
		"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char
		http_index_html[] =
				"<html><head><title>IPRadio</title></head><body><h1>RADIO</h1><p>Teraz gra:<br>";
const static char
		http_index_html_2[] =
				"<form name=\"aForm\" action=\"http://192.168.196.124/\" method=\"get\"><select name=\"nazwa\"><option>OPT1</option><option>OPT2</option></select><input type=\"submit\" value=\"Update IO\"></form><br></body></html>";

#define sizeHttp  400
#define prioHttp  4

extern xSemaphoreHandle xDhcpCmplSemaphore_2;

unsigned char HTTP_init(void) {
	xTaskHandle xHttpTaskHandle = NULL;

	xHttpTaskHandle = sys_thread_new("HttpTsk", http_server, NULL, sizeHttp, prioHttp);

//	xTaskCreate( http_server, ( signed portCHAR * ) "HttpTsk", 400, NULL, 5, &xHttpTaskHandle );

	if (xHttpTaskHandle == SYS_THREAD_NULL) {
		return 1;
	}

	return 0;
}

void http_server_serve(struct netconn *conn) {
	struct netbuf *inbuf;
	char *buf, *ptr;
	u16_t buflen;

	/* Read the data from the port, blocking if nothing yet there.
	 We assume the request (the part we care about) is in one netbuf */
	if (netconn_recv(conn, &inbuf) == ERR_OK) {
		netbuf_data(inbuf, (void **) &buf, &buflen);

		/* Is this an HTTP GET command? (only check the first 5 chars, since
		 there are other formats for GET, and we're keeping it very simple )*/
		if (buflen >= 17 && (ptr = strstr(buf, "/?nazwa=OPT")) != NULL){
			if(*(ptr+11) == '1')
				LED_On(4);
			else
				LED_Off(4);

			netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);

			/* Send our HTML page */
			netconn_write(conn, http_index_html, sizeof(http_index_html) - 1, NETCONN_NOCOPY);
			netconn_write(conn, Title, strlen(Title), NETCONN_COPY);
			netconn_write(conn, http_index_html_2, sizeof(http_index_html_2) - 1, NETCONN_NOCOPY);

		}else if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T'
				&& buf[3] == ' ' && buf[4] == '/') {

			/* Send the HTML header
			 * subtract 1 from the size, since we dont send the \0 in the string
			 * NETCONN_NOCOPY: our data is const static, so no need to copy it
			 */
			netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);

			/* Send our HTML page */
			netconn_write(conn, http_index_html, sizeof(http_index_html) - 1, NETCONN_NOCOPY);
			netconn_write(conn, Title, strlen(Title), NETCONN_COPY);
			netconn_write(conn, http_index_html_2, sizeof(http_index_html_2) - 1, NETCONN_NOCOPY);
		}
	}
	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership,
	 so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);
}

void http_server(void *pdata) {
	struct netconn *HTTPNetConn = NULL, *xHTTPNetConn;
	err_t rc1;
	if (xSemaphoreTake(xDhcpCmplSemaphore_2, portMAX_DELAY) == pdTRUE) {

		/* Create a new TCP connection handle */
		HTTPNetConn = netconn_new(NETCONN_TCP);
//		netconn_set_recvtimeout(HTTPNetConn, 100);
		if (HTTPNetConn == NULL) {
			/*No memory for new connection? */
			UART_PrintStr("No mem for new HTTP con\r\n");
		}
		/* Bind to port 80 (HTTP) with default IP address */
		netconn_bind(HTTPNetConn, NULL, 80);

		/* Put the connection into LISTEN state */
		netconn_listen(HTTPNetConn);

		while (1) {
			rc1 = netconn_accept(HTTPNetConn, &xHTTPNetConn);
			if(rc1 == ERR_OK){
//				http_server_serve(xHTTPNetConn);
				UART_PrintStr("netconn accept\r\n");
				netconn_close(xHTTPNetConn);
				netconn_delete(xHTTPNetConn);
			}else{
				PrintERR(rc1);
			}
		}
	}
	while (1)
		vTaskDelay(1000/portTICK_RATE_MS);
}

#endif /* LWIP_NETCONN*/
