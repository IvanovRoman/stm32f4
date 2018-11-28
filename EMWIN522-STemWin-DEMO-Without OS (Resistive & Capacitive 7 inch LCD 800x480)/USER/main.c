/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-9-10
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "GLCD.h" 
#include "GUI.h"
#include "DIALOG.h"
#include "GUIDEMO.h"
#include "ft5x06.h"
#include "GUI_Touch_Calibrate.h"

/* Private variables ---------------------------------------------------------*/
static unsigned char touch_flag;

/* Private functions ---------------------------------------------------------*/
void ts_touch_task(void);

/**
  * @brief Configures the TIM IRQ Handler.
  * @note  tout=((arr+1)*(psc+1))/Ft us.
  * @param  None
  * @retval None
  */
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/* Time base configuration */
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	/* TIM Interrupts enable */	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	/* TIM3enable counter */
	TIM_Cmd(TIM3,ENABLE);

	/* Enable the TIM4 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	extern __IO int32_t OS_TimeMS;

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		OS_TimeMS++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

/**
  * @brief Configures the TIM IRQ Handler.
  * @note  tout=((arr+1)*(psc+1))/Ft us.
  * @param  None
  * @retval None
  */
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	/* Time base configuration */
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	/* TIM Interrupts enable */
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	/* TIM4 enable counter */
	TIM_Cmd(TIM4,ENABLE);
	
	/* Enable the TIM4 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

		if( touch_type == RESISTIVE_TOUCH )
		{
			ts_touch_task();
		}
		else   /* capacitive touch */
		{		
			/* ft5x0x_report_value Function */
			if( ts_event.key_status == touch_key_down )
			{
				EXTI->IMR&=~(1<<6); /* disable PH6 interrupt */
				ft5x0x_read_data();
				touch_flag = 1;
				ts_event.key_status = touch_key_up;                
				EXTI->IMR|=1<<6;    /* enable PH6 interrupt */
			}
			/* end */
			if( touch_flag )
			{
				touch_flag = 0;
				GUI_TOUCH_StoreState(ts_event.point[0].x, ts_event.point[0].y);
			}
			GUI_TOUCH_StoreState(-1, -1);
		}
	}
}

/**
  * @brief  resistance touch.
  * @param  None
  * @retval None
  */
void ts_touch_task(void)
{
	static U16 xOld;
	static U16 yOld;
	static U8  PressedOld;
	U16 x=0, y=0, xDiff=0, yDiff=0;
	Coordinate* p;

	p = Read_Value();

	/* touch screen is pressed */
	if ( p != (void *)0 ) 
	{
		x = calibrate_X(p->x, p->y);    /* ADC values converted to coordinate values */
		y = calibrate_Y(p->x, p->y);    /* ADC values converted to coordinate values */
		
		/* the touch has already been pressed */
		if (PressedOld == 1) 
		{
			/* calculate difference between new and old position */
			xDiff = (x > xOld) ? (x - xOld) : (xOld - x);
			yDiff = (y > yOld) ? (y - yOld) : (yOld - y);
			/* store state if new position differs significantly from old position */
			if (xDiff + yDiff > 2) 
			{
				xOld = x;
				yOld = y;
				GUI_TOUCH_StoreState(x, y);
			}
		}
		/* the touch was previously released, store state regardless position */
		else 
		{
			if ((x != 0) && (y != 0)) 
			{
				xOld = x;
				yOld = y;
				PressedOld = 1;
				GUI_TOUCH_StoreState(x, y);
			}
		}
	}
	else   /* touch screen is not pressed */
	{
		if (PressedOld == 1) 
		{
			PressedOld = 0;
			GUI_TOUCH_StoreState(-1, -1);
		}
	}
}

/**
  * @brief  Main Programme
  * @param  None
  * @retval None
  */
int main(void) 
{
	FMC_SDRAM_Init();
	touchpanel_init();	
	TIM3_Int_Init(999,83);   /* 1ms */
	TIM4_Int_Init(999,839);  /* 10ms */

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();

	if( touch_type == RESISTIVE_TOUCH )
	{
		exec_calibration(LCD_X_SIZE, LCD_Y_SIZE);   /* resistance calibration */
	}

	GUIDEMO_Main();

	/* GUI initialization */
	while (1)
	{
	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
