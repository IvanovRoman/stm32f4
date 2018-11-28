/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               glcd.c
** Descriptions:            LCD driver
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-7-15
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h" 
#include "HzLib.h"
#include "AsciiLib.h"

/**
 * @brief GPIO configuration for LTDC peripheral.
 * @param None
 * @retval None
 */
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO AHB Clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG 
	                       | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);

	/*****************************************************************************
	*R3 -->  PH9 | G2 --> PH13 | B3 --> PG11 | VSYNC  -->  PI9
	*R4 --> PH10 | G3 --> PH14 | B4 -->  PI4 | HSYNC  --> PI10
	*R5 --> PH11 | G4 --> PH15 | B5 -->  PI5 | DE     --> PF10
	*R6 --> PH12 | G5 -->  PI0 | B6 -->  PI6 | CLK    -->  PG7
	*R7 -->  PG6 | G6 -->  PI1 | B7 -->  PI7 | BL_EN  -->  PA4
	*            | G7 -->  PI2 | 							 BL_PWM -->  PA3
	*****************************************************************************/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	/* R3 --> PH9, R4 --> PH10, R5 --> PH11, R6 --> PH12 */
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource9, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* R7 --> PG6 */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* G2 --> PH13, G3 --> PH14, G4 --> PH15 */
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource13, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource14, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource15, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* G5 --> PI0, G6 --> PI1, G7 --> PI2 */
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

	/* B3 --> PG11 */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* B4 --> PI4, B5 --> PI5, B6 --> PI6, B7 --> PI7 */
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource4, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource5, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

	/* VSYNC --> PI9, HSYNC --> PI10 */
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource9, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

	/* DE --> PF10 */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* CLK --> PG7 */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* BL_EN --> PA4, BL_PWM --> PA3  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief LCD Initialization.
  * @note  This function configure LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 480x272 & 800x480 & 1024x600
  * @retval None
  */
void LCD_Init(void)
{
	LTDC_InitTypeDef               LTDC_InitStruct;

	/* Configure the LCD Control pins */
	LCD_GPIO_Init(); 

	/* Enable the LTDC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

	/* Configure PLLSAI prescalers for LCD */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/3 = 64 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64/8 = 8 Mhz */
	/* RCC_PLLSAIConfig(192, 7, 3); */
	/* RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8); */
	#if (USED_LCD == LCD_4_3_TFT || USED_LCD == LCD_5_0_TFT)
		RCC_PLLSAIConfig(192, 7, 3);
		RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
	#elif (USED_LCD == LCD_5_0_HD_TFT || USED_LCD == LCD_7_0_TFT)
		RCC_PLLSAIConfig(192, 7, 3);
		RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div2);
	#elif (USED_LCD == LCD_7_0_HD_TFT || USED_LCD == LCD_10_TFT)
		RCC_PLLSAIConfig(192, 7, 3);
		RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div2);
	#endif
	/* Enable PLLSAI Clock */
	RCC_PLLSAICmd(ENABLE);
	/* Wait for PLLSAI activation */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
	{
	}

	/* LTDC Initialization -----------------------------------------------------*/
	/* Initialize the horizontal synchronization polarity as active low*/
	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
	/* Initialize the data enable polarity as active low */ 
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Timing configuration */
	/* Configure horizontal synchronization width */     
	LTDC_InitStruct.LTDC_HorizontalSync = H_PW - 1;
	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = V_PW - 1;
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = H_BP - 1;
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = V_BP - 1;
	/* Configure accumulated active width */  
	LTDC_InitStruct.LTDC_AccumulatedActiveW = H_VD + H_BP - 1;
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = V_VD + V_BP - 1;
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = H_VD + H_BP + H_FP - 1;
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = V_VD + V_BP + V_FP - 1;

	/* Configure R,G,B component values for LCD background color */                   
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 

	LTDC_Init(&LTDC_InitStruct);

	LCD_InitLayers();
}

/**
  * @brief LCD initialize Layers.
  * @param None
  * @retval None
  */
void LCD_InitLayers(void) 
{
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

	/* 	Windowing configuration */
	/* 	Horizontal start = horizontal synchronization + Horizontal back porch = 43 
	    Vertical start   = vertical synchronization + vertical back porch     = 12
	    Horizontal stop = Horizontal start + LCD_PIXEL_WIDTH - 1 
	    Vertical stop   = Vertical start + LCD_PIXEL_HEIGHT - 1        
	*/      
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = H_BP;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (H_VD + H_BP - 1);
	LTDC_Layer_InitStruct.LTDC_VerticalStart = V_BP;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (V_VD + V_BP - 1);

	/* Pixel Format configuration*/
	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	/* Alpha constant (255 totally opaque) */
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
	/* Default Color configuration (configure A,R,G,B component values) */          
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;

	/* Configure blending factors */       
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

	/* the length of one line of pixels in bytes + 3 then :
	Line Lenth = Active high width x number of bytes per pixel + 3 
	Active high width         = LCD_PIXEL_WIDTH 
	number of bytes per pixel = 2    (pixel_format : RGB565) 
	*/
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_X_SIZE * 2) + 3);

	/* the pitch is the increment from the start of one line of pixels to the 
	   start of the next line in bytes, then :
	   Pitch = Active high width x number of bytes per pixel     
	*/ 
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_X_SIZE * 2);
	/* Configure the number of lines */ 
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_Y_SIZE;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */   
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;
	/* Initialize Layer 1 */
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	/* Configure Layer2 */
	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */  
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + LCD_FRAME_OFFSET;

	/* Configure blending factors */ 
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
	/* Initialize Layer 2 */
	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

	LTDC_ReloadConfig(LTDC_IMReload);
	/* Enable foreground & background Layers */
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	LTDC_ReloadConfig(LTDC_IMReload);

	LCD_DisplayOn();
}

/**
 * @brief	lcd set point
 * @param	Xpos: Coordinate X
 * @param	Ypos: Coordinate Y
 * @param	color: point color
 * @retval None
 */
void LCD_SetPixel_16bpp (int Xpos, int Ypos, unsigned short color)
{
	volatile unsigned short *pLCDbuf = (unsigned short *)LCD_FRAME_BUFFER;  /* LCD buffer start address */

	pLCDbuf[ Ypos * LCD_X_SIZE + Xpos ] = color;	
}

/**
 * @brief	lcd get point
 * @param	Xpos: Coordinate X
 * @param	Ypos: Coordinate Y
 * @param	color: point color
 * @retval None
 */
int LCD_GetPixel_16bpp (int Xpos, int Ypos)
{
	volatile unsigned short *pLCDbuf = (unsigned short *)LCD_FRAME_BUFFER;  /* LCD buffer start address */

	return pLCDbuf[ Ypos * LCD_X_SIZE + Xpos ];
}

/**
 * @brief	clear lcd screen
 * @param	color: point color
 * @retval None
 */
void LCD_Clear(unsigned short color)
{
	int i;
	volatile uint16_t *pLCDbuf = (uint16_t *)LCD_FRAME_BUFFER;  /* LCD buffer start address */

	for( i = 0; i < LCD_PIXEL; i++ )
	{
		pLCDbuf[i] = color;
	}
}

/**
 * @brief	LCD enable power supply
 * @param	None
 * @retval None
 */
void LCD_DisplayOn(void)
{
	/* Enable LCD Backlight */ 
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	/* Display On */
	LTDC_Cmd(ENABLE); /* display ON */
}

/**
 * @brief	LCD disable power supply
 * @param	None
 * @retval None
 */
void LCD_DisplayOff(void)
{
	/* Enable LCD Backlight */ 
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	/* Display Off */
	LTDC_Cmd(DISABLE); /* display off */
}

/**
 * @brief	Lcd screen displays a character
 * @param	Xpos: horizontal coordinate x
 * @param	Ypos: vertical coordinate y
 * @param	ASCI: character
 * @param	charColor: character color
 * @param	bkColor: background color
 * @retval None
 */
void PutChar( int Xpos, int Ypos, unsigned char ASCI, unsigned short charColor, unsigned short bkColor )
{
	int i, j;
	unsigned char buffer[16], tmp_char;
	GetASCIICode(buffer,ASCI);  /* get font data */
	
	for( i=0; i<16; i++ )
	{
		tmp_char = buffer[i];
		for( j=0; j<8; j++ )
		{
			if( (tmp_char >> 7 - j) & 0x01 == 0x01 )
			{
				LCD_SetPixel_16bpp( Xpos + j, Ypos + i, charColor );  /* character color */
			}
			else
			{
				LCD_SetPixel_16bpp( Xpos + j, Ypos + i, bkColor );  /* background color */
			}
		}
	}
}

/**
 * @brief	Lcd screen displays a character string
 * @param	Xpos: horizontal coordinate x
 * @param	Ypos: vertical coordinate y
 * @param	ASCI: sharacter string
 * @param	charColor: character color
 * @param	bkColor: background color
 * @retval None
 */
void GUI_Text(int Xpos, int Ypos, unsigned char *str,unsigned short Color, unsigned short bkColor)
{
	unsigned char TempChar;
	do
	{
		TempChar = *str++;  
		PutChar( Xpos, Ypos, TempChar, Color, bkColor );    
		if( Xpos < LCD_X_SIZE - 8 )
		{
			Xpos += 8;
		}
		else if ( Ypos < LCD_Y_SIZE - 16 )
		{
			Xpos = 0;
			Ypos += 16;
		}
		else
		{
			Xpos = 0;
			Ypos = 0;
		}  
	}
	while ( *str != 0 );
}

/**
 * @brief	Lcd screen bresenham's line algorithm
 * @param	x0: horizontal coordinate x
 * @param	y0: vertical coordinate y
 * @param	x1: horizontal coordinate x
 * @param	y1: vertical coordinate y
 * @param	color: character color
 * @retval None
 */
void LCD_DrawLine( int x0, int y0, int x1, int y1 , unsigned short color )
{
	short dx,dy;
	short temp;

	if( x0 > x1 )
	{
		temp = x1;
		x1 = x0;
		x0 = temp;   
	}
	if( y0 > y1 )
	{
		temp = y1;
		y1 = y0;
		y0 = temp;   
	}

	dx = x1-x0;
	dy = y1-y0;  

	if( dx == 0 )
	{
		do
		{ 
			LCD_SetPixel_16bpp(x0, y0, color);
			y0++;
		}
		while( y1 >= y0 ); 
		return; 
	}
	if( dy == 0 )
	{
		do
		{
			LCD_SetPixel_16bpp(x0, y0, color);
			x0++;
		}
		while( x1 >= x0 ); 
		return;
	}

	if( dx > dy ) 
	{
		temp = 2 * dy - dx;   
		while( x0 != x1 )
		{
			LCD_SetPixel_16bpp(x0,y0,color);
			x0++;
			if( temp > 0 )           
			{
				y0++;                
				temp += 2 * dy - 2 * dx; 
			}
			else         
			{
				temp += 2 * dy;   
			}       
		}
		LCD_SetPixel_16bpp(x0,y0,color);
	}  
	else
	{
		temp = 2 * dx - dy;              
		while( y0 != y1 )
		{
			LCD_SetPixel_16bpp(x0,y0,color);     
			y0++;                 
			if( temp > 0 )           
			{
				x0++;               
				temp+=2*dy-2*dx; 
			}
			else
			{
				temp += 2 * dy;
			}
		} 
		LCD_SetPixel_16bpp(x0,y0,color);
	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
