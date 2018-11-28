/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：main.c
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "eth_bsp.h"
#include "httpd.h"

__IO uint32_t SystickCnt = 0;

void Delay(uint32_t cnt)
{
  __IO uint32_t delaytimer;
  
  delaytimer = SystickCnt;
  while (delaytimer > SystickCnt);
}

uint32_t get_systick(void)
{
  return SystickCnt;
}

void SysTick_Handler(void)
{
    SystickCnt += 1;
}


/*
 ******************************************************************************
 *函数：void main(void)
 *输入：void
 *输出：int
 *描述：主编程体
 ******************************************************************************
 */
int main (void)
{

  eth_init();
  
  SysTick_Config(SystemCoreClock / 1000);	
  
  LwIP_Init();
	
  httpd_init();
	
  while (1) {
    if (ETH_CheckFrameReceived()) { 
      LwIP_Pkt_Handle();
    }
    LwIP_Periodic_Handle(SystickCnt);
  }
}

