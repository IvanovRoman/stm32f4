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
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "udp_echoclient.h"

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

/**
  * @brief  This function handles EXTI15_10
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void) 
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {

    /*connect to tcp server */ 
    udp_echoclient_connect();

    /* Clear the EXTI line  pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;


  /* Enable the BUTTON Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;


  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
  KEY_Init();
  SysTick_Config(SystemCoreClock / 1000);	
  
  LwIP_Init();


  while (1) {
    /* check if any packet received */
    if (ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(SystickCnt);
  }
}

