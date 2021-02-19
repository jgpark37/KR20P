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
* source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_i2c.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2020/02/19
* Description	: I2C Driver
*******************************************************************************
*
*/

#ifndef _I2C_H_
#define _I2C_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_Message.h"

/* Private define ------------------------------------------------------------*/
#define TLCD_SoundOut	uart_putstring

/* Private typedef -----------------------------------------------------------*/
#define WINDOW_CHILD_NUM		6


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern HWND hDeskTop, hParent, hChild[WINDOW_CHILD_NUM];


/* Private function prototypes -----------------------------------------------*/
extern void OS_Init(void);

#endif //_I2C_H_

