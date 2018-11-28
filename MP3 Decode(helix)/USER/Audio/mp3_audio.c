/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			Audio.C
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "mp3_audio.h"
#include "MP3Sample.c"

/* Private define ------------------------------------------------------------*/
#define READBUF_SIZE  (5 * 1024)       /* Value must min be 2xMAINBUF_SIZE = 2x1940 = 3880bytes */


/* Private variables ---------------------------------------------------------*/
AUDIO_PlaybackBuffer_Status BufferStatus; /* Status of buffer */
s16 AudioBuffer[4608];          /* Playback buffer - Value must be 4608 to hold 2xMp3decoded frames */
MP3FrameInfo mp3FrameInfo;      /* Content is the output from MP3GetLastFrameInfo, 
                                   we only read this once, and conclude it will be the same in all frames
                                   Maybe this needs to be changed, for different requirements */
static HMP3Decoder hMP3Decoder; /* Content is the pointers to all buffers and information for the MP3 Library */
volatile uint32_t bytesLeft;    /* Saves how many bytes left in readbuf	*/
volatile uint32_t outOfData;    /* Used to set when out of data to quit playback */
volatile AUDIO_PlaybackBuffer_Status  audio_buffer_fill ; 
volatile int Audio_buffer_index = 0;
volatile uint16_t Audio_buffer_size = 0;
volatile s16 Audio_buffer_local[LOCAL_BUFFER_SIZE];       /* working buffer for MONO mode management */
AUDIO_Playback_status_enum AUDIO_Playback_status ;
AUDIO_Format_enum AUDIO_Format ;
AUDIO_Length_enum AUDIO_Length ;
AUDIO_PlaybackBuffer_Status bufferstatus_local;
s8  *Audio_buffer;
int Audio_buffer_local_size = LOCAL_BUFFER_SIZE;
uint8_t readBuf[READBUF_SIZE];       /* Read buffer where data from SD card is read to */ 
uint8_t *readPtr;                    /* Pointer to the next new data */
uint32_t MP3Size;                  
uint32_t MP3_Data_Index;            
uint16_t AUDIO_Frequency ;
uint32_t n_Read;                     /* File R/W count */

/*******************************************************************************
* Function Name  : f_read
* Description    : read MP3 data
* Input          : - buffer: Pointer to data buffer
*				   - size: Number of bytes to read 
*                  - Readsize: Pointer to number of bytes read
* Output         : None
* Return         : return ENABLE mean succeed
* Attention		 : None
*******************************************************************************/
__inline FunctionalState f_read(void *buffer, uint16_t size , uint32_t *Readsize) 
{
	if(  MP3Size - MP3_Data_Index >= size )  
	{
	  memcpy(buffer,(MP3_DATA + MP3_Data_Index),size);
	  MP3_Data_Index+=size;
	  *Readsize=size; 
	  return ENABLE;
	}
	else 
	{
	  memcpy(buffer,(MP3_DATA + MP3_Data_Index),MP3Size - MP3_Data_Index);
	  MP3_Data_Index+=MP3Size - MP3_Data_Index;
	  *Readsize=MP3Size - MP3_Data_Index;
      return DISABLE;
	}	
}

/*******************************************************************************
* Function Name  : f_read
* Description    : read MP3 data
* Input          : - buffer: Pointer to data buffer
*				   - size: Number of bytes to read 
*                  - Readsize: Pointer to number of bytes read
* Output         : None
* Return         : return ENABLE mean succeed
* Attention		 : None
*******************************************************************************/
__inline FunctionalState f_lseek(long size)  
{
	if(  size  <=  MP3Size )  
	{
	   MP3_Data_Index=size;
	   return ENABLE;
	}
	else 
	{
	   return DISABLE;
	}
}

/*******************************************************************************
* Function Name  : AUDIO_PlaybackBuffer_Status
* Description    : Gets the status of Playback buffer
* Input          : - value: 0 = get the current status, else reset the flag
*                           passed in parameter
* Output         : None
* Return         : FULL=0, LOW_EMPTY=1, HIGH_EMPTY=2
* Attention		 : None
*******************************************************************************/
AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_GetStatus(AUDIO_PlaybackBuffer_Status value)
{
    if ( value )
        audio_buffer_fill &= ~value;
    return audio_buffer_fill; 
}

/*******************************************************************************
* Function Name  : AUDIO_Playback_Stop
* Description    : Stop the playback by stopping the DMA transfer
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void AUDIO_Playback_Stop(void)
{
  /* Enable the SAI DMA Stream*/
  DMA_Cmd(AUDIO_MAL_DMA_STREAM, DISABLE);

   AUDIO_Playback_status = NO_SOUND;       
   /* Shutdwon codec in order to avoid non expected voices */
   //codec_send( ACTIVE_CONTROL | INACTIVE );
}

/*******************************************************************************
* Function Name  : Read_ID3V1
* Description    : Read the ID3 Tag from the MP3, and add it to the flying text array
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Read_ID3V1(void)
{
    tID3V3 tst_buff;
	uint8_t i;
    f_lseek  ( sizeof(MP3_DATA) - sizeof(tst_buff) ); 
    f_read (&tst_buff,sizeof(tst_buff),(uint32_t *)&i); 
    f_lseek  (0);       
    
    if ( strstr(tst_buff.tag, "TAG") )
    {
 	   printf("-- title %s   \r\n",tst_buff.title);	 
 	   printf("-- artist %s  \r\n",tst_buff.artist);	 
 	   printf("-- album %s   \r\n",tst_buff.album); 
	   printf("-- year %s    \r\n",tst_buff.year);
	   printf("-- comment %s \r\n",tst_buff.comment);	 	 
    }
}

/*******************************************************************************
* Function Name  : FillBuffer
* Description    : Fill the buffer with Music from MP3 Data
* Input          : - FileObject: file system 
*                  - Start: If 1, we will fill buffer nomatter the status of the playback,
*                           used in beginning of file (pre-fill)
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int FillBuffer(uint8_t start)  
{
    uint8_t word_Allignment; /* Variable used to make sure DMA transfer is alligned to 32bit */                
    int err=0;           /* Return value from MP3decoder */
    int offset;          /* Used to save the offset to the next frame */
    
    if(!start)    /* If we are in start pos we overwrite bufferstatus */ 
        BufferStatus = AUDIO_PlaybackBuffer_GetStatus( (AUDIO_PlaybackBuffer_Status) 0 ); 
    else
        BufferStatus = (AUDIO_PlaybackBuffer_Status) ( LOW_EMPTY | HIGH_EMPTY );    

        /* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */  
		if (bytesLeft < 2*MAINBUF_SIZE /*&& !eofReached*/) 
		{ 
          /* move last, small chunk from end of buffer to start, then fill with new data */ 
          word_Allignment = 4 - (bytesLeft % 4 );   /* Make sure the first byte in writing pos. is word alligned */ 
          memmove(readBuf+word_Allignment, readPtr, bytesLeft);
          f_read((uint8_t *)readBuf+word_Allignment + bytesLeft,READBUF_SIZE - bytesLeft - word_Allignment,&n_Read);
		 
		  if( n_Read !=0 )
		  {
             /* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
             if (n_Read < READBUF_SIZE - bytesLeft - word_Allignment)
             {    
		        memset(readBuf + bytesLeft + n_Read, 0, READBUF_SIZE - bytesLeft - n_Read);
		     }	          
             bytesLeft += n_Read;
		     readPtr = readBuf+word_Allignment;
		  }	
		  else	  
		  {
		     outOfData = 1;
		  }		  	 		  		 
		}  

        if(outOfData == 1)
        {
            /* Stop Playback */  
            AUDIO_Playback_Stop();
            return 0; 
        }

		/* find start of next MP3 frame - assume EOF if no sync found */
		offset = MP3FindSyncWord(readPtr, bytesLeft);   
		if (offset < 0) 
		{
		    bytesLeft = 0;
		    return -1;
		}
    
		readPtr += offset;			  /* data start point */
		bytesLeft -= offset;		  /* in buffer */
		if (BufferStatus & LOW_EMPTY) /* Decode data to low part of buffer */
        {
           err = MP3Decode(hMP3Decoder, &readPtr, (int *)&bytesLeft, (short *)AudioBuffer, 0); 	
  		   BufferStatus = AUDIO_PlaybackBuffer_GetStatus(LOW_EMPTY);				
        }
 		else if (BufferStatus & HIGH_EMPTY)     /* Decode data to the high part of buffer */
        {
		   err = MP3Decode(hMP3Decoder, &readPtr, (int *)&bytesLeft, (short *)AudioBuffer+mp3FrameInfo.outputSamps,0);	
		   BufferStatus = AUDIO_PlaybackBuffer_GetStatus(HIGH_EMPTY);	 			 
        } 
        if (err!=0) 
        {  
            bytesLeft=0;
            readPtr = readBuf;
			printf("-- MP3Decode Error Occurred %s \r\n",MP3_GetStrResult(err));
            /* error occurred */
            switch (err) /* There is not implemeted any error handling. it will quit the playing in case of error */
            {
                /* do nothing - next call to decode will provide more mainData */
                default:
				  return -1; 				  
            }
        }     
  return 0;
}

/*******************************************************************************
* Function Name  : MP3_GetStrResult
* Description    : Convert file access error number in string
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
const char* MP3_GetStrResult(int err)
{
    switch(err) {
        case ERR_MP3_NONE :                   return "ERR_MP3_NONE";
        case ERR_MP3_INDATA_UNDERFLOW :       return "ERR_MP3_INDATA_UNDERFLOW";
        case ERR_MP3_MAINDATA_UNDERFLOW :     return "ERR_MP3_MAINDATA_UNDERFLOW";
        case ERR_MP3_FREE_BITRATE_SYNC :      return "ERR_MP3_FREE_BITRATE_SYNC";
        case ERR_MP3_OUT_OF_MEMORY :          return "ERR_MP3_OUT_OF_MEMORY";
        case ERR_MP3_NULL_POINTER :           return "ERR_MP3_NULL_POINTER";
        case ERR_MP3_INVALID_FRAMEHEADER :    return "ERR_MP3_INVALID_FRAMEHEADER";   
        case ERR_MP3_INVALID_SIDEINFO :       return "ERR_MP3_INVALID_SIDEINFO"; 
        case ERR_MP3_INVALID_SCALEFACT :      return "ERR_MP3_INVALID_SCALEFACT";
        case ERR_MP3_INVALID_HUFFCODES :      return "ERR_MP3_INVALID_HUFFCODES";
        case ERR_MP3_INVALID_DEQUANTIZE :     return "ERR_MP3_INVALID_DEQUANTIZE";
        case ERR_MP3_INVALID_IMDCT :          return "ERR_MP3_INVALID_IMDCT";
        case ERR_MP3_INVALID_SUBBAND :        return "ERR_MP3_INVALID_SUBBAND";
        case ERR_UNKNOWN :                    return "ERR_UNKNOWN";  
        default :                             return "?";
    }  
}

/*******************************************************************************
* Function Name  : DMA_Configuration
* Description    : DMA1 channel5 configuration
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DMA_Configuration( s8 * addr, int size)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable the SAI  */
  //SAI_Cmd(CODEC_SAI, DISABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(AUDIO_MAL_DMA_CLOCK, ENABLE); 

	  /* Configure the DMA Stream */
  DMA_Cmd(AUDIO_MAL_DMA_STREAM, DISABLE);
  DMA_DeInit(AUDIO_MAL_DMA_STREAM);
	
    if(AUDIO_Format == STEREO)
    {
       /* Uses the original buffer	*/            
	   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) addr; /* Set the buffer */
	   DMA_InitStructure.DMA_BufferSize = size;			       /* Set the size */
    }
    else
    {
       /* Uses the local buffer */
       if ( AUDIO_Length == LG_16_BITS )   size *= 2;   
       Audio_buffer = addr;                         /* Copy the pointer to real databuffer/source */
       Audio_buffer_index = 0;                      /* Copy the pointer to real databuffer/source */
       Audio_buffer_size = size;                    /* Size to the real databuffer/source */
	   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Audio_buffer_local;   /* Set the buffer */
	   DMA_InitStructure.DMA_BufferSize = ((Audio_buffer_local_size)/2);;	  /* Set the size */
    }

  /* Set the parameters to be configured */
  DMA_InitStructure.DMA_Channel = AUDIO_MAL_DMA_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = CODEC_SAI_ADDRESS;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = AUDIO_MAL_DMA_PERIPH_DATA_SIZE;
  DMA_InitStructure.DMA_MemoryDataSize = AUDIO_MAL_DMA_MEM_DATA_SIZE; 
#ifdef AUDIO_MAL_MODE_NORMAL
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
#elif defined(AUDIO_MAL_MODE_CIRCULAR)
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
#else
  #error "AUDIO_MAL_MODE_NORMAL or AUDIO_MAL_MODE_CIRCULAR should be selected !!"
#endif /* AUDIO_MAL_MODE_NORMAL */  
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
  DMA_Init(AUDIO_MAL_DMA_STREAM, &DMA_InitStructure);  

  /* Enable the selected DMA interrupts (selected in "stm32_eval_audio_codec.h" defines) */
#ifdef AUDIO_MAL_DMA_IT_TC_EN
  DMA_ITConfig(AUDIO_MAL_DMA_STREAM, DMA_IT_TC, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_TC_EN */
#ifdef AUDIO_MAL_DMA_IT_HT_EN
  DMA_ITConfig(AUDIO_MAL_DMA_STREAM, DMA_IT_HT, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_HT_EN */
#ifdef AUDIO_MAL_DMA_IT_TE_EN
  DMA_ITConfig(AUDIO_MAL_DMA_STREAM, DMA_IT_TE | DMA_IT_FE | DMA_IT_DME, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_TE_EN */
  
  /* Enable the SAI DMA request */
  SAI_DMACmd(CODEC_SAI, ENABLE);
  /* Enable the SAI DMA Stream*/
  DMA_Cmd(AUDIO_MAL_DMA_STREAM, ENABLE);

  
  /* If the SAI peripheral is still not enabled, enable it */
  if (SAI_GetCmdStatus(CODEC_SAI) == DISABLE)
  {
    SAI_Cmd(CODEC_SAI, ENABLE);
  }  
}


/*******************************************************************************
* Function Name  : PlayAudioFile
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PlayAudioFile(void)
{
   /* Reset counters */
   bytesLeft = 0;   
   outOfData = 0;   
 
   readPtr = readBuf;  
   hMP3Decoder = MP3InitDecoder();	 
   MP3Size = sizeof(MP3_DATA); 
   MP3_Data_Index=0; 
   Read_ID3V1();  
   /* Read the first data to get info about the MP3 File */
   while( FillBuffer( 1 ) ); 	 /* Must Read MP3 file header information */

   /* Get the length of the decoded data, so we can set correct play length */                        
   MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo); 
  
   /* Select the correct samplerate and Mono/Stereo */
   AUDIO_Init_audio_mode( ( AUDIO_Length_enum )mp3FrameInfo.bitsPerSample,  \
						((MP3DecInfo *)hMP3Decoder)->samprate,  \
						(((MP3DecInfo *)hMP3Decoder)->nChans==1) ? MONO : STEREO);

   /* Start the playback */
   AUDIO_Playback_status = IS_PLAYING;   
   while( !(outOfData==1) )
   {
      FillBuffer(0);
   } 					 
}

void DMA2_Stream5_IRQHandler(void)
{   
  /* Transfer complete interrupt */
  if (DMA_GetFlagStatus(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_TC) != RESET)  /* DMA1 通道5 传输完成中断 */
  {           
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_TC);
      bufferstatus_local |= HIGH_EMPTY;
      if(AUDIO_Format == MONO)
      {
         AUDIO_Cpy_Mono();
      }
      else
      {
         audio_buffer_fill |= HIGH_EMPTY;
      }
  }

  if (DMA_GetFlagStatus(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_HT) != RESET)
  {      
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_STREAM, AUDIO_MAL_DMA_FLAG_HT); 
     bufferstatus_local |= LOW_EMPTY;
     if(AUDIO_Format == MONO)
     {
        AUDIO_Cpy_Mono();
     }
     else 
     {
        audio_buffer_fill |= LOW_EMPTY;
     }		
  }
}


/*******************************************************************************
* Function Name  : AUDIO_Cpy_Mono
* Description    : Copy Mono data to small buffer, set both Left+Right Channel 
*                  to samme value
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void AUDIO_Cpy_Mono(void)
{
    volatile uint16_t i;
    s16  temp;
    
    /* Stop if end of original buffer */
    if ( Audio_buffer_index > Audio_buffer_size )
        AUDIO_Playback_Stop();
    
    /* End of the first half working buffer	*/
    if(bufferstatus_local & LOW_EMPTY)
    {
        if (AUDIO_Length == LG_16_BITS)
        {
            for( i=0 ; i < ((Audio_buffer_local_size / 4) - 1) ; i = i + 2 )
            {
                temp = *(s16*)(Audio_buffer+Audio_buffer_index);
                Audio_buffer_local[i+1] = Audio_buffer_local[i] = temp;
                Audio_buffer_index += 2;   /* 16 bit */
            }
        }
        else
        {
            for(i=0; i < ((Audio_buffer_local_size / 4)-1); i=i+2)
            {
                temp = ((s16)((*(Audio_buffer+Audio_buffer_index)^0x80)))<<8;
                Audio_buffer_local[i+1] = Audio_buffer_local[i] = temp;
                Audio_buffer_index++;      /* 8 bit */     
            }
        }
        bufferstatus_local &= ~LOW_EMPTY;
    }

    /* End of the second half working buffer */
    if(bufferstatus_local & HIGH_EMPTY)
    {
        if (AUDIO_Length == LG_16_BITS)
        {
            for( i = (Audio_buffer_local_size / 4) ; i < (Audio_buffer_local_size / 2 ); i = i + 2 )
            {
                temp = *(s16*)(Audio_buffer+Audio_buffer_index);
                Audio_buffer_local[i+1] = Audio_buffer_local[i] = temp;
                Audio_buffer_index += 2; /* 16 bit */
            }
        }
        else
        {
            for(i = (Audio_buffer_local_size / 4); i < ((Audio_buffer_local_size) / 2)-1; i=i+2)
            {
                temp = ((s16)((*(Audio_buffer+Audio_buffer_index)^0x80)))<<8;
                Audio_buffer_local[i+1] = Audio_buffer_local[i] = temp;
                Audio_buffer_index++;    /* 8 bit */     
            }
        }
        bufferstatus_local &= ~HIGH_EMPTY;
    }
    
    if ( Audio_buffer_index == (Audio_buffer_size/2) )
    {
        audio_buffer_fill |= LOW_EMPTY;
        if ( audio_buffer_fill & HIGH_EMPTY )         
        {
            AUDIO_Playback_Stop();
        }
    }
    if ( Audio_buffer_index >= Audio_buffer_size )
    {
        audio_buffer_fill |= HIGH_EMPTY;
        Audio_buffer_index = 0;
        if ( audio_buffer_fill & LOW_EMPTY )         
        {
            AUDIO_Playback_Stop();
        }
    }
}


/*******************************************************************************
* Function Name  : AUDIO_Init_audio_mode
* Description    : Initialization for Audio Mode use of the WM8731 codec.set WM8731
*                  to Audio Mode via I2C. enable STM32 I2S communication to send
*                  audio samples (SPI2/I2S2 port) in DMA mode
* Input          : - length: 8 bits or 16 bits length sample
*				   - frequency:	8 KHz, 16 KHz, 22 KHz, 44 KHz, 48 KHz sample
*				   - format: mono, stereo
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void AUDIO_Init_audio_mode(AUDIO_Length_enum length, uint16_t frequency, AUDIO_Format_enum format)
{
   /* The MONO mode uses working buffer to dupplicate datas on the two channels
      and switch buffer half by half => uses DMA in circular mode */
  
   NVIC_InitTypeDef NVIC_InitStructure;

   StartPlay(frequency);
   /* DMA IRQ Channel configuration */
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
  /* SAI DMA IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_MAL_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EVAL_AUDIO_IRQ_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = EVAL_AUDIO_IRQ_SUBRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif 
	
   Audio_buffer_index = 0;
   AUDIO_Playback_status = NO_SOUND;
   AUDIO_Length = length;		
   AUDIO_Frequency = frequency;
   AUDIO_Format = format;

   /* Buffers are supposed to be empty here	*/
   audio_buffer_fill = (AUDIO_PlaybackBuffer_Status) (LOW_EMPTY | HIGH_EMPTY );
   bufferstatus_local =  (AUDIO_PlaybackBuffer_Status) (LOW_EMPTY | HIGH_EMPTY );
   
   DMA_Configuration((s8 *)AudioBuffer,(mp3FrameInfo.outputSamps*2)); 

   printf( "-- MP3 file header information \r\n");
   printf( "-------------------------------- \r\n");
   printf( "-- MP3 bitrate        = %d\r\n", mp3FrameInfo.bitrate);	   /* 比特率 */
   printf( "-- MP3 bitsPerSample  = %d\r\n", AUDIO_Length);
   printf( "-- MP3 samprate       = %d\r\n", AUDIO_Frequency);  
   printf( "-- MP3 nChans         = %d\r\n", AUDIO_Format);  
   printf( "-- MP3 nGranSamps     = %d\r\n", ((MP3DecInfo *)hMP3Decoder)->nGranSamps);
   /* 数据帧大小 FrameSize = (((MpegVersion == MPEG1 ? 144 : 72) * Bitrate) / SamplingRate) + PaddingBit */
   printf( "-- MP3 outputSamps    = %d\r\n", mp3FrameInfo.outputSamps);  
   printf( "-- MP3 layer          = %d\r\n", mp3FrameInfo.layer);
   printf( "-- MP3 version        = %d\r\n", mp3FrameInfo.version);
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
