/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               GUI_Touch_Calibrate.c
** Descriptions:            GUI resistance touch calibration
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-10
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GUI_Touch_Calibrate.h"

#if GUI_SUPPORT_TOUCH  /* Used when touch screen support is enabled */

/* Private variables ---------------------------------------------------------*/
static calibration cal;

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
		//printf("ts_calibrate: determinant is too small -- %f\n\r",det);
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

	//printf("%f %f %f\n\r",(a*z + b*zx + c*zy), (b*z + e*zx + f*zy), (c*z + f*zx + i*zy));

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

	//printf("%f %f %f\n\r",(a*z + b*zx + c*zy), (b*z + e*zx + f*zy), (c*z + f*zx + i*zy));

	/* If we got here, we're OK, so assign scaling to a[6] and return */
	cal->a[6] = (int)scaling;
	return 1;
}

/**
  * @brief  calibration touch screen
  * @param	x_size: lcd width
  * @param	y_size: lcd height
  * @retval None
  */
void exec_calibration(int x_size, int y_size) 
{
	/* calculate log. Positions */
	int ax[5], ay[5];
	Coordinate *p;
	ax[0] = 20;             ay[0] = 20;
	ax[1] = x_size -20;     ay[1] = 20;
	ax[2] = x_size -20;     ay[2] = y_size-20;
	ax[3] = 20;             ay[3] = y_size-20;
	ax[4] = x_size/2;       ay[4] = y_size/2;

	GUI_TOUCH_SetDefaultCalibration();
	GUI_Delay (1);
	/* _Calibrate upper left */
	GUI_SetFont(&GUI_Font13_ASCII);

	GUI_SetBkColor(GUI_BLUE);  
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[0], ay[0], 10);
	GUI_SetColor(GUI_BLUE);    GUI_FillCircle(ax[0], ay[0], 5);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("Press here", ax[0]+20, ay[0]);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 ) 
		{
			cal.xfb[0] = 20;
			cal.yfb[0] = 20;
			cal.x[0] = p->x;
			cal.y[0] = p->y;
			break;
		}
		GUI_Delay (10);
	} while (1);

	GUI_Clear();
	GUI_DispStringAt("OK", ax[0]+20, ay[0]);

	GUI_Delay (1000);
	GUI_SetBkColor(GUI_BLUE);  
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[1], ay[1], 10);
	GUI_SetColor(GUI_BLUE);    GUI_FillCircle(ax[1], ay[1], 5);
	GUI_SetColor(GUI_WHITE);
	GUI_SetTextAlign(GUI_TA_RIGHT);
	GUI_DispStringAt("Press here", ax[1]-20, ay[1]);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 ) 
		{
			cal.xfb[1] = x_size-20;
			cal.yfb[1] = 20;
			cal.x[1] = p->x ;
			cal.y[1] = p->y;
			break;
		}
		GUI_Delay (10);
	} while (1);
	GUI_Clear();
	GUI_DispStringAt("OK", ax[1]-20, ay[1]);

	GUI_Delay (1000);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[2], ay[2], 10);
	GUI_SetColor(GUI_BLUE);    GUI_FillCircle(ax[2], ay[2], 5);
	GUI_SetColor(GUI_WHITE);
	GUI_SetTextAlign(GUI_TA_RIGHT);
	GUI_DispStringAt("Press here", ax[2]-20, ay[2]);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 ) 
		{
			cal.xfb[2] = x_size -20;
			cal.yfb[2] = y_size -20;
			cal.x[2] = p->x;
			cal.y[2] = p->y;
			break;           
		}
		GUI_Delay (10);
	} while (1);
	GUI_Clear();
	GUI_DispStringAt("OK", ax[2]-20, ay[2]);

	GUI_Delay (1000);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[3], ay[3], 10);
	GUI_SetColor(GUI_BLUE);    GUI_FillCircle(ax[3], ay[3], 5);
	GUI_SetColor(GUI_WHITE);
	//GUI_SetTextAlign(GUI_TA_RIGHT);
	GUI_SetTextAlign(GUI_TA_LEFT);
	GUI_DispStringAt("Press here", ax[3]+20, ay[3]);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 ) 
		{
			cal.xfb[3] = 20;
			cal.yfb[3] = y_size -20;
			cal.x[3] = p->x ;
			cal.y[3] = p->y ;
			break;
		}
		GUI_Delay (10);
	} while (1);
	GUI_Clear();
	GUI_DispStringAt("OK", ax[3]+20, ay[3]);

	GUI_Delay (1000);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[4], ay[4], 10);
	GUI_SetColor(GUI_BLUE);    GUI_FillCircle(ax[4], ay[4], 5);
	GUI_SetColor(GUI_WHITE);
	GUI_SetTextAlign(GUI_TA_LEFT);
	GUI_DispStringAt("Press here", ax[4]+20, ay[4]);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 )  
		{
			cal.xfb[4] = x_size/2;
			cal.yfb[4] = y_size/2;
			cal.x[4] = p->x ;
			cal.y[4] = p->y ;
			break;
		}
		GUI_Delay (10);
	} while (1);

	/* calculate and display values for configuration file */  
	GUI_Clear();
	GUI_DispString  ("x0: ");	GUI_DispDec(cal.x[0], 4);	
	GUI_DispString  ("  y0: ");	GUI_DispDec(cal.y[0], 4);   GUI_DispNextLine();

	GUI_DispString  ("x1: ");	GUI_DispDec(cal.x[1], 4);
	GUI_DispString  ("  y1: ");	GUI_DispDec(cal.y[1], 4);	GUI_DispNextLine();

	GUI_DispString  ("x2: ");	GUI_DispDec(cal.x[2], 4);
	GUI_DispString  ("  y2: ");	GUI_DispDec(cal.y[2], 4);	GUI_DispNextLine();

	GUI_DispString  ("x3: ");	GUI_DispDec(cal.x[3], 4);
	GUI_DispString  ("  y3: ");	GUI_DispDec(cal.y[3], 4);	GUI_DispNextLine();

	GUI_DispString  ("x4: ");	GUI_DispDec(cal.x[4], 4);
	GUI_DispString  ("  y4: ");	GUI_DispDec(cal.y[4], 4);	GUI_DispNextLine();

	GUI_DispString  ("Please touch display to continue...");

	GUI_Delay(1000);
	do 
	{
		p = Read_Value();
		if ( p != (void*)0 )
		{
			break;
		}
		GUI_Delay (10);
	} while (1);

	perform_calibration(&cal);
}

/**
  * @brief  input ADC value, get X coordinates
  * @param  None
  * @retval None
  */
int calibrate_X(unsigned  int ad_x, unsigned int ad_y)
{
	int temp;
	temp =(unsigned int)((ad_x*cal.a[0]+ad_y*cal.a[1]+cal.a[2])/cal.a[6]); 
	return temp;
}

/**
  * @brief  input ADC value, get Y coordinates
  * @param  None
  * @retval None
  */
int calibrate_Y(unsigned  int ad_x, unsigned int ad_y)
{
	int temp;
	temp =(unsigned int)((ad_x*cal.a[3]+ad_y*cal.a[4]+cal.a[5])/cal.a[6]); 
	return temp;
}

#endif  /* GUI_SUPPORT_TOUCH */

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
