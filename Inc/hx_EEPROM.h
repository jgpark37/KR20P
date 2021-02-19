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
* Filename		: hx_EEPROM.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: EEPROM control by SPI
*******************************************************************************
*
*/

#ifndef _EEPROM_H_
#define _EEPROM_H_

/* Includes ------------------------------------------------------------------*/
#include "ff.h"

/* Private define ------------------------------------------------------------*/
//#define SUPPORT_EEPROM				
//#define EEPROM_PAGE_SIZE			256	//byte
/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern FIL EEPFp; //file handle
extern DIR EEPFatDir;				    /* Directory object */
extern FILINFO EEPFno;			  /* File information object */
//extern FATFS EEPFatFS; //structure with file system information
//extern uint8_t retEEP;    /* Return value for USBH */
//extern char EEPPath[4];   /* USBH logical drive path */


/* Private function prototypes -----------------------------------------------*/
extern void EEPROMDrv_Init(SPI_HandleTypeDef *p_hSPI);
extern int EEPROMDrv_Read(uint8_t fd, uint32_t addr, uint8_t *buf, uint32_t len);
extern int EEPROMDrv_WriteByte(uint8_t fd, uint32_t addr, uint8_t data);
extern int EEPROMDrv_WritePage(uint8_t fd, uint32_t addr,  uint8_t *data);//, uint32_t len);
//EEPROM Disk
extern int EEPROMDisk_Link(void);
extern void EEPROMDisk_UnLink(void);
extern void EEPROMDisk_UnMount(void);
extern int EEPROMDisk_Mount(void);
extern uint32_t EEPROMDisk_OpenWrite(char *name);
extern uint32_t EEPROMDisk_OpenRead(char *name);
extern void EEPROMDisk_Close();
extern uint32_t EEPROMDisk_Write(uint8_t *buf, uint32_t len);
extern int EEPROMDisk_IsReady(uint16_t wait);
extern uint32_t EEPROMDisk_Read(uint8_t *buf, uint32_t len);
extern int EEPROMDisk_ReadDirectory(void);
extern int EEPROMDisk_Format(void);
extern int EEPROMDisk_MkDir(char *dir);
extern int EEPROMDisk_OpenDir(char *path);
extern uint32_t EEPROMDisk_GetSize(void);
extern int EEPROMDisk_lseek(DWORD ofs);
extern int EEPROMDisk_stat(char *path, FILINFO *fno);
extern uint32_t EEPROMDisk_GetFree(char *path);
extern uint32_t EEPROMDisk_GetFreeFromMem(void);
extern uint32_t EEPROMDisk_GetTotalFromMem(void);
extern int EEPROMDisk_Delete(char *path);
extern int EEPROMDisk_FindFirst(char *path, char *pattern, char* filename);
extern int EEPROMDisk_FindNext(char *pattern, char *filename);
extern int EEPROMDisk_CloseDir(void);
extern void EEPROMDisk_Init(void);

#endif //_EEPROM_H_

