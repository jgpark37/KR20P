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
* Filename		: hx_API.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Message box
*******************************************************************************
*
*/

#ifndef _API_H_
#define _API_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_Message.h"

/* Private define ------------------------------------------------------------*/
#define TLCD_SoundOut	uart_putstring
#define WINDOW_CHILD_NUM		6
#define OS_TIMER_NUM			2

/* Private typedef -----------------------------------------------------------*/
typedef struct _tagOS_TIMER {
	uint32_t cnt;
	uint32_t stop;
}OS_TIMER;

enum _OS_TIMER_NUM {
	OS_TIMER_ID_0,		//key check
	OS_TIMER_ID_1,		//led check
	OS_TIMER_ID_2,		//usb check
	OS_TIMER_ID_MAX,
};


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern HWND hDeskTop, hParent, hOldParent, hChild[WINDOW_CHILD_NUM];
extern OS_TIMER OSTimer[OS_TIMER_NUM];

/* Private function prototypes -----------------------------------------------*/
extern void OS_Init(void);
extern void OS_Process(void);
extern void OS_Timer_Process(void);
extern HWND API_CreateWindow(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
			int x, int y, int nWidth, int nHeight,	HWND hWndParent, HMENU hMenu, //HINSTANCE hInstance, 
			LPVOID lpParam);
extern void API_SetTouchDownFnPtr(void (*fn)(int rID, POINT pt));
extern void API_SetTouchUpFnPtr(void (*fn)(int rID, POINT pt, DWORD dwStyle));
extern void API_SetTouchMoveFnPtr(void (*fn)(int rID, POINT pt));
extern void API_SetSndResFnPtr(void (*fn)(uint16_t sndID));
extern void API_SetAppProcessFnPtr(void (*fn)(void));
extern void API_SetWndNum(uint8_t num);
extern void API_ChangeHMenu(HWND hwnd, HMENU oldhMenu, HMENU newhMenu);
extern void API_SetTimer(int id, uint32_t num);
extern void API_KillTimer(int id);
extern void API_Timer_Init(void);
extern void API_ChangeDnEventedHMenu(HWND hwnd, HMENU newhMenu);
extern void API_ErrorCode_Init(void);
extern uint16_t API_GetErrorCode(void);
extern void API_SetErrorCode(uint16_t code, uint8_t type);
extern uint8_t API_CheckErrorCode(void);
extern void DeviceDrv_Init(void);
#endif //_API_H_
