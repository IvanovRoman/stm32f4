/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               xpt2046.c
** Descriptions:            resistive touch screen, compatible ads7843
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             AVRman
** Modified date:           2015-7-2
** Version:                 V1.1
** Descriptions:            add english description
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h"
#include "xpt2046.h"

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2   /* The bigger of the value, the lower the sensitivity */

/**
 * @brief	XPT2046 SPI initialization
 * @param	None
 * @retval None
 */  
void xpt2046_init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* xpt2046  CS */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* xpt2046  IRQ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure); 
	SPI_Cmd(SPI2, ENABLE);
}

/**
 * @brief	delay function
 * @param	cnt: Delay value
 * @retval None
 */
static void delay_nus(int cnt)
{
	int i,us;
	for(i = 0;i<cnt;i++)
	{
		us = 40;
		while (us--)     /* delay	*/
		{
		}
	}
}

/**
 * @brief	SPI bus transmit and receive data
 * @param	cmd: send value
 * @retval return receive data 
 */  
static unsigned char WR_CMD (unsigned char cmd)  
{ 
	/* Wait for SPI Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI data */ 
	SPI_I2S_SendData(SPI2,cmd); 
	/* Wait for SPI data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI received data */ 
	return SPI_I2S_ReceiveData(SPI2); 
} 

/**
 * @brief	read xpt2046 adc value
 * @param	None
 * @retval return adc value 
 */  
static int RD_AD(void)  
{ 
	unsigned short buf,temp; 

	temp = WR_CMD(0x00);
	buf = temp << 8; 
	delay_nus(1); 
	temp = WR_CMD(0x00);;
	buf |= temp; 
	buf >>= 3; 
	buf &= 0xfff; 
	return buf;
}

/**
 * @brief	read xpt2046 channel X+ adc value
 * @param	None
 * @retval return channel X+ adc value
 */  
int Read_X(void)  
{  
	unsigned short temp;
	TP_CS(0); 
	delay_nus(1); 
	WR_CMD(CHX); 
	delay_nus(1); 
	temp = RD_AD(); 
	TP_CS(1); 
	return temp;    
} 

/**
 * @brief	read xpt2046 channel Y+ adc value
 * @param	None
 * @retval return channel Y+ adc value
 */  
int Read_Y(void)  
{  
	unsigned short temp;
	TP_CS(0);
	delay_nus(1);
	WR_CMD(CHY);
	delay_nus(1);
	temp = RD_AD();
	TP_CS(1); 
	return temp;
} 

/**
 * @brief	read xpt2046 channel X+ channel Y+ adc value
 * @param	None
 * @retval return channel X+ channel Y+ adc value
 */  
void TP_GetAdXY(int *x,int *y)  
{ 
	int adx,ady; 
	adx = Read_X(); 
	delay_nus(1); 
	ady = Read_Y(); 
	*x = adx; 
	*y = ady; 
}

/**
 * @brief	get xpt2046 channel X+ channel Y+ adc filtering value, some interference values can be excluded
 * @param	None
 * @retval return channel X+ channel Y+ adc filtered value
 */
Coordinate *Read_Value(void)
{
	static Coordinate screen;
	int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
	unsigned char count=0;
	int buffer[2][9]={{0},{0}};  /* channel X+ Y+ for sampling buffer */
	do                           /* 9 sampling times */
	{		   
		TP_GetAdXY(TP_X,TP_Y);  
		buffer[0][count]=TP_X[0];  
		buffer[1][count]=TP_Y[0];
		count++;  
	}
	while(!TP_INT_IN&& count<9);  /* TP_INT_IN interrupt pin for the touch screen, when the user clicks on the touch screen£¬
	                                 TP_INT_IN Low level */

	if(count==9)   /* sampled 9 times, then filtering some interference values can be excluded*/ 
	{  
		/* average value of the 3 groups */
		temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
		temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
		temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
		/* D-value between the 3 groups */
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		/* absolute value of D-value */
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);

		/* judging Whether the absolute value of D-value is more than threshold, if three absolute value of D-value is 
		   more than threshold value, then determine the sampling point for the interference, ignore the sampling point, 
		   the threshold value is setting to 2, in this example */
		if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
		/* calculate channel X+ average values,then assign them to screen */
		if(m0<m1)
		{
			if(m2<m0) 
			screen.x=(temp[0]+temp[2])/2;
			else 
			screen.x=(temp[0]+temp[1])/2;	
		}
		else if(m2<m1) 
			screen.x=(temp[0]+temp[2])/2;
		else 
			screen.x=(temp[1]+temp[2])/2;

		/* same as above, this is channel Y+ */
		temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
		temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
		temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);
		if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;

		if(m0<m1)
		{
			if(m2<m0) 
				screen.y=(temp[0]+temp[2])/2;
			else 
				screen.y=(temp[0]+temp[1])/2;	
		}
		else if(m2<m1) 
			screen.y=(temp[0]+temp[2])/2;
		else
			screen.y=(temp[1]+temp[2])/2;

		return &screen;
	}  
	return 0; 
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
