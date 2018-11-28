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
/* Include my libraries here */

/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);
void Delay(uint32_t nCount);

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {
	
	GPIO_Configuration();
	while (1) 
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);   //LED1 light;
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);   //LED2 light;
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);   //core board LED2 light;
		Delay(0xffffff);
		GPIO_SetBits(GPIOB,GPIO_Pin_0);     //LED1 light;
		GPIO_SetBits(GPIOB,GPIO_Pin_1);     //LED2 light;
		GPIO_SetBits(GPIOD,GPIO_Pin_3);     //core board LED2 light;
		Delay(0xffffff);
	}
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD , ENABLE); 						 
  /**
  *  LED1 -> PB0 , LED2 -> PB1 , STATUS_LED -> PD3
  */					 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Delay(uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
