#include "stm32f4xx.h"
#include "lwip/opt.h"
#include "main.h"

uint32_t sys_now(void)
{
  return  get_systick();
}
