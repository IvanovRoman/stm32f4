/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               FlashDriver.c
** Descriptions:            SPI flash Function
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

#ifndef __FLASHDRIVER_H 
#define __FLASHDRIVER_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private function prototypes -----------------------------------------------*/
void df_read_open(uint32_t addr);
void df_write_open(uint32_t addr);
void df_read(uint8_t *buf,uint16_t size);
void df_write(uint8_t *buf,uint16_t size);
void df_read_seek(uint32_t addr);
void df_write_seek(uint32_t addr);
void df_read_close(void);
void df_write_close(void);
#endif 

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
