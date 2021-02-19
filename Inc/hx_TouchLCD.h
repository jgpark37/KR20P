/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2000, IM, Inc., Ingae, Suwon
*
* All rights reserved. Hexar Systems¡¯s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of IM, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_TouchLCD.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Touch LCD UI (included graphic driver), Maker : Ganasys
*******************************************************************************
*
*/

#ifndef _TOUCH_LCD_H_
#define _TOUCH_LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/
#define TLCD_BUF_SIZE			300

enum _TOUCH_EVENT{
	TOUCH_NONE	= 0x00,
	TOUCH_DOWN	= 0x01,
	TOUCH_UP		= 0x02,
	TOUCH_MOVE	= 0x04,
	TOUCE_MAX		= 0xFF
};

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	uint8_t buf[TLCD_BUF_SIZE];
	uint16_t head;
	uint16_t tail;
}sTLCD_Que;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern sTLCD_Que sTLcd_q;


/* Private function prototypes -----------------------------------------------*/
extern void TLCD_Driver_Init(void);
extern void TLCD_Driver_Process(void);

#endif //_TOUCH_LCD_H_
