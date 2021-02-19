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
* Filename		: hx_util.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: UI
*******************************************************************************
*
*/

#ifndef _UTIL_H_
#define _UTIL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_util.h"

/* Private define ------------------------------------------------------------*/
//#define MOVE_AVG_BUF_SIZE					8

/* Private typedef -----------------------------------------------------------*/
typedef	struct _tagMOVING_AVG{
	//uint8_t cnt;
	uint32_t *pBuf;//[MOVE_AVG_BUF_SIZE];
	uint16_t bufSize;
	//uint16_t delValue;
	uint32_t sum;
	uint16_t savePos;
	uint16_t delPos;
}MOVING_AVG; //moving avg


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern uint8_t ConvertDec2Hex(uint8_t data);
extern uint16_t ConvertString2U16(char *buf, char _char, char findpos);
extern float ConvertString2Float(uint8_t *buf, uint8_t cnt);
extern uint32_t Multiplier(int x, int n);
extern void ConvertLong2String(int32_t data, char *buf, uint8_t maxbuf);
extern void ConvertFloat2String(float data, char *buf, uint8_t maxbuf);
extern uint32_t ConvertString2U32(uint8_t *buf, uint8_t len);
extern void ConvertULong2String(int32_t data, uint8_t *buf, uint8_t maxbuf);
extern uint16_t MovingAvg_Process(MOVING_AVG * pMvAvg, uint32_t data);
extern void MovingAvg_Init(MOVING_AVG *pMvAvg, uint32_t *buf, uint16_t bufSize, uint16_t initData);

/* Private functions ---------------------------------------------------------*/

#endif //_UTIL_H_

