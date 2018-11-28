/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ft5x06.c
** Descriptions:            ft5x06 application program, support ft5206, ft5306, ft5406, ft5506
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-7-2
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

#ifndef _FT5X06_H_
#define _FT5X06_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "GLCD.h" 

/* Private define ------------------------------------------------------------*/
#define CONFIG_FT5X0X_MULTITOUCH    //multi touch

#define FT5X06_ADDRESS	             0x38

#define FT5X06_RESET_H()     GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define FT5X06_RESET_L()     GPIO_ResetBits(GPIOB, GPIO_Pin_10)

#define FT5X06_SCL_H()     GPIO_SetBits(GPIOH, GPIO_Pin_7)
#define FT5X06_SCL_L()     GPIO_ResetBits(GPIOH, GPIO_Pin_7)

#define FT5X06_SDA_H()     GPIO_SetBits(GPIOH, GPIO_Pin_8)
#define FT5X06_SDA_L()     GPIO_ResetBits(GPIOH, GPIO_Pin_8)

#define FT5X06_READ_SDA()	 GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_8)

#define LCD_HY070CTP        0
#define LCD_HY070CTP_HD     1
#define LCD_HY101CTP        2

#if (USED_LCD == LCD_5_0_HD_TFT || USED_LCD == LCD_7_0_TFT)
	#define TOUCH_PANEL  LCD_HY070CTP
#elif (USED_LCD == LCD_7_0_HD_TFT)
	#define TOUCH_PANEL  LCD_HY070CTP_HD
#elif (USED_LCD == LCD_10_TFT)
	#define TOUCH_PANEL  LCD_HY101CTP
#else
	#warning "invalid capacitive touch panel, use resistive touch"
#endif

#if (TOUCH_PANEL == LCD_HY070CTP)    // compatible with HY050CTP-HD
  #define ft5x06_chip_model  5206
  #define ft5x06_max_x       800
  #define ft5x06_max_y       480
  #define ft5x06_xy_swap     0
  #define ft5x06_x_revert    0
  #define ft5x06_y_revert    0
#elif (TOUCH_PANEL == LCD_HY070CTP_HD)
  #define ft5x06_chip_model  5206
  #define ft5x06_max_x       1024
  #define ft5x06_max_y       600
  #define ft5x06_xy_swap     0
  #define ft5x06_x_revert    0
  #define ft5x06_y_revert    1  //Important
#elif (TOUCH_PANEL == LCD_HY101CTP)
  #define ft5x06_chip_model  5406
  #define ft5x06_max_x       1024
  #define ft5x06_max_y       600
  #define ft5x06_xy_swap     0
  #define ft5x06_x_revert    0
  #define ft5x06_y_revert    0  //different with HY070CTP_HD, the drive is same. if you want to use fully compatible with HY070CTP_HD, can be customized.
#endif

#define CAPACITIVE_TOUCH        0
#define RESISTIVE_TOUCH         1

#define touch_key_down          0    
#define touch_key_up            1

/* Private typedef -----------------------------------------------------------*/
struct point_data {
	int x;
	int y;
};

struct event {
  int  touch_point;
  struct point_data point[5];
	int  key_status;   /* touch key state, up or down */
};

/* Private variables ---------------------------------------------------------*/
extern struct event ts_event; 
extern int touch_type;

/* Private function prototypes -----------------------------------------------*/
int ft5x0x_read_data(void);
void touchpanel_init(void);
void ft5x0x_report_value(void);
#endif 

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
