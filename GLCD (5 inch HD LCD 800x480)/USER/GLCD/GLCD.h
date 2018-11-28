/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               glcd.h
** Descriptions:            LCD driver
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-7-15
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

#ifndef __GLCD_H 
#define __GLCD_H

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "stm32f4xx.h"
#include "fmc_sdram.h"

/* Private define ------------------------------------------------------------*/
#define LCD_4_3_TFT       0           /* 4.3 inch TFT LCD */
#define LCD_5_0_TFT       1           /* 5 inch TFT LCD */
#define LCD_5_0_HD_TFT    2           /* 5 inch HD TFT LCD, compatible HY050CTP-HD */
#define LCD_7_0_TFT       3           /* 7 inch TFT LCD, compatible HY070CTP */
#define LCD_7_0_HD_TFT    4           /* 7 inch HD TFT LCD, compatible HY070CTP-HD, HY101CTP */
#define LCD_10_TFT        6           /* 10 inch TFT LCD, compatible HY070CTP-HD */

/* what LCD type do you want to use */
#define USED_LCD            LCD_5_0_HD_TFT  

#if (USED_LCD == LCD_4_3_TFT || USED_LCD == LCD_5_0_TFT)
#define LCD_X_SIZE     480	          /* LCD Horizontal display area */
#define LCD_Y_SIZE     272	          /* LCD Vertical display area */

#define LCD_DCLK       9000000        /* LCD DCLK 9M */ 

#define H_VD           LCD_X_SIZE     /* LCD Horizontal display area */
#define H_PW           41             /* HSYNC pulse width */
#define H_BP           43             /* Horizontal back porch */
#define H_FP           2              /* Horizontal front porch */

#define V_VD			     LCD_Y_SIZE     /* LCD Vertical display area */
#define V_PW           10             /* VSYNC pulse width */
#define V_BP           12             /* Vertical back porch */
#define V_FP           2              /* Vertical front porch */

#elif (USED_LCD == LCD_5_0_HD_TFT)
#define LCD_X_SIZE     800	          /* LCD Horizontal display area */
#define LCD_Y_SIZE     480		        /* LCD Vertical display area */

#define LCD_DCLK       30000000       /* LCD DCLK 30M */ 

#define H_VD           LCD_X_SIZE     /* LCD Horizontal display area */
#define H_PW           48             /* HSYNC pulse width */
#define H_BP           88             /* Horizontal back porch */
#define H_FP           40             /* Horizontal front porch */

#define V_VD			     LCD_Y_SIZE     /* LCD Vertical display area */
#define V_PW           3              /* VSYNC pulse width */
#define V_BP           32             /* Vertical back porch */
#define V_FP           13             /* Vertical front porch */

#elif (USED_LCD == LCD_7_0_TFT)
#define LCD_X_SIZE     800	          /* LCD Horizontal display area */
#define LCD_Y_SIZE     480		        /* LCD Vertical display area */

#define LCD_DCLK       34000000       /* LCD DCLK 34M */ 

#define H_VD           LCD_X_SIZE     /* LCD Horizontal display area */
#define H_PW           30             /* HSYNC pulse width */
#define H_BP           46             /* Horizontal back porch */
#define H_FP           210            /* Horizontal front porch */

#define V_VD			     LCD_Y_SIZE     /* LCD Vertical display area */
#define V_PW           10             /* VSYNC pulse width */
#define V_BP           23             /* Vertical back porch */
#define V_FP           22             /* Vertical front porch */

#elif (USED_LCD == LCD_7_0_HD_TFT || USED_LCD == LCD_10_TFT)
#define LCD_X_SIZE     1024	          /* LCD Horizontal display area */
#define LCD_Y_SIZE     600		        /* LCD Vertical display area */

#define LCD_DCLK       58000000       /* LCD DCLK 34M */ 

#define H_VD           LCD_X_SIZE     /* LCD Horizontal display area */
#define H_PW           10             /* HSYNC pulse width */
#define H_BP           100             /* Horizontal back porch */
#define H_FP           210            /* Horizontal front porch */

#define V_VD			     LCD_Y_SIZE     /* LCD Vertical display area */
#define V_PW           10             /* VSYNC pulse width */
#define V_BP           10             /* Vertical back porch */
#define V_FP           18             /* Vertical front porch */

#else
 #error "Invalid LCD!"
#endif


/* LCD settings */
#define LCD_PIXEL				  LCD_X_SIZE * LCD_Y_SIZE

/* Starting buffer address in RAM */
/* Offset for Layer 1 = SDRAM START ADDRESS + FRAME_BUFFER */
#define LCD_FRAME_BUFFER		SDRAM_BANK_ADDR
/* Offset for Layer 2 */
#define LCD_FRAME_OFFSET		(uint32_t)LCD_PIXEL * 2

/* LCD color */
#define LCD_COLOR_WHITE			0xFFFF
#define LCD_COLOR_BLACK			0x0000
#define LCD_COLOR_RED			  0xF800
#define LCD_COLOR_GREEN			0x07E0
#define LCD_COLOR_GREEN2		0xB723
#define LCD_COLOR_BLUE			0x001F
#define LCD_COLOR_BLUE2			0x051D
#define LCD_COLOR_YELLOW		0xFFE0
#define LCD_COLOR_ORANGE		0xFBE4
#define LCD_COLOR_CYAN			0x07FF
#define LCD_COLOR_MAGENTA		0xA254
#define LCD_COLOR_GRAY			0x7BEF
#define LCD_COLOR_BROWN			0xBBCA

#define RGB565CONVERT(red, green, blue) (int) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

/* Private function prototypes -----------------------------------------------*/
void LCD_Init(void);
void LCD_SetPixel_16bpp (int Xpos, int Ypos, unsigned short color);
int LCD_GetPixel_16bpp (int Xpos, int Ypos);
void LCD_Clear(unsigned short color);
void LCD_DisplayOff(void);
void LCD_DisplayOn(void);
void LCD_InitLayers(void);
void PutChar( int Xpos, int Ypos, unsigned char ASCI, unsigned short charColor, unsigned short bkColor );
void GUI_Text(int Xpos, int Ypos, unsigned char *str,unsigned short Color, unsigned short bkColor);
void LCD_DrawLine( int x0, int y0, int x1, int y1 , unsigned short color );

#endif 

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
