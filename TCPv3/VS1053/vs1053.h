/*
 * vs1053.h
 *
 *  Created on: 24-02-2012
 *      Author: ja
 */

#ifndef VS1053_H_
#define VS1053_H_

/*--- xRST pin ---*/

#define VS_RST_GPIO				LPC_GPIO2							//xRST PORT
#define VS_RST_BIT				9									//xRST PIN
#define VS_RST_SET_OUTPUT()		VS_RST_GPIO	->FIODIR |= (1<<VS_RST_BIT);	//xRST dir = output
#define VS_RST_LOW()			VS_RST_GPIO	->FIOPIN &= ~(1<<VS_RST_BIT)	//Set xRST low - VS reset enable
#define VS_RST_HIGH()			VS_RST_GPIO	->FIOPIN |= (1<<VS_RST_BIT)		//Set xRST high - VS reset disable

/*--- xCS pin ---*/

#define CS_GPIO				LPC_GPIO0							//xCS PORT
#define CS_BIT				30									//xCS PIN
#define CS_SET_OUTPUT()		CS_GPIO->FIODIR |= (1<<CS_BIT);		//xCS dir = output
#define CS_LOW()			CS_GPIO->FIOPIN &= ~(1<<CS_BIT)		//Set xCS low - enable command interface
#define CS_HIGH()			CS_GPIO->FIOPIN |= (1<<CS_BIT)		//Set xCS high - disable command interface

/*--- xDSC pin ---*/

#define DSC_GPIO			LPC_GPIO0								//xDSC PORT
#define DSC_BIT				29										//xDSC PIN
#define DSC_SET_OUTPUT()	DSC_GPIO->FIODIR |= (1<<DSC_BIT)		//xDSC dir = output
#define DSC_LOW()			DSC_GPIO->FIOPIN &= ~(1<<DSC_BIT)		//Set xDSC low - enable data interface
#define DSC_HIGH()			DSC_GPIO->FIOPIN |= (1<<DSC_BIT)		//Set xDSC high - disable data interface

/*--- DREQ pin ---*/

#define DREQ_GPIO			LPC_GPIO0								//DREQ PORT
#define DREQ_BIT			26										//DREQ PIN
#define DREQ_SET_INPUT()	DREQ_GPIO->FIODIR &= ~(1<<DREQ_BIT)		//DREQ dir = input

/*---VS opcodes ---*/

#define VS_READ							0x03
#define VS_WRITE						0x02

/*---VS registers --*/

#define VS_MODE							0x00	//Mode control
#define VS_STATUS						0x01	//Status of VS1053
#define VS_BASS							0x02	//Built-in bass/treble control
#define VS_CLOCKF						0x03	//Clock freq + multiplier
#define VS_DECODETIME					0x04	//Decode time in seconds
#define VS_AUDATA						0x05	//Misc. audio data
#define VS_WRAM							0x06	//RAM write/read
#define VS_WRAMADDR                    	0x07	//Base address for RAM write/read
#define VS_HDAT0                       	0x08	//Stream header data 0
#define VS_HDAT1                       	0x09	//Stream header data 1
#define VS_AIADDR                      	0x0A	//Start address of application
#define VS_VOL                         	0x0B	//Volume control

#define VS_SC_MUL_1X					0x0000
#define VS_SC_MUL_2X					0x2000
#define VS_SC_MUL_2_5X					0x4000
#define VS_SC_MUL_3X					0x6000
#define VS_SC_MUL_3_5X					0x8000
#define VS_SC_MUL_4X					0xA000
#define VS_SC_MUL_4_5X					0xC000
#define VS_SC_MUL_5X					0xE000

#define VS_SC_ADD_0X					0x0000
#define VS_SC_ADD_1X					0x0800
#define VS_SC_ADD_1_5X					0x1000
#define VS_SC_ADD_2X					0x1800

#define SM_RESET						(1<<2)
#define SM_CANCEL						(1<<3)
#define SM_SDINEW						(1<<11)

/*---SPI definitions----*/
#define SSPSR_RNE       2
#define SSPSR_BSY       4
#define SSPSR_TNF		1

#define SPI_2MHz		50
#define SPI_10MHz		10
#define SPI_20MHz		5

/*---Function prototypes----*/

void 							SPI_Config(void);
uint8_t 						spi_write(uint8_t data_out);
void 							SPI0_FIFO_write(uint8_t byte_s);
void 							SPI_SetSpeed(uint8_t speed);
void 							vs_write_reg(uint16_t reg, uint16_t data);
void 							vs_write_reg_NoDREQ(uint16_t reg, uint16_t data);
uint16_t 						vs_read_reg(uint16_t reg);
void 							vs_write_patch(const uint8_t *adr, const uint16_t *data, uint16_t patch_len);
void 							vs_set_volume(uint8_t volume);
void							vs_reset(void);
void 							vs_init(void);
void 							VS_feed(void);


/*---GLOBAL---*/

//extern char VS_BUF[];// __attribute__ ((section ("RamAHB32")));


#endif /* VS1053_H_ */
