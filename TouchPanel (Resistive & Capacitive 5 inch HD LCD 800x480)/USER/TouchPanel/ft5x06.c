/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ft5x06.c
** Descriptions:            ft5x06 application program, support ft5206, ft5306, ft5406, ft5506
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-7-2
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "ft5x06.h"
#include "ts_calibrate.h"

/* Private variables ---------------------------------------------------------*/
struct event ts_event;
int touch_type;

/**
 * @brief	delay function
 * @param	cnt: delay value
 * @retval None
 */
static void delay_nus(uint32_t cnt)
{
	int i,us;
	for(i = 0;i<cnt;i++)
	{
		us = 200;
		while (us--)     /* delay	*/
		{
		}
	}
}

/**
  * @brief  ft5x06 initialization
  * @param  None
  * @retval None
  */
static void ft5x06_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOH, ENABLE);

	/* PH7 --- I2C_SCL, PH8 --- I2C_SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			       
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* PB10 --- CTP_INT */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	FT5X06_SCL_H();
	FT5X06_SDA_H();

	/* PH6 --- CTP_INT*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* Configure priority and enable interrupt */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	FT5X06_RESET_L();
	delay_nus(10000);
	FT5X06_RESET_H();
	delay_nus(10000);
}

/**
  * @brief  setting ft5x06 I2C SDA output mode
  * @param  None
  * @retval None
  */
static void FT5x06_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;               
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
}

/**
  * @brief  setting ft5x06 I2C SDA input mode
  * @param  None
  * @retval None
  */
static void FT5x06_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;               
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
}

/**
  * @brief  IIC start
  * @param  None
  * @retval None
  */
static void I2C_Start(void)
{
	FT5x06_SDA_OUT();   
	FT5X06_SDA_H();  	  
	FT5X06_SCL_H();
	delay_nus(2);
	FT5X06_SDA_L();
	delay_nus(2);
	FT5X06_SCL_L();
}	  

/**
  * @brief  IIC stop
  * @param  None
  * @retval None
  */
static void I2C_Stop(void)
{
	FT5x06_SDA_OUT();
	FT5X06_SCL_L();
	FT5X06_SDA_L();
	delay_nus(2);
	FT5X06_SCL_H();
	FT5X06_SDA_H();
	delay_nus(2);							   	
}

/**
  * @brief  IIC wait for ack
  * @param  None
  * @retval return = 1 ACK, return = 0 no ACK
  */
static int I2C_WaitAck(void)
{	
	FT5x06_SDA_IN(); 
	FT5X06_SDA_H();
	delay_nus(2);	   
	FT5X06_SCL_H();
	delay_nus(2);	

	if ( FT5X06_READ_SDA() )
	{
		I2C_Stop();
		return 0;
	}
	FT5X06_SCL_L();	   
	return 1;
}

/**
  * @brief  IIC ack
  * @param  None
  * @retval None
  */
static void I2C_Ack(void)
{
	FT5x06_SDA_OUT();
	FT5X06_SCL_L();
	FT5X06_SDA_L();
	delay_nus(2);
	FT5X06_SCL_H();
	delay_nus(2);
	FT5X06_SCL_L();
}

/**
  * @brief  IIC no ack
  * @param  None
  * @retval None
  */
static void I2C_NoAck(void)
{
	FT5x06_SDA_OUT();
	FT5X06_SCL_L();
	FT5X06_SDA_H();
	delay_nus(2);
	FT5X06_SCL_H();
	delay_nus(2);
	FT5X06_SCL_L();
}	
	
/**
 * @brief	I2C Transmits data as a master to a slave
 * @param	SendByte: send data
 * @retval None
 */  
static void I2C_SendByte(unsigned char SendByte) 
{                        
	unsigned char i;

	FT5x06_SDA_OUT();  
	for( i=0; i<8; i++ )
	{         
		FT5X06_SCL_L();
		delay_nus(2);
		if ( SendByte&0x80 ) 	
		{
			FT5X06_SDA_H();
		}
		else 
		{
			FT5X06_SDA_L();
		}
		SendByte <<= 1;
		delay_nus(2);   
		FT5X06_SCL_H();
		delay_nus(2); 
	}
	FT5X06_SCL_L();
} 

/**
 * @brief	I2C Transmits data as a master to a slave
 * @param	ack: ack = 0, I2C no response, ack = 1, I2C response
 * @retval None
 */
int I2C_ReceiveByte(unsigned char ack)
{
	unsigned char i, ReceiveByte;

	FT5X06_SDA_H();

	FT5x06_SDA_IN();    /* SDA set to input mode */
	for( i=0; i<8; i++ )
	{
		ReceiveByte <<= 1;

		FT5X06_SCL_L(); 
		delay_nus(2);
		FT5X06_SCL_H();
		delay_nus(2);

		if( FT5X06_READ_SDA())
		{
			ReceiveByte |= 0x01;
		}
	}					 
	if (!ack)
	{
		I2C_NoAck();  //send nACK
	}
	else      
	{
		I2C_Ack();   //send ACK
	}

	return ReceiveByte;
}

/**
 * @brief	Transmits data as a master to a slave
 * @param	DevAddress: Address for the slave device
 * @param	pBuffer: Pointer to the buffer of data to send
 * @param	Size: Size of the buffer
 * @retval None
 */
static void I2C_Transmit(unsigned char DevAddress, unsigned char* pBuffer, unsigned char Size)
{
	unsigned char i;

	I2C_Start();
	I2C_SendByte( DevAddress << 1 );
	I2C_WaitAck();	
	for( i=0; i<Size; i++ )
	{	
		I2C_SendByte(pBuffer[i]);
		I2C_WaitAck();
	}
	I2C_Stop();
}

/**
 * @brief	Transmits data as a master to a slave
 * @param	DevAddress: Address for the slave device
 * @param	Data: Pointer to a buffer where data will be stored
 * @param	Size: Size of the amount of data to receive
 * @retval None
 */
static void I2C_Receive(unsigned char DevAddress, unsigned char* pBuffer, unsigned char Size)
{
	unsigned char i;

	I2C_Start();
	I2C_SendByte( (DevAddress << 1) + 1 );
	I2C_WaitAck();	
	for( i=0; i<Size; i++ )
	{	
		if(i==(Size-1)) pBuffer[i] = I2C_ReceiveByte(0);
		else            pBuffer[i] = I2C_ReceiveByte(1);
	}
	I2C_Stop();
}

/**
 * @brief	read data from ft5x06
 * @param	rxdata: pointer to the buffer of data
 * @param	length: size of the buffer
 * @retval None
 */
static void ft5x_i2c_rxdata(unsigned char *rxdata, int length)
{
	unsigned char i;

	I2C_Start();
	I2C_SendByte( (FT5X06_ADDRESS << 1) + 1 );
	I2C_WaitAck();	
	for( i=0; i<length; i++ )
	{	
		if(i==(length-1)) *(rxdata+i) = I2C_ReceiveByte(0);
		else              *(rxdata+i) = I2C_ReceiveByte(1);
	}
	I2C_Stop();	
}

/**
 * @brief	read ft5x06 ID
 * @param	None
 * @retval return 0, read ft5x06 success
 */
int ft5x06_get_id(void)
{
	#define FT5X0X_REG_FIRMID         0xA6         /* ***************firmware version **********************/

	unsigned char chipId = 0;

	/* Try to get chip id */
	uint8_t data = FT5X0X_REG_FIRMID;
	I2C_Transmit(FT5X06_ADDRESS, &data, 1);

	I2C_Receive(FT5X06_ADDRESS, &chipId, 1);
	if (chipId == 0xff)
	{
		/* TODO: Do something */
		return 1;
	}
	return 0;
}

/**
 * @brief	read data from ft5x06
 * @param	None
 * @retval return 0 is success
 */
int ft5x0x_read_data(void)
{
	unsigned char buf[32]={0};
	int i;

#ifdef CONFIG_FT5X0X_MULTITOUCH
	ft5x_i2c_rxdata(buf, 31);
#else
	ft5x_i2c_rxdata(buf, 7);
#endif
	ts_event.touch_point = buf[2] & 0xf;
	if (ts_event.touch_point == 0)
	{
		return 1;
	}

#ifdef CONFIG_FT5X0X_MULTITOUCH
	switch (ts_event.touch_point) {
		case 5:
			ts_event.point[4].x = (s16)(buf[0x1b] & 0x07)<<8 | (s16)buf[0x1c];
			ts_event.point[4].y = (s16)(buf[0x1d] & 0x07)<<8 | (s16)buf[0x1e];
		
		case 4:
			ts_event.point[3].x = (s16)(buf[0x15] & 0x07)<<8 | (s16)buf[0x16];
			ts_event.point[3].y = (s16)(buf[0x17] & 0x07)<<8 | (s16)buf[0x18];
		
		case 3:
			ts_event.point[2].x = (s16)(buf[0x0f] & 0x07)<<8 | (s16)buf[0x10];
			ts_event.point[2].y = (s16)(buf[0x11] & 0x07)<<8 | (s16)buf[0x12];
		
		case 2:
			ts_event.point[1].x = (s16)(buf[0x09] & 0x07)<<8 | (s16)buf[0x0a];
			ts_event.point[1].y = (s16)(buf[0x0b] & 0x07)<<8 | (s16)buf[0x0c];
		
		case 1:
			ts_event.point[0].x = (s16)(buf[0x03] & 0x0f)<<8 | (s16)buf[0x04];
			ts_event.point[0].y = (s16)(buf[0x05] & 0x0f)<<8 | (s16)buf[0x06];

			if(ts_event.point[0].x < 0){
				ts_event.point[0].x = 0;
			}

		for(i=0; i<ts_event.touch_point; i++)
		{
			if( ft5x06_xy_swap )
			{
				swap(ts_event.point[i].x, ts_event.point[i].y);
			}

			if( ft5x06_x_revert)
			{
				ts_event.point[i].x = ft5x06_max_x - ts_event.point[i].x;	
			}

			if( ft5x06_y_revert)
			{
				ts_event.point[i].y = ft5x06_max_y - ts_event.point[i].y;
			}
		}

		//printf("point%d: (%d, %d)\r\n", ts_event.touch_point, ts_event.point[i].x, ts_event.point[i].y);

		default:
		return 0;
	}
#endif
}

/**
 * @brief	read data from ft5x06
 * @param	None
 * @retval None
 */
void ft5x0x_report_value(void)
{
	if( ts_event.key_status == touch_key_down )
	{
		EXTI->IMR&=~(1<<6); /* disable PH6 interrupt */
		ft5x0x_read_data();
		ts_event.key_status = touch_key_up;                
		EXTI->IMR|=1<<6;    /* enable PH6 interrupt */
	}
}

/**
 * @brief	detection as a resistive touch or capacitive touch
 * @param	None
 * @retval None
 */
void touchpanel_init(void)
{
	int ret = -1;

	ft5x06_init();
	xpt2046_init();
	ret = ft5x06_get_id();
	if( !ret ) /* capacitive touch */
	{
		touch_type = CAPACITIVE_TOUCH;
	}
	else       /* resistive touch */
	{
		touch_type = RESISTIVE_TOUCH;
	}
}

/**
  * @brief  This function handles External line 9-5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6)==SET)
	{
		ts_event.key_status = touch_key_down;  /* touch panel pressed */
		EXTI_ClearITPendingBit(EXTI_Line6);
	}		
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
