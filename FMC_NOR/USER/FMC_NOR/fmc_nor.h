/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fsmc_nor.h
** Descriptions:            The FSMC NOR application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-6-19
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __FSMC_NOR_H
#define __FSMC_NOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define NOR_FLASH_START_ADDR       ((uint32_t)0x60000000)
#define NOR_FLASH_END_ADDR         ((uint32_t)0x607FFFFF)
												
/* Delay definition */   
#define BlockErase_Timeout         ((uint32_t)0x00A00000)
#define ChipErase_Timeout          ((uint32_t)0x30000000) 
#define Program_Timeout            ((uint32_t)0x00001400)

#define ADDR_SHIFT(A)              (NOR_FLASH_START_ADDR + (2 * (A)))
#define NOR_WRITE(Address, Data)   (*(vu16 *)(Address) = (Data))

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint16_t Manufacturer_Code;
  uint16_t Device_Code1;
  uint16_t Device_Code2;
  uint16_t Device_Code3;
}
NOR_IDTypeDef;

/* NOR Status */
typedef enum
{
  NOR_SUCCESS = 0,
  NOR_ONGOING,
  NOR_ERROR,
  NOR_TIMEOUT
}
NOR_Status;

/* Private function prototypes -----------------------------------------------*/
void FMC_NOR_Init(void);
void FMC_NOR_Test(void);

#endif /* __FSMC_NOR_H */

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
