/**
 *	Keil project template
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@conf		PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
/* Include core modules */
#include "stm32f4xx.h"
#include "fmc_nand.h"
/* In stdio.h file is everything related to output stream */
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/
uint8_t ReadBuf[NAND_PAGE_DATA_SIZE];
uint8_t WriteBuf[NAND_PAGE_DATA_SIZE];
uint8_t InvalidBlockTable[NAND_BLOCK_NUM];

/* Private functions ---------------------------------------------------------*/
void USART_Configuration(void);

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {
	uint32_t i;
	uint32_t OperateBlock;

	USART_Configuration();
	NAND_Init();

	FMC_NAND_Test();

	for ( i = 0; i < NAND_BLOCK_NUM; i++ ) {
	if (FMC_NAND_EraseBlock(i) == NAND_TIMEOUT_ERROR) 
	{
		InvalidBlockTable[i] = 1;
		printf("Erase NAND Flash fail at block: %d\r\n", i);
	}
	}

	memset( ReadBuf,0,sizeof(ReadBuf) );

	for ( i = 0; i < NAND_PAGE_DATA_SIZE; i++ ) {
		WriteBuf[i] = i % 255;
	}

	OperateBlock = 0;
	printf("Write a block of 2K data to NAND Flash...\r\n");
	if (FMC_NAND_WritePage(&WriteBuf[0],		OperateBlock * NAND_PAGES_PER_BLOCK, 0, NAND_PAGE_DATA_SIZE) == NAND_FAIL) 
	{
		printf("Writing fail, testing terminated!\r\n");
	}

	printf("Read back a block of 2K data from NAND Flash...\r\n");

	if (FMC_NAND_ReadPage(&ReadBuf[0],		OperateBlock * NAND_PAGES_PER_BLOCK, 0, NAND_PAGE_DATA_SIZE) == NAND_FAIL) 
	{                    
		printf("Reading fail, testing terminated!\r\n");
	}

	printf("Verify data...\r\n");

	if( memcmp( (char*)WriteBuf, (char*)ReadBuf, NAND_PAGE_DATA_SIZE ) == 0 )
	{
		printf("Nand Flash is OK \r\n");
	}
	else
	{
		printf("Nand Flash is error \r\n");
	}

	printf("Verifying okay! Testing terminated!\r\n");
	
	while (1) 
	{
	}
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Configure GPIO to the USART1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /**************************************************************************** 
	 *    baud rate:     115200
   *    Word length:   8bit
   *    Stop bit:      bit
   *    Flow control:  None
   ****************************************************************************/
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);
}

/* We need to implement own __FILE struct */
/* FILE struct is used from __FILE */
struct __FILE {
    int dummy;
};
 
/* You need this if you want use printf */
/* Struct FILE is implemented in stdio.h */
FILE __stdout;
 
int fputc(int ch, FILE *f) {
	
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* Do your stuff here */
    /* Send your custom byte */
		/* Send byte to USART */
		USART_SendData(USART1, ch);
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);   
	
    /* If everything is OK, you have to return character written */
    return ch;
    /* If character is not correct, you can return EOF (-1) to stop writing */
    //return -1;
}

int fgetc(FILE *f) 
{
    int ch;
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    ch = USART_ReceiveData(USART1);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (uint8_t) ch);
    return ch;
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
