/****************************************Copyright (c)****************************************************
**                                      
**                        http://www.powermcu.com & http://www.hotmcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               wm8978.c
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
#include "wm8978.h"

/* Private function prototypes -----------------------------------------------*/
static uint16_t wm8978_ReadReg(uint8_t _ucRegAddr);
static uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue);
static void wm8978_Reset(void);

/*
	wm8978�Ĵ�������
	����WM8978��I2C���߽ӿڲ�֧�ֶ�ȡ����,��˼Ĵ���ֵ�������ڴ���,��д�Ĵ���ʱͬ�����»���,���Ĵ���ʱ
	ֱ�ӷ��ػ����е�ֵ.
	�Ĵ���MAP��WM8978.pdf �ĵ�67ҳ,�Ĵ�����ַ��7bit,�Ĵ���������9bit
	
  wm8978 register cache
  WM8978 I2C interface does not support read operation, therefore the register value store in memory, 
	when write register, synchronized the memory, when read register return the value of memory.
  register MAP in WM8978.pdf datasheet 67 pages, the register address is 7bit, register data is 9bit
*/
static uint16_t wm8978_RegCash[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
};

/**
  * @brief  configure GPIO I2C, then check WM8978 on the I2C bus
  * @param  None
  * @retval return 1 mean true, 0 mean failure
  */
uint8_t wm8978_Init(void)
{
	uint8_t re;
	
  InitI2C();
	
	if (i2c_CheckDevice(WM8978_SLAVE_ADDRESS) == 0)
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	wm8978_Reset();			/* hardware reset WM8978, set all registers to default value */
	return re;
}

/**
  * @brief  modify headphone output volume
  * @param  _ucVolume: volume value
  * @retval None
  */
void wm8978_SetEarVolume(uint8_t _ucVolume)
{
	uint16_t regL;
	uint16_t regR;

	if (_ucVolume > 0x3F)
	{
		_ucVolume = 0x3F;
	}

	regL = _ucVolume;
	regR = _ucVolume;

	/*
		R52	LOUT1 Volume control
		R53	ROUT1 Volume control
	*/
	/* update left channel volume value */
	wm8978_WriteReg(52, regL | 0x00);

	/* update right channel volume value */
	/* 0x180 mean that the volume is 0 and then updated, avoid adjusting the volume of the sound, 
	   appear disturbing noises, such as rattling, creaking, clicking or squeaking 
	   0x180��ʾ ������Ϊ0ʱ�ٸ���,��������������ֵġ����ա��� */
	wm8978_WriteReg(53, regR | 0x100);	
}

/**
  * @brief  modify speaker output volume
  * @param  _ucVolume: volume value
  * @retval None
  */
void wm8978_SetSpkVolume(uint8_t _ucVolume)
{
	uint16_t regL;
	uint16_t regR;

	if (_ucVolume > 0x3F)
	{
		_ucVolume = 0x3F;
	}

	regL = _ucVolume;
	regR = _ucVolume;

	/*
		R54	LOUT2 (SPK) Volume control
		R55	ROUT2 (SPK) Volume control
	*/
	/* update left channel volume value */
	wm8978_WriteReg(54, regL | 0x00);

	/* update right channel volume value */
	/* 0x180 mean that the volume is 0 and then updated, avoid adjusting the volume of the sound, 
	   appear disturbing noises, such as rattling, creaking, clicking or squeaking 
	   0x180��ʾ ������Ϊ0ʱ�ٸ���,��������������ֵġ����ա��� */
	wm8978_WriteReg(55, regR | 0x100);
}

/**
  * @brief  read headphone volume
  * @param  None
  * @retval volume value
  */
uint8_t wm8978_ReadEarVolume(void)
{
	return (uint8_t)(wm8978_ReadReg(52) & 0x3F );
}

/**
  * @brief  read speaker volume
  * @param  None
  * @retval volume value
  */
uint8_t wm8978_ReadSpkVolume(void)
{
	return (uint8_t)(wm8978_ReadReg(54) & 0x3F );
}

/**
  * @brief  output mute
  * @param  _ucMute: 1 is mute, 0 is not mute.
  * @retval None
  */
void wm8978_OutMute(uint8_t _ucMute)
{
	uint16_t usRegValue;

	if (_ucMute == 1) /* mute */
	{
		usRegValue = wm8978_ReadReg(52); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
	else	/* not mute */
	{
		usRegValue = wm8978_ReadReg(52);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
}

/**
  * @brief  Set MIC gain
  * @param  _ucGain: gain value, 0-63
  * @retval None
  */
void wm8978_SetMicGain(uint8_t _ucGain)
{
	if (_ucGain > GAIN_MAX)
	{
		_ucGain = GAIN_MAX;
	}

	/* PGA ��������  R45�� R46   pdf 19ҳ
		Bit8	INPPGAUPDATE
		Bit7	INPPGAZCL		�����ٸ���
		Bit6	INPPGAMUTEL		PGA����
		Bit5:0	����ֵ��010000��0dB
	
    PGA Volume control  R45�� R46   pdf 19 pages
		Bit8	INPPGAUPDATE
		Bit7	INPPGAZCL
		Bit6	INPPGAMUTEL		PGA mute
		Bit5:0	Gain value��010000 is 0dB   
	*/
	wm8978_WriteReg(45, _ucGain);
	wm8978_WriteReg(46, _ucGain | (1 << 8));
}

/**
  * @brief  set Line gain for input channel
  * @param  _ucGain: gain value, 0-7. 7 maximum, 0 min. ����ֵ,0-7,7���,0��С,��˥���ɷŴ�.
  * @retval None
  */
void wm8978_SetLineGain(uint8_t _ucGain)
{
	uint16_t usRegValue;

	if (_ucGain > 7)
	{
		_ucGain = 7;
	}

	/*
		Mic �����ŵ��������� PGABOOSTL �� PGABOOSTR ����
		Aux �����ŵ������������� AUXL2BOOSTVO[2:0] �� AUXR2BOOSTVO[2:0] ����
		Line �����ŵ��������� LIP2BOOSTVOL[2:0] �� RIP2BOOSTVOL[2:0] ����
	
	  Mic input signal gain control by PGABOOSTL and PGABOOSTR
    Aux input signal gain control by AUX L2BOOST [2: 0] and AUXR2BOOSTVO [2: 0]
    Line input signal gain control by LIP2BOOSTVOL [2: 0] and RIP2BOOSTVOL [2: 0]
	*/
	/*	pdf 21ҳ,R47(������),R48(������), MIC ������ƼĴ���
		R47 (R48���������ͬ)
		B8		PGABOOSTL	= 1, 0��ʾMIC�ź�ֱͨ������, 1��ʾMIC�ź�+20dB����(ͨ���Ծٵ�·)
		B7 = 0, ����
		B6:4	L2_2BOOSTVOL = x, 0��ʾ��ֹ, 1-7��ʾ����-12dB ~ +6dB
		B3 = 0, ����
		B2:0 AUXL2BOOSTVOL = x, 0��ʾ��ֹ,1-7��ʾ����-12dB ~ +6dB
	
	  pdf of 21 page, R47 (left channel), R48 (right channel), MIC gain control register
	  R47 (R48 is same)
	  B8 PGABOOSTL = 1, 0 mean MIC signal no gain, 1 mean MIC signal + 20dB gain (bootstrap circuit)
		B7 = 0, reserved
		B6: 4 L2_2BOOSTVOL = x, 0 mean disabled, 1-7 mean gain -12dB ~ + 6dB
		B3 = 0, reserved
		B2: 0 AUXL2BOOSTVOL = x, 0 is disabled, 1-7 indicates a gain -12dB ~ + 6dB
	*/

	usRegValue = wm8978_ReadReg(47);
	usRegValue &= 0x8F;               /* Bit6: 4 write 0, 1000 1111  ��Bit6:4��0, 1000 1111 */
	usRegValue |= (_ucGain << 4);
	wm8978_WriteReg(47, usRegValue);	/* write input gain value to left channel control register д����������������ƼĴ��� */

	usRegValue = wm8978_ReadReg(48);
	usRegValue &= 0x8F;               /* Bit6: 4 write 0, 1000 1111  ��Bit6:4��0, 1000 1111 */
	usRegValue |= (_ucGain << 4);
	wm8978_WriteReg(48, usRegValue);	/* write input gain value to right channel control register д����������������ƼĴ��� */
}

/**
  * @brief  enters low-power mode
  * @param  None
  * @retval None
  */
void wm8978_PowerDown(void)
{
	wm8978_Reset();			/* hardware reset WM8978, set all registers to default value */
}

/**
  * @brief  configuring WM8978 audio interface (I2S)  ����WM8978����Ƶ�ӿ�(I2S)
  * @param  _usStandard: interface standard, I2S Standard Phillips, I2S Standard MSB or I2S Standard_LSB
            _ucWordLen: word length, 16��24��32
  * @retval None
  */
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen)
{
	uint16_t usReg;

	/* 67 page of Pdf, register list   pdf 67ҳ,�Ĵ����б� */

	/*	REG R4, ��Ƶ�ӿڿ��ƼĴ���
		B8		BCP	 = X, BCLK���ԣ�0��ʾ����,1��ʾ����
		B7		LRCP = x, LRCʱ�Ӽ��ԣ�0��ʾ����,1��ʾ����
		B6:5	WL = x, �ֳ�,00=16bit,01=20bit,10=24bit,11=32bit (�Ҷ���ģʽ����ֳ�24bit)
		B4:3	FMT = x, ��Ƶ���ݸ�ʽ,00=�Ҷ���,01=�����,10=I2S��ʽ,11=PCM
		B2		DACLRSWAP = x, ����DAC���ݳ�����LRCʱ�ӵ���߻����ұ�
		B1 		ADCLRSWAP = x, ����ADC���ݳ�����LRCʱ�ӵ���߻����ұ�
		B0		MONO	= 0, 0��ʾ������,1��ʾ������,����������Ч
	
    REG R4, audio interface control register
    B8		BCP	 = X, BCLK polarity, 0 mean normal, 1 mean reverse
    B7		LRCP = x, LRC clock polarity, 0 mean normal, 1 mean reverse
    B6:5	WL = x, word length, 00=16bit, 01=20bit, 10=24bit, 11=32bit (right aligned mode, maximum word length is 24bit)
    B4:3	FMT = x, audio data format, 00= right aligned, 01= left aligned aligned, 10=I2S format, 11=PCM
    B2		DACLRSWAP = x, control DAC data, appear on the left or right of the LRC clock
    B1 		ADCLRSWAP = x, control ADC data, appear on the left or right of the LRC clock
    B0		MONO	= 0, 0 mean stereo, 1 mean mono, only the left channel is effective
	*/
	usReg = 0;
	if (_usStandard == I2S_Standard_Phillips)	/* I2S Phillips Standard */
	{
		usReg |= (2 << 3);
	}
	else if (_usStandard == I2S_Standard_MSB)	/* MSB left alignment MSB����� */
	{
		usReg |= (1 << 3);
	}
	else if (_usStandard == I2S_Standard_LSB)	/* LSB right alignment LSB�Ҷ��� */
	{
		usReg |= (0 << 3);
	}
	/* PCM standard (16 bit channel frame with long or short frame sync or 16 bit data frame extended to 32 bit frame) 
	   PCM��׼(16λͨ��֡�ϴ������֡ͬ������16λ����֡��չΪ32λͨ��֡) */
	else	
	{
		usReg |= (3 << 3);;
	}

	if (_ucWordLen == 24)
	{
		usReg |= (2 << 5);
	}
	else if (_ucWordLen == 32)
	{
		usReg |= (3 << 5);
	}
	else
	{
		usReg |= (0 << 5);		/* 16bit */
	}
	wm8978_WriteReg(4, usReg);

	/*
		R6��ʱ�Ӳ������ƼĴ���
		MS = 0,  WM8978����ʱ�ӣ���MCU�ṩMCLKʱ��
	
    R6, clock generator control register
    MS = 0, WM8978 passive clock, MCU clock provided by MCLK
	*/
	wm8978_WriteReg(6, 0x000);
}

/**
  * @brief  read wm8978 register
  * @param  _ucRegAddr: register address
  * @retval None
  */
static uint16_t wm8978_ReadReg(uint8_t _ucRegAddr)
{
	return wm8978_RegCash[_ucRegAddr];
}

/**
  * @brief  write wm8978 register
  * @param  _ucRegAddr: register address
            _usValue: register value
  * @retval None
  */
static uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue)
{
	uint8_t ucAck;

	/* starting signal */
	i2c_Start();

	/* send device address + read and write control bit��0 = w�� 1 = r) */
	i2c_SendByte(WM8978_SLAVE_ADDRESS | I2C_WR);

	/* detect ACK response */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		return 0;
	}

	/* send control byte */
	i2c_SendByte(((_ucRegAddr << 1) & 0xFE) | ((_usValue >> 8) & 0x1));

	/* detect ACK response */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		return 0;
	}

	/* send control byte */
	i2c_SendByte(_usValue & 0xFF);

	/* detect ACK response */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		return 0;
	}

	/* stop signal */
	i2c_Stop();

	wm8978_RegCash[_ucRegAddr] = _usValue;
	return 1;
}

/**
  * @brief  configure wm8978 audio channel
  * @param  _InPath: audio input channel configure
            _OutPath: audio output channel configure
  * @retval None
  */
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath)
{
	uint16_t usReg;
	
	if ((_InPath == IN_PATH_OFF) && (_OutPath == OUT_PATH_OFF))
	{
		wm8978_PowerDown();
		return;
	}
	/*
		R1 register Power manage 1
		Bit8    BUFDCOPEN,  Output stage 1.5xAVDD/2 driver enable
 		Bit7    OUT4MIXEN, OUT4 mixer enable
		Bit6    OUT3MIXEN, OUT3 mixer enable
		Bit5    PLLEN, not used ����
		Bit4    MICBEN	,Microphone Bias Enable (MICƫ�õ�·ʹ��)
		Bit3    BIASEN	,Analogue amplifier bias control ��������Ϊ1ģ��Ŵ����Ź���
		Bit2    BUFIOEN , Unused input/output tie off buffer enable
		Bit1:0  VMIDSEL, not 00, the analog amplifier can be work ��������Ϊ��00ֵģ��Ŵ����Ź���
	*/
	usReg = (1 << 3) | (3 << 0);
	if (_OutPath & OUT3_4_ON) 	/* OUT3 and OUT4, enable output to GSM module   OUT3��OUT4ʹ�������GSMģ�� */
	{
		usReg |= ((1 << 7) | (1 << 6));
	}
	if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= (1 << 4);
	}
	wm8978_WriteReg(1, usReg);	/* write register */

	/*
		R2 register Power manage 2
		Bit8	ROUT1EN,	ROUT1 output enable �������������ʹ��
		Bit7	LOUT1EN,	LOUT1 output enable �������������ʹ��
		Bit6	SLEEP, 		0 = Normal device operation   1 = Residual current reduced in device standby mode
		Bit5	BOOSTENR,	Right channel Input BOOST enable ����ͨ���Ծٵ�·ʹ��. �õ�PGA�Ŵ���ʱ����ʹ��
		Bit4	BOOSTENL,	Left channel Input BOOST enable
		Bit3	INPGAENR,	Right channel input PGA enable ����������PGAʹ��
		Bit2	INPGAENL,	Left channel input PGA enable
		Bit1	ADCENR,		Enable ADC right channel
		Bit0	ADCENL,		Enable ADC left channel
	*/
	usReg = 0;
	if (_OutPath & EAR_LEFT_ON)
	{
		usReg |= (1 << 7);
	}
	if (_OutPath & EAR_RIGHT_ON)
	{
		usReg |= (1 << 8);
	}
	if (_InPath & MIC_LEFT_ON)
	{
		usReg |= ((1 << 4) | (1 << 2));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 3));
	}
	if (_InPath & LINE_ON)
	{
		usReg |= ((1 << 4) | (1 << 5));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 3));
	}
	if (_InPath & ADC_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(2, usReg);	/* write register */

	/*
		R3 register Power manage 3
		Bit8	OUT4EN,		OUT4 enable
		Bit7	OUT3EN,		OUT3 enable
		Bit6	LOUT2EN,	LOUT2 output enable
		Bit5	ROUT2EN,	ROUT2 output enable
		Bit4	0,
		Bit3	RMIXEN,		Right mixer enable
		Bit2	LMIXEN,		Left mixer enable
		Bit1	DACENR,		Right channel DAC enable
		Bit0	DACENL,		Left channel DAC enable
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= ((1 << 8) | (1 << 7));
	}
	if (_OutPath & SPK_ON)
	{
		usReg |= ((1 << 6) | (1 << 5));
	}
	if (_OutPath != OUT_PATH_OFF)
	{
		usReg |= ((1 << 3) | (1 << 2));
	}
	if (_InPath & DAC_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(3, usReg);	/* write register */

	/*
		R44 register Input ctrl

		Bit8	MBVSEL, 		Microphone Bias Voltage Control   0 = 0.9 * AVDD   1 = 0.6 * AVDD
		Bit7	0
		Bit6	R2_2INPPGA,		Connect R2 pin to right channel input PGA positive terminal
		Bit5	RIN2INPPGA,		Connect RIN pin to right channel input PGA negative terminal
		Bit4	RIP2INPPGA,		Connect RIP pin to right channel input PGA amplifier positive terminal
		Bit3	0
		Bit2	L2_2INPPGA,		Connect L2 pin to left channel input PGA positive terminal
		Bit1	LIN2INPPGA,		Connect LIN pin to left channel input PGA negative terminal
		Bit0	LIP2INPPGA,		Connect LIP pin to left channel input PGA amplifier positive terminal
	*/
	usReg = 0 << 8;
	if (_InPath & LINE_ON)
	{
		usReg |= ((1 << 6) | (1 << 2));
	}
	if (_InPath & MIC_RIGHT_ON)
	{
		usReg |= ((1 << 5) | (1 << 4));
	}
	if (_InPath & MIC_LEFT_ON)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(44, usReg);	/* write register */


	/*
		R14 register ADC Control
		set high pass filter (optional)  ���ø�ͨ�˲���(��ѡ��)
		Bit8 	HPFEN,	High Pass Filter Enable��ͨ�˲���ʹ�ܣ�0��ʾ��ֹ,1��ʾʹ��
		BIt7 	HPFAPP,	Select audio mode or application mode ѡ����Ƶģʽ��Ӧ��ģʽ,0��ʾ��Ƶģʽ��
		Bit6:4	HPFCUT��Application mode cut-off frequency  000-111ѡ��Ӧ��ģʽ�Ľ�ֹƵ��
		Bit3 	ADCOSR,	ADC oversample rate select: 0=64x (lower power) 1=128x (best performance)
		Bit2   	0
		Bit1 	ADC right channel polarity adjust:  0=normal  1=inverted
		Bit0 	ADC left channel polarity adjust:  0=normal 1=inverted
	*/
	if (_InPath & ADC_ON)
	{
		usReg = (1 << 3) | (0 << 8) | (4 << 0);		/* disable ADC high pass filter, set cut-off frequency ��ֹADC��ͨ�˲���, ���ý���Ƶ�� */
	}
	else
	{
		usReg = 0;
	}
	wm8978_WriteReg(14, usReg);	/* write register */

	/* �����ݲ��˲��� (notch filter),��Ҫ�������ƻ�Ͳ����������,����Х��. disable
		R27��R28��R29��R30 ���ڿ����޲��˲���,pdf 26ҳ
		R7�� Bit7 NFEN   0 ��ʾ��ֹ,1��ʾʹ��

    set notch filter, used to suppress the microphone acoustic feedback, avoid howling.
		R27��R28��R29��R30 control limited wave filter
		R7�� Bit7 NFEN   0 mean disable, 1 mean enable
	*/
	if (_InPath & ADC_ON)
	{
		usReg = (0 << 7);
		wm8978_WriteReg(27, usReg);	/* write register, disable */
		usReg = 0;
		wm8978_WriteReg(28, usReg);	/* write register, disable */
		wm8978_WriteReg(29, usReg);	/* write register, disable */
		wm8978_WriteReg(30, usReg);	/* write register, disable */
	}

	/* auto gain control ALC, R32  - 34  pdf 19ҳ */
	{
		usReg = 0;		/* disable auto gain control */
		wm8978_WriteReg(32, usReg);
		wm8978_WriteReg(33, usReg);
		wm8978_WriteReg(34, usReg);
	}

	/*  R35  ALC Noise Gate Control
		Bit3	NGATEN, Noise gate function enable
		Bit2:0	Noise gate threshold:
	*/
	usReg = (3 << 1) | (7 << 0);		/* disable auto gain control */
	wm8978_WriteReg(35, usReg);

	/*
		Mic �����ŵ��������� PGABOOSTL �� PGABOOSTR ����
		Aux �����ŵ������������� AUXL2BOOSTVO[2:0] �� AUXR2BOOSTVO[2:0] ����
		Line �����ŵ��������� LIP2BOOSTVOL[2:0] �� RIP2BOOSTVOL[2:0] ����
		
		Mic input channel gain control by PGABOOSTL and PGABOOSTR
		Aux input channel gain control by AUXL2BOOSTVO[2:0] and AUXR2BOOSTVO[2:0]
		Line input channel gain control by LIP2BOOSTVOL[2:0] and RIP2BOOSTVOL[2:0]			
	*/

	/*	pdf 21ҳ,R47(������),R48(������), MIC ������ƼĴ���
		R47 (R48���������ͬ)
		B8		PGABOOSTL	= 1, 0��ʾMIC�ź�ֱͨ������, 1��ʾMIC�ź�+20dB����(ͨ���Ծٵ�·)
		B7 = 0, ����
		B6:4	L2_2BOOSTVOL = x, 0��ʾ��ֹ, 1-7��ʾ����-12dB ~ +6dB
		B3 = 0, ����
		B2:0 AUXL2BOOSTVOL = x, 0��ʾ��ֹ,1-7��ʾ����-12dB ~ +6dB
	
	  pdf of 21 page, R47 (left channel), R48 (right channel), MIC gain control register
	  R47 (R48 is same)
	  B8 PGABOOSTL = 1, 0 mean MIC signal no gain, 1 mean MIC signal + 20dB gain (bootstrap circuit)
		B7 = 0, reserved
		B6: 4 L2_2BOOSTVOL = x, 0 mean disabled, 1-7 mean gain -12dB ~ + 6dB
		B3 = 0, reserved
		B2: 0 AUXL2BOOSTVOL = x, 0 is disabled, 1-7 indicates a gain -12dB ~ + 6dB
	*/

	usReg = 0;
	if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= (1 << 8);	/* MIC gain value +20dB */
	}
	if (_InPath & AUX_ON)
	{
		usReg |= (3 << 0);	/* Aux gain value set to 3, users can adjust  Aux����̶�ȡ3,�û��������е��� */
	}
	if (_InPath & LINE_ON)
	{
		usReg |= (3 << 4);	/* Line gain value set to 3, users can adjust  Line����̶�ȡ3,�û��������е��� */
	}
	wm8978_WriteReg(47, usReg);	/* write input gain value to left channel control register д����������������ƼĴ��� */
	wm8978_WriteReg(48, usReg);	/* write input gain value to right channel control register д����������������ƼĴ��� */

	/* ����ADC��������,pdf 27ҳ
		R15 ����������ADC����,R16����������ADC����
		Bit8 	ADCVU  = 1 ʱ�Ÿ��£�����ͬ����������������ADC����
		Bit7:0 	����ѡ�� 0000 0000 = ����
						   0000 0001 = -127dB
						   0000 0010 = -12.5dB  (0.5dB ����)
						   1111 1111 = 0dB  (��˥��)
						   
    Digital ADC volume control
		R15 control the left channel ADC volume, R16 controls the right channel ADC volume
		Bit8 	ADCVU  = 1 update, used to synchronize the left and right channels of the ADC volume
		Bit7:0 	gain select�� 0000 0000 = Mute
						   0000 0001 = -127dB
						   0000 0010 = -12.5dB  (0.5dB step)
						   1111 1111 = 0dB  (unattenuated)
	*/
	usReg = 0xFF;
	wm8978_WriteReg(15, usReg);	/* Select 0dB */
	usReg = 0x1FF;
	wm8978_WriteReg(16, usReg);	/* update */

	/* wm8978_SetMicGain set mic PGA gain */

	/*	R43 register AUXR �C ROUT2 BEEP Mixer Function
		B8:6 = 0
		B5	 MUTERPGA2INV,	Mute input to INVROUT2 mixer
		B4	 INVROUT2,  Invert ROUT2 output �����������������
		B3:1 BEEPVOL = 7;	AUXR input to ROUT2 inverter gain
		B0	 BEEPEN = 1;	Enable AUXR beep input
	*/
	usReg = 0;
	if (_OutPath & SPK_ON)
	{
		usReg |= (1 << 4);	/* ROUT2 reverse, used to drive the speaker */
	}
	if (_InPath & AUX_ON)
	{
		usReg |= ((7 << 1) | (1 << 0));
	}
	wm8978_WriteReg(43, usReg);

	/* R49  Output ctrl
		B8:7	0
		B6		DACL2RMIX,	Left DAC output to right output mixer
		B5		DACR2LMIX,	Right DAC output to left output
		B4		OUT4BOOST,	0 = OUT4 output gain = -1; DC = AVDD / 2��1 = OUT4 output gain = +1.5��DC = 1.5 x AVDD / 2
		B3		OUT3BOOST,	0 = OUT3 output gain = -1; DC = AVDD / 2��1 = OUT3 output gain = +1.5��DC = 1.5 x AVDD / 2
		B2		SPKBOOST,	0 = Speaker gain = -1;  DC = AVDD / 2 ; 1 = Speaker gain = +1.5; DC = 1.5 x AVDD / 2
		B1		TSDEN,   Thermal Shutdown Enable  �������ȱ���ʹ��(ȱʡ1)
		B0		VROI,	Disabled Outputs to VREF Resistance
	*/
	usReg = 0;
	if (_InPath & DAC_ON)
	{
		usReg |= ((1 << 6) | (1 << 5));
	}
	if (_OutPath & SPK_ON)
	{
		usReg |=  ((1 << 2) | (1 << 1));	/* SPK 1.5x gain, temperature protection enable */
	}
	if (_OutPath & OUT3_4_ON)
	{
		usReg |=  ((1 << 4) | (1 << 3));	/* BOOT3 BOOT4 1.5x gain */
	}
	wm8978_WriteReg(49, usReg);

	/*	REG 50
		B8:6	AUXLMIXVOL = 111	AUX for FM radio signal input
		B5		AUXL2LMIX = 1		Left Auxilliary input to left channel
		B4:2	BYPLMIXVOL			volume
		B1		BYPL2LMIX = 0;		Left bypass path (from the left channel input boost output) to left output mixer
		B0		DACL2LMIX = 1;		Left DAC output to left output mixer
	*/
	usReg = 0;
	if (_InPath & AUX_ON)
	{
		usReg |= ((7 << 6) | (1 << 5));
	}
	if ((_InPath & LINE_ON) || (_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
	{
		usReg |= ((7 << 2) | (1 << 1));
	}
	if (_InPath & DAC_ON)
	{
		usReg |= (1 << 0);
	}
	wm8978_WriteReg(50, usReg);
	wm8978_WriteReg(51, usReg);

	/*	R56 register OUT3 mixer ctrl
		B8:7	0
		B6		OUT3MUTE,  	0 = Output stage outputs OUT3 mixer;  1 = Output stage muted �C drives out VMID.
		B5:4	0
		B3		BYPL2OUT3,	OUT4 mixer output to OUT3  (reverse)
		B4		0
		B2		LMIX2OUT3,	Left ADC input to OUT3
		B1		LDAC2OUT3,	Left DAC mixer to OUT3
		B0		LDAC2OUT3,	Left DAC output to OUT3
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= (1 << 3);
	}
	wm8978_WriteReg(56, usReg);

	/* R57 register OUT4 (MONO) mixer ctrl
		B8:7	0
		B6		OUT4MUTE,	0 = Output stage outputs OUT4 mixer  1 = Output stage muted �C drives outVMID.
		B5		HALFSIG,	0 = OUT4 normal output	1 = OUT4 attenuated by 6dB
		B4		LMIX2OUT4,	Left DAC mixer to OUT4
		B3		LDAC2UT4,	Left DAC to OUT4
		B2		BYPR2OUT4,	Right ADC input to OUT4
		B1		RMIX2OUT4,	Right DAC mixer to OUT4
		B0		RDAC2OUT4,	Right DAC output to OUT4
	*/
	usReg = 0;
	if (_OutPath & OUT3_4_ON)
	{
		usReg |= ((1 << 4) |  (1 << 1));
	}
	wm8978_WriteReg(57, usReg);


	/* R11,12  register DAC digital volume
		R11		Left DAC Digital Volume
		R12		Right DAC Digital Volume
	*/
	if (_InPath & DAC_ON)
	{
		wm8978_WriteReg(11, 255);
		wm8978_WriteReg(12, 255 | 0x100);
	}
	else
	{
		wm8978_WriteReg(11, 0);
		wm8978_WriteReg(12, 0 | 0x100);
	}

	/*	R10 register DAC Control
		B8	0
		B7	0
		B6	SOFTMUTE,	Softmute enable:
		B5	0
		B4	0
		B3	DACOSR128,	DAC oversampling rate: 0=64x (lowest power) 1=128x (best performance)
		B2	AMUTE,		Automute enable
		B1	DACPOLR,	Right DAC output polarity
		B0	DACPOLL,	Left DAC output polarity:
	*/
	if (_InPath & DAC_ON)
	{
		wm8978_WriteReg(10, 0);
	}
	;
}

/**
  * @brief  set notch filter, used to suppress the microphone acoustic feedback, avoid howling
  * @param  _NFA0: NFA0[13:0]
            _NFA1: NFA1[13:0]
  * @retval None
  */
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1)
{
	uint16_t usReg;

	/*  page 26
		A programmable notch filter is provided. This filter has a variable centre frequency and bandwidth,
		programmable via two coefficients, a0 and a1. a0 and a1 are represented by the register bits
		NFA0[13:0] and NFA1[13:0]. Because these coefficient values require four register writes to setup
		there is an NFU (Notch Filter Update) flag which should be set only when all four registers are setup.
	*/
	usReg = (1 << 7) | (_NFA0 & 0x3F);
	wm8978_WriteReg(27, usReg);	/* write register */

	usReg = ((_NFA0 >> 7) & 0x3F);
	wm8978_WriteReg(28, usReg);	/* write register */

	usReg = (_NFA1 & 0x3F);
	wm8978_WriteReg(29, usReg);	/* write register */

	usReg = (1 << 8) | ((_NFA1 >> 7) & 0x3F);
	wm8978_WriteReg(30, usReg);	/* write register */
}

/**
  * @brief  control WM8978 GPIO1 output value, 0 or 1
  * @param  _ucValue: GPIO1 output value, 0 or 1
  * @retval None
  */
void wm8978_CtrlGPIO1(uint8_t _ucValue)
{
	uint16_t usRegValue;

	/* R8 register */
	if (_ucValue == 0)
	{
		usRegValue = 6; /* B2:0 = 110 */
	}
	else
	{
		usRegValue = 7; /* B2:0 = 111 */
	}
	wm8978_WriteReg(8, usRegValue);
}

/**
  * @brief  reset wm8978
  * @param  None
  * @retval None
  */
static void wm8978_Reset(void)
{
	/* wm8978 register default value */
	const uint16_t reg_default[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
	};
	uint8_t i;

	wm8978_WriteReg(0x00, 0);

	for (i = 0; i < sizeof(reg_default) / 2; i++)
	{
		wm8978_RegCash[i] = reg_default[i];
	}
}

/************************ (C) COPYRIGHT HAOYU Electronics *****END OF FILE****/
