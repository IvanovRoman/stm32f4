/**
 *	Keil project template
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@conf		PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */

/* Include core modules */
#include "stm32f4xx.h"

/* In stdio.h file is everything related to output stream */
#include "stdio.h"

/* Include my libraries here */
uint8_t rxbuf[256];
uint32_t rxptrh, rxptrl;
uint32_t txlen;
uint8_t *txptr;
#define RXBUFSIZE  (sizeof(rxbuf) - 1)
const uint8_t menu[] = "This is uart interrupt example!\n\r";

/* Private functions ---------------------------------------------------------*/
void USART_Configuration(void);
void uart_send(uint8_t *buf, uint32_t len);
uint16_t uart_getchar(void);

/**
  * @brief  This function handles USART.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
  uint8_t ch;

  if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
    ch = USART1->DR;
    if (((rxptrh + 1) & RXBUFSIZE) != rxptrl) {
      rxbuf[rxptrh++] = ch;
      rxptrh &= RXBUFSIZE;
    }
  }
  if (USART_GetITStatus(USART1, USART_IT_TXE)) {
    if (txlen) {
      txlen--;
      USART1->DR = *txptr++;
    }
    if (!txlen) {
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {

	USART_Configuration();

	uart_send((uint8_t*)menu, sizeof(menu));
	while (1) 
	{
    uint16_t ch = uart_getchar();
    if (ch < 0x100) {
      uart_send((uint8_t*)&ch, 1);
    }
	}
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void USART_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Configure GPIO to the USART1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /**************************************************************************** 
	 *    baud rate:     115200
   *    Word length:   8bit
   *    Stop bit:      bit
   *    Flow control:  None
   ****************************************************************************/
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  serial transmit function.
  * @param  uint8_t *buf: pointer to send data buffer.
  *      		uint32_t len: transmit data length.
  * @retval None
  */
void uart_send(uint8_t *buf, uint32_t len)
{
  txptr = buf;
  txlen = len;
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/**
  * @brief  receive a byte from buffer.
  * @param  None
	* @retval uint16_t : 0x100 no data buffer, 0 - 0xff data buffer length
  */
uint16_t uart_getchar(void)
{
  uint16_t ch = 0x100;
 
  if (rxptrl != rxptrh) {
    ch = rxbuf[rxptrl++];
    rxptrl &= RXBUFSIZE;
  }
  return ch;	
}
