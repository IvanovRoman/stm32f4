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

/* In stdio.h file is everything related to output stream */
#include "stdio.h"

/* Include my libraries here */
#include "WavSample.c"
#include "audio.h"

/* Private functions ---------------------------------------------------------*/
void USART_Configuration(void);
void PLLSAI_Config(void);

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {

	PLLSAI_Config();
	USART_Configuration();

	printf("*****************************************************************\r\n");
	printf("*                                                               *\r\n");
	printf("*  Thank you for using SOM-STM32F429IG Development Board ! ^_^  *\r\n");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");
	
	memcpy(&userWav,WAV_DATA,sizeof(WavHeader));
	DisplayWavInfo();
	StartPlay(userWav.sampleRate);
	wm8978_SetEarVolume(VOLUME_MAX);
	while (1) 
	{
		Audio_Wav_Play( (uint8_t*)WAV_DATA, sizeof(WAV_DATA) );
	}
}

/**
  * @brief  Configure PLLSAI.
  * @param  None
  * @retval None
  */
void PLLSAI_Config(void)
{ 
  /* Configure PLLSAI prescalers */
  /* PLLSAI_VCO : VCO_429M */
  /* SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz */
  RCC_PLLSAIConfig(429, 2, 4);
  
  /* SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */  
  RCC_SAIPLLSAIClkDivConfig(19);
  
  /* Configure PLLI2S prescalers */
  /* PLLI2S_VCO : VCO_344M */
  /* SAI_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 344/7 = 49.142 Mhz */
  RCC_PLLI2SConfig(344, 7, 4);
  
  /* SAI_CLK_x = SAI_CLK(first level)/PLLI2SDIVQ = 49.142/1 = 49.142 Mhz */  
  RCC_SAIPLLI2SClkDivConfig(1);
  
  /* Configure Clock source for SAI Block A */
  RCC_SAIBlockACLKConfig(RCC_SAIACLKSource_PLLSAI);
  
  /* Configure Clock source for SAI Block B */
  RCC_SAIBlockBCLKConfig(RCC_SAIBCLKSource_PLLI2S);
  
  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  
  /* Wait till PLLSAI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET) 
  {
  }
  
  /* Enable PLLI2S Clock */
  RCC_PLLI2SCmd(ENABLE);
  
  /* Wait till PLLI2S is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET) 
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
