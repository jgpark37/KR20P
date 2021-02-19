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
* Filename		: hx_USBDisk.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: USB Disk control
*******************************************************************************
*
*/

#ifndef _USBDISK_H_
#define _USBDISK_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern void USBDisk_Init(void);
extern uint32_t USBDisk_Read(uint8_t *buf, uint32_t len);
extern uint32_t USBDisk_Write(uint8_t *buf, uint32_t len);
extern uint32_t USBDisk_OpenRead(char *name);
extern uint32_t USBDisk_OpenWrite(char *name);
extern uint32_t USBDisk_OpenAlwaysWrite(char *name);
extern void USBDisk_Close(void);
extern int USBDisk_IsReady(uint16_t wait);
extern void USBDisk_SetReadyFlag(uint8_t flag);
extern uint8_t USBDisk_GetReadyFlag(void);
extern int USBDisk_CheckDetect(void);
extern void USBDisk_Process(void);
extern int USBDisk_Mount(void);
extern void USBDisk_UnMount(void);
extern int USBDisk_Link(void);
extern void USBDisk_UnLink(void);
extern int USBDisk_lseek(uint32_t ofs);
#endif //_USBDISK_H_

