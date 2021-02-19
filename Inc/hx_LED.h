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
* Filename		: hx_LED.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: LED
*******************************************************************************
*
*/

#ifndef _LED_H_
#define _LED_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define LED_CHECK_TIME			20			// timer event time(msec)
#define LED_ORDER_BUF_SIZE		3			// led turn on order buf size
#define LED_ON					1
#define LED_OFF					0

/* Private typedef -----------------------------------------------------------*/
enum _LED_SELECT {
	LS_RED,
	LS_GREEN,
	LS_BLUE,
	LS_MAX
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern void LEDDrv_Process(void);
extern void LEDDrv_Init(void);
//
//repeat : 0xffff is no limit, other is limit
extern void LEDDrv_SetParam(uint8_t led, uint8_t on, uint32_t ontime, uint32_t offtime, uint32_t repeat);
//
// order : led turn on order buffer number
extern void LEDDrv_SetOrder(uint8_t order, uint8_t led, uint16_t ontime);

#endif //_LED_H_

