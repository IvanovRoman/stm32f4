/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               GUIConf.c
** Descriptions:            GUI configure
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-11
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GUI.h"
#include "glcd.h"

/* Private define ------------------------------------------------------------*/

/* you can be defined as the use of internal RAM or external RAM. external RAM can be used in SDRAM, SRAM */
#define USE_IRAM      1    /* too small, some demo can not run, but faster */
#define USE_SRAM      2
#define USE_SDRAM     3

#define USE_RAM       USE_SDRAM

/* define the available number of bytes available for the GUI */
#if (USE_RAM == USE_IRAM)
#define GUI_NUMBYTES  (1024*150)    /* 150k */
#elif (USE_RAM == USE_SRAM || USE_RAM == USE_SDRAM)
#define GUI_NUMBYTES  (1024*1024*2)   /* 2M */
#endif

/* define the average block size */
#define GUI_BLOCKSIZE 0X80

/* define the GUI memory address */
#if (USE_RAM == USE_IRAM)
static unsigned char LCD_GUI_RAM_BASE[GUI_NUMBYTES];
#elif (USE_RAM == USE_SRAM)
#define LCD_GUI_RAM_BASE  (U32)( SRAM_BLANK1_START_ADDR )
#elif (USE_RAM == USE_SDRAM)
#define LCD_GUI_RAM_BASE  (U32)( LCD_FRAME_BUFFER + LCD_FRAME_OFFSET )
#endif

/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*********************************************************************/
void GUI_X_Config(void) {
  //
  // Assign memory to emWin
  //
	GUI_ALLOC_AssignMemory((void*)LCD_GUI_RAM_BASE, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
	/* Set default font */
	GUI_SetDefaultFont(GUI_FONT_6X8);
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
