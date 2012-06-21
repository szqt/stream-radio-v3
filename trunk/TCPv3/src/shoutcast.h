/*
 * shoutcast.h
 *
 *  Created on: 03-06-2012
 *      Author: ja
 */

#ifndef SHOUTCAST_H_
#define SHOUTCAST_H_

#define sizeScast				500		//wykorzystuje ok 130 można dać 200
#define prioScast				4

#define BUFF_TRESH_3			500*1024	// próg ładowania bufora
#define BUFF_TRESH_2			256*1024
#define BUFF_TRESH_1			128*1024
#define PLAY					1
#define STOP					0

#define STATION_NAME_MAX_LEN	100
#define TITLE_MAX_LEN			100

/* Request header parameters */
#define USER_NAME					"WEBRADIO"
#define REQUEST_HEADER_MAX_LEN		200
#define HOST_NAME_MAX_LEN			100
#define DIR_MAX_LEN					30

enum state {MODE1, MODE2, MODE3, MODE4};
enum BOLEN {TRUE, FALSE};
struct{
	enum BOLEN Metadata;				/* Obecnosc w strumieniu metadanych: TRUE FALSE */
	int MetaInt;						/* interwał meta-danych stacji */
	unsigned short int Bitrate;			/* Bitrate strumienia */
	char Name[STATION_NAME_MAX_LEN];	/* pole nazwy stacji */
	char Title[TITLE_MAX_LEN];

}RadioInf;

void 		shoutcast(void *pdata);
void 		PrintERR(err_t rc);

#endif /* SHOUTCAST_H_ */
