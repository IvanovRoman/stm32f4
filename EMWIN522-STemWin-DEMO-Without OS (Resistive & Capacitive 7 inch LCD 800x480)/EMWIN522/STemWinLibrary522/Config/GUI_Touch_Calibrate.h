/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               GUI_Touch_Calibrate.h
** Descriptions:            GUI resistance touch calibration
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-10
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

#ifndef _GUI_TOUCH_CALIBRATE_H_
#define _GUI_TOUCH_CALIBRATE_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "xpt2046.h"
#include "GUI.h"

/* Private typedef -----------------------------------------------------------*/

/* Touch screen */
typedef struct {
      int x[5], xfb[5];
      int y[5], yfb[5];
      int a[7];
} calibration;

/* Private function prototypes -----------------------------------------------*/

int calibrate_X(unsigned  int ad_x,unsigned int ad_y);
int calibrate_Y(unsigned  int ad_x,unsigned int ad_y);
void exec_calibration(int x_size, int y_size);

#endif

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
