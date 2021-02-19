/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems’s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_MetaInfo.c
* Programmer(s)	: PJG
*                 Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2019/09/19
* Description		: Meta Info in flash
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

////////////////////////////Function////////////////////////////////////////
#define FW_VERSION_MAJOR		0x0101
#define FW_VERSION_MINOR		0x0100
#define FW_DATE_YEAR			0x2019
#define FW_DATE_DATE			0x1024
#define PRODUCT_ID_HIGH			0x9999
#define PRODUCT_ID_LOW			0x9999
#define APP_DELAY				100
#define APP_VARIABLE			100

#pragma location = "METAINFO"
__root const uint16_t metainfo[10] = //sync with xxflash.icf
{
	0xCAFE,
	FW_VERSION_MAJOR,
	FW_VERSION_MINOR,
	FW_DATE_YEAR,
	FW_DATE_DATE,
	PRODUCT_ID_HIGH,
	PRODUCT_ID_LOW,
	APP_DELAY,
	APP_VARIABLE,
	//0x1111,
	//0x2222,
	//0x3333,
	//0x4444,
	//0x5555,
	//0x6666,
	//0x7777,
	//0x8888,
	//0x9999,
	//0xAAAA,
	0xDEAD
};


