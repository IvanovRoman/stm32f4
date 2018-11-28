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

/* Includes ------------------------------------------------------------------*/
#include "SST25VF016B.h"

/**
 * @brief SST25VF016B SPI initialization
 * @param None
 * @retval None
 */
void SPI_FLASH_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_GPIO_SPI_FLASH_CS, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SPI_FALSH_CS_PIN;
	GPIO_Init(SPI_FALSH_CS_PORT, &GPIO_InitStructure);

	SPI_FLASH_CS_HIGH();

	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure); 
	SPI_Cmd(SPI2, ENABLE);
}

/**
  * @brief  SPI bus transmit and receive data
  * @param  data: send valuesend value
  * @retval return receive data
  */
unsigned char Flash_ReadWriteByte(unsigned char data)		
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPI peripheral */
	SPI_I2S_SendData(SPI2, data);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/**
  * @brief  SST25VF016B read ID
  * @param  IDType: ID type, users can choose Jedec_ID, Dev_ID, Manu_ID
  * @param  RcvbufPt: pointer tostore ID
  * @retval return 0 is success
  */
int SSTF016B_RdID(idtype IDType,int* RcvbufPt)
{
	int temp = 0;

	if (IDType == Jedec_ID)
	{
		SPI_FLASH_CS_LOW();	
		Flash_ReadWriteByte(0x9F);                       /* send read ID JEDEC command (9Fh) */
		temp = (temp | Flash_ReadWriteByte(0x00)) << 8;  /* receive data */
		temp = (temp | Flash_ReadWriteByte(0x00)) << 8;	
		temp = (temp | Flash_ReadWriteByte(0x00));       /* the value of temp should be 0xBF2541 */

		SPI_FLASH_CS_HIGH();

		*RcvbufPt = temp;
		return 0;
	}

	if ((IDType == Manu_ID) || (IDType == Dev_ID) )
	{
		SPI_FLASH_CS_LOW();	

		Flash_ReadWriteByte(0x90);               /* send read ID command (90h or ABh) */
		Flash_ReadWriteByte(0x00);               /* send address */
		Flash_ReadWriteByte(0x00);               /* send address */
		Flash_ReadWriteByte(IDType);		  
		temp = Flash_ReadWriteByte(0x00);        /* receive data */

		SPI_FLASH_CS_HIGH();

		*RcvbufPt = temp;
		return 0;
	}
	else
	{
		return -1;	
	}
}


/**
  * @brief  SST25VF016B read function, can write 1 and more data to the specified address.
  * @param  Dst: address, range MAX_ADDR - 0x0 (MAX_ADDR = 0x1FFFFF)
  * @param  SndbufPt: pointer to the buffer of data
  * @param  NByte: size of the buffer
  * @retval return 0 is success
  */
int SSTF016B_RD(int Dst, unsigned char* RcvBufPt ,int NByte)
{
	int i = 0;

	/* check entry parameters */
	if ((Dst+NByte > MAX_ADDR)||(NByte == 0))
	{
		return -1;
	}

	SPI_FLASH_CS_LOW();
	Flash_ReadWriteByte(0x0B);                      /* send read command */
	Flash_ReadWriteByte(((Dst & 0xFFFFFF) >> 16));  /* send 3 bytes of address	*/
	Flash_ReadWriteByte(((Dst & 0xFFFF) >> 8));
	Flash_ReadWriteByte(Dst & 0xFF);
	Flash_ReadWriteByte(0xFF);                      /* send a dummy byte to read data */
	for (i = 0; i < NByte; i++)		
	{
		RcvBufPt[i] = Flash_ReadWriteByte(0xFF);		
	}
	SPI_FLASH_CS_HIGH();
	return 0;
}

/**
  * @brief  SST25VF016B write function, can write 1 and more data to the specified address.
  * @param  Dst: address, range MAX_ADDR - 0x0 (MAX_ADDR = 0x1FFFFF)
  * @param  SndbufPt: pointer to the buffer of data
  * @param  NByte: size of the buffer
  * @retval return 0 is success
  */
int SSTF016B_WR(int Dst, unsigned char* SndbufPt,int NByte)
{
	unsigned char temp = 0,StatRgVal = 0;
	int i = 0;

	/* check entry parameters */	
	if (( (Dst+NByte-1 > MAX_ADDR)||(NByte == 0) ))
	{
		return -1;
	}


	SPI_FLASH_CS_LOW();				 
	Flash_ReadWriteByte(0x05);            /* send read status register command */
	temp = Flash_ReadWriteByte(0xFF);     /* save the status register value */
	SPI_FLASH_CS_HIGH();								

	SPI_FLASH_CS_LOW();				
	Flash_ReadWriteByte(0x50);            /* enable the status register to be written	*/
	SPI_FLASH_CS_HIGH();	

	SPI_FLASH_CS_LOW();				
	Flash_ReadWriteByte(0x01);            /* send write status register command */
	Flash_ReadWriteByte(0);
	SPI_FLASH_CS_HIGH();			

	for(i = 0; i < NByte; i++)
	{
		SPI_FLASH_CS_LOW();				
		Flash_ReadWriteByte(0x06);          /* send write enable command */
		SPI_FLASH_CS_HIGH();			

		SPI_FLASH_CS_LOW();					
		Flash_ReadWriteByte(0x02);                          /* send write data command */
		Flash_ReadWriteByte((((Dst+i) & 0xFFFFFF) >> 16));  /* send 3 bytes of address */
		Flash_ReadWriteByte((((Dst+i) & 0xFFFF) >> 8));
		Flash_ReadWriteByte((Dst+i) & 0xFF);
		Flash_ReadWriteByte(SndbufPt[i]);                   /* send data */
		SPI_FLASH_CS_HIGH();			

		do
		{
			SPI_FLASH_CS_LOW();					 
			Flash_ReadWriteByte(0x05);                  /* send read status register command */
			StatRgVal = Flash_ReadWriteByte(0xFF);      /* save the status register value */
			SPI_FLASH_CS_HIGH();							
		}
		while (StatRgVal == 0x03 );                   /* wait until the chips idle */

	}

	SPI_FLASH_CS_LOW();					
	Flash_ReadWriteByte(0x06);                      /* send write enable command */
	SPI_FLASH_CS_HIGH();			

	SPI_FLASH_CS_LOW();				
	Flash_ReadWriteByte(0x50);                      /* enable the status register to be written	*/
	SPI_FLASH_CS_HIGH();

	SPI_FLASH_CS_LOW();				
	Flash_ReadWriteByte(0x01);                      /* send write status register command */
	Flash_ReadWriteByte(temp);                      /* restore the status register value */
	SPI_FLASH_CS_HIGH();

	return 0;		
}

/**
  * @brief  select the most efficient algorithm based on the specified sector number to erase
  * @param  sec1: The start sector, range (0~511)
  * @param  sec2: The end sector, range (0~511)
  * @retval return 0 is success
  */
int SSTF016B_Erase(int sec1, int sec2)
{
	unsigned char  temp1 = 0,temp2 = 0,StatRgVal = 0;
	int SecnHdAddr = 0;	  			
	int no_SecsToEr = 0;                    /* number of sectors to be erased */
	int CurSecToEr = 0;                     /* the current to erase sector */

	/* check entry parameters */
	if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX))	
	{
		return -1;	
	}

	SPI_FLASH_CS_LOW();			 
	Flash_ReadWriteByte(0x05);                /* send read status register command */
	temp1 = Flash_ReadWriteByte(0x00);        /* save the status register value */
	SPI_FLASH_CS_HIGH();								

	SPI_FLASH_CS_LOW();			
	Flash_ReadWriteByte(0x50);                /* enable the status register to be written */
	SPI_FLASH_CS_HIGH();			

	SPI_FLASH_CS_LOW();								  	
	Flash_ReadWriteByte(0x01);                /* send write status register command */
	Flash_ReadWriteByte(0);
	SPI_FLASH_CS_HIGH();

	SPI_FLASH_CS_LOW();			
	Flash_ReadWriteByte(0x06);                /* send write enable command */
	SPI_FLASH_CS_HIGH();			

	/* start sector greater than end sector */
	if (sec1 > sec2)
	{
		temp2 = sec1;
		sec1  = sec2;
		sec2  = temp2;
	}
	/* if the start and end sector number is equal, then erase single sector */
	if (sec1 == sec2)	
	{
		SPI_FLASH_CS_LOW();				
		Flash_ReadWriteByte(0x06);                            /* send write enable command */
		SPI_FLASH_CS_HIGH();			

		SecnHdAddr = SEC_SIZE * sec1;                         /* calculate the start sector addresses */
		SPI_FLASH_CS_LOW();	
		Flash_ReadWriteByte(0x20);                            /* send sector erase command */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFFFF) >> 16)); /* send 3 bytes of address */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFF) >> 8));
		Flash_ReadWriteByte(SecnHdAddr & 0xFF);
		SPI_FLASH_CS_HIGH();			
		do
		{
			SPI_FLASH_CS_LOW();			 
			Flash_ReadWriteByte(0x05);                    /* send read status register command */
			StatRgVal = Flash_ReadWriteByte(0x00);        /* save the status register value */
			SPI_FLASH_CS_HIGH();								
		}
		while (StatRgVal == 0x03);                      /* wait until the chips idle */
		return 0;			
	}

	/* use the most quickly erase function based on the starting sector and the end of the sector */	
	if (sec2 - sec1 == SEC_MAX)	
	{
		SPI_FLASH_CS_LOW();			
		Flash_ReadWriteByte(0x60);                      /* send chip erase command (60h or C7h) */
		SPI_FLASH_CS_HIGH();			
		do
		{
			SPI_FLASH_CS_LOW();			 
			Flash_ReadWriteByte(0x05);                    /* send read status register command */
			StatRgVal = Flash_ReadWriteByte(0x00);        /* save the status register value */
			SPI_FLASH_CS_HIGH();								
		}
		while (StatRgVal == 0x03);                      /* wait until the chips idle */
		return 0;
	}

	no_SecsToEr = sec2 - sec1 +1;                     /* get the number of sectors to be erased */
	CurSecToEr  = sec1;                               /* erase from the start sector */

	/* use 8 sector erase algorithm */
	while (no_SecsToEr >= 8)
	{
		SPI_FLASH_CS_LOW();				
		Flash_ReadWriteByte(0x06);                            /* send write enable command */
		SPI_FLASH_CS_HIGH();			

		SecnHdAddr = SEC_SIZE * CurSecToEr;                   /* calculate the start sector addresses */
		SPI_FLASH_CS_LOW();	
		Flash_ReadWriteByte(0x52);                            /* send 32KB erase command */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFFFF) >> 16)); /* send 3 bytes of address */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFF) >> 8));
		Flash_ReadWriteByte(SecnHdAddr & 0xFF);
		SPI_FLASH_CS_HIGH();			
		do
		{
			SPI_FLASH_CS_LOW();			 
			Flash_ReadWriteByte(0x05);                    /* send read status register command */
			StatRgVal = Flash_ReadWriteByte(0x00);        /* save the status register value */
			SPI_FLASH_CS_HIGH();								
		}
		while (StatRgVal == 0x03);                      /* wait until the chips idle */
		CurSecToEr  += 8;
		no_SecsToEr -=  8;
	}
	
	/* erase the sector by sector erase algorithm */
	while (no_SecsToEr >= 1)
	{
		SPI_FLASH_CS_LOW();				
		Flash_ReadWriteByte(0x06);                            /* send write enable command */
		SPI_FLASH_CS_HIGH();			

		SecnHdAddr = SEC_SIZE * CurSecToEr;                   /* calculate the start sector addresses */
		SPI_FLASH_CS_LOW();	
		Flash_ReadWriteByte(0x20);                            /* send sector erase command */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFFFF) >> 16)); /* send 3 bytes of address */
		Flash_ReadWriteByte(((SecnHdAddr & 0xFFFF) >> 8));
		Flash_ReadWriteByte(SecnHdAddr & 0xFF);
		SPI_FLASH_CS_HIGH();			
		do
		{
			SPI_FLASH_CS_LOW();			 
			Flash_ReadWriteByte(0x05);                     /* send read status register command */
			StatRgVal = Flash_ReadWriteByte(0x00);         /* save the status register value */
			SPI_FLASH_CS_HIGH();								
		}
		while (StatRgVal == 0x03);                       /* wait until the chips idle */
		CurSecToEr  += 1;
		no_SecsToEr -=  1;
	}
	/* erase end, recovery status register information */
	SPI_FLASH_CS_LOW();			
	Flash_ReadWriteByte(0x06);                  /* send write enable command */
	SPI_FLASH_CS_HIGH();			

	SPI_FLASH_CS_LOW();			
	Flash_ReadWriteByte(0x50);                  /* enable the status register to be written */
	SPI_FLASH_CS_HIGH();			
	SPI_FLASH_CS_LOW();			
	Flash_ReadWriteByte(0x01);                  /* send write status register command */
	Flash_ReadWriteByte(temp1);                 /* restore the status register value */
	SPI_FLASH_CS_HIGH();    
	return 0;
}


/**
  * @brief  spi flash test
  * @param  None
  * @retval None
  */
void SPI_FLASH_Test(void)
{
	int  ChipID = 0;
	SSTF016B_RdID(Jedec_ID, &ChipID);
	ChipID &= ~0xff000000;                      /* only 24 bit data is reserved */
	/* 0x00BF254A ID is SST25VF032B, 0xBF2541 ID is SST25VF016B */
	if (ChipID != 0xBF2541 && ChipID != 0xBF254A)    /* if ID is not right, enter while */
	{
		while(1);
	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
