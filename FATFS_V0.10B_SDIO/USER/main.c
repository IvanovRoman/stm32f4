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
#include "ff.h"

/* In stdio.h file is everything related to output stream */
#include "stdio.h"
#include <string.h>

/* Include my libraries here */

/* Private variables ---------------------------------------------------------*/
char path[512]="0:";

/* Private functions ---------------------------------------------------------*/
void USART_Configuration(void);
FRESULT scan_files (char* path);
FRESULT FATFS_DriveSize(uint32_t* total, uint32_t* free);

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
int main(void) {
	/* Fatfs object */
	FATFS FatFs;
	/* File object */
	FIL fil;
	/* Free and total space */
	uint32_t total, free;
	
	USART_Configuration();

	printf("*****************************************************************\r\n");
	printf("*                                                               *\r\n");
	printf("*  Thank you for using SOM-STM32F429IG Development Board ! ^_^  *\r\n");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");

	/* Mount drive */
	if (f_mount(&FatFs, "0:", 1) == FR_OK) {

		/* Try to open file */
		if (f_open(&fil, "0:1stfile.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {
			
			/* If we put more than 0 characters (everything OK) */
			if (f_puts("First string in my file\n", &fil) > 0) {
				
				scan_files(path);
				if (FATFS_DriveSize(&total, &free) == FR_OK) {
					/* Data for drive size are valid */
					printf("\r\n%d MB total drive space.\r\n"
								 "%d MB available.\r\n",
                 total/1024 , free/1024);
				}
			}
			/* Close file, don't forget this! */
			f_close(&fil);
		}
		
		/* Unmount drive, don't forget this! */
		f_mount(0, "0:", 1);
	}
	
	while (1) 
	{

	}
}

/*******************************************************************************
* Function Name  : scan_files
* Description    : File space occupancy
* Input          : None
* Output         : None
* Return         : FRESULT
* Attention		   : None
*******************************************************************************/
FRESULT FATFS_DriveSize(uint32_t* total, uint32_t* free) {
	FATFS *fs;
	DWORD fre_clust;
	FRESULT res;

	/* Get volume information and free clusters of drive */
	res = f_getfree("0:", &fre_clust, &fs);
	if (res != FR_OK) {
		return res;
	}

   /* Get total sectors and free sectors */
   *total = (fs->n_fatent - 2) * fs->csize / 2;
   *free = fre_clust * fs->csize / 2;
	
	/* Return OK */
	return FR_OK;
}

/*******************************************************************************
* Function Name  : scan_files
* Description    : search directory all the files
* Input          : - path: root directory
* Output         : None
* Return         : FRESULT
* Attention		   : Does not support long file names
*******************************************************************************/
FRESULT scan_files (char* path)
{
    FILINFO fno;
		FRESULT res;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
                printf("%s/%s \r\n", path, fn);
            }
        }
    }
    return res;
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

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
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
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
  USART_Cmd(USART1, ENABLE);
}

/* We need to implement own __FILE struct */
/* FILE struct is used from __FILE */
struct __FILE {
    int dummy;
};
 
/* You need this if you want use printf */
/* Struct FILE is implemented in stdio.h */
FILE __stdout;
 
int fputc(int ch, FILE *f) {
	
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* Do your stuff here */
    /* Send your custom byte */
		/* Send byte to USART */
		USART_SendData(USART1, ch);
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);   
	
    /* If everything is OK, you have to return character written */
    return ch;
    /* If character is not correct, you can return EOF (-1) to stop writing */
    //return -1;
}

int fgetc(FILE *f) 
{
    int ch;
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    ch = USART_ReceiveData(USART1);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (uint8_t) ch);
    return ch;
}
