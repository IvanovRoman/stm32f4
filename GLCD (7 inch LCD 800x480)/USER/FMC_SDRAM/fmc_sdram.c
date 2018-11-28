/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fmc_sdram.c
** Descriptions:            The FMC SDRAM application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-06-19
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

/* Includes ------------------------------------------------------------------*/
#include "fmc_sdram.h"

/**
  * @brief  delay function
  * @param  nCount: Delay value
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--);
}

/**
 * @brief FMC SDRAM configuration for GPIO.
 * @param None
 * @retval None
 */
static void FMC_SDRAM_GPIOInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO AHB Clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | 
                         RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH , ENABLE);

  /****************************************************************************
   * PH2 --> SDCKE0  | PE0 --> NBL0  | PF0 --> A0    | PG0 --> A10            *
   * PH3 --> SDNE0   | PE1 --> NBL1  | PF1 --> A1    | PG1 --> A11            *
   * ----------------| PE8 --> D5    | PF2 --> A2    | PG2 --> A12            *
   * PC0 --> SDNWE   | PE9 --> D6    | PF3 --> A3    | PG4 --> BA0            *
   * ----------------| PE10--> D7    | PF4 --> A4    | PG5 --> BA1            *
   * PD0 --> D2      | PE11--> D8    | PF5 --> A5    | PG8 --> SDCLK          *
   * PD1 --> D3      | PE12--> D9    | PF11--> SDNRAS| PG15--> SDNCAS         *
   * PD8 --> D13     | PE13--> D10   | PF12--> A6    |                        *
   * PD9 --> D14     | PE14--> D11   | PF13--> A7    |                        *
   * PD10--> D15     | PE15--> D12   | PF14--> A8    |                        *
   * PD14--> D0      | PE7 --> D4    | PF15--> A9    |                        *
   * PD15--> D1      |                                                        *
   ****************************************************************************/
	
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	/* PH2 --> SDCKE0, PH3 --> SDNE0*/
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource2, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource3, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  | GPIO_Pin_3;
  GPIO_Init(GPIOH, &GPIO_InitStructure);	
	
	/* PC0 --> SDNWE*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	

	/* PD0 --> D2, PD1 --> D3, PD8 --> D13, PD9 --> D14, PD10 --> D15, PD14 --> D0, PD15 --> D1 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  |GPIO_Pin_1  |GPIO_Pin_8 |GPIO_Pin_9 |
                                GPIO_Pin_10 |GPIO_Pin_14 |GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);	

	/* PE0 --> NBL0, PE1 --> NBL1, PE7 --> D4, PE8 --> D5, PE9 --> D6, PE10 --> D7, PE11 --> D8, 
	   PE12 --> D9, PE13 --> D10, PE14 --> D11, PE15 --> D12 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_7 | GPIO_Pin_8  |
                                GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11| GPIO_Pin_12 |
                                GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PF0 --> A0, PF1 --> A1, PF2 --> A2, PF3 --> A3, PF4 --> A4, PF5 --> A5, 
	   PF11 --> SDNRAS, PF12 --> A6, PF13 --> A7, PF14 --> A8, PF15 --> A9 */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource11 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  |
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_11 | GPIO_Pin_12 |
                                GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;      
  GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* PG0 --> A10, PG1 --> A11, PG2 --> A12, PG4 --> BA0, PG5 --> BA1, PG8 --> SDCLK, 
	   PG15 --> SDNCAS */	
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource8 , GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource15 , GPIO_AF_FMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 
                                | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
 * @brief This function configure FMC peripheral.
 * @param None
 * @retval None
 */
static void FMC_SDRAM_Initsquence(void)
{
  FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
  uint32_t tmpr = 0;
  
  /* Step 1 --------------------------------------------------------------------*/
  /* Configure a clock configuration enable command */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
  
/* Step 2 --------------------------------------------------------------------*/
  /* Insert 100 ms delay */
  delay(10);
    
/* Step 3 --------------------------------------------------------------------*/
  /* Configure a PALL (precharge all) command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 4 --------------------------------------------------------------------*/
  /* Configure a Auto-Refresh command */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the  first command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the second command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 5 --------------------------------------------------------------------*/
  /* Program the external memory mode register */
  tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                   SDRAM_MODEREG_CAS_LATENCY_3           |
                   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  /* Configure a load Mode register command*/ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
	/* Step 6 --------------------------------------------------------------------*/

  /* Set the refresh rate counter */
  /* (7.81 us x Freq) - 20 */
  /* Set the device refresh counter */
  FMC_SetRefreshCount(28);
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
}

/**
 * @brief FMC SDRAM Initialization.
 * @param None
 * @retval None
 */
void FMC_SDRAM_Init(void)
{
  FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
  FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 

  FMC_SDRAM_GPIOInit();
	
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
  /****************************************************************************
   * TMRD = 40ns
   * TXSR = 80ns
   * TRAS = 44ns
   * TRC  = 70ns
   * TWR  = 40ns
   * TRP  = 20ns
   * TRCD = 20ns
   ***************************************************************************/
  /* TMRD: 2 Clock cycles */	
  FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay = 4;
  /* TXSR: min=72ns (6x11.90ns) */      
  FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 8;  
  /* TRAS: min=42ns (4x11.90ns) max=100k (ns) */
  FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime = 4; 
  /* TRC:  min=60 (6x11.90ns) */
  FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay = 7; 
  /* TWR:  2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime = 4;
  /* TRP:  15ns => 2x11.90ns */
  FMC_SDRAMTimingInitStructure.FMC_RPDelay = 2;
  /* TRCD: 15ns => 2x11.90ns */ 
  FMC_SDRAMTimingInitStructure.FMC_RCDDelay = 2; 
	
  FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank1_SDRAM;
  FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;
  FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;
  FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
  FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_3; 
  FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
  FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2;  
  FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;
  FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
  FMC_SDRAMInit(&FMC_SDRAMInitStructure);

  FMC_SDRAM_Initsquence();
}

/**
 * @brief test SDRAM
 * @param None
 * @retval None
 */
void FMC_SDRAM_Test(void)
{
	__IO uint32_t addr;

	printf("SDRAM 32M size test....\r\n");

	for( addr = SDRAM_BANK_ADDR; addr < SDRAM_BANK_ADDR + SDRAM_SIZE; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SDRAM_BANK_ADDR; addr < SDRAM_BANK_ADDR + SDRAM_SIZE; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}
			
	if( addr >= SDRAM_BANK_ADDR + SDRAM_SIZE )
	{
	    printf("SDRAM 32M size test OK....\r\n");
	}
	else
	{
		printf("SDRAM 32M size test error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}
	
	printf("\r\n");
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
