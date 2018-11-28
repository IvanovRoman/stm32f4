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

/* Include my libraries here */

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

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
	LCD_Clear(LCD_COLOR_RED);
	GUI_Text( ( LCD_X_SIZE - 120 ) / 2, LCD_Y_SIZE / 2 - 8, "SOM-STM32F429IG", LCD_COLOR_WHITE, LCD_COLOR_BLUE);
	GUI_Text( ( LCD_X_SIZE - 136 ) / 2, LCD_Y_SIZE / 2 + 8, "Development Board", LCD_COLOR_WHITE, LCD_COLOR_BLUE);
	 
	while (1) 
	{
	}
}
