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
* Filename		: hx_flash.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: embeded flah program in mcu
*******************************************************************************
*
*/

#ifndef _MCU_FLASH_H_
#define _MCU_FLASH_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
// MCU : STM32F405RGT6 ( Flash:1024Kbytes, SRAM: 192Kbytes, Page(16KB~128KB)), LQFP, -40~105 deg
// MCU : STM32F403RCT7 ( Flash:512Kbytes, SRAM: 192Kbytes, Page(16KB)), LQFP, -40~105 deg
#define MCU_STM32F4XX		1
// MCU : STM32F303RCT7 ( Flash:256Kbytes, SRAM: 40Kbytes ), LQFP, -40~105 deg
//#define MCU_STM32F3XX		1
// MCU : STM32F103        ( Flash:64Kbytes, SRAM: 20Kbytes, Page(1KB)), LQFP, -40~105 deg
//#define MCU_STM32F2XX		1
// memory map - Flash memory : 0x08000000-0x08040000
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define FLASH_USER_START_ADDR		ADDR_FLASH_SECTOR_10   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     	ADDR_FLASH_SECTOR_11   /* End @ of user Flash area */
#define FLASH_ENDURANCE			(10000-100)

#define FLASH_PAGE_SIZE1         		((uint32_t)(1024*1))   /* FLASH Page Size (1024 byte) */
#define FLASH_PAGE_SIZE16         		((uint32_t)(1024*16))   /* FLASH Page Size (16384 byte) */
#define FLASH_PAGE_SIZE64         		((uint32_t)(1024*64))   /* FLASH Page Size (16384 byte) */
#define FLASH_PAGE_SIZE128        	((uint32_t)(1024*128))   /* FLASH Page Size (16384 byte) */
#define FLASH_PI_DATA_SIZE				64 //patient info data size
#define FLASH_PI_TOTAL_NUM				512

#define FLASH_SYSTEM_INFO_ADDR		(uint32_t)FLASH_USER_START_ADDR
#define FLASH_SYSTEM_INFO2_ADDR		(uint32_t)(FLASH_SYSTEM_INFO_ADDR+FLASH_PAGE_SIZE16)	//for bad block after 100,000 write
#define FLASH_PATIENT_DATA_ADDR		(uint32_t)(FLASH_SYSTEM_INFO2_ADDR+FLASH_PAGE_SIZE16)
#define FLASH_PATIENT_DATA2_ADDR		(uint32_t)(FLASH_PATIENT_DATA_ADDR+ \
										(FLASH_PI_DATA_SIZE*FLASH_PI_TOTAL_NUM)*FLASH_PAGE_SIZE16)	//for bad block

/* Private typedef -----------------------------------------------------------*/
typedef struct _FLASH_REGION {
	int offset;				//Offset of this region from start of the flash
	int region_size;			//Size of this erase region
	int number_of_blocks;	//Number of blocks in this region
	int block_size;			//Size of each block in this erase region
}FLASH_REGION;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
extern int FlashDrv_Read(uint32_t addr, char *buf, uint32_t size);
extern int FlashDrv_Write(uint32_t addr, char *buf, uint32_t size);
extern int FlashDrv_Write2(uint32_t addr, char *buf, uint32_t size); //no backup
extern uint32_t GetSector(uint32_t Address);
extern void FlashDrv_SetTempBuf(char *buf);
extern void FlashDrv_SetParam(uint32_t page_size);
//extern void flash_test(void);

/* Private functions ---------------------------------------------------------*/

#endif //_MCU_FLASH_H_

