/*
 * shoutcast.h
 *
 *  Created on: 03-06-2012
 *      Author: ja
 */

#ifndef SHOUTCAST_H_
#define SHOUTCAST_H_

#define sizeScast				400		//wykorzystuje ok 130 można dać 200
#define prioScast				4

#define BUFF_TRESH				256*1024	// próg ładowania bufora
#define PLAY					1
#define STOP					0

#define STATION_NAME_MAX_LEN	100
#define TITLE_MAX_LEN			100

struct{
	int MetaInt;					/* interwał meta-danych stacji */
	char Name[STATION_NAME_MAX_LEN];	/* pole nazwy stacji */
	char Title[TITLE_MAX_LEN];

}RadioInf;

enum state {MODE1, MODE2, MODE3, MODE4};

void 		shoutcast(void *pdata);
void 		PrintERR(err_t rc);

#endif /* SHOUTCAST_H_ */
