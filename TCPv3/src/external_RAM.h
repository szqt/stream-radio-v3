/*
 * external_RAM.h
 *
 *  Created on: 23-03-2012
 *      Author: ja
 */

#ifndef EXTERNAL_RAM_H_
#define EXTERNAL_RAM_H_

/*--- External serial ram FM25H20 memory space ---*/
#define RAM_CHIPSIZE	256*1024
#define RAM_BUFSIZE		2*RAM_CHIPSIZE		/* External RAM space in Bytes */

/*--- External FRAM instructions ---*/
#define	WREN		0x06	/* Enable write operation (set WEL bit in Status Register) */
#define WRDI		0x04	/* Disable all write activity (clear WEL bit) */
#define RDSR		0x05	/* Read Status Register */
#define	WRSR		0x01	/* Write Status Register  */
#define READ		0x03	/* Read memory data */
#define WRITE		0x02	/* Write memory data */
#define SLEEP		0xB9	/* Enter sleep mode */

/*--- FRAM block memory protection ---*/
#define NO_PROTECT		0x00	/* None */
#define	MODE1_PROTECT	0x04	/* Protect block 0x30000 - 0x3FFFF */
#define MODE2_PROTECT	0x08	/* Protect block 0x20000 - 0x3FFFF */
#define MODE3_PROTECT	0x0C	/* Protect all memory */

/*--- FRAM Write protection pin --*/
#define SET_WPEN	0x80	/* /W pin controls write access to the Status Register */
#define CLR_WPEN	0x00	/* /W pin is ignored */


#define DUMMY_BYTE	0x00

/*--- SPI1 ---*/
#define SSPSR_RNE       2
#define SSPSR_BSY       4
#define SSPSR_TNF		1

#define SPI_2MHz		50
#define SPI_10MHz		10
#define SPI_11MHz		9
#define SPI_16MHz		6
#define SPI_20MHz		5
#define SPI_25MHz		4
#define SPI_33MHz		3


/*--- CS_RAM1 pin ---*/
#define CS_RAM1_GPIO				LPC_GPIO1									/* CS_RAM1 PORT */
#define CS_RAM1_BIT					20											/* CS_RAM1 PIN */
#define CS_RAM1_SET_OUTPUT()		CS_RAM1_GPIO->FIODIR |= (1<<CS_RAM1_BIT);	/* CS_RAM1 dir = output */
#define CS_RAM1_LOW()				CS_RAM1_GPIO->FIOPIN &= ~(1<<CS_RAM1_BIT)	/* Set CS_RAM1 low - enable RAM1 chip */
#define CS_RAM1_HIGH()				CS_RAM1_GPIO->FIOPIN |= (1<<CS_RAM1_BIT)	/* Set CS_RAM1 high - disable RAM1 chip */

/*--- CS_RAM2 pin ---*/
#define CS_RAM2_GPIO				LPC_GPIO1									/* CS_RAM2 PORT */
#define CS_RAM2_BIT					21											/* CS_RAM2 PIN */
#define CS_RAM2_SET_OUTPUT()		CS_RAM2_GPIO->FIODIR |= (1<<CS_RAM2_BIT);	/* CS_RAM2 dir = output */
#define CS_RAM2_LOW()				CS_RAM2_GPIO->FIOPIN &= ~(1<<CS_RAM2_BIT)	/* Set CS_RAM2 low - enable RAM2 chip */
#define CS_RAM2_HIGH()				CS_RAM2_GPIO->FIOPIN |= (1<<CS_RAM2_BIT)	/* Set CS_RAM2 high - disable RAM2 chip */

/*---Function prototypes----*/
void 			SPI1_Config(void);
uint8_t 		SPI1_Write(uint8_t byte_s);
void 			SPI1_FIFO_write(uint8_t byte_s);
void 			SPI1_SetSpeed(uint8_t speed);
void 			RAM_Init(void);
void 			RAM_test(void);
void 			RAM_bufput(char *data_in, uint16_t len_in);
void 			RAM_bufget(uint8_t *buf, uint16_t len);
uint32_t 		RAM_buflen(void);
uint32_t 		RAM_buffree(void);
void 			resetbuff(void);

#endif /* EXTERNAL_RAM_H_ */
