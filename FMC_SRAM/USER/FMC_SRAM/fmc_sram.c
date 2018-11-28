/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               FMC_sram.c
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

/* Includes ------------------------------------------------------------------*/
#include "fmc_sram.h"
#include <stdio.h>
  
/*******************************************************************************
* Function Name  : FMC_SRAM_Init
* Description    : Configures the FMC and GPIOs to interface with the SRAM memory.
*				           This function must be called before any write/read operation
*				           on the SRAM.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void FMC_SRAM_Init(void)
{ 
  FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
  FMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable FMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE); 

  /***********************************************************************
   * PD0 --> D2    | PE0 --> NBL0 | PF0 --> A0  | PG0 --> A10            *
   * PD1 --> D3    | PE1 --> NBL1 | PF1 --> A1  | PG1 --> A11            *
   * PD4 --> F_NOE | PE3 --> A19  | PF2 --> A2  | PG2 --> A12            *
   * PD5 --> F_NWE | PE4 --> A20  | PF3 --> A3  | PG3 --> A13            *
   * PD7 --> F_CS  | PE5 --> A21  | PF4 --> A4  | PG4 --> BA0            *
   * PD8 --> D13   | PE7 --> D4   | PF5 --> A5  | PG5 --> BA1            *
   * PD9 --> D14   | PE8 --> D5   | PF12--> A6  | PG9 --> N_CS           *
   * PD10--> D15   | PE9 --> D6   | PF13--> A7  | PG12 --> R_CS          *
   * PD11--> A16   | PE10--> D7   | PF14--> A8  |                        *
   * PD12--> A17   | PE11--> D8   | PF15--> A9  |                        *
   * PD13--> D0    | PE12--> D9   |                                      *
   * PD14--> D1    | PE13--> D10  |                                      *
   * PD15--> A18   | PE14--> D11  |                                      *
   *               | PE15--> D12  |                                      *
   ***********************************************************************/  

  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	/* PD0 --> D2, PD1 --> D3, PD4 --> F_NOE, PD5 --> F_NWE, PD7 --> F_CS, PD8 --> D13, PD9 --> FD14, 
	PD10 --> D15, PD11 --> A16, PD12 --> A17, PD13 --> D0, PD14 --> D1, PD15 --> A18 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;											
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* PE0 --> NBL0, PE1 --> NBL1, PE3 --> A19, PE4 --> A20, PE5 --> A21, PE7 --> D4, PE8 --> D5, 
	   PE9 --> D6, PE10 --> D7, PE11 --> D8, PE12 --> D9, PE13 --> D10, PE14 --> D11, PE15 --> D12 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4  | GPIO_Pin_5 |
	                              GPIO_Pin_7| GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11| 
																GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PF0 --> A0, PF1 --> A1, PF2 --> A2, PF3 --> A3, PF4 --> A4, PF5 --> A5, PF12 --> A6, 
	   PF13 --> A7, PF14 --> A8, PF15 --> A9 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_12 | GPIO_Pin_13 | 
																GPIO_Pin_14 | GPIO_Pin_15;      
  GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* PG0 --> A10, PG1 --> A11, PG2 --> A12, PG3 --> A13, PG4 --> BA0, PG5 --> BA1, 
     PG9 --> N_CS, PG12 --> R_CS	*/
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12 , GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  |GPIO_Pin_9 | GPIO_Pin_12;      
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /*-- FMC Configuration ------------------------------------------------------*/
  p.FMC_AddressSetupTime = 6;
  p.FMC_AddressHoldTime = 0;
  p.FMC_DataSetupTime = 8;
  p.FMC_BusTurnAroundDuration = 1;
  p.FMC_CLKDivision = 0;
  p.FMC_DataLatency = 0;
  p.FMC_AccessMode = FMC_AccessMode_A;

  FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM4;
  FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
  FMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_PSRAM;
  FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
  FMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
  FMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;  
  FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
  FMC_NORSRAMInitStructure.FMC_WrapMode = FMC_WrapMode_Disable;
  FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
  FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
  FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
  FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Disable;
  FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
  FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &p;
  FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &p;

  FMC_NORSRAMInit(&FMC_NORSRAMInitStructure); 

  /* Enable FMC Bank1_SRAM4 Bank */
  FMC_NORSRAMCmd(FMC_Bank1_NORSRAM4, ENABLE); 
}

/*******************************************************************************
* Function Name  : FMC_SRAM_Test
* Description    : test SRAM
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_SRAM_Test(void)
{
	__IO uint32_t addr;

	printf("SRAM 512K size test....\r\n");

	for( addr = SRAM_BLANK1_START_ADDR; addr < SRAM_BLANK1_END_ADDR; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK1_START_ADDR; addr < SRAM_BLANK1_END_ADDR; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}
			
	if( addr >= SRAM_BLANK1_END_ADDR )
	{
	    printf("SRAM 512K size test OK....\r\n");
	}
	else
	{
		printf("SRAM 512K size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}
		
	printf("SRAM 1M size test....\r\n");

	for( addr = SRAM_BLANK2_START_ADDR; addr < SRAM_BLANK2_END_ADDR; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK2_START_ADDR; addr < SRAM_BLANK2_END_ADDR; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}
			
	if( addr >= SRAM_BLANK2_END_ADDR )
	{
	    printf("SRAM 1M size test OK....\r\n");
	}
	else
	{
		printf("SRAM 1M size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}

	printf("SRAM 4M size test....\r\n");

	for( addr = SRAM_BLANK3_START_ADDR; addr < SRAM_BLANK3_END_ADDR ; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK3_START_ADDR; addr < SRAM_BLANK3_END_ADDR ; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}

	if( addr >= SRAM_BLANK3_END_ADDR )
	{
	    printf("SRAM 4M size test OK....\r\n");
	}
	else
	{
		printf("SRAM 4M size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}

	printf("SRAM 8M size test....\r\n");

	for( addr = SRAM_BLANK4_START_ADDR; addr < SRAM_BLANK4_END_ADDR ; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK4_START_ADDR; addr < SRAM_BLANK4_END_ADDR ; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}

	if( addr >= SRAM_BLANK4_END_ADDR )
	{
	    printf("SRAM 8M size test OK....\r\n");
	}
	else
	{
		printf("SRAM 8M size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}
	printf("\r\n");
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
