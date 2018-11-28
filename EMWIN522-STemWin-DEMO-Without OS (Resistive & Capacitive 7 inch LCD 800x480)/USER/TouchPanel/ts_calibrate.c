/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ts_calibrate.c
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

/* Includes ------------------------------------------------------------------*/
#include "ts_calibrate.h"

/* Private variables ---------------------------------------------------------*/
static Coordinate ts_position[5];
static Coordinate display_position[5];
static calibration cal;

Matrix matrix ;
Coordinate display ;

 /**
  * @brief  Calling this function with valid input data in the display and screen input arguments 
            causes the calibration factors between the screen and display points to be calculated,
            and the output argument - matrixPtr - to be populated.
            This function needs to be called only when new  calibration factors are desired.
  * @param  displayPtr (input) - Pointer to an array of three sample, reference points.
  * @param  screenPtr (input) - Pointer to the array of touch screen points corresponding to the reference display points.
  * @param  matrixPtr (output) - Pointer to the calibration matrix computed for the set of points being provided.
  * @retval return 0 is success
  */
int setCalibrationMatrix( Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr)
{

  /* K£½(X0£­X2) (Y1£­Y2)£­(X1£­X2) (Y0£­Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    return -1;
  }
  else
  {
    /* A£½((XD0£­XD2) (Y1£­Y2)£­(XD1£­XD2) (Y0£­Y2))£¯K	*/
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
	/* B£½((X0£­X2) (XD1£­XD2)£­(XD0£­XD2) (X1£­X2))£¯K	*/
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C£½(Y0(X2XD1£­X1XD2)+Y1(X0XD2£­X2XD0)+Y2(X1XD0£­X0XD1))£¯K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D£½((YD0£­YD2) (Y1£­Y2)£­(YD1£­YD2) (Y0£­Y2))£¯K	*/
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E£½((X0£­X2) (YD1£­YD2)£­(YD0£­YD2) (X1£­X2))£¯K	*/
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F£½(Y0(X2YD1£­X1YD2)+Y1(X0YD2£­X2YD0)+Y2(X1YD0£­X0YD1))£¯K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return 0;
}

 /**
  * @brief  Given a valid set of calibration factors and a point value reported by the touch screen, 
            this function calculates and returns the true (or closest to true) display point below 
            the spot where the touch screen was touched.
  * @param  displayPtr (output) - Pointer to the calculated (true) display point.
  * @param  screenPtr (input) - Pointer to the reported touch screen point.
  * @param  matrixPtr (input) - Pointer to calibration factors matrix previously calculated from a call to  setCalibrationMatrix()
  * @retval return 0 is success
  */
int getDisplayPoint(Coordinate * displayPtr, Coordinate * screenPtr, Matrix * matrixPtr )
{
	/* Operation order is important since we are doing integer math. 
	   Make sure you add all terms together before dividing, so that 
	   the remainder is not rounded off prematurely. */
	
  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    return -1;
  }
  return 0;
} 

/**
  * @brief  calling this function with valid input data, calculate the calibration factors.
  * @param  None
  * @retval None
  */
int perform_calibration(calibration *cal) 
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	/* Get sums for matrix */
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

	/* Get determinant of matrix -- check if determinant is too small */
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		/* ts_calibrate determinant is too small */
		return 0;
	}

	/* Get elements of inverse matrix */
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

	/* Get sums for x calibration */
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j]*cal->x[j]);
		zy += (float)(cal->xfb[j]*cal->y[j]);
	}
	/* Now multiply out to get the calibration for framebuffer x coord */
	cal->a[2] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[0] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[1] = (int)((c*z + f*zx + i*zy)*(scaling));

	/* Get sums for y calibration */
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j]*cal->x[j]);
		zy += (float)(cal->yfb[j]*cal->y[j]);
	}
	/* Now multiply out to get the calibration for framebuffer y coord */
	cal->a[5] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[3] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[4] = (int)((c*z + f*zx + i*zy)*(scaling));

	/* assign scaling */
	cal->a[6] = (int)scaling;
	return 1;
}

/**
 * @brief	calibration touch screen
 * @param	x_size: lcd width
 * @param	y_size: lcd height
 * @retval None
 */
void ts_calibrate( int x_size, int y_size )
{
	unsigned char i;
	int count;
	Coordinate * Ptr;
	
	display_position[0].x = 20;              display_position[0].y = 20;
	display_position[1].x = x_size - 20;     display_position[1].y = 20;
	display_position[2].x = x_size - 20;     display_position[2].y = y_size - 20;
	display_position[3].x = 20;              display_position[3].y = y_size - 20;
	display_position[4].x = x_size / 2;      display_position[4].y = y_size / 2;

	for( i=0; i<5; i++ )
	{
		LCD_Clear(LCD_COLOR_BLACK);
		
		for( count=0; count<1000*10000; count++ );   /* delay */
		
		ts_draw_cross(display_position[i].x,display_position[i].y);
		do
		{
			Ptr = Read_Value();
		}
		while( Ptr == (void*)0 );
		ts_position[i].x= Ptr->x; ts_position[i].y= Ptr->y;
	}
	
	LCD_Clear(LCD_COLOR_BLACK);
	
	cal.xfb[0] = display_position[0].x;
	cal.yfb[0] = display_position[0].y;

	cal.x[0] = ts_position[0].x;
	cal.y[0] = ts_position[0].y;

	cal.xfb[1] = display_position[1].x;
	cal.yfb[1] = display_position[1].y;

	cal.x[1] = ts_position[1].x;
	cal.y[1] = ts_position[1].y;

	cal.xfb[2] = display_position[2].x;
	cal.yfb[2] = display_position[2].y;

	cal.x[2] = ts_position[2].x;
	cal.y[2] = ts_position[2].y;

	cal.xfb[3] = display_position[3].x;
	cal.yfb[3] = display_position[3].y;

	cal.x[3] = ts_position[3].x;
	cal.y[3] = ts_position[3].y;

	cal.xfb[4] = display_position[4].x;
	cal.yfb[4] = display_position[4].y;

	cal.x[4] = ts_position[4].x;
	cal.y[4] = ts_position[4].y;

	perform_calibration(&cal);

	matrix.An = cal.a[0];
	matrix.Bn = cal.a[1];
	matrix.Cn = cal.a[2];
	matrix.Dn = cal.a[3];
	matrix.En = cal.a[4];
	matrix.Fn = cal.a[5];
	matrix.Divider = cal.a[6];
}

/**
 * @brief	draw cross for calibration
 * @param	x: x coordinate
 * @param	y: y coordinate 
 * @retval None
 */
void ts_draw_cross(int xpos, int ypos)
{
	LCD_DrawLine(xpos-15, ypos, xpos-2, ypos, 0xffff);
	LCD_DrawLine(xpos+2, ypos, xpos+15, ypos, 0xffff);
	LCD_DrawLine(xpos, ypos-15, xpos, ypos-2, 0xffff);
	LCD_DrawLine(xpos, ypos+2, xpos, ypos+15, 0xffff);

	LCD_DrawLine(xpos-15, ypos+15, xpos-7, ypos+15, RGB565CONVERT(184,158,131));
	LCD_DrawLine(xpos-15, ypos+7, xpos-15, ypos+15, RGB565CONVERT(184,158,131));

	LCD_DrawLine(xpos-15, ypos-15, xpos-7, ypos-15, RGB565CONVERT(184,158,131));
	LCD_DrawLine(xpos-15, ypos-7, xpos-15, ypos-15, RGB565CONVERT(184,158,131));

	LCD_DrawLine(xpos+7, ypos+15, xpos+15, ypos+15, RGB565CONVERT(184,158,131));
	LCD_DrawLine(xpos+15, ypos+7, xpos+15, ypos+15, RGB565CONVERT(184,158,131));

	LCD_DrawLine(xpos+7, ypos-15, xpos+15, ypos-15, RGB565CONVERT(184,158,131));
	LCD_DrawLine(xpos+15, ypos-15, xpos+15, ypos-7, RGB565CONVERT(184,158,131));
}

/**
 * @brief	draw point in lcd
 * @param	x: coordinate
 * @param	y: coordinate
 * @param	color: color
 * @retval None
 */
void ts_draw_point(int x, int y, int color)
{
	if( x >= LCD_X_SIZE || y >= LCD_Y_SIZE )
	{
		return;
	}

	if( x == 0 || y == 0 )
	{
		return;
	}

	LCD_SetPixel_16bpp(x,y,color);         
	LCD_SetPixel_16bpp(x-1,y,color);                
	LCD_SetPixel_16bpp(x,y-1,color);
	LCD_SetPixel_16bpp(x-1,y-1,color);
	LCD_SetPixel_16bpp(x+1,y-1,color);
	LCD_SetPixel_16bpp(x,y+1,color);
	LCD_SetPixel_16bpp(x-1,y+1,color);
	LCD_SetPixel_16bpp(x+1,y+1,color);
	LCD_SetPixel_16bpp(x+1,y,color);
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
