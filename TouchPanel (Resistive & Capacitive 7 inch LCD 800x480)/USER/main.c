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
#include "GLCD.h" 
#include "fmc_sdram.h"
#include "xpt2046.h"

/* In stdio.h file is everything related to output stream */
#include "stdio.h"

/* Include my libraries here */

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void USART_Configuration(void);

#include "ft5x06.h"
#include "ts_calibrate.h"

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {
	FMC_SDRAM_Init();
	LCD_Init();
	LCD_Clear(LCD_COLOR_BLACK);

	USART_Configuration();
	touchpanel_init();

	if( touch_type == RESISTIVE_TOUCH )
	{
		ts_calibrate(LCD_X_SIZE, LCD_Y_SIZE);
	}

	while (1) 
	{
		if( touch_type == RESISTIVE_TOUCH )
		{
			getDisplayPoint(&display, Read_Value(), &matrix ) ;
			ts_draw_point(display.x,display.y,LCD_COLOR_BLUE );
		}
		else
		{		
			ft5x0x_report_value();
			ts_draw_point( ts_event.point[0].x, ts_event.point[0].y , 0xF800);
		}
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
