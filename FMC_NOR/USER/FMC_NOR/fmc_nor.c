/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               FMC_nor.c
** Descriptions:            The FMC NOR application function
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

/* Includes ------------------------------------------------------------------*/
#include "FMC_nor.h"

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE                0x400
#define WRITE_READ_ADDR            0x8000

/* Private variables ---------------------------------------------------------*/
static uint16_t TxBuffer[BUFFER_SIZE];
static uint16_t RxBuffer[BUFFER_SIZE];

/* Private functions ---------------------------------------------------------*/
NOR_Status FMC_NOR_GetStatus(uint32_t Timeout);

/**
  * @brief Configures the FMC and GPIOs to interface with the NOR memory,
  *        This function must be called before any write/read operation on the NOR.
  * @param None
  * @retval None
  */
void FMC_NOR_Init(void)
{
  FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
  FMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure;

  /***********************************************************************
   * PD0 --> D2    | PE0 --> NBL0 | PF0 --> A0  | PG0 --> A10            *
   * PD1 --> D3    | PE1 --> NBL1 | PF1 --> A1  | PG1 --> A11            *
   * PD4 --> F_NOE | PE3 --> A19  | PF2 --> A2  | PG2 --> A12            *
   * PD5 --> F_NWE | PE4 --> A20  | PF3 --> A3  | PG3 --> A13            *
   * PD7 --> F_CS  | PE5 --> A21  | PF4 --> A4  | PG4 --> BA0            *
   * PD8 --> D13   | PE7 --> D4   | PF5 --> A5  | PG5 --> BA1            *
   * PD9 --> D14   | PE8 --> D5   | PF12--> A6  |                        *
   * PD10--> D15   | PE9 --> D6   | PF13--> A7  |                        *
   * PD11--> A16   | PE10--> D7   | PF14--> A8  |                        *
   * PD12--> A17   | PE11--> D8   | PF15--> A9  |                        *
   * PD13--> D0    | PE12--> D9   |                                      *
   * PD14--> D1    | PE13--> D10  |                                      *
   * PD15--> A18   | PE14--> D11  |                                      *
   * PD6 --> NWAIT | PE15--> D12  |                                      *
   ***********************************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE); 

	/* PD0 --> FMC_D2, PD1 --> FMC_D3, PD4 --> FMC_NOE, PD5 --> FMC_NWE, PD7 --> FMC_CS, PD8 --> FMC_D13, PD9 --> FMC_D14, 
	   PD10 --> FMC_D15, PD11 --> FMC_A16, PD12 --> FMC_A17, PD13 --> FMC_D0, PD14 --> FMC_D1, PD15 --> FMC_A18 */
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
	                              GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* PE0 --> NBL0, PE1 --> NBL1, PE3 --> A19, PE4 --> A20, PE5 --> A21, PE7 --> D4, PE8 --> D5, 
	   PE9 --> D6, PE10 --> D7, PE11 --> D8, PE12 --> D9, PE13 --> D10, PE14 --> D11, PE15 --> D12 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF_FMC); 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_FMC); 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_FMC); 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC); 
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FMC);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
																GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);


	/* PF0 --> A0, PF1 --> A1, PF2 --> A2, PF3 --> A3, PF4 --> A4, PF5 --> A5, PF12 --> A6, 
	   PF13 --> A7, PF14 --> A8, PF15 --> A9 */
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource2, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource3, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource4, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource5, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource13, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource14, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource15, GPIO_AF_FMC);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* PG0 --> A10, PG1 --> A11, PG2 --> A12, PG3 --> A13, PG4 --> BA0, PG5 --> BA1 */
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource0, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource1, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource3, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource4, GPIO_AF_FMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource5, GPIO_AF_FMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;                            
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* NWAIT Configuration */
	/* PD6 is a busy pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* input mode */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  /*-- FMC Configuration ----------------------------------------------------*/
	p.FMC_AddressSetupTime = 0x06;			/* 0x05 ok£¬ 0x04 error */
	p.FMC_AddressHoldTime = 0x01;
	p.FMC_DataSetupTime = 0x0C;			/* 0x0B ok£¬ 0x0A error */
	p.FMC_BusTurnAroundDuration = 0x00;
	p.FMC_CLKDivision = 0x00;
	p.FMC_DataLatency = 0x00;
	p.FMC_AccessMode = FMC_AccessMode_B;

  FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
  FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
  FMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_NOR;
  FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
  FMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
  FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
  FMC_NORSRAMInitStructure.FMC_WrapMode = FMC_WrapMode_Disable;
  FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
  FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
  FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
  FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Disable;
  FMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;
  FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
  FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &p;
  FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &p;

  FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);

  /* Enable FMC Bank1_NOR Bank */
  FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
}

/******************************************************************************
* Function Name  : FMC_NOR_ReadID
* Description    : Reads NOR memory's Manufacturer and Device Code.
* Input          : - NOR_ID: pointer to a NOR_IDTypeDef structure which will hold
*                    the Manufacturer and Device Code.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_NOR_ReadID(NOR_IDTypeDef* NOR_ID)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0090);

  NOR_ID->Manufacturer_Code = *(vu16 *) ADDR_SHIFT(0x0000);
  NOR_ID->Device_Code1 = *(vu16 *) ADDR_SHIFT(0x0001);
  NOR_ID->Device_Code2 = *(vu16 *) ADDR_SHIFT(0x000E);
  NOR_ID->Device_Code3 = *(vu16 *) ADDR_SHIFT(0x000F);
}

/*******************************************************************************
* Function Name  : FMC_NOR_EraseBlock
* Description    : Erases the specified Nor memory block.
* Input          : - BlockAddr: address of the block to erase.
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_EraseBlock(uint32_t BlockAddr)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE((NOR_FLASH_START_ADDR + BlockAddr), 0x30);

  return (FMC_NOR_GetStatus(BlockErase_Timeout));
}

/*******************************************************************************
* Function Name  : FMC_NOR_EraseChip
* Description    : Erases the entire chip.
* Input          : None                      
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_EraseChip(void)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0010);

 return (FMC_NOR_GetStatus(ChipErase_Timeout));
}

/******************************************************************************
* Function Name  : FMC_NOR_WriteHalfWord
* Description    : Writes a half-word to the NOR memory. 
* Input          : - WriteAddr : NOR memory internal address to write to.
*                  - Data : Data to write. 
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00A0);
  NOR_WRITE((NOR_FLASH_START_ADDR + WriteAddr), Data);

  return (FMC_NOR_GetStatus(Program_Timeout));
}

/*******************************************************************************
* Function Name  : FMC_NOR_WriteBuffer
* Description    : Writes a half-word buffer to the FMC NOR memory. 
* Input          : - pBuffer : pointer to buffer. 
*                  - WriteAddr : NOR memory internal address from which the data 
*                    will be written.
*                  - NumHalfwordToWrite : number of Half words to write. 
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  NOR_Status status = NOR_ONGOING; 

  do
  {
    /* Transfer data to the memory */
    status = FMC_NOR_WriteHalfWord(WriteAddr, *pBuffer++);
    WriteAddr = WriteAddr + 2;
    NumHalfwordToWrite--;
  }
  while((status == NOR_SUCCESS) && (NumHalfwordToWrite != 0));
  
  return (status); 
}

/*******************************************************************************
* Function Name  : FMC_NOR_ProgramBuffer
* Description    : Writes a half-word buffer to the FMC NOR memory. This function 
*                  must be used only with S29GL128P NOR memory.
* Input          : - pBuffer : pointer to buffer. 
*                  - WriteAddr: NOR memory internal address from which the data 
*                    will be written.
*                  - NumHalfwordToWrite: number of Half words to write.
*                    The maximum allowed value is 32 Half words (64 bytes).
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{       
  uint32_t lastloadedaddress = 0x00;
  uint32_t currentaddress = 0x00;
  uint32_t endaddress = 0x00;

  /* Initialize variables */
  currentaddress = WriteAddr;
  endaddress = WriteAddr + NumHalfwordToWrite - 1;
  lastloadedaddress = WriteAddr;

  /* Issue unlock command sequence */
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA);

  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);  

  /* Write Write Buffer Load Command */
  NOR_WRITE(ADDR_SHIFT(WriteAddr), 0x0025);
  NOR_WRITE(ADDR_SHIFT(WriteAddr), (NumHalfwordToWrite - 1));

  /* Load Data into NOR Buffer */
  while(currentaddress <= endaddress)
  {
    /* Store last loaded address & data value (for polling) */
    lastloadedaddress = currentaddress;
 
    NOR_WRITE(ADDR_SHIFT(currentaddress), *pBuffer++);
    currentaddress += 1; 
  }

  NOR_WRITE(ADDR_SHIFT(lastloadedaddress), 0x29);
  
  return(FMC_NOR_GetStatus(Program_Timeout));
}

/******************************************************************************
* Function Name  : FMC_NOR_ReadHalfWord
* Description    : Reads a half-word from the NOR memory. 
* Input          : - ReadAddr : NOR memory internal address to read from.
* Output         : None
* Return         : Half-word read from the NOR memory
* Attention		 : None
*******************************************************************************/
uint16_t FMC_NOR_ReadHalfWord(uint32_t ReadAddr)
{
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
  NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055);  
  NOR_WRITE((NOR_FLASH_START_ADDR + ReadAddr), 0x00F0 );

  return (*(vu16 *)((NOR_FLASH_START_ADDR + ReadAddr)));
}

/*******************************************************************************
* Function Name  : FMC_NOR_ReadBuffer
* Description    : Reads a block of data from the FMC NOR memory.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the NOR memory.
*                  - ReadAddr : NOR memory internal address to read from.
*                  - NumHalfwordToRead : number of Half word to read.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE((NOR_FLASH_START_ADDR + ReadAddr), 0x00F0);

  for(; NumHalfwordToRead != 0x00; NumHalfwordToRead--) /* while there is data to read */
  {
    /* Read a Halfword from the NOR */
    *pBuffer++ = *(vu16 *)((NOR_FLASH_START_ADDR + ReadAddr));
    ReadAddr = ReadAddr + 2; 
  }  
}

/******************************************************************************
* Function Name  : FMC_NOR_ReturnToReadMode
* Description    : Returns the NOR memory to Read mode.
* Input          : None
* Output         : None
* Return         : NOR_SUCCESS
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_ReturnToReadMode(void)
{
  NOR_WRITE(NOR_FLASH_START_ADDR, 0x00F0);

  return (NOR_SUCCESS);
}

/******************************************************************************
* Function Name  : FMC_NOR_Reset
* Description    : Returns the NOR memory to Read mode and resets the errors in
*                  the NOR memory Status Register.
* Input          : None
* Output         : None
* Return         : NOR_SUCCESS
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_Reset(void)
{
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
  NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055); 
  NOR_WRITE(NOR_FLASH_START_ADDR, 0x00F0); 

  return (NOR_SUCCESS);
}

/******************************************************************************
* Function Name  : FMC_NOR_GetStatus
* Description    : Returns the NOR operation status.
* Input          : - Timeout: NOR progamming Timeout
* Output         : None
* Return         : NOR_Status:The returned value can be: NOR_SUCCESS, NOR_ERROR
*                  or NOR_TIMEOUT
* Attention		 : None
*******************************************************************************/
NOR_Status FMC_NOR_GetStatus(uint32_t Timeout)
{ 
  uint16_t val1 = 0x00, val2 = 0x00;
  NOR_Status status = NOR_ONGOING; 
  uint32_t timeout = Timeout;

  /* Poll on NOR memory Ready/Busy signal ------------------------------------*/
  while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) != RESET) && (timeout > 0)) 
  {
    timeout--;
  }

  timeout = Timeout;
  
  while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET) && (timeout > 0))   
  {
    timeout--;
  }
  
  /* Get the NOR memory operation status -------------------------------------*/
  while((Timeout != 0x00) && (status != NOR_SUCCESS))
  {
    Timeout--;

	  /* Read DQ6 and DQ5 */
    val1 = *(vu16 *)(NOR_FLASH_START_ADDR);
    val2 = *(vu16 *)(NOR_FLASH_START_ADDR);

    /* If DQ6 did not toggle between the two reads then return NOR_Success */
    if((val1 & 0x0040) == (val2 & 0x0040)) 
    {
      return NOR_SUCCESS;
    }

    if((val1 & 0x0020) != 0x0020)
    {
      status = NOR_ONGOING;
    }

    val1 = *(vu16 *)(NOR_FLASH_START_ADDR);
    val2 = *(vu16 *)(NOR_FLASH_START_ADDR);
    
    if((val1 & 0x0040) == (val2 & 0x0040)) 
    {
      return NOR_SUCCESS;
    }
    else if((val1 & 0x0020) == 0x0020)
    {
      return NOR_ERROR;
    }
  }

  if(Timeout == 0x00)
  {
    status = NOR_TIMEOUT;
  } 

  /* Return the operation status */
  return (status);
}

/******************************************************************************
* Function Name  : FMC_NOR_Test
* Description    : NOR Test
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FMC_NOR_Test(void)
{ 
  uint16_t index;
  NOR_IDTypeDef NOR_ID;

  /* Initialize the FMC NOR Flash Interface */
  FMC_NOR_Init();
  /* Set the NOR read modes */
  FMC_NOR_ReturnToReadMode();

  FMC_NOR_ReadID(&NOR_ID);
  printf("Nor Flash ID = %04X,%04X,%04X,%04X  ", NOR_ID.Manufacturer_Code , NOR_ID.Device_Code1,
                                                 NOR_ID.Device_Code2 , NOR_ID.Device_Code3 );
  FMC_NOR_ReturnToReadMode(); /* after read ID, must be return to read mode */

  if( ( NOR_ID.Manufacturer_Code == 0x0001 ) && (NOR_ID.Device_Code1 == 0x227E) &&
	  ( NOR_ID.Device_Code2 == 0x2212 ) && ( NOR_ID.Device_Code3 == 0x2200 ) )
  {
     printf("Type = AM29LV128ML\r\n");
  }
  else if ( ( NOR_ID.Manufacturer_Code == 0x0001 ) && ( NOR_ID.Device_Code1 == 0x227E ) &&
		    ( NOR_ID.Device_Code2 == 0x2221 ) && ( NOR_ID.Device_Code3 == 0x2201 ) )
  {
	 printf("Type = S29GL128P\r\n");
  }
  else if ( ( NOR_ID.Manufacturer_Code == 0x0020 ) && ( NOR_ID.Device_Code1 == 0x227E ) &&
		      ( NOR_ID.Device_Code2 == 0x2220 ) && ( NOR_ID.Device_Code3 == 0x2200 ) )
  {
	 printf("Type = M29DW128F\r\n");
  }
  else if ( ( NOR_ID.Manufacturer_Code == 0x00EC ) && ( NOR_ID.Device_Code1 == 0x257E ) &&
		      ( NOR_ID.Device_Code2 == 0x2508 ) && ( NOR_ID.Device_Code3 == 0x2501 ) )
  {
	 printf("Type = K5L2731CAA,  128Mb NOR Flash + 32Mb UtRAM \r\n");
  }

  else if ( ( NOR_ID.Manufacturer_Code == 0x00EC ) && ( NOR_ID.Device_Code1 == 0x2206 ) &&
		      ( NOR_ID.Device_Code2 == 0x2207 ) && ( NOR_ID.Device_Code3 == 0x301B ) )
  {
	 printf("Type = K5L5666ATB,  256Mb NOR Flash + 64Mb UtRAM \r\n");
  }	

  else if ( ( NOR_ID.Manufacturer_Code == 0x00EC ) && ( NOR_ID.Device_Code1 == 0x227E ) &&
		      ( NOR_ID.Device_Code2 == 0x2263 ) && ( NOR_ID.Device_Code3 == 0x2260 ) )
  {
	 printf("Type = K5L5563CAA,  256Mb NOR Flash + 64Mb UtRAM \r\n");
  }		
  else
  {
	 printf("Type = Unknow\r\n");
  }

  /* Erase the NOR memory block to write on */
  FMC_NOR_EraseBlock(WRITE_READ_ADDR);

  /* Write data to FMC NOR memory */
  /* Fill the buffer to send */
  for (index = 0; index < BUFFER_SIZE; index++ )
  {
    TxBuffer[index] = index + 0x3210;
  }

  FMC_NOR_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

  /* Read data from FMC NOR memory */
  FMC_NOR_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  

  if( memcmp( (char*)TxBuffer, (char*)RxBuffer, BUFFER_SIZE ) == 0 )
  {
     printf("Nor Flash is OK \r\n");
  }
  else
  {
     printf("Nor Flash is error \r\n");
  }
  printf("\r\n");
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
