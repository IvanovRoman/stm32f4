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

/* Includes ------------------------------------------------------------------*/
#include "fmc_nand.h"

/*
	[ NAND Flash structure definition | NAND Flash结构定义 ]
   spare area have 16x4 bytes, 1024 bytes per page, 512 bytes per sector, each sector have 16 bytes spare area:
	 备用区有16x4字节,每page 1024字节,每512字节一个扇区,每个扇区对应16自己的备用区:

	 page logical structure, 512x4 bytes is data area, spare area is 16x4 bytes in the behand:
	 每个PAGE的逻辑结构,前面512Bx4是主数据区,后面16Bx4是备用区:
	┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐
	│ Main area  ││ Main area  ││ Main area  ││Main area   ││ Spare area ││ Spare area ││ Spare area ││Spare area  │
	│            ││            ││            ││            ││            ││            ││            ││            │
	│   512B     ││    512B    ││    512B    ││    512B    ││    16B     ││     16B    ││     16B    ││    16B     │
	└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘

   each of the logical structure of spare area 16B is (samsung standard):
	 每16B的备用区的逻辑结构如下(三星推荐标准):
	┌───┐┌───┐┌──┐┌──┐┌──┐┌───┐┌───┐┌───┐┌──┐┌──┐┌──┐┌──┐┌───┐┌───┐┌───┐┌───┐┌───┐
	│  BI  ││RESER ││LSN0││LSN1││LSN2││RESER ││RESER ││RESER ││ECC0││ECC1││ECC2││ECC0││S-ECC1││S-ECC0││RESER ││RESER ││RESER │
	│      ││ VED  ││    ││    ││    ││ VED  ││ VED  ││ VED  ││    ││    ││    ││    ││      ││      ││ VED  ││ VED  ││ VED  │
	└───┘└───┘└──┘└──┘└──┘└───┘└───┘└───┘└──┘└──┘└──┘└──┘└───┘└───┘└───┘└───┘└───┘

	K9F1G08U0A and HY27UF081G2A are compatible. when the product is out of the factory, the chip makers must ensure that first block is a good block. 
	if some block is a bad block, the block of first page spare area, first bytes written data is not 0xFF, mean this block is bad block.
	K9F1G08U0A和HY27UF081G2A是兼容的.芯片出厂时,厂商保证芯片的第1个块是好块.
	如果有些块是坏块,块内第1个page备用区的第1个字节写入非0xFF数据表示坏块.
*/

/* Private define ------------------------------------------------------------*/

#define Bank_NAND_ADDR     ((uint32_t)0x80000000)  /* NAND Flash address */				

#define NAND_CMD_AREA   *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA)  /* NAND write command */
#define NAND_ADDR_AREA  *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) /* NAND write address */
#define NAND_DATA_AREA  *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) /* NAND write data */

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t FMC_NAND_GetStatus(void);

/**
  * @brief configures the FMC and GPIO interface for NAND,
  *        this function must be called before any write/read operation on the NAND.
  * @param None
  * @retval None
  */
static void FMC_NAND_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	FMC_NANDInitTypeDef  FMC_NANDInitStructure;
	FMC_NAND_PCCARDTimingInitTypeDef  p;

  /***********************************************************************
   * PD0  --> D2  | PD1 --> D3     | PD14 --> D0    | PD15 --> D1        *
   * PE7  --> D4  | PE8 --> D5     | PE9  --> D6    | PE10 --> D7        *
   * PD4  --> NOE | PD5 --> NWE    | PG9  --> N_CS  | PD11 --> A16       *
   * PD12 --> A17 | PB7 --> NWAIT  |                                     *
   ***********************************************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOG, ENABLE);

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	/* PD0 --> D2, PD1 --> D3, PD4 --> NOE, PD5 --> NWE, PD11 --> A16, PD12 --> A17, PD14 --> D0, PD15 --> D1 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                              GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* PG9 --> N_CS */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* PE7 --> D4, PE8 --> D5, PE9 --> D6, PE10 --> D7 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PB7 --> NWAIT, configured as an input pin, use for busy signal */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Defines the number of HCLK cycles to setup address before the command assertion for NAND-Flash
		read or write access  to common/Attribute or I/O memory space (depending on the memory space
		timing to be configured).This parameter can be a value between 0 and 0xFF */
  p.FMC_SetupTime = 0x1;

	/* Defines the minimum number of HCLK cycles to assert the command for NAND-Flash read or write
		 access to common/Attribute or I/O memory space (depending on the memory space timing to be
		 configured). This parameter can be a number between 0x00 and 0xFF */
	p.FMC_WaitSetupTime = 0x3;

	/* Defines the number of HCLK clock cycles to hold address (and data for write access) after the
		 command deassertion for NAND-Flash read or write access to common/Attribute or I/O memory space
		 (depending on the memory space timing to be configured).
		 This parameter can be a number between 0x00 and 0xFF */
	p.FMC_HoldSetupTime = 0x2;

	/* Defines the number of HCLK clock cycles during which the databus is kept in HiZ after the start
		of a NAND-Flash  write access to common/Attribute or I/O memory space (depending on the memory
		space timing to be configured). This parameter can be a number between 0x00 and 0xFF */
	p.FMC_HiZSetupTime = 0x1;

	FMC_NANDInitStructure.FMC_Bank = FMC_Bank3_NAND; 
	FMC_NANDInitStructure.FMC_Waitfeature = FMC_Waitfeature_Disable;
	FMC_NANDInitStructure.FMC_MemoryDataWidth = FMC_NAND_MemoryDataWidth_8b;
	FMC_NANDInitStructure.FMC_ECC = FMC_ECC_Enable;
	FMC_NANDInitStructure.FMC_ECCPageSize = FMC_ECCPageSize_2048Bytes;
	FMC_NANDInitStructure.FMC_TCLRSetupTime = 0x01;
	FMC_NANDInitStructure.FMC_TARSetupTime = 0x01;
	FMC_NANDInitStructure.FMC_CommonSpaceTimingStruct = &p;				  /* FMC Common Space Timing */
	FMC_NANDInitStructure.FMC_AttributeSpaceTimingStruct = &p;			/* FMC Attribute Space Timing */
	FMC_NANDInit(&FMC_NANDInitStructure);
	
	FMC_NANDCmd(FMC_Bank3_NAND, ENABLE);
}

/**
  * @brief wait nand ready
  * @param None
  * @retval None
  */
static void nand_waitready(void)
{
	uint16_t i;
	for (i = 0; i < 20; i++);
	while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0 );
}


/**
  * @brief read nand ID
  * @param None
  * @retval 32bit ID
  */
uint32_t NAND_ReadID(void)
{
	uint32_t data = 0;

	/* command to the command area */
	NAND_CMD_AREA = 0x90;
	NAND_ADDR_AREA = 0x00;

	/* read data */
	data = *(__IO uint32_t *)(Bank_NAND_ADDR | DATA_AREA);
	data =  ((data << 24) & 0xFF000000) | ((data << 8 ) & 0x00FF0000) | 
	        ((data >> 8 ) & 0x0000FF00) | ((data >> 24) & 0x000000FF) ;
	
	return data;
}

/**
	* @brief write data to a specified location in NandFlash specified page,
	*        the data length must less than the page size.
	*        写数据至NandFlash指定页面的指定位置,数据长度小于页大小.
	* @param _pBuffer: data buffer
	*        _ulPageNo: page number
	*        _usAddrInPage: address in page, (2048 + 64) bytes per page
	*        _usByteCount: data size
	* @retval NAND_FAIL failure, NAND_OK success
  */
uint8_t FMC_NAND_WritePage(uint8_t *_pBuffer, uint32_t _ulPageNo, uint16_t _usAddrInPage, uint16_t _usByteCount)
{
	uint16_t i;

	/* write page command */
	NAND_CMD_AREA = NAND_CMD_WRITE0;

	/* For HY27UF081G2A
	              Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0
	first byte    A7   A6   A5   A4   A3   A2   A1   A0		(_usPageAddr bit7 - bit0)
	second byte   0    0    0    0    A11  A10  A9   A8		(_usPageAddr low 4 bit bit11 - bit8, high 4bit is 0)
	third byte    A19  A18  A17  A16  A15  A14  A13  A12
	fourth byte   A27  A26  A25  A24  A23  A22  A21  A20
	*/
	NAND_ADDR_AREA = _usAddrInPage;
	NAND_ADDR_AREA = _usAddrInPage >> 8;
	NAND_ADDR_AREA = _ulPageNo;
	NAND_ADDR_AREA = (_ulPageNo & 0xFF00) >> 8;

	/* write date */
	for(i = 0; i < _usByteCount; i++)
	{
		NAND_DATA_AREA = _pBuffer[i];
	}
	NAND_CMD_AREA = NAND_CMD_WRITE_TRUE1;

	nand_waitready();

	if (FMC_NAND_GetStatus() == NAND_READY)
	{
		return NAND_OK;
	}
	return NAND_FAIL;
}

/**
	* @brief read data from specified location in NandFlash specified page,
	*        the data length must less than the page size.
	*        读数据至NandFlash指定页面的指定位置,数据长度小于页大小.
	* @param _pBuffer: data buffer
	*        _ulPageNo: page number
	*        _usAddrInPage: address in page, (2048 + 64) bytes per page
	*        _usByteCount: data size
	* @retval NAND_FAIL failure, NAND_OK success
  */
uint8_t FMC_NAND_ReadPage(uint8_t *_pBuffer, uint32_t _ulPageNo, uint16_t _usAddrInPage, uint16_t _usByteCount)
{
	uint16_t i;

	/* read page command */
    NAND_CMD_AREA = NAND_CMD_AREA_A;

	/* For HY27UF081G2A
	              Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0
	first byte    A7   A6   A5   A4   A3   A2   A1   A0		(_usPageAddr bit7 - bit0)
	second byte   0    0    0    0    A11  A10  A9   A8		(_usPageAddr low 4 bit bit11 - bit8, high 4bit is 0)
	third byte    A19  A18  A17  A16  A15  A14  A13  A12
	fourth byte   A27  A26  A25  A24  A23  A22  A21  A20
	*/
	NAND_ADDR_AREA = _usAddrInPage;
	NAND_ADDR_AREA = _usAddrInPage >> 8;
	NAND_ADDR_AREA = _ulPageNo;
	NAND_ADDR_AREA = (_ulPageNo & 0xFF00) >> 8;

	NAND_CMD_AREA = NAND_CMD_AREA_TRUE1;

	nand_waitready();

	
	for(i = 0; i < _usByteCount; i++)
	{
		_pBuffer[i] = NAND_DATA_AREA;
	}

	return NAND_OK;
}

/**
	* @brief NAND Flash erasing a block
	*        擦除NAND Flash一个块
	* @param _ulBlockNo: Block number, 0-1023
	* @retval NAND_TIMEOUT_ERROR Timeout error, NAND_READY success
  */
uint8_t FMC_NAND_EraseBlock(uint32_t _ulBlockNo)
{
  if (_ulBlockNo >= NAND_BLOCK_NUM)
	{
		return 0;
	}
	
	/* send erase command */
	NAND_CMD_AREA = NAND_CMD_ERASE0;

	_ulBlockNo <<= 6;	/* block number converte to page number */
	NAND_ADDR_AREA = _ulBlockNo;
	NAND_ADDR_AREA = _ulBlockNo >> 8;

	NAND_CMD_AREA = NAND_CMD_ERASE1;

  nand_waitready();
	
	return (FMC_NAND_GetStatus());
}

/**
	* @brief reset NAND Flash
	* @param None
	* @retval None
  */
static uint8_t FMC_NAND_Reset(void)
{
	NAND_CMD_AREA = NAND_CMD_RESET;

	if (FMC_NAND_GetStatus() == NAND_READY)
	{
		return NAND_OK;
	}

	return NAND_FAIL;
}

/**
	* @brief read NAND Flash status
	* @param None
	* @retval NAND_BUSY busy, NAND_ERROR error, NAND_READY success
  */
static uint8_t FMC_NAND_ReadStatus(void)
{
	uint8_t ucData;
	uint8_t ucStatus = NAND_BUSY;

	NAND_CMD_AREA = NAND_CMD_STATUS;
	ucData = *(__IO uint8_t *)(Bank_NAND_ADDR);

	if((ucData & NAND_ERROR) == NAND_ERROR)
	{
		ucStatus = NAND_ERROR;
	}
	else if((ucData & NAND_READY) == NAND_READY)
	{
		ucStatus = NAND_READY;
	}
	else
	{
		ucStatus = NAND_BUSY;
	}

	return (ucStatus);
}

/**
	* @brief get NAND Flash status
	* @param None
	* @retval NAND_TIMEOUT_ERROR Timeout error, NAND_READY success
  */
static uint8_t FMC_NAND_GetStatus(void)
{
	uint32_t ulTimeout = 0x10000;
	uint8_t ucStatus = NAND_READY;

	ucStatus = FMC_NAND_ReadStatus();

	/* wait for the end of the NAND operation, will quit after the timeout */
	while ((ucStatus != NAND_READY) &&( ulTimeout != 0x00))
	{
		ucStatus = FMC_NAND_ReadStatus();
		ulTimeout--;
	}

	if(ulTimeout == 0x00)
	{
		ucStatus =  NAND_TIMEOUT_ERROR;
	}

	return (ucStatus);
}

/**
	* @brief initialization NAND Flash
	* @param None
	* @retval None
  */
void NAND_Init(void)
{
	FMC_NAND_Init();
	
	FMC_NAND_Reset();
}

/**
	* @brief read NAND FLASH model
	* @param None
	* @retval None
  */
void FMC_NAND_Test(void)
{
	uint32_t id;

	FMC_NAND_Init();

	id = NAND_ReadID();

	printf("\r\nNAND Flash ID = 0x%04X, Type = ", id);
	if (id == HY27UF081G2A)
	{
		printf("HY27UF081G2A\r\n  1024 Blocks, 64 pages per block, (2048 + 64) bytes per page\r\n");
	}
	else if (id == K9F1G08U0A)
	{
		printf("K9F1G08U0A\r\n  1024 Blocks, 64 pages per block, (2048 + 64) bytes per page\r\n");
	}
	else if (id == K9F1G08U0B)
	{
		printf("K9F1G08U0B\r\n  1024 Blocks, 64 pages per block, (2048 + 64) bytes per page\r\n");
	}
	else
	{
		printf("unkonow\r\n");
	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
