/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               FMC_nand.c
** Descriptions:            The FMC nand application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-6-19
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             AVRman
** Modified date:           2015-9-16
** Version:                 v1.1
** Descriptions:            add english description
**
*********************************************************************************************************/

#ifndef __FMC_NAND_H
#define __FMC_NAND_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <string.h>
#include "stdio.h"

/* Private define ------------------------------------------------------------*/
/* NAND ID*/
#define HY27UF081G2A	0xADF1801D   /* HY27UF081G2A = 0xAD 0xF1 0x80 0x1D */
#define K9F1G08U0A		0xECF18015   /* K9F1G08U0A = 0xEC 0xF1 0x80 0x15 */
#define K9F1G08U0B		0xECF10095   /* K9F1G08U0B = 0xEC 0xF1 0x00 0x95 */

/* Exported constants --------------------------------------------------------*/
#define CMD_AREA                   (uint32_t)(0x010000)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(0x020000)  /* A17 = ALE high */
#define DATA_AREA                  (uint32_t)(0x000000)

/* FMC NAND memory address computation */
#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */

/* FMC NAND memory command */
#define	NAND_CMD_AREA_A            ((uint8_t)0x00)
#define	NAND_CMD_AREA_B            ((uint8_t)0x01)
#define NAND_CMD_AREA_C            ((uint8_t)0x50)
#define NAND_CMD_AREA_TRUE1        ((uint8_t)0x30)

#define NAND_CMD_WRITE0            ((uint8_t)0x80)
#define NAND_CMD_WRITE_TRUE1       ((uint8_t)0x10)

#define NAND_CMD_ERASE0            ((uint8_t)0x60)
#define NAND_CMD_ERASE1            ((uint8_t)0xD0)

#define NAND_CMD_STATUS				      ((uint8_t)0x70)

#define NAND_CMD_READID            ((uint8_t)0x90)

#define NAND_CMD_LOCK_STATUS       ((uint8_t)0x7A)
#define NAND_CMD_RESET             ((uint8_t)0xFF)

/* NAND memory status */
#define NAND_BUSY                  ((uint8_t)0x00)
#define NAND_ERROR                 ((uint8_t)0x01)
#define NAND_READY                 ((uint8_t)0x40)
#define NAND_TIMEOUT_ERROR         ((uint8_t)0x80)


/* FMC NAND memory parameters */
/************************************************************************************/
#define NAND_PAGES_PER_BLOCK        64       /* 64 pages per block */
#define NAND_PAGE_DATA_SIZE         2048     /* 2 * 1024 bytes per page w/o Spare Area */
#define NAND_PAGE_OOB_SIZE          64       /* last 64 bytes as spare area */
#define NAND_BLOCK_NUM              1024     /* block number */
#define NAND_PAGE_NUM               (NAND_BLOCK_NUM * NAND_PAGES_PER_BLOCK)   /* total page size 页面总大小 */
/************************************************************************************/

#define NAND_OK   0
#define NAND_FAIL 1

/* Private function prototypes ------------------------------------------------*/
void NAND_Init(void);
uint32_t NAND_ReadID(void);
void FMC_NAND_Test(void);
uint8_t FMC_NAND_EraseBlock(uint32_t _ulBlockNo);
uint8_t FMC_NAND_ReadPage(uint8_t *_pBuffer, uint32_t _ulPageNo, uint16_t _usAddrInPage, uint16_t _usByteCount);
uint8_t FMC_NAND_WritePage(uint8_t *_pBuffer, uint32_t _ulPageNo, uint16_t _usAddrInPage, uint16_t _usByteCount);
#endif /* __FMC_NAND_H */

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
