/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               LCDConf_FlexColor_Template.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-14
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GUI.h"
#include "GLCD.h"
#include "GUIDRV_Template.h"
#include "GUIDRV_FlexColor.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************/

/* Physical display size */
#define XSIZE_PHYS                        LCD_X_SIZE
#define YSIZE_PHYS                        LCD_Y_SIZE

#define VXSIZE_PHYS                       (XSIZE_PHYS)
#define VYSIZE_PHYS                       (YSIZE_PHYS)

/* Color conversion */
#define COLOR_CONVERSION GUICC_M565

/* Display driver */
#define DISPLAY_DRIVER &GUIDRV_Lin_16_API
#define PIXEL_WIDTH                     2

/* LCD frame address */
#define VRAM_ADDR    LCD_FRAME_BUFFER

#define NUM_BUFFERS  1 /* Number of multiple buffers to be used */
#define NUM_VSCREENS 1 /* Number of virtual screens to be used */

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR 0 // TBD by customer: This has to be the frame buffer start address
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

#define WIDTH       XSIZE_PHYS  /* Screen Width (in pixels) */
#define HEIGHT      YSIZE_PHYS  /* Screen Hight (in pixels) */

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetDisplayOrigin()
*/
static void _SetDisplayOrigin(int x, int y) {
  (void)x;
  //
  // Set start address for display data and enable LCD controller
  //
  LTDC_Layer1->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
  LTDC_Layer1->CFBAR =  LCD_FRAME_BUFFER + (y * XSIZE_PHYS * PIXEL_WIDTH);  
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(GUIDRV_LIN_16, COLOR_CONVERSION, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
  
  /* Set user palette data (only required if no fixed palette is used) */
  #if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
		case LCD_X_INITCONTROLLER: {
			//
			// Called during the initialization process in order to set up the
			// display controller and put it into operation. If the display
			// controller is not initialized by any external routine this needs
			// to be adapted by the customer...
			//
			// ...
			FMC_SDRAM_Init();
			LCD_Init();
			return 0;
		}
		case LCD_X_SETORG: {
			//
			// Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
			//
			LCD_X_SETORG_INFO * p;
			p = (LCD_X_SETORG_INFO *)pData;
			_SetDisplayOrigin(p->xPos, p->yPos);
			return 0;
		}

		default:
			r = -1;
  }
  return r;
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
