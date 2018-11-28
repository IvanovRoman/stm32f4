/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ts_calibrate.h
** Descriptions:            The TouchPanel application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-10
** Version:                 v1.0
** Descriptions:            The original version
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:     
*********************************************************************************************************/

#ifndef _CALIBRATE_H_
#define _CALIBRATE_H_

/* Includes ------------------------------------------------------------------*/
#include "xpt2046.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct Matrix 
{
	/* This arrangement of values facilitates  calculations within getDisplayPoint() */
	int  
	An,     /* A = An/Divider */
	Bn,     /* B = Bn/Divider */   
	Cn,     /* C = Cn/Divider */   
	Dn,     /* D = Dn/Divider */   
	En,     /* E = En/Divider */   
	Fn,     /* F = Fn/Divider */   
	Divider ;   
} Matrix ;

typedef struct {
      int x[5], xfb[5];
      int y[5], yfb[5];
      int a[7];
} calibration;

/* Private variables ---------------------------------------------------------*/
extern Matrix matrix ;
extern Coordinate  display ;

/* Private function prototypes -----------------------------------------------*/				
int getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr );
int setCalibrationMatrix( Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr);
void ts_draw_point(int x,int y,int color);
void ts_draw_cross(int Xpos,int Ypos);
#endif

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
