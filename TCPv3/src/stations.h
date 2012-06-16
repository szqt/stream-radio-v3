/*
 * stations.h
 *
 *  Created on: 03-06-2012
 *      Author: ja
 */

#ifndef STATIONS_H_
#define STATIONS_H_

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

	const char string8p1[] = "GET / HTTP/1.0\r\n";
	const char string8p2[] = "Host: 217.74.72.10\r\n";
	const char string8p3[] = "User-Agent: WEBRADIO\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
	/* RMF AAC+ 48bps port 9000 */
	const char string8[] = "GET / HTTP/1.0\r\nHost: 217.74.72.10\r\nUser-Agent: WEBRADIO\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
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

	/* Eska Rock 128kbps port 8000 */
	const char string11[] = "GET /pl/eskarock.mp3 HTTP/1.0\r\nHost: wroclaw.radio.pionier.net.pl\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";

	/* Radio Plus 192kbps port 5000 */
	const char string12[] = "GET / HTTP/1.0\r\nHost: stream.pulsradio.com\r\nUser-Agent: WinampMPEG/5.62, Ultravox/2.1\r\nUltravox-transport-type: TCP\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n";
#endif /* STATIONS_H_ */
