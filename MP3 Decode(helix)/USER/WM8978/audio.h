/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               audio.h
** Descriptions:            WM8978 audio and STM32 SAI driver
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-10-25
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

#ifndef _AUDIO_H
#define _AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "WAV.h"
#include "wm8978.h"
#include <string.h>
#include "wm8978_i2c.h"

/*----------------------------------------------------------------------------
                    OPTIONAL Configuration defines parameters
  ----------------------------------------------------------------------------*/

#define SAI_AudioFreq_192k          ((uint32_t)192000)
#define SAI_AudioFreq_96k           ((uint32_t)96000)
#define SAI_AudioFreq_48k           ((uint32_t)48000)
#define SAI_AudioFreq_44k           ((uint32_t)44100)
#define SAI_AudioFreq_32k           ((uint32_t)32000)
#define SAI_AudioFreq_22k           ((uint32_t)22050)
#define SAI_AudioFreq_16k           ((uint32_t)16000)
#define SAI_AudioFreq_11k           ((uint32_t)11025)
#define SAI_AudioFreq_8k            ((uint32_t)8000)

/*------------------------------------------------------------------------------
                    USER SAI defines parameters
 -----------------------------------------------------------------------------*/
/* In Main Application the PLL_SAI or PLL_I2S is configured to have this specific
   output, used to have a clean Audio Frequency */
#define SAI_ClockPLLSAI             ((uint32_t)11289600)
#define SAI_ClockPLLI2S             ((uint32_t)49152000)
#define SAI_ClockExtern             ((uint32_t)14000000)

/* Uncomment one these lines to configure SAI PLL source */
#define SAI_CLOCK_SOURCE            SAI_ClockPLLI2S       /* Default configuration */
/* #define SAI_CLOCK_SOURCE            SAI_ClockPLLSAI */
/* #define SAI_CLOCK_SOURCE            SAI_ClockExtern */

/* In W8994 codec the Audio frame contains 4 slots : TDM Mode */
/* TDM format :
 +------------------|------------------|--------------------|-------------------+ 
 | CODEC_SLOT0 Left | CODEC_SLOT1 Left | CODEC_SLOT0 Right  | CODEC_SLOT1 Right |
 +------------------------------------------------------------------------------+ */ 
 
/* To have 2 separate audio stream in Both headphone and speaker the 4 slot must be activated */
#define SAI_CODEC_SLOT0_1           SAI_SlotActive_0 | SAI_SlotActive_1 | SAI_SlotActive_2 | SAI_SlotActive_3
/* To have an audio stream in headphone only SAI Slot 0 and Slot 2 must be activated */ 
#define SAI_CODEC_SLOT0             SAI_SlotActive_0 | SAI_SlotActive_2 
/* To have an audio stream in speaker only SAI Slot 1 and Slot 3 must be activated */ 
#define SAI_CODEC_SLOT1             SAI_SlotActive_1 | SAI_SlotActive_3

/* Uncomment this line to enable SAI mono mode */
/* #define MONO_MODE */

/* Uncomment one of these lines to configure SAI active slot parameter */
/* #define USR_SAI_SlotActive          SAI_CODEC_TDM  */
#define USR_SAI_SlotActive          SAI_CODEC_SLOT0        /* Default configuration */
/* #define USR_SAI_SlotActive          SAI_CODEC_SOLT1 */

/*----------------------------------------------------------------------------
                    Hardware Configuration defines parameters
 -----------------------------------------------------------------------------*/
/* SAI peripheral configuration defines */
#define CODEC_SAI                      SAI1_Block_B
#define CODEC_SAI_CLK                  RCC_APB2Periph_SAI1
#define CODEC_SAI_ADDRESS              (uint32_t)(&SAI1_Block_B->DR)
#define CODEC_SAI_GPIO_AF              GPIO_AF_SAI1
#define CODEC_SAI_GPIO_CLOCK           (RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF)
#define CODEC_SAI_FS_PIN               GPIO_Pin_9
#define CODEC_SAI_SCK_PIN              GPIO_Pin_8
#define CODEC_SAI_SD_PIN               GPIO_Pin_6
#define CODEC_SAI_MCK_PIN              GPIO_Pin_7
#define CODEC_SAI_FS_PINSRC            GPIO_PinSource9
#define CODEC_SAI_SCK_PINSRC           GPIO_PinSource8
#define CODEC_SAI_SD_PINSRC            GPIO_PinSource6
#define CODEC_SAI_MCK_PINSRC           GPIO_PinSource7
#define CODEC_SAI_GPIO                 GPIOF

/* SAI DMA Stream definitions */
#define AUDIO_MAL_DMA_CLOCK            RCC_AHB1Periph_DMA2
#define AUDIO_MAL_DMA_STREAM           DMA2_Stream5
#define AUDIO_MAL_DMA_CHANNEL          DMA_Channel_0
#define AUDIO_MAL_DMA_IRQ              DMA2_Stream5_IRQn
#define AUDIO_MAL_DMA_FLAG_TC          DMA_FLAG_TCIF5
#define AUDIO_MAL_DMA_FLAG_HT          DMA_FLAG_HTIF5
#define AUDIO_MAL_DMA_FLAG_FE          DMA_FLAG_FEIF5
#define AUDIO_MAL_DMA_FLAG_TE          DMA_FLAG_TEIF5
#define AUDIO_MAL_DMA_FLAG_DME         DMA_FLAG_DMEIF5
#define AUDIO_MAL_DMA_FLAG_ALL         (uint32_t)(AUDIO_MAL_DMA_FLAG_TC | AUDIO_MAL_DMA_FLAG_HT | AUDIO_MAL_DMA_FLAG_FE | AUDIO_MAL_DMA_FLAG_TE | AUDIO_MAL_DMA_FLAG_DME)
#define AUDIO_MAL_DMA_PERIPH_DATA_SIZE DMA_PeripheralDataSize_HalfWord
#define AUDIO_MAL_DMA_MEM_DATA_SIZE    DMA_MemoryDataSize_HalfWord
#define DMA_MAX_SZE                    0xFFFF

#define Audio_MAL_IRQHandler           DMA2_Stream5_IRQHandler

/*----------------------------------------------------------------------------
             CONFIGURATION: Audio Codec Driver Configuration parameters
  ----------------------------------------------------------------------------*/
/* Audio Transfer mode (DMA, Interrupt or Polling) */
/* Uncomment this line to enable the audio Transfer using DMA in normal mode */
//#define AUDIO_MAL_MODE_NORMAL

/* Uncomment this line to enable the audio Transfer using DMA in circular mode */         
#define AUDIO_MAL_MODE_CIRCULAR

/* For the DMA modes select the interrupt that will be used */
/* Uncomment this line to enable DMA Transfer Complete interrupt */
#define AUDIO_MAL_DMA_IT_TC_EN  
/* Uncomment this line to enable DMA Half Transfer Complete interrupt */      
#define AUDIO_MAL_DMA_IT_HT_EN
/* Uncomment this line to enable DMA Transfer Error interrupt */  
/* #define AUDIO_MAL_DMA_IT_TE_EN */  

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
/* Select the preemption priority level(0 is the highest) */
#define EVAL_AUDIO_IRQ_PREPRIO           0
/* Select the sub-priority level (0 is the highest) */  
#define EVAL_AUDIO_IRQ_SUBRIO            0  

/* Private function prototypes -----------------------------------------------*/
void SAI_Stop(void);
void StartRecord( uint32_t _uiAudioFreq );
void StartPlay( uint32_t _uiAudioFreq );
void Audio_Wav_Play(uint8_t* pBuffer, uint32_t Size);
#endif

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
