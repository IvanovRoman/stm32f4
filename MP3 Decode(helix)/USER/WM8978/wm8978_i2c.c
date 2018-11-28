/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               wm8978_i2c.c
** Descriptions:            gpio simulation I2C
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-10-25
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "wm8978_i2c.h"

/* Private define ------------------------------------------------------------*/
#define GPIO_PORT_I2C	GPIOH
#define RCC_I2C_PORT 	RCC_AHB1Periph_GPIOH
#define I2C_SCL_PIN		GPIO_Pin_4
#define I2C_SDA_PIN		GPIO_Pin_5

/* I2C SCL and SDA defined macros to read and write */
#define I2C_SCL_1()  GPIO_PORT_I2C->BSRRL = I2C_SCL_PIN				/* SCL = 1 */
#define I2C_SCL_0()  GPIO_PORT_I2C->BSRRH = I2C_SCL_PIN				/* SCL = 0 */

#define I2C_SDA_1()  GPIO_PORT_I2C->BSRRL = I2C_SDA_PIN				/* SDA = 1 */
#define I2C_SDA_0()  GPIO_PORT_I2C->BSRRH = I2C_SDA_PIN				/* SDA = 0 */

#define I2C_SDA_READ()  ((GPIO_PORT_I2C->IDR & I2C_SDA_PIN) != 0)	/* read SDA port */
#define I2C_SCL_READ()  ((GPIO_PORT_I2C->IDR & I2C_SCL_PIN) != 0)	/* read SCL port */

/**
  * @brief  initialization I2C
  * @param  None
  * @retval None
  */
void InitI2C(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_I2C_PORT, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* set output to open drain */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* don't need any pull up or pull down */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);

	/* stop signal, reset all the devices on the I2C bus to standby mode */
	i2c_Stop();
}

/**
  * @brief  i2c delay, fastest speed is 400KHz
  * @param  None
  * @retval None
  */
static void i2c_Delay(void)
{
	uint8_t i;

	for (i = 0; i < 30; i++);
}

/**
  * @brief  i2c bus, starting signal
  * @param  None
  * @retval None
  */
void i2c_Start(void)
{
	/* when SCL is high, SDA low mean I2C bus start signal */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/**
  * @brief  i2c bus, stop signal
  * @param  None
  * @retval None
  */
void i2c_Stop(void)
{
	/* when SCL is high, SDA high mean I2C bus stop signal */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/**
  * @brief  i2c bus send 8bit data
  * @param  _ucByte: sent bytes
  * @retval None
  */
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* sent bytes */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); /* release bus */
		}
		_ucByte <<= 1;	/* left shift a bit */
		i2c_Delay();
	}
}

/**
  * @brief  i2c bus read 8bit data
  * @param  None
  * @retval read data
  */
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/**
  * @brief  wait i2c bus ACK response signal
  * @param  None
  * @retval return 0 mean ACK response, 1 mean no device response
  */
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* release SDA */
	i2c_Delay();
	I2C_SCL_1();	/* SCL = 1, the device returns a ACK response */
	i2c_Delay();
	
	/* read SDA status */
	if ( I2C_SDA_READ() )
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/**
  * @brief  response a ACK signal to i2c bus
  * @param  None
  * @retval None
  */
void i2c_Ack(void)
{
	I2C_SDA_0();	/* SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* release SDA bus */
}

/**
  * @brief  response a NACK signal to i2c bus
  * @param  None
  * @retval None
  */
void i2c_NAck(void)
{
	I2C_SDA_1();	/* SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/**
  * @brief  detect i2c bus, send device address, then read the device response to determine the device is exists
  * @param  _Address: i2c bus address of the device
  * @retval  return 0 mean true, 1 mean failure
  */
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (I2C_SDA_READ() && I2C_SCL_READ())
	{
		i2c_Start();		/* starting signal */

		/* send device address + read and write control bit£¨0 = w£¬ 1 = r) */
		i2c_SendByte(_Address | I2C_WR);
		
		ucAck = i2c_WaitAck();	/* detect ACK response */

		i2c_Stop();			/* stop signal */

		return ucAck;
	}
	return 1;
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
