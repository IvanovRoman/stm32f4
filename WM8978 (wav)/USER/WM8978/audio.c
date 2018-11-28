/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               audio.c
** Descriptions:            WM8978 audio and STM32 SAI driver
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2015-10-25
** Version:                 v1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "audio.h"

/* Private function prototypes -----------------------------------------------*/
static void SAI_GPIO_Config(void);
static void SAI_StartPlay(uint32_t _uiAudioFreq);
static void SAI_StartRecord(uint32_t _uiAudioFreq);
static void SAI_Mode_Config(uint32_t AudioFreq);


/**
  * @brief  configure SAI
  * @param  _uiAudioFreq: sampling frequency, SAI_AudioFreq_8k, SAI_AudioFreq_11k, SAI_AudioFreq_16k, etc.
  * @retval None
  */
static void SAI_StartPlay(uint32_t _uiAudioFreq)
{
  /* Enable SAI Block A to Start the communication */    
	SAI_Mode_Config(_uiAudioFreq);
	SAI_Cmd(CODEC_SAI, ENABLE); 
  SAI_SendData(CODEC_SAI, 0x00);
}

/**
  * @brief  configure SAI
  * @param  _uiAudioFreq: sampling frequency, SAI_AudioFreq_8k, SAI_AudioFreq_11k, SAI_AudioFreq_16k, etc.
  * @retval None
  */
static void SAI_StartRecord(uint32_t _uiAudioFreq)
{
  /* Enable SAI Block A to Start the communication */    
	SAI_Mode_Config(_uiAudioFreq);
	SAI_Cmd(CODEC_SAI, ENABLE); 
  SAI_SendData(CODEC_SAI, 0x00);
}

/**
  * @brief  configure SAI GPIO
  * @param  None
  * @retval None
  */
static void SAI_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SAI and I2C GPIO clocks */
	RCC_AHB1PeriphClockCmd( CODEC_SAI_GPIO_CLOCK, ENABLE);

	/*CODEC_SAI SD MCLK SCK and FS pins configuration --------------------------------*/
	/* SAI1_Block_B Pins configuration  *****************************************/
	/* Connect pin to Periph */
	/* Configuration SAI1_BLockB_SD Alternate */
	GPIO_PinAFConfig(CODEC_SAI_GPIO, CODEC_SAI_SD_PINSRC, GPIO_AF_SAI1); 
	/* Configuration SAI1_BLockB_MCLK Alternate */
	GPIO_PinAFConfig(CODEC_SAI_GPIO, CODEC_SAI_MCK_PINSRC, GPIO_AF_SAI1);  
	/* Configuration SAI1_BLockB_SCK Alternate */  
	GPIO_PinAFConfig(CODEC_SAI_GPIO, CODEC_SAI_SCK_PINSRC, GPIO_AF_SAI1); 
	/* Configuration SAI1_BLockB_FS Alternate */  
	GPIO_PinAFConfig(CODEC_SAI_GPIO, CODEC_SAI_FS_PINSRC, GPIO_AF_SAI1); 

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SAI1); 

	/* Configure pins as AF pushpull */
	GPIO_InitStructure.GPIO_Pin = CODEC_SAI_MCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(CODEC_SAI_GPIO, &GPIO_InitStructure); 

	/* Configure pins as AF pushpull */
	GPIO_InitStructure.GPIO_Pin = CODEC_SAI_FS_PIN | CODEC_SAI_SD_PIN | CODEC_SAI_SCK_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(CODEC_SAI_GPIO, &GPIO_InitStructure); 

	/* Configure pins as AF pushpull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 
}

/**
  * @brief  configure SAI mode
  * @param  _uiAudioFreq: sampling frequency, SAI_AudioFreq_8k, SAI_AudioFreq_11k, SAI_AudioFreq_16k, etc.
  * @retval None
  */
static void SAI_Mode_Config(uint32_t AudioFreq)
{
	uint32_t tmpdiv;

	SAI_InitTypeDef       SAI_InitStructure;
	SAI_FrameInitTypeDef  SAI_FrameInitStructure;
	SAI_SlotInitTypeDef   SAI_SlotInitStructure;

	/* Enable the CODEC_SAI peripheral clock */
	RCC_APB2PeriphClockCmd(CODEC_SAI_CLK, ENABLE);

	/* Configure Master Clock using the following formula :
	MCLK_x = SAI_CK_x / (MCKDIV[3:0] * 2) with MCLK_x = 256 * FS
	FS = SAI_CK_x / (MCKDIV[3:0] * 2) * 256
	MCKDIV[3:0] = SAI_CK_x / FS * 512 */  
	tmpdiv = SAI_CLOCK_SOURCE / (AudioFreq * 512);
	SAI_InitStructure.SAI_NoDivider = SAI_MasterDivider_Enabled;
	SAI_InitStructure.SAI_MasterDivider = tmpdiv;

	/* Configure SAI_Block_x 
	LSBFirst : Disabled 
	DataSize : 16 */
	SAI_InitStructure.SAI_AudioMode = SAI_Mode_MasterTx;
	SAI_InitStructure.SAI_Protocol = SAI_Free_Protocol;
	SAI_InitStructure.SAI_DataSize = SAI_DataSize_16b;
	SAI_InitStructure.SAI_FirstBit = SAI_FirstBit_MSB;
	SAI_InitStructure.SAI_ClockStrobing = SAI_ClockStrobing_RisingEdge;
	SAI_InitStructure.SAI_Synchro = SAI_Asynchronous;
	SAI_InitStructure.SAI_OUTDRIV = SAI_OutputDrive_Enabled;
	SAI_InitStructure.SAI_FIFOThreshold = SAI_FIFOThreshold_1QuarterFull;
	SAI_Init(CODEC_SAI, &SAI_InitStructure);

	/* Configure SAI_Block_x Frame 
	Frame Length : 32
	Frame active Length: 16
	FS Definition : Start frame + Channel Side identification
	FS Polarity: FS active Low
	FS Offset: FS asserted one bit before the first bit of slot 0 */ 
	SAI_FrameInitStructure.SAI_FrameLength = 64;
	SAI_FrameInitStructure.SAI_ActiveFrameLength = 32;  
	SAI_FrameInitStructure.SAI_FSDefinition = I2S_FS_ChannelIdentification;
	SAI_FrameInitStructure.SAI_FSPolarity = SAI_FS_ActiveLow;  
	SAI_FrameInitStructure.SAI_FSOffset = SAI_FS_BeforeFirstBit;
	SAI_FrameInit(CODEC_SAI, &SAI_FrameInitStructure);

	/* Configure SAI Block_x Slot 
	Slot First Bit Offset : 0
	Slot Size   : 16
	Slot Number : 4
	Slot Active : All slot actives */
	SAI_SlotInitStructure.SAI_FirstBitOffset = 0;
	SAI_SlotInitStructure.SAI_SlotSize = SAI_SlotSize_16b; 
	SAI_SlotInitStructure.SAI_SlotSize = SAI_SlotSize_DataSize;
	SAI_SlotInitStructure.SAI_SlotNumber = 4;
	SAI_SlotInitStructure.SAI_SlotActive = USR_SAI_SlotActive;
	SAI_SlotInit(CODEC_SAI, &SAI_SlotInitStructure); 

	SAI_FlushFIFO(CODEC_SAI);

	/* Mono Mode Config */
	#ifdef MONO_MODE
	SAI_MonoModeConfig(CODEC_SAI, SAI_MonoMode);
	#endif /* MONO_MODE */
}

/**
  * @brief  configure WM8978, SAI and to start play
  * @param  _uiAudioFreq: sampling frequency, SAI_AudioFreq_8k, SAI_AudioFreq_11k, SAI_AudioFreq_16k, etc.
  * @retval None
  */
void StartPlay( uint32_t _uiAudioFreq )
{
	wm8978_Init();		/* reset WM8978 */
	SAI_GPIO_Config();
  SAI_Stop();

	/* configure WM8978, input is DAC, output is headphone  配置WM8978芯片,输入为DAC,输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);

	/* adjust the volume */
	wm8978_SetEarVolume(VOLUME_MAX * 0.7);
	wm8978_SetSpkVolume(VOLUME_MAX * 0.7);

	/* configure WM8978 audio interface, Philips standard I2S interface 16bit  配置WM8978音频接口为飞利浦标准I2S接口,16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
	
  SAI_StartPlay(_uiAudioFreq);
}

/**
  * @brief  configure WM8978, SAI and to start recording
  * @param  _uiAudioFreq: sampling frequency, SAI_AudioFreq_8k, SAI_AudioFreq_11k, SAI_AudioFreq_16k, etc.
  * @retval None
  */
void StartRecord( uint32_t _uiAudioFreq )
{
	wm8978_Init();		/* reset WM8978 */
	SAI_GPIO_Config();
  SAI_Stop();
		
	/* configure WM8978, input is MIC, output is headphone  配置WM8978芯片,输入为MIC,输出为耳机 */
	//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* adjust the volume */
	wm8978_SetEarVolume(VOLUME_MAX * 0.7);

	/* set MIC channel gain 设置MIC通道增益 */
	wm8978_SetMicGain(GAIN_MAX * 0.7);

	/* configure WM8978 audio interface, Philips standard I2S interface 16bit  配置WM8978音频接口为飞利浦标准I2S接口,16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	SAI_StartRecord(_uiAudioFreq);
}

/**
  * @brief  disable SAI
  * @param  None
  * @retval None
  */
void SAI_Stop(void)
{
	 SAI_Cmd(CODEC_SAI, DISABLE);
}


/**
  * @brief  starts playing wav audio from a data buffer
  * @param  pBuffer: Pointer to the buffer 
  * @param  Size: Number of audio data BYTES.
  * @retval None
  */
void Audio_Wav_Play(uint8_t* pBuffer, uint32_t Size)
{
  typedef union 
  {
     uint16_t Val;
     struct
     { 
      uint16_t low	 :8;				
	  uint16_t high	 :8;			
	 }bits;	    
  }wav;
  
  uint32_t addr;
  wav temp;
	
	memcpy(&userWav,pBuffer,sizeof(WavHeader));

 if(userWav.numChannels==1 && userWav.bitsPerSample==8 )         /* WAV mono 8bit */
  {
	 for(addr=0; addr<Size; addr++ )
	 {
	   temp.bits.high=0;
	   temp.bits.low=pBuffer[addr];

		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);                           /* left channel data */
		 
		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);                           /* right channel data */
		 
	 } 
  }
  else if(userWav.numChannels==1 && userWav.bitsPerSample==16 )   /* WAV mono 16bit */
  {
     for(addr=0; addr<Size; addr+=2 )
	 { 
	   temp.bits.low=pBuffer[addr];
	   temp.bits.high=pBuffer[addr+1];

		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);                            /* left channel data */

		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);                            /* right channel data */
	 }
  }
  else if(userWav.numChannels==2 && userWav.bitsPerSample==8 )    /* WAV stereo 8bit */
  {
     for(addr=0; addr<Size; addr++ )
	 {
	   temp.bits.high=0;
	   temp.bits.low=pBuffer[addr];                                 /* channel data */

		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);
	 } 
  }
  else if(userWav.numChannels==2 && userWav.bitsPerSample==16 )   /* WAV stereo 16bit */
  {
     for(addr=0; addr<Size; addr+=2 )
	 { 
	   temp.bits.low=pBuffer[addr];
	   temp.bits.high=pBuffer[addr+1];

		while (SAI_GetFlagStatus(CODEC_SAI, SAI_FLAG_FREQ) == RESET) ;
		SAI_SendData (CODEC_SAI,temp.Val);                            /* channel data */
	 } 
  }	 
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
