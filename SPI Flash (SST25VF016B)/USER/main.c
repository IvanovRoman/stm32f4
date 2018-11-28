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
#include <string.h>
#include "SST25VF016B.h"
#include "GLCD.h"
#include "fmc_sdram.h"
#include "picture.c"

/* Private functions ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define PICTURE_ADDR  (0)		   /* PICTURE address in SPI FLASH */

uint8_t buffer[960];

void ILI9488_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usWidth, uint16_t _usHeight)
{
	uint32_t index_x = 0;
	uint32_t index_y = 0;
	
	uint16_t temp;
	
	for (index_y = 0; index_y < _usHeight; index_y++)
	{
		  df_read( &buffer[0] , sizeof(buffer) );
			for (index_x = 0; index_x < _usWidth; index_x++)
			{
				
				temp = *( index_x*2+1+buffer );
				
				temp = ( (temp<<8) | *( index_x*2+0+buffer ) );
				
				LCD_SetPixel_16bpp(index_x, index_y, temp);
			}
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) 
{
  uint16_t count;

	FMC_SDRAM_Init();
	LCD_Init();
	
  SPI_FLASH_Init();
  SPI_FLASH_Test();
	SSTF016B_Erase( PICTURE_ADDR/SEC_SIZE , ( PICTURE_ADDR+sizeof(picture) )/SEC_SIZE +1 );	  /* ??????? */
	df_write_open( PICTURE_ADDR );    /* ??????? */

	LCD_Clear(LCD_COLOR_RED);
	GUI_Text( ( LCD_X_SIZE - strlen("Writing To SPI Flash")*8 ) / 2, LCD_Y_SIZE / 2 - 8, "Writing To SPI Flash", LCD_COLOR_WHITE, LCD_COLOR_BLUE);
	 	
  for(count=0;count<sizeof(picture)/255;count++)
  {
    df_write( (uint8_t*)&picture[count*255],255); 
    df_write_close(); 
  }
  df_write( (uint8_t*)&picture[count*255],sizeof(picture)%255);  
  df_write_close();   
 
	df_read_open( PICTURE_ADDR );    /* ??????? */

	ILI9488_DrawBMP(0,0,480,272);	
	while (1) 
	{

	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
