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
* Filename		: hx_Key.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: GPIO Key
*******************************************************************************
*
*/

#ifndef _KEY_H_
#define _KEY_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define KEY_CHECK_TIME						2//0
#define KEY_SHORT_KEY_CNT					3
#define KEY_LONG_KEY_CNT					(KEY_SHORT_KEY_CNT+20)

/* Private typedef -----------------------------------------------------------*/
enum _KEY_VALUE {
	KEY_NONE,
	KEY_PLAY,
	KEY_PAUSE,
	KEY_STOP,
	KEY_MAX,
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t Key_ChkTimer;


/* Private function prototypes -----------------------------------------------*/
extern void KeyDrv_Init(void);
extern void KeyDrv_Process(void);
extern void KeyDrv_Disable(uint8_t onoff);
extern void KeyDrv_SetLongKeyTime(uint16_t value);
extern void KeyDrv_SetShortKeyTime(uint8_t value);
extern uint32_t KeyDrv_GetLongKeyCnt(void);

#endif //_KEY_H_

