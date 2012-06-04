/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			GLCD.c
** Descriptions:		TFT���������(ģ��IO)
**						
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-1-26
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "HzLib.h"
#include "AsciiLib.h"
#include <math.h>

/* Private variables ---------------------------------------------------------*/
static uint16_t DeviceCode;


/*******************************************************************************
* Function Name  : Lcd_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Configuration(void)
{
  /* Configure the LCD Control pins */

  /* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
  /* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */  
  LPC_GPIO0->FIODIR   |= 0x03f80000;
  LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : LCDд���
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline void LCD_Send (uint16_t byte) 
{
  LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
  LCD_DIR(1)		   				  /* Interface A->B */
  LCD_EN(0)	                          /* Enable 2A->2B */
  LPC_GPIO2->FIOPIN0 =  byte;          /* Write D0..D7 */
  LCD_LE(1)                         
  LCD_LE(0)							  /* latch D0..D7	*/
  LPC_GPIO2->FIOPIN0 =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void wait_delay(int count)
{
  while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : LCD�����
* Input          : - byte: byte to be read
* Output         : None
* Return         : ���ض�ȡ�������
* Attention		 : None
*******************************************************************************/
static __inline uint16_t LCD_Read (void) 
{
  uint16_t value;

  LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
  LCD_DIR(0);		   				           /* Interface B->A */
  LCD_EN(0);	                               /* Enable 2B->2A */
  wait_delay(80);							   /* delay some times */
  value = LPC_GPIO2->FIOPIN0;                   /* Read D8..D15 */
  LCD_EN(1);	                               /* Enable 1B->1A */
  wait_delay(80);							   /* delay some times */
  value = (value << 8) | LPC_GPIO2->FIOPIN0;       /* Read D0..D7 */
  LCD_DIR(1);
  return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCDд�Ĵ�����ַ
* Input          : - index: �Ĵ�����ַ
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline void LCD_WriteIndex(uint16_t index)
{
  LCD_CS(0);
  LCD_RS(0);
  LCD_RD(1);
  LCD_Send( index ); 
  wait_delay(70);	
  LCD_WR(0);  
  wait_delay(1);
  LCD_WR(1);
  LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCDд�Ĵ������
* Input          : - index: �Ĵ������
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline void LCD_WriteData(uint16_t data)
{				
  LCD_CS(0);
  LCD_RS(1);   
  LCD_Send( data );
  LCD_WR(0);     
  wait_delay(1);
  LCD_WR(1);
  LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : ��ȡ���������
* Input          : None
* Output         : None
* Return         : ���ض�ȡ�������
* Attention		 : None
*******************************************************************************/
static __inline uint16_t LCD_ReadData(void)
{ 
  uint16_t value;
  
  LCD_CS(0);
  LCD_RS(1);
  LCD_WR(1);
  LCD_RD(0);
  value = LCD_Read();

  LCD_RD(1);
  LCD_CS(1);

  return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{ 
  /* Write 16-bit Index, then Write Reg */  
  LCD_WriteIndex(LCD_Reg);         
  /* Write 16-bit Reg */
  LCD_WriteData(LCD_RegValue);  
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention		 : None
*******************************************************************************/
static __inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
  uint16_t LCD_RAM;

  /* Write 16-bit Index (then Read Reg) */
  LCD_WriteIndex(LCD_Reg);
  /* Read 16-bit Reg */
  LCD_RAM = LCD_ReadData();      	
  return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
  if(DeviceCode==0x8989)
  {
	LCD_WriteReg(0x004e,Ypos);          	  /* Line */
    LCD_WriteReg(0x004f,0x13f-Xpos);          /* Row */
  }
  else if(DeviceCode==0x9919)
  {
    LCD_WriteReg(0x004e,Ypos);          	  /* Line */
    LCD_WriteReg(0x004f,0x13f-Xpos);          /* Row */	
  }
  else
  {
    LCD_WriteReg(0x0020,Ypos);          	  /* Line */
    LCD_WriteReg(0x0021,0x13f-Xpos);          /* Row */
  }
}

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_Delay(uint16_t nCount)
{
 uint16_t TimingDelay; 
 while(nCount--)
   {
    for(TimingDelay=0;TimingDelay<10000;TimingDelay++);
   }
}

/*******************************************************************************
* Function Name  : LCD_Initializtion
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Initializtion()
{
  LCD_Configuration();
  LCD_Delay(5);  /* delay 50 ms */		
  DeviceCode = LCD_ReadReg(0x0000);		/* ��ȡ��ID	*/
  if(DeviceCode==0x9325 || DeviceCode==0x9328)	/* ��ͬ����IC ��ʼ����ͬ */
  {
    LCD_WriteReg(0x00e7,0x0010);      
    LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
    LCD_WriteReg(0x0001,0x0100);     
    LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
	LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
    LCD_WriteReg(0x0004,0x0000);                                   
    LCD_WriteReg(0x0008,0x0207);	           
    LCD_WriteReg(0x0009,0x0000);         
    LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
    LCD_WriteReg(0x000c,0x0001);	/* display setting */        
    LCD_WriteReg(0x000d,0x0000); 			        
    LCD_WriteReg(0x000f,0x0000);
    /* Power On sequence */
    LCD_WriteReg(0x0010,0x0000);   
    LCD_WriteReg(0x0011,0x0007);
    LCD_WriteReg(0x0012,0x0000);                                                                 
    LCD_WriteReg(0x0013,0x0000);                 
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0010,0x1590);   
    LCD_WriteReg(0x0011,0x0227);
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0012,0x009c);                  
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0013,0x1900);   
    LCD_WriteReg(0x0029,0x0023);
    LCD_WriteReg(0x002b,0x000e);
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0020,0x0000);                                                            
    LCD_WriteReg(0x0021,0x0000);           
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0030,0x0007); 
    LCD_WriteReg(0x0031,0x0707);   
    LCD_WriteReg(0x0032,0x0006);
    LCD_WriteReg(0x0035,0x0704);
    LCD_WriteReg(0x0036,0x1f04); 
    LCD_WriteReg(0x0037,0x0004);
    LCD_WriteReg(0x0038,0x0000);        
    LCD_WriteReg(0x0039,0x0706);     
    LCD_WriteReg(0x003c,0x0701);
    LCD_WriteReg(0x003d,0x000f);
    LCD_Delay(5);  /* delay 50 ms */		
    LCD_WriteReg(0x0050,0x0000);        
    LCD_WriteReg(0x0051,0x00ef);   
    LCD_WriteReg(0x0052,0x0000);     
    LCD_WriteReg(0x0053,0x013f);
    LCD_WriteReg(0x0060,0xa700);        
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006a,0x0000);
    LCD_WriteReg(0x0080,0x0000);
    LCD_WriteReg(0x0081,0x0000);
    LCD_WriteReg(0x0082,0x0000);
    LCD_WriteReg(0x0083,0x0000);
    LCD_WriteReg(0x0084,0x0000);
    LCD_WriteReg(0x0085,0x0000);
      
    LCD_WriteReg(0x0090,0x0010);     
    LCD_WriteReg(0x0092,0x0000);  
    LCD_WriteReg(0x0093,0x0003);
    LCD_WriteReg(0x0095,0x0110);
    LCD_WriteReg(0x0097,0x0000);        
    LCD_WriteReg(0x0098,0x0000);  
    /* display on sequence */    
    LCD_WriteReg(0x0007,0x0133);
    
    LCD_WriteReg(0x0020,0x0000);  /* ����ַ0 */                                                          
    LCD_WriteReg(0x0021,0x0000);  /* ����ַ0 */     
  }
  else if(DeviceCode==0x9320 || DeviceCode==0x9300)
  {
    LCD_WriteReg(0x00,0x0000);
	LCD_WriteReg(0x01,0x0100);	/* Driver Output Contral */
	LCD_WriteReg(0x02,0x0700);	/* LCD Driver Waveform Contral */
	LCD_WriteReg(0x03,0x1018);	/* Entry Mode Set */
	
	LCD_WriteReg(0x04,0x0000);	/* Scalling Contral */
    LCD_WriteReg(0x08,0x0202);	/* Display Contral */
	LCD_WriteReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
	LCD_WriteReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
    LCD_WriteReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
	LCD_WriteReg(0x0d,0x0000);	/* Frame Maker Position */
	LCD_WriteReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */
	
    LCD_Delay(10);  /* delay 100 ms */		
	LCD_WriteReg(0x07,0x0101);	/* Display Contral */
    LCD_Delay(10);  /* delay 100 ms */		

	LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
	LCD_WriteReg(0x11,0x0007);								/* Power Control 2 */
	LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				/* Power Control 3.(0x0138)	*/
	LCD_WriteReg(0x13,0x0b00);								/* Power Control 4 */
	LCD_WriteReg(0x29,0x0000);								/* Power Control 7 */
	
	LCD_WriteReg(0x2b,(1<<14)|(1<<4));
		
	LCD_WriteReg(0x50,0);       /* Set X Start */
	LCD_WriteReg(0x51,239);	    /* Set X End */
	LCD_WriteReg(0x52,0);	    /* Set Y Start */
	LCD_WriteReg(0x53,319);	    /* Set Y End */
	
	LCD_WriteReg(0x60,0x2700);	/* Driver Output Control */
	LCD_WriteReg(0x61,0x0001);	/* Driver Output Control */
	LCD_WriteReg(0x6a,0x0000);	/* Vertical Srcoll Control */
	
	LCD_WriteReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
	LCD_WriteReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
	LCD_WriteReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
	LCD_WriteReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
	LCD_WriteReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
	LCD_WriteReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */
	
    LCD_WriteReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
	LCD_WriteReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
	LCD_WriteReg(0x93,0x0001);	/* Panel Interface Contral 3. */
    LCD_WriteReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
	LCD_WriteReg(0x97,(0<<8));	
	LCD_WriteReg(0x98,0x0000);	/* Frame Cycle Contral */

    LCD_WriteReg(0x07,0x0173);
  }
  else if(DeviceCode==0x9331)
  {
	LCD_WriteReg(0x00E7, 0x1014);
	LCD_WriteReg(0x0001, 0x0100);   /* set SS and SM bit */
	LCD_WriteReg(0x0002, 0x0200);   /* set 1 line inversion */
	LCD_WriteReg(0x0003, 0x1030);   /* set GRAM write direction and BGR=1 */
	LCD_WriteReg(0x0008, 0x0202);   /* set the back porch and front porch */
    LCD_WriteReg(0x0009, 0x0000);   /* set non-display area refresh cycle ISC[3:0] */
	LCD_WriteReg(0x000A, 0x0000);   /* FMARK function */
	LCD_WriteReg(0x000C, 0x0000);   /* RGB interface setting */
	LCD_WriteReg(0x000D, 0x0000);   /* Frame marker Position */
	LCD_WriteReg(0x000F, 0x0000);   /* RGB interface polarity */
	/* Power On sequence */
	LCD_WriteReg(0x0010, 0x0000);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
	LCD_WriteReg(0x0011, 0x0007);   /* DC1[2:0], DC0[2:0], VC[2:0] */
	LCD_WriteReg(0x0012, 0x0000);   /* VREG1OUT voltage	*/
	LCD_WriteReg(0x0013, 0x0000);   /* VDV[4:0] for VCOM amplitude */
    LCD_Delay(20);                  /* delay 200 ms */		
	LCD_WriteReg(0x0010, 0x1690);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
	LCD_WriteReg(0x0011, 0x0227);   /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_Delay(5);                   /* delay 50 ms */		
	LCD_WriteReg(0x0012, 0x000C);   /* Internal reference voltage= Vci	*/
    LCD_Delay(5);                    /* delay 50 ms */		
	LCD_WriteReg(0x0013, 0x0800);   /* Set VDV[4:0] for VCOM amplitude */
	LCD_WriteReg(0x0029, 0x0011);   /* Set VCM[5:0] for VCOMH */
	LCD_WriteReg(0x002B, 0x000B);   /* Set Frame Rate */
    LCD_Delay(5);                   /* delay 50 ms */		
	LCD_WriteReg(0x0020, 0x0000);   /* GRAM horizontal Address */
	LCD_WriteReg(0x0021, 0x0000);   /* GRAM Vertical Address */
	/* Adjust the Gamma Curve */
	LCD_WriteReg(0x0030, 0x0000);
	LCD_WriteReg(0x0031, 0x0106);
	LCD_WriteReg(0x0032, 0x0000);
	LCD_WriteReg(0x0035, 0x0204);
	LCD_WriteReg(0x0036, 0x160A);
	LCD_WriteReg(0x0037, 0x0707);
	LCD_WriteReg(0x0038, 0x0106);
	LCD_WriteReg(0x0039, 0x0707);
	LCD_WriteReg(0x003C, 0x0402);
	LCD_WriteReg(0x003D, 0x0C0F);
	/* Set GRAM area */
	LCD_WriteReg(0x0050, 0x0000);   /* Horizontal GRAM Start Address */
	LCD_WriteReg(0x0051, 0x00EF);   /* Horizontal GRAM End Address */
	LCD_WriteReg(0x0052, 0x0000);   /* Vertical GRAM Start Address */
	LCD_WriteReg(0x0053, 0x013F);   /* Vertical GRAM Start Address */
	LCD_WriteReg(0x0060, 0x2700);   /* Gate Scan Line */
	LCD_WriteReg(0x0061, 0x0001);   /*  NDL,VLE, REV */
	LCD_WriteReg(0x006A, 0x0000);   /* set scrolling line */
	/* Partial Display Control */
	LCD_WriteReg(0x0080, 0x0000);
	LCD_WriteReg(0x0081, 0x0000);
	LCD_WriteReg(0x0082, 0x0000);
	LCD_WriteReg(0x0083, 0x0000);
	LCD_WriteReg(0x0084, 0x0000);
	LCD_WriteReg(0x0085, 0x0000);
	/* Panel Control */
	LCD_WriteReg(0x0090, 0x0010);
	LCD_WriteReg(0x0092, 0x0600);
	LCD_WriteReg(0x0007,0x0021);		
    LCD_Delay(5);                   /* delay 50 ms */		
	LCD_WriteReg(0x0007,0x0061);
    LCD_Delay(5);                   /* delay 50 ms */		
	LCD_WriteReg(0x0007,0x0133);    /* 262K color and display ON */
    LCD_Delay(5);                   /* delay 50 ms */		
  }
  else if(DeviceCode==0x9919)
  {
    /* POWER ON &RESET DISPLAY OFF */
	LCD_WriteReg(0x28,0x0006);
	LCD_WriteReg(0x00,0x0001);		
	LCD_WriteReg(0x10,0x0000);		
	LCD_WriteReg(0x01,0x72ef);
	LCD_WriteReg(0x02,0x0600);
	LCD_WriteReg(0x03,0x6a38);	
	LCD_WriteReg(0x11,0x6874);
	LCD_WriteReg(0x0f,0x0000);    /* RAM WRITE DATA MASK */
	LCD_WriteReg(0x0b,0x5308);    /* RAM WRITE DATA MASK */
	LCD_WriteReg(0x0c,0x0003);
	LCD_WriteReg(0x0d,0x000a);
	LCD_WriteReg(0x0e,0x2e00);  
	LCD_WriteReg(0x1e,0x00be);
	LCD_WriteReg(0x25,0x8000);
	LCD_WriteReg(0x26,0x7800);
	LCD_WriteReg(0x27,0x0078);
	LCD_WriteReg(0x4e,0x0000);
	LCD_WriteReg(0x4f,0x0000);
	LCD_WriteReg(0x12,0x08d9);
	/* Adjust the Gamma Curve */
	LCD_WriteReg(0x30,0x0000);
	LCD_WriteReg(0x31,0x0104);	 
	LCD_WriteReg(0x32,0x0100);	
    LCD_WriteReg(0x33,0x0305);	
    LCD_WriteReg(0x34,0x0505);	 
	LCD_WriteReg(0x35,0x0305);	
    LCD_WriteReg(0x36,0x0707);	
    LCD_WriteReg(0x37,0x0300);	
	LCD_WriteReg(0x3a,0x1200);	
	LCD_WriteReg(0x3b,0x0800);		 
    LCD_WriteReg(0x07,0x0033);
  }
  else if(DeviceCode==0x1505)
  {
    /* second release on 3/5  ,luminance is acceptable,water wave appear during camera preview */
    LCD_WriteReg(0x0007,0x0000);
    LCD_Delay(5);                   /* delay 50 ms */		
    LCD_WriteReg(0x0012,0x011C);    /* why need to set several times?	*/
    LCD_WriteReg(0x00A4,0x0001);    /* NVM */
    LCD_WriteReg(0x0008,0x000F);
    LCD_WriteReg(0x000A,0x0008);
    LCD_WriteReg(0x000D,0x0008);    
    /* GAMMA CONTROL */
    LCD_WriteReg(0x0030,0x0707);
    LCD_WriteReg(0x0031,0x0007); 
    LCD_WriteReg(0x0032,0x0603); 
    LCD_WriteReg(0x0033,0x0700); 
    LCD_WriteReg(0x0034,0x0202); 
    LCD_WriteReg(0x0035,0x0002); 
    LCD_WriteReg(0x0036,0x1F0F);
    LCD_WriteReg(0x0037,0x0707); 
    LCD_WriteReg(0x0038,0x0000); 
    LCD_WriteReg(0x0039,0x0000); 
    LCD_WriteReg(0x003A,0x0707); 
    LCD_WriteReg(0x003B,0x0000); 
    LCD_WriteReg(0x003C,0x0007); 
    LCD_WriteReg(0x003D,0x0000); 
    LCD_Delay(5);                   /* delay 50 ms */		
    LCD_WriteReg(0x0007,0x0001);
    LCD_WriteReg(0x0017,0x0001);    /* Power supply startup enable */
    LCD_Delay(5);                   /* delay 50 ms */		
    /* power control */
    LCD_WriteReg(0x0010,0x17A0); 
    LCD_WriteReg(0x0011,0x0217);    /* reference voltage VC[2:0]   Vciout = 1.00*Vcivl */
    LCD_WriteReg(0x0012,0x011E);    /* Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?	*/
    LCD_WriteReg(0x0013,0x0F00);    /* VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl */
    LCD_WriteReg(0x002A,0x0000);  
    LCD_WriteReg(0x0029,0x000A);    /* Vcomh = VCM1[4:0]*Vreg1out    gate source voltage?? */
    LCD_WriteReg(0x0012,0x013E);    /* power supply on */
    /* Coordinates Control */
    LCD_WriteReg(0x0050,0x0000);
    LCD_WriteReg(0x0051,0x00EF); 
    LCD_WriteReg(0x0052,0x0000); 
    LCD_WriteReg(0x0053,0x013F); 
    /* Pannel Image Control */
    LCD_WriteReg(0x0060,0x2700); 
    LCD_WriteReg(0x0061,0x0001); 
    LCD_WriteReg(0x006A,0x0000); 
    LCD_WriteReg(0x0080,0x0000); 
    /* Partial Image Control */
    LCD_WriteReg(0x0081,0x0000); 
    LCD_WriteReg(0x0082,0x0000); 
    LCD_WriteReg(0x0083,0x0000); 
    LCD_WriteReg(0x0084,0x0000); 
    LCD_WriteReg(0x0085,0x0000); 
    /* Panel Interface Control */
    LCD_WriteReg(0x0090,0x0013);      /* frenqucy */	
    LCD_WriteReg(0x0092,0x0300); 
    LCD_WriteReg(0x0093,0x0005); 
    LCD_WriteReg(0x0095,0x0000); 
    LCD_WriteReg(0x0097,0x0000); 
    LCD_WriteReg(0x0098,0x0000); 
  
    LCD_WriteReg(0x0001,0x0100); 
    LCD_WriteReg(0x0002,0x0700); 
    LCD_WriteReg(0x0003,0x1030); 
    LCD_WriteReg(0x0004,0x0000); 
    LCD_WriteReg(0x000C,0x0000); 
    LCD_WriteReg(0x000F,0x0000); 
    LCD_WriteReg(0x0020,0x0000); 
    LCD_WriteReg(0x0021,0x0000); 
    LCD_WriteReg(0x0007,0x0021); 
    LCD_Delay(20);                   /* delay 200 ms */		
    LCD_WriteReg(0x0007,0x0061); 
    LCD_Delay(20);                   /* delay 200 ms */		
    LCD_WriteReg(0x0007,0x0173); 
    LCD_Delay(20);                   /* delay 200 ms */		
  }							 
  else if(DeviceCode==0x8989)
  {
    LCD_WriteReg(0x0000,0x0001);    LCD_Delay(5);   /* �򿪾��� */
    LCD_WriteReg(0x0003,0xA8A4);    LCD_Delay(5);   
    LCD_WriteReg(0x000C,0x0000);    LCD_Delay(5);   
    LCD_WriteReg(0x000D,0x080C);    LCD_Delay(5);   
    LCD_WriteReg(0x000E,0x2B00);    LCD_Delay(5);   
    LCD_WriteReg(0x001E,0x00B0);    LCD_Delay(5);   
    LCD_WriteReg(0x0001,0x2B3F);    LCD_Delay(5);   /* ���������320*240 0x2B3F */
    LCD_WriteReg(0x0002,0x0600);    LCD_Delay(5);
    LCD_WriteReg(0x0010,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0011,0x6070);    LCD_Delay(5);   /* ������ݸ�ʽ 16λɫ ���� 0x6070 */
    LCD_WriteReg(0x0005,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0006,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0016,0xEF1C);    LCD_Delay(5);
    LCD_WriteReg(0x0017,0x0003);    LCD_Delay(5);
    LCD_WriteReg(0x0007,0x0133);    LCD_Delay(5);         
    LCD_WriteReg(0x000B,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x000F,0x0000);    LCD_Delay(5);   /* ɨ�迪ʼ��ַ */
    LCD_WriteReg(0x0041,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0042,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0048,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0049,0x013F);    LCD_Delay(5);
    LCD_WriteReg(0x004A,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x004B,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0044,0xEF00);    LCD_Delay(5);
    LCD_WriteReg(0x0045,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0046,0x013F);    LCD_Delay(5);
    LCD_WriteReg(0x0030,0x0707);    LCD_Delay(5);
    LCD_WriteReg(0x0031,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0032,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0033,0x0502);    LCD_Delay(5);
    LCD_WriteReg(0x0034,0x0507);    LCD_Delay(5);
    LCD_WriteReg(0x0035,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0036,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0037,0x0502);    LCD_Delay(5);
    LCD_WriteReg(0x003A,0x0302);    LCD_Delay(5);
    LCD_WriteReg(0x003B,0x0302);    LCD_Delay(5);
    LCD_WriteReg(0x0023,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0024,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0025,0x8000);    LCD_Delay(5);
    LCD_WriteReg(0x004f,0);        /* ����ַ0 */
    LCD_WriteReg(0x004e,0);        /* ����ַ0 */
  }
  LCD_Delay(5);  /* delay 50 ms */		
}

/******************************************************************************
* Function Name  : LCD_SetWindows
* Description    : Sets Windows Area.
* Input          : - StartX: Row Start Coordinate 
*                  - StartY: Line Start Coordinate  
*				   - xLong:  
*				   - yLong: 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetWindows(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong)
{
  LCD_SetCursor(xStart,yStart); 
  LCD_WriteReg(0x0050,xStart);         /* ˮƽGRAM��ʼλ�� */
  LCD_WriteReg(0x0051,xStart+xLong-1); /* ˮƽGRAM��ֹλ�� */
  LCD_WriteReg(0x0052,yStart);         /* ��ֱGRAM��ʼλ�� */
  LCD_WriteReg(0x0053,yStart+yLong-1); /* ��ֱGRAM��ֹλ�� */ 
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : ����Ļ����ָ������ɫ��������������� 0xffff
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
  uint32_t index=0;
  LCD_SetCursor(0,0); 
  LCD_WriteIndex(0x0022);
  for(index=0;index<76800;index++)
  {
     LCD_WriteData(Color);
  }
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : ��ȡָ��������ɫֵ
* Input          : - Xpos: Row Coordinate
*                  - Xpos: Line Coordinate 
* Output         : None
* Return         : Screen Color
* Attention		 : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
  uint16_t dummy;

  LCD_SetCursor(Xpos,Ypos);
  LCD_WriteIndex(0x0022); 
  dummy = LCD_ReadData();   /* �ն�һ�� */
  dummy = LCD_ReadData(); 

  if( DeviceCode==0x7783 || DeviceCode==0x4531 || DeviceCode==0x8989 )
    return dummy;
  else
    return LCD_BGR2RGB( dummy );
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : ��ָ����껭��
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
  LCD_SetCursor(Xpos,Ypos);
  LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : ��ָ����껭ֱ��
* Input          : - x0: A�������
*                  - y0: A������� 
*				   - x1: B�������
*				   - y1: B������� 
*				   - color: ����ɫ
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
 	uint16_t x,y;
 	uint16_t dx;      /* abs(x1 - x0) */
 	uint16_t dy;      /* abs(y1 - y0) */

	if(y0==y1)
	{
		if(x0<=x1)
		{
			x=x0;
		}
		else
		{
			x=x1;
			x1=x0;
		}
  		while(x <= x1)
  		{
   			LCD_SetPoint(x,y0,color);
   			x++;
  		}
  		return;
	}
	else if(y0>y1)
	{
		dy=y0-y1;
	}
	else
	{
		dy=y1-y0;
	}
 
 	if(x0==x1)
	{
		if(y0<=y1)
		{
			y=y0;
		}
		else
		{
			y=y1;
			y1=y0;
		}
  		while(y <= y1)
  		{
   			LCD_SetPoint(x0,y,color);
   			y++;
  		}
  		return;
	}
	else if(x0 > x1)
 	{
		dx=x0-x1;
  		x = x1;
  		x1 = x0;
  		y = y1;
  		y1 = y0;
 	}
 	else
 	{
		dx=x1-x0;
  		x = x0;
  		y = y0;
 	}

 	if(dx == dy)
 	{
  		while(x <= x1)
  		{

   			x++;
			if(y>y1)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			LCD_SetPoint(x,y,color);
  		}
 	}
 	else
 	{
 		LCD_SetPoint(x, y, color);
  		if(y < y1)
  		{
   			if(dx > dy)
   			{
    			int16_t p = dy * 2 - dx;
    			int16_t twoDy = 2 * dy;
    			int16_t twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				LCD_SetPoint(x, y,color);
    			}
   			}
   			else
   			{
    			int16_t p = dx * 2 - dy;
    			int16_t twoDx = 2 * dx;
    			int16_t twoDxMinusDy = 2 * (dx - dy);
    			while(y < y1)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				LCD_SetPoint(x, y, color);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			int16_t p = dy * 2 - dx;
    			int16_t twoDy = 2 * dy;
	    		int16_t twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				LCD_SetPoint(x, y,color);
    			}
   			}
	   		else
   			{
    			int16_t p = dx * 2 - dy;
    			int16_t twoDx = 2 * dx;
	    		int16_t twoDxMinusDy = 2 * (dx - dy);
    			while(y1 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				LCD_SetPoint(x, y,color);
    			}
   			}
  		}
 	}
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
* Input          : - color: BRG ��ɫֵ  
* Output         : None
* Return         : RGB ��ɫֵ
* Attention		 : �ڲ��������
*******************************************************************************/
uint16_t LCD_BGR2RGB(uint16_t color)
{
  uint16_t  r, g, b, rgb;

  b = ( color>>0 )  & 0x1f;
  g = ( color>>5 )  & 0x3f;
  r = ( color>>11 ) & 0x1f;
 
  rgb =  (b<<11) + (g<<5) + (r<<0);

  return( rgb );
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

