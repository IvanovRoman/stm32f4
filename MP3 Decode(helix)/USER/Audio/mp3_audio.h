/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			Audio.h
** Descriptions:		None
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-11-18
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/
#ifndef __MP3_AUDIO_H
#define __MP3_AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "mp3common.h"
#include "wm8978.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/ 
#define LOCAL_BUFFER_SIZE 1024          /* size of the local buffer in 16 bits words */

/* Private typedef -----------------------------------------------------------*/
typedef enum { FULL=0,LOW_EMPTY=1,HIGH_EMPTY=2 }                AUDIO_PlaybackBuffer_Status ;
typedef enum { MONO, STEREO}                                    AUDIO_Format_enum;
typedef enum { NO_SOUND, IS_PLAYING }                           AUDIO_Playback_status_enum;
typedef enum { LG_8_BITS=8, LG_16_BITS=16}  AUDIO_Length_enum;

/* Type to hold data for the Flying text */
typedef struct
{
    char text[64];  /* Maximum 64 chars */
    u8 text_length; /* Length of the text */
    u8 curr_pos;    /* Current pointer for first char to write	*/
    u8 x;           /* Position to write first char */
    u8 y;           /* Position to write first char */
    u8 disp_length; /* How many chars to show on screen */
    u16 time;       /* How fast to roll the text */
} tflying_text;

/* Type used to read out ID3 Tag, this type fits to the ID3V1 Format */
typedef struct
{
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    uint8_t genre;
} tID3V3;

/* Private function prototypes -----------------------------------------------*/
void Read_ID3V1(void);
void PlayAudioFile(void);
void AUDIO_Cpy_Mono(void); 
int FillBuffer(uint8_t start);
void AUDIO_Playback_Stop(void);
const char* MP3_GetStrResult(int err);
void AUDIO_Play( s8 * buffer, int size ); 
__inline FunctionalState f_lseek(long size) ;
void DMA_Configuration( s8 * addr, int size);
void I2S_Configuration(uint32_t I2S_AudioFreq);
__inline FunctionalState f_read(void *buffer, uint16_t size , uint32_t *Readsize) ;	 
AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_GetStatus(AUDIO_PlaybackBuffer_Status value);
void AUDIO_Init_audio_mode(AUDIO_Length_enum length, uint16_t frequency, AUDIO_Format_enum format);

#endif 

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
