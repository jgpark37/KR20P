/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems¡¯s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_USBDisk.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: USB Disk control
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "hx_USBDisk.h"
#include "fatfs.h"
#include "usb_host.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern ApplicationTyDIR Dir; //Directory object
//extern ApplicationTypeDef Appli_state;
//FIL fp; //file handle
DIR USBHFatDir;				    /* Directory object */
FILINFO USBHFno;			  /* File information object */
//FATFS fatfs; //structure with file system information
//char text[]="test";//text which will be written into file
//char name[]="test.txt";//name of the file
//char text2[100];//buffer for data read from file

struct {
	uint8_t linked;
	uint8_t mounted;
	uint8_t ready;  //pjg++180403: usb disk plug in check
}USBDiskInfo;

//char USBH_Path[4];  /* USBH logical drive path */

extern USBH_HandleTypeDef hUsbHostFS;
extern ApplicationTypeDef Appli_state;

/* Private function prototypes -----------------------------------------------*/
int USBDisk_Link(void)
{
	if (!USBDiskInfo.linked) {
		retUSBH = FATFS_LinkDriver(&USBH_Driver, USBHPath);
		if (retUSBH) return 0; //pjg<>190902
		USBDiskInfo.linked = 1;
	}
	return 1;
}

void USBDisk_UnLink(void)
{
	if (USBDiskInfo.linked == 1) {
		retUSBH = FATFS_UnLinkDriver(USBHPath);
		//if (retUSBH) return 0; //pjg<>190902
		USBDiskInfo.linked = 0;
	}
}

void USBDisk_UnMount(void)
{
	if (USBDiskInfo.mounted) {
		if(f_mount(0, (TCHAR const*)USBHPath, 0) != FR_OK){
			//USBDiskInfo.mounted = 0;
			/* FatFs Initialization Error */
			return;
		}
	}
	USBDiskInfo.mounted = 0;
}

int USBDisk_Mount(void)
{
	FRESULT rv;
	
	//Please note that FLASH disk must be formatted in FAT32 file system
	rv = f_mount(&USBHFatFS, (TCHAR const*)USBHPath, 0);
	if(rv != FR_OK){
		USBDiskInfo.mounted = 0;
		/* FatFs Initialization Error */
		return 0;
	}
	USBDiskInfo.mounted = 1;

	return 1;
}

FRESULT USBDisk_set_timestamp (
    char *obj,     /* Pointer to the file name */
    int year,
    int month,
    int mday,
    int hour,
    int min,
    int sec
)
{
    FILINFO fno;

    fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);

    return f_utime(obj, &fno);
}

uint32_t USBDisk_OpenWrite(char *name)
{
	FRESULT rv;
	
	/*open or create file for writing*/
	rv = f_open(&USBHFile, name, FA_CREATE_ALWAYS | FA_WRITE);
	if (rv != FR_OK  ) {
		//while(1);
		return 0;
	}

	//if (USBDisk_set_timestamp(name, 2017, 6, 15, 10, 58, 0) != FR_OK) return 0;
	
	return 1;
}

uint32_t USBDisk_OpenAlwaysWrite(char *name)
{
	FRESULT rv;
	
	/*open or create file for writing*/
	rv = f_open(&USBHFile, name, FA_OPEN_ALWAYS | FA_WRITE);
	if (rv != FR_OK  ) {
		//while(1);
		return 0;
	}

	//if (USBDisk_set_timestamp(name, 2017, 6, 15, 10, 58, 0) != FR_OK) return 0;
	
	return 1;
}

uint32_t USBDisk_OpenRead(char *name)
{
	FRESULT rv;
	
	/*open file for reading*/
	rv = f_open(&USBHFile, name, FA_OPEN_EXISTING | FA_READ);
	if (rv != FR_OK) {
		//while(1);
		return 0;
	}

	return 1;
}

void USBDisk_Close()
{
	f_close(&USBHFile);
}

uint32_t USBDisk_Write(uint8_t *buf, uint32_t len)
{
	uint32_t ret;//return variable 
	/*write data into flashdisk*/
	FRESULT rv;
    
	rv = f_write(&USBHFile, buf, len, &ret);
	if(rv != FR_OK) {
		//while(1);
		return 0;
	}
	
	return ret;
}

int USBDisk_IsReady(uint16_t wait)
{
	uint32_t i;
	uint32_t flag;

	USBDiskInfo.ready = 0; //pjg++180403
	if (!USBDisk_Link()) return 0;
	if (!USBDisk_Mount()) {
		USBDisk_UnLink();
		return 0;
	}
	i = wait;
	//i = HAL_GetTick();
	flag = 0;
	while (i != 0) {
	//while ((HAL_GetTick()-i) <=  wait) {
		MX_USB_HOST_Process();
		if (Appli_state == APPLICATION_READY) {
			//if (!USBDisk_Mount()) {
			//	USBDisk_UnLink();
			//	return 0;
			//}
			flag = 1;
			break;
		}
		HAL_Delay(1);
		i--;
		//if (!i) {
		//	USBDisk_UnMount();
		//	USBDisk_UnLink();
		//	return 0;
		//}
	}
	if (flag) USBDiskInfo.ready = 1;
	USBDisk_UnMount();
	USBDisk_UnLink();
	return 1;
}

int USBDisk_CheckDetect(void)
{
	MX_USB_HOST_Process();
	if (Appli_state == APPLICATION_READY) {
		USBDiskInfo.ready = 1;
		return 1;
	}
	return 0;
}

uint32_t USBDisk_Read(uint8_t *buf, uint32_t len)
{
	uint32_t ret;//return variable 

	/*read data from flash*/
	if (f_read(&USBHFile, buf, len, &ret) != FR_OK) {
		//while(1);
		return 0;
	}
	return ret;
}

int USBDisk_ReadDirectory(void)
{
 	uint32_t ret;//return variable 

 /*  directory display test*/ 
	//Delay(50);	 
	//printf("Open root directory\n\r"); 
	ret = f_opendir(&USBHFatDir, "");
	
	if (ret) { 
		//printf("Open root directory error\n\r");	 
	} 
	else { 
		//printf("Directory listing...\n\r"); 
		for (;;) { 
			ret = f_readdir(&USBHFatDir, &USBHFno);		/* Read a directory item */ 
			if (ret || !USBHFno.fname[0]) { 
				break;	/* Error or end of dir */ 
			} 
			
			if (USBHFno.fattrib & AM_DIR) { 
				//printf("  <dir>  %s\n\r", fno.fname); 
			} 
			else { 
				//printf("%8lu  %s\n\r", fno.fsize, fno.fname); 
			} 
		} 
	
		if (ret) { 
			//printf("Read a directory error\n\r"); 
			//fault_err(ret); 
			return 0;
		} 
	} 
	//Delay(50);	 
	//printf("Test completed\n\r"); 


	/* Infinite loop */ 
	//while (1) { 
	//	STM_EVAL_LEDToggle(LED3); 
	//	Delay(100);	 
	//}

	return 1;
}

uint8_t USBDisk_GetReadyFlag(void)
{
	return USBDiskInfo.ready;
}

void USBDisk_SetReadyFlag(uint8_t flag)
{
	USBDiskInfo.ready = flag;
}

int USBDisk_lseek(uint32_t ofs)
{
	FRESULT res;

	res = f_lseek(&USBHFile, ofs);
	if (res != FR_OK) return 0;
	return 1;
}

void USBDisk_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	USBDiskInfo.linked = 0;
	USBDiskInfo.mounted = 0;
	//if (!retUSBH) USBDiskInfo.linked = 1;

	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); //power on
}

void USBDisk_Process(void)
{
	MX_USB_HOST_Process(); //pjg++180614

}
