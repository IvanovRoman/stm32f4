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
#include "FMC_nor.h"
#include "fmc_sdram.h"

/* Include my libraries here */

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*
	picture （RGB565 format，stored in NOR Flash
	NOR Flash address 0x6000 0000

	0x0020 0000   255K字节 480x272_海滩.bin  + 0x3FC00
	0x0023 FC00   255K字节 480x272_树叶.bin
	0x0027 F800   255K字节 480x272_水珠.bin

	0x002B F400   755K字节 800x480_海滩.bin  + 0xBB800
	0x0037 AC00   755K字节 800x480_树叶.bin
	0x0043 6400   755K字节 800x480_水珠.bin
*/

	#define picture1 (uint16_t *)(0x60000000 + 0x00200000)	/* 480x272 沙滩背景 */
	#define picture2 (uint16_t *)(0x60000000 + 0x0023FC00)	/* 480x272 树叶背景 */
	#define picture3 (uint16_t *)(0x60000000 + 0x0027F800)	/* 480x272 水珠背景 */
	

	#define picture4 (uint16_t *)(0x60000000 + 0x002BF400)  /* 800x480 沙滩背景 */
	#define picture5 (uint16_t *)(0x60000000 + 0x0037AC00)  /* 800x480 树叶背景 */
	#define picture6 (uint16_t *)(0x60000000 + 0x00436400)  /* 800x480 水珠背景 */


void ILI9488_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usWidth, uint16_t _usHeight, uint16_t *_ptr)
{
	uint32_t index_x = 0;
	uint32_t index_y = 0;

	const uint16_t *p;

	p = _ptr;
	for (index_y = 0; index_y < _usHeight; index_y++)
	{
			for (index_x = 0; index_x < _usWidth; index_x++)
			{
				LCD_SetPixel_16bpp(index_x, index_y, *p++);
			}
	}
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
/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {
	FMC_NOR_Init();
	FMC_SDRAM_Init();
	LCD_Init();
	while (1)
	{
		#if (USED_LCD == LCD_4_3_TFT || USED_LCD == LCD_5_0_TFT)
			ILI9488_DrawBMP(0,0,480,272,picture1);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture2);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture3);
			Delay(0x2ffffff);
		#elif (USED_LCD == LCD_5_0_HD_TFT || LCD_7_0_TFT)
			ILI9488_DrawBMP(0,0,480,272,picture1);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture2);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture3);
			Delay(0x2ffffff);
		#elif (USED_LCD == LCD_7_0_HD_TFT)
			ILI9488_DrawBMP(0,0,480,272,picture1);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture2);
			Delay(0x2ffffff);
			ILI9488_DrawBMP(0,0,480,272,picture3);
			Delay(0x2ffffff);
		#endif
	}
}
