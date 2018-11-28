/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include <stm32f4xx.h>
#include "sdio_sd.h"



/* Definitions of physical drive number for each media */
#define ATA		0
#define MMC		1
#define USB		2


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  SD_Error res = SD_OK;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  res =  SD_Init(); 

  if(res == SD_OK)
  {
    res = (SD_Error)0x0;
  }
  return ((DSTATUS)res);
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
  if (pdrv) return STA_NOINIT;		/* Supports only single drive */
  return 0;

}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
  SD_Error status = SD_OK;

  SD_ReadMultiBlocks(buff, sector << 9, 512, 1);

  /* Check if the Transfer is finished */
  status =  SD_WaitReadOperation();
  while (SD_GetStatus() != SD_TRANSFER_OK);

  if (status == SD_OK) {
    return RES_OK;
  } else {
    return RES_ERROR;
  }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
  SD_Error status = SD_OK;

  SD_WriteMultiBlocks((BYTE *)buff, sector << 9, 512, 1);

  /* Check if the Transfer is finished */
  status = SD_WaitWriteOperation();
  while(SD_GetStatus() != SD_TRANSFER_OK);     
  if (status == SD_OK) {
    return RES_OK;
  } else {
    return RES_ERROR;
  }
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res = RES_OK;
  SD_CardInfo sdinfo;

  if (SD_GetCardInfo(&sdinfo) != SD_OK) return RES_ERROR;

  switch (cmd) {
	

    case GET_SECTOR_COUNT :	  // Get number of sectors on the disk (DWORD)
      *(DWORD*)buff = sdinfo.CardCapacity / 512;	
      res = RES_OK;
      break;

    case GET_SECTOR_SIZE :	  // Get R/W sector size (WORD) 
      *(WORD*)buff = 512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE :	    // Get erase block size in unit of sector (DWORD)
      *(DWORD*)buff = 32;
      res = RES_OK;
  }
	  
  return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime ()
{
  return ((2013UL-1980) << 25)// Year
          | (8UL << 21)       // Month
          | (14UL << 16)      // Day
          | (9U << 11)        // Hour 
          | (0U << 5)         // Min
          | (0U >> 1)         // Sec
          ;
}

/*
 ******************************************************************************
 *函数：void SDIO_IRQHandler(void)
 *输入：void
 *输出：void
 *描述：SDIO中断处理函数
 ******************************************************************************
 */
void SDIO_IRQHandler(void)
{
  SD_ProcessIRQSrc();
}

/*
 ******************************************************************************
 *函数：void SD_SDIO_DMA_IRQHANDLER(void)
 *输入：void
 *输出：void
 *描述：SDIO DMA中断处理函数
 ******************************************************************************
 */
void SD_SDIO_DMA_IRQHANDLER(void)
{
  SD_ProcessDMAIRQ();
}
