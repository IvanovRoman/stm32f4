/*
*********************************************************************************************************
*
*	模块名称 : WM8978音频芯片驱动模块
*	文件名称 : bsp_wm8978.h
*	版    本 : V1.0
*	说    明 : 头文件
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-03-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _WM8978_H
#define _WM8978_H

#include "WAV.h"
#include "wm8978_i2c.h"

#define WM8978_SLAVE_ADDRESS    0x34	/* WM8978 I2C从机地址 */

/* WM8978 音频输入通道控制选项, 可以选择多路，比如 MIC_LEFT_ON | LINE_ON */
typedef enum
{
	IN_PATH_OFF		= 0x00,	/* 无输入 */
	MIC_LEFT_ON 	= 0x01,	/* LIN,LIP脚，MIC左声道（接板载咪头） */
	MIC_RIGHT_ON	= 0x02,	/* RIN,RIP脚，MIC右声道（接GPRS模块音频输出） */
	LINE_ON			= 0x04, /* L2,R2 立体声输入 */
	AUX_ON			= 0x08,	/* AUXL,AUXR 立体声输入 */
	DAC_ON			= 0x10,	/* I2S数据DAC (CPU产生音频信号) */
	ADC_ON			= 0x20	/* 输入的音频馈入WM8978内部ADC （I2S录音) */
}IN_PATH_E;

/* WM8978 音频输出通道控制选项, 可以选择多路 */
typedef enum
{
	OUT_PATH_OFF	= 0x00,	/* 无输出 */
	EAR_LEFT_ON 	= 0x01,	/* LOUT1 耳机左声道 */
	EAR_RIGHT_ON	= 0x02,	/* ROUT1 耳机右声道 */
	SPK_ON			= 0x04,	/* LOUT2和ROUT2反相输出单声道,接扬声器 */
	OUT3_4_ON		= 0x08,	/* OUT3 和 OUT4 输出单声道音频， 接GSM模块的音频输入 */
}OUT_PATH_E;

/* 定义最大音量 */
#define VOLUME_MAX		63		/* 最大音量 */
#define VOLUME_STEP		1		/* 音量调节步长 */

/* 定义最大MIC增益 */
#define GAIN_MAX		63		/* 最大增益 */
#define GAIN_STEP		1		/* 增益步长 */


/* 供外部引用的函数声明 */
uint8_t wm8978_Init(void);

void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void wm8978_OutMute(uint8_t _ucMute);
void wm8978_PowerDown(void);

void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);

void wm8978_SetMicGain(uint8_t _ucGain);
void wm8978_SetLineGain(uint8_t _ucGain);
void wm8978_SetSpkVolume(uint8_t _ucVolume);
void wm8978_SetEarVolume(uint8_t _ucVolume);
uint8_t wm8978_ReadEarVolume(void);
uint8_t wm8978_ReadSpkVolume(void);

void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);

void I2S_CODEC_Init(void);
void I2S_StartPlay(uint32_t _uiAudioFreq);
void I2S_StartRecord(uint32_t _uiAudioFreq);
void SAI_Stop(void);

void SAI_Mode_Config(uint32_t AudioFreq);
void SAI_StartPlay(uint32_t _uiAudioFreq);
void SAI_StartRecord(uint32_t _uiAudioFreq);


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

void sai_init(uint32_t AudioFreq);
void I2S_Stop(void);
void StartPlay( uint32_t _uiAudioFreq );
//#endif /* __STM324x9i_EVAL_AUDIOCODEC_H */


#endif
