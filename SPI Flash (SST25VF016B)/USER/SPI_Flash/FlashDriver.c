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

/* Includes ------------------------------------------------------------------*/
#include "SST25VF016B.h"
#include "FlashDriver.h"

/* Private variables ---------------------------------------------------------*/
static uint32_t CurReadAddr;	/* read address */
static uint32_t CurWriteAddr;	/* write address */

/**
  * @brief  read initialization.
  * @param  cnt: read the initial address
  * @retval None
  */
void df_read_open(uint32_t addr)
{
	CurReadAddr=addr;	/* read address	*/
}

/**
  * @brief  write initialization.
  * @param  cnt: write the initial address
  * @retval None
  */
void df_write_open(uint32_t addr)
{
	CurWriteAddr=addr;	/* write address */
}

/**
  * @brief  read flash buffer.
  * @param  buf: pointer to the buffer of data
  * @param  size: size of the buffer
  * @retval None
  */
void df_read(uint8_t *buf,uint16_t size)
{
	if( CurReadAddr + size <= MAX_ADDR )
	{
		SSTF016B_RD(CurReadAddr,buf,size);
		CurReadAddr+=size;
	}
}

/**
  * @brief  write flash buffer.
  * @param  buf: pointer to the buffer of data
  * @param  size: size of the buffer
  * @retval None
  */
void df_write(uint8_t *buf,uint16_t size)
{
	if( CurWriteAddr + size <= MAX_ADDR )
	{
		SSTF016B_WR(CurWriteAddr,buf,size);
		CurWriteAddr+=size;
	}
}

/**
  * @brief  adjust to read the new flash address.
  * @param  addr: new read address
  * @retval None
  */
void df_read_seek(uint32_t addr)
{
	df_read_close();
	df_read_open(addr);
}

/**
  * @brief  adjust to write the new flash address.
  * @param  addr: new write address
  * @retval None
  */
void df_write_seek(uint32_t addr)
{
	df_write_close();
	df_write_open(addr); 
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
void df_read_close(void)
{
	/* nothing */
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
void df_write_close(void)
{
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
