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
* Filename		: hx_App.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: 
*******************************************************************************
*
*/

#ifndef _APP_H_
#define _APP_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_Message.h"

/* Private define ------------------------------------------------------------*/
#define APP_TIMER_NUM	8				//pjg<>190814


/* Private typedef -----------------------------------------------------------*/
enum _eVIRTUAL_KEY {
	VK_NONE,
	VK_PLAY,
	VK_PAUSE,
	VK_STOP,
	VK_MAX,
};

typedef struct _tagTimer {
	uint32_t cnt;
	uint32_t stop;
}sTIMER;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern sTIMER AppTimer[APP_TIMER_NUM];
 
/* Private function prototypes -----------------------------------------------*/
extern void App_Process(void);
extern void App_Init(void);
extern void App_SetTimer(int id, uint32_t num);
extern void App_KillTimer(int id);
extern void App_Timer_Process(void);
extern void App_SetUIProcess(void (*fn)(void));
extern void App_SetButtonOption(HMENU rID, uint8_t value);
extern void App_SetWndProcFnPtr(LRESULT (*fn)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam));
extern void App_KeyDown(uint32_t nChar);
extern void App_KeyUp(uint32_t nChar);
extern void App_KeyLong(uint32_t nChar);
extern void App_KeyLongUp(uint32_t nChar);
extern uint32_t App_GetLongTouchCnt(void);
extern uint32_t App_GetLongButtonCnt(void);

#endif //_APP_H_

