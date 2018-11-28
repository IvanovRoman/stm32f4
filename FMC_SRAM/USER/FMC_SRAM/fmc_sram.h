/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               FMC_sram.h
** Descriptions:            The FMC SRAM application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-6-19
** Version:                 v1.1
** Descriptions:            The second version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __FSMC_SRAM_H
#define __FSMC_SRAM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define SRAM_BLANK1_START_ADDR  ((uint32_t)0x6c000000)	    /* 512K SRAM */
#define SRAM_BLANK1_END_ADDR	((uint32_t)0x6c080000)
										
#define SRAM_BLANK2_START_ADDR  ((uint32_t)0x6c000000)	    /* 1M SRAM */
#define SRAM_BLANK2_END_ADDR	((uint32_t)0x6c100000)

#define SRAM_BLANK3_START_ADDR  ((uint32_t)0x6c000000)	    /* 4M SRAM */
#define SRAM_BLANK3_END_ADDR	((uint32_t)0x6c400000)

#define SRAM_BLANK4_START_ADDR  ((uint32_t)0x6c000000)	    /* 8M SRAM */
#define SRAM_BLANK4_END_ADDR	((uint32_t)0x6c800000)

/* Private function prototypes -----------------------------------------------*/
void FMC_SRAM_Init(void);
void FMC_SRAM_Test(void);

#endif /* __FSMC_SRAM_H */

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
