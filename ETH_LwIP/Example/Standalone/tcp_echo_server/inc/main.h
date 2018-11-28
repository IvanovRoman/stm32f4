/*
 ******************************************************************************
 *              COPYRIGHT 2013 乐科电子工作室
 *
 *文件：main.h
 *作者：乐科电子工作室
 *日期：2013.12.20
 *版本：V1.0 
 *描述：main.c头文件
 ******************************************************************************
 */
#ifndef __MAIN_H__
#define __MAIN_H__


#include "stm32f4xx.h"
#include "eth_bsp.h"

#define MAC_ADDR0   2
#define MAC_ADDR1   1
#define MAC_ADDR2   2
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0
 
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   0
#define IP_ADDR3   100
   
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   0
#define GW_ADDR3   1  

#define RMII_MODE


void Delay(uint32_t cnt);
uint32_t get_systick(void);
#endif /* __MAIN_H__ */
