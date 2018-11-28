/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SST25VF016B.c
** Descriptions:            SST25VF016B SPI flash Function
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

#ifndef __SST25VF016B_H 
#define __SST25VF016B_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;

/* Private define ------------------------------------------------------------*/
#define MAX_ADDR		0x1FFFFF	   /* define the MAX address of the chip */
#define	SEC_MAX     	511        /* maximum sector number */
#define SEC_SIZE		0x1000       /* sector buffer size */

#define RCC_GPIO_SPI_FLASH_CS               RCC_AHB1Periph_GPIOB
#define SPI_FALSH_CS_PORT                   GPIOB
#define SPI_FALSH_CS_PIN                    GPIO_Pin_12

#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)

/* Private function prototypes -----------------------------------------------*/
void SPI_FLASH_Init(void);
int SSTF016B_RD(int Dst, unsigned char* RcvBufPt ,int NByte);
int SSTF016B_RdID(idtype IDType,int* RcvbufPt);
int SSTF016B_WR(int Dst, unsigned char* SndbufPt,int NByte);
int SSTF016B_Erase(int sec1, int sec2);
void SPI_FLASH_Test(void);

#endif

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/

