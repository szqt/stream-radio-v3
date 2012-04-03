/*
 * external_RAM.h
 *
 *  Created on: 23-03-2012
 *      Author: ja
 */

#ifndef EXTERNAL_RAM_H_
#define EXTERNAL_RAM_H_

#define RAM_CHIPSIZE	32*1024
#define RAM_BUFSIZE		4*RAM_CHIPSIZE		/* External RAM space in Bytes */

/*--- External SRAM instructions ---*/
#define	READ		0x03	/* Read data from memory array beginning at selected address */
#define WRITE		0x02	/* Write data to memory array beginning at selected address */
#define RDSR		0x05	/* Read STATUS register */
#define	WRSR		0x01	/* Write STATUS register  */

/*--- SRAM modes of  operation ---*/
#define BYTE_MODE	0x00	/* Byte mode */
#define	PAGE_MODE	0x80	/* Page mode */
#define SEQ_MODE	0x40	/* Sequential mode */

/*--- HOLD function ---*/
#define	HOLD_OFF	0x01	/* Disable the Hold pin functionality */
#define	HOLD_ON		0x00	/* Enable the Hold pin functionality */

#define DUMMY_BYTE	0x00

/*--- SPI1 ---*/

#define SSPSR_RNE       2
#define SSPSR_BSY       4
#define SSPSR_TNF		1

#define SPI_2MHz		50
#define SPI_10MHz		10
#define SPI_11MHz		9
#define SPI_20MHz		5


/*--- CS_RAM1 pin ---*/
#define CS_RAM1_GPIO				LPC_GPIO1									/* CS_RAM1 PORT */
#define CS_RAM1_BIT					24											/* CS_RAM1 PIN */
#define CS_RAM1_SET_OUTPUT()		CS_RAM1_GPIO->FIODIR |= (1<<CS_RAM1_BIT);	/* CS_RAM1 dir = output */
#define CS_RAM1_LOW()				CS_RAM1_GPIO->FIOPIN &= ~(1<<CS_RAM1_BIT)	/* Set CS_RAM1 low - enable RAM1 chip */
#define CS_RAM1_HIGH()				CS_RAM1_GPIO->FIOPIN |= (1<<CS_RAM1_BIT)	/* Set CS_RAM1 high - disable RAM1 chip */

/*--- CS_RAM2 pin ---*/
#define CS_RAM2_GPIO				LPC_GPIO1									/* CS_RAM2 PORT */
#define CS_RAM2_BIT					23											/* CS_RAM2 PIN */
#define CS_RAM2_SET_OUTPUT()		CS_RAM2_GPIO->FIODIR |= (1<<CS_RAM2_BIT);	/* CS_RAM2 dir = output */
#define CS_RAM2_LOW()				CS_RAM2_GPIO->FIOPIN &= ~(1<<CS_RAM2_BIT)	/* Set CS_RAM2 low - enable RAM2 chip */
#define CS_RAM2_HIGH()				CS_RAM2_GPIO->FIOPIN |= (1<<CS_RAM2_BIT)	/* Set CS_RAM2 high - disable RAM2 chip */

/*--- CS_RAM3 pin ---*/
#define CS_RAM3_GPIO				LPC_GPIO1									/* CS_RAM3 PORT */
#define CS_RAM3_BIT					20											/* CS_RAM3 PIN */
#define CS_RAM3_SET_OUTPUT()		CS_RAM3_GPIO->FIODIR |= (1<<CS_RAM3_BIT);	/* CS_RAM3 dir = output */
#define CS_RAM3_LOW()				CS_RAM3_GPIO->FIOPIN &= ~(1<<CS_RAM3_BIT)	/* Set CS_RAM3 low - enable RAM3 chip */
#define CS_RAM3_HIGH()				CS_RAM3_GPIO->FIOPIN |= (1<<CS_RAM3_BIT)	/* Set CS_RAM3 high - disable RAM3 chip */

/*--- CS_RAM4 pin ---*/
#define CS_RAM4_GPIO				LPC_GPIO1									/* CS_RAM4 PORT */
#define CS_RAM4_BIT					21											/* CS_RAM4 PIN */
#define CS_RAM4_SET_OUTPUT()		CS_RAM4_GPIO->FIODIR |= (1<<CS_RAM4_BIT);	/* CS_RAM4 dir = output */
#define CS_RAM4_LOW()				CS_RAM4_GPIO->FIOPIN &= ~(1<<CS_RAM4_BIT)	/* Set CS_RAM4 low - enable RAM2 chip */
#define CS_RAM4_HIGH()				CS_RAM4_GPIO->FIOPIN |= (1<<CS_RAM4_BIT)	/* Set CS_RAM4 high - disable RAM2 chip */

/*---Function prototypes----*/
void 			SPI1_Config(void);
uint8_t 		SPI1_Write(uint8_t byte_s);
void 			SPI1_FIFO_write(uint8_t byte_s);
void 			SPI1_SetSpeed(uint8_t speed);
void 			RAM_init(void);
void 			RAM_test(void);
void 			RAM_bufputs(char *s, uint16_t len);
void 			RAM_bufget(uint8_t *buf, uint16_t len);
uint32_t 		RAM_buflen(void);
uint32_t 		RAM_buffree(void);

#endif /* EXTERNAL_RAM_H_ */
