/*
 ******************************************************************************
 *              COPYRIGHT 2013 �ֿƵ��ӹ�����
 *
 *�ļ���main.c
 *���ߣ��ֿƵ��ӹ�����
 *���ڣ�2013.12.20
 *�汾��V1.0 
 *������
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
 *������void main(void)
 *���룺void
 *�����int
 *�������������
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

