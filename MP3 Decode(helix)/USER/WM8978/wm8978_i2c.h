/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               wm8978_i2c.h
** Descriptions:            gpio simulation I2C
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-10-25
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

#ifndef _WM8978_I2C_H
#define _WM8978_I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define I2C_WR	0		/* write control bit */
#define I2C_RD	1		/* read control bit */

/* Private function prototypes -----------------------------------------------*/
void InitI2C(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

#endif

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
