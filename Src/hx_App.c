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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_App.h"
#include "hx_API.h"
#include "hx_RS232.h"
#include "hx_Message.h"
#include "hx_Resource.h"
#include "hx_UI.h"

/* Private define ------------------------------------------------------------*/
#define TOUCH_LONGKEY_CHK_TIME		10
#define BUTTON_LONGKEY_CHK_TIME		1
/* Private typedef -----------------------------------------------------------*/
typedef void (*FnUI_Process)(void);
typedef LRESULT (*FnApp_WndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//
// API

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sTIMER AppTimer[APP_TIMER_NUM];
FnUI_Process fnUi_Process;
FnApp_WndProc fnApp_WndProc;

struct _tagKEY_CFG{
	//uint8_t playDisable;
	//uint8_t pauseDisable;
	uint8_t leftDisable;
	uint8_t rightDisable;
}KeyCfg;

struct _tagTOUCH_KEY_EVENT{
	uint8_t longChkCnt;
	uint8_t accel;	//accelerate
	uint8_t longPress;
}TouchKeyEvent;				   
struct _tagBUTTON_KEY_EVENT{
	uint8_t longChkCnt;
	uint8_t accel;	//accelerate
	uint8_t longPress;
}ButtonKeyEvent;				   
//uint8_t playDisable;
//uint8_t pauseDisable;
//uint8_t leftDisable;
//uint8_t rightDisable;

/* Private function prototypes -----------------------------------------------*/
void API_WndDisable(HWND hwnd, HMENU hMenu, uint8_t value);	//pjg++170825

//
/////////////////////////////////// APP ///////////////////////////////////
//
void App_DrawTouchUp(int rID, POINT pt, DWORD dwStyle)
{
	char *p;
	//uint8_t style;
	uint8_t long_key_event;
	
	p = pBtnInfo[rID*2];

	if (hOldParent != hParent) return;

	//style = (uint8_t)(dwStyle&0xff);
	long_key_event = (uint8_t)(dwStyle>>31);
	if (dwStyle < BS_CHECKBOX || dwStyle > BS_RADIOBUTTON);
       	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)p);
	if (long_key_event != 0) {
		APP_SendMessage(hParent, WM_COMMAND, rID, BN_UNHILITE); //pjg++190816
		return;
	}
	if (TouchKeyEvent.longPress == 0) {
		APP_SendMessage(hParent, WM_COMMAND, rID, BN_CLICKED);
	}
	else {
		if (TouchKeyEvent.longPress > 0) {
			APP_SendMessage(hParent, WM_COMMAND, rID, BN_LONGPUSHEDUP);
		}
	}
	TouchKeyEvent.longPress = 0;
	TouchKeyEvent.longChkCnt = 0;
	return;
}

void App_DrawTouchDown(int rID, POINT pt)
{	
	char *p;

	p = pBtnInfo[rID*2+1];
	TouchKeyEvent.longChkCnt = 0;
	TouchKeyEvent.accel = 0;
	TouchKeyEvent.longPress = 0;

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)p);
	APP_SendMessage(hParent, WM_COMMAND, rID, BN_PUSHED);

	APP_SendMessage(hParent, WM_SOUND, 0,  SNDID_BN_UP_DOWN);
	hOldParent = hParent;
}

void App_DrawTouchMove(int rID, POINT pt)
{
	if (rID < RID_MAX) {
		TouchKeyEvent.longChkCnt++;
		if (TouchKeyEvent.longChkCnt > TOUCH_LONGKEY_CHK_TIME) {
			if (TouchKeyEvent.accel < 3) TouchKeyEvent.accel++;
			TouchKeyEvent.longChkCnt = 4 + TouchKeyEvent.accel;
			//temp = (KeyEvent.longCnt*KeyEvent.longCnt)/32;
			APP_SendMessage(hParent, WM_COMMAND, rID, BN_LONGPUSHED);
			TouchKeyEvent.longPress++;
		}
	}
	else {
		//KeyEvent.longCnt = 0;
	}
	APP_SendMessage(hParent, WM_TOUCH_MOVE, rID, BN_MOVEPUSHED);

}

uint32_t App_GetLongTouchCnt(void)
{
	return TouchKeyEvent.longPress;
}

void App_KeyDown(uint32_t nChar)
{
	ButtonKeyEvent.longChkCnt = 0;
	ButtonKeyEvent.accel = 0;
	ButtonKeyEvent.longPress = 0;
	APP_SendMessage(hParent, WM_KEYDOWN, nChar, 0);
}

void App_KeyUp(uint32_t nChar)
{
	if (ButtonKeyEvent.longPress == 0) {
		APP_SendMessage(hParent, WM_KEYUP, nChar, 0);
	}
	else {
		if (ButtonKeyEvent.longPress > 0) {
			APP_SendMessage(hParent, WM_KEYLONGUP, nChar, 0);
		}
	}
	ButtonKeyEvent.longPress = 0;
	ButtonKeyEvent.longChkCnt = 0;
	APP_SendMessage(hParent, WM_SOUND, 0,  SNDID_BN_UP_DOWN);
}

void App_KeyLong(uint32_t nChar)
{
	ButtonKeyEvent.longChkCnt++;
	if (ButtonKeyEvent.longChkCnt > BUTTON_LONGKEY_CHK_TIME) {
		if (ButtonKeyEvent.accel < 3) ButtonKeyEvent.accel++;
		ButtonKeyEvent.longChkCnt = 4 + ButtonKeyEvent.accel;
		//temp = (KeyEvent.longCnt*KeyEvent.longCnt)/32;
		APP_SendMessage(hParent, WM_KEYLONG, nChar, 0);
		ButtonKeyEvent.longPress++;
	}
	//APP_SendMessage(hParent, WM_KEYLONG, nChar, 0);
}

//void App_KeyLongUp(uint32_t nChar)
//{
//	APP_SendMessage(hParent, WM_KEYLONGUP, nChar, 0);
//}

uint32_t App_GetLongButtonCnt(void)
{
	return ButtonKeyEvent.longPress;
}

//void App_SetSoundRes(uint16_t sndID)
//{
//	TLCD_SoundOut(pSndInfo[sndID]);
//}

void App_SetWndProcFnPtr(LRESULT (*fn)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam))
{
	fnApp_WndProc = fn;
}

LRESULT App_WndProcNull(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

#if 0
LRESULT App_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, POINT pt)
{
	//if (hParent != hWnd) return 0;
	
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_COMMAND:
		switch(wParam) { // wParamÀÇ word low¿¡ id³¯¶ó¿È
		//case menu1_id: /*do*/ break;
		//case menu2_id: /*do*/ break;
		//case accel_id: /*do*/ break;
		case RID_LD_BTN_STOP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Loading_OnBnClickedBtnStop();
				break;
			}
			break;
		//
		//setup
		case RID_SETUP_SND_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnSndUp();
				break;
			}
			break;
		case RID_SETUP_SND_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnSndDn();
				break;
			}
			break;
		case RID_SETUP_BL_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnBLUp();
				break;
			}
			break;
		case RID_SETUP_BL_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnBLDn();
				break;
			}
			break;
		case RID_SETUP_SAVE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnSave();
				break;
			}
			break;
		case RID_SETUP_CANCLE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnCancel();
				break;
			}
			break;	
		//
		// PI
		case RID_PI_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				//MessageBox(hWnd,TEXT("Hello"),TEXT("Button"),MB_OK);
				UI_PatientInfo_OnBnClickedPiBtnLeft();
				break;
			}
			break;
		case RID_PI_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PatientInfo_OnBnClickedPiBtnRight();
				break;
			}
			break;
		case RID_PI_BTN_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PatientInfo_OnBnClickedPiBtnUp();
				break;
			}
			break;
		case RID_PI_BTN_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PatientInfo_OnBnClickedPiBtnDown();
				break;
			}
			break;
		case RID_PI_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PatientInfo_OnBnClickedPiBtnSetup();
				break;
			}
			break;
		case RID_PI_BTN_GO_HOME:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PatientInfo_OnBnClickedPiBtnGoHome();
				break;
			}
			break;
		//
		// ASE
		case RID_ASE_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleEX_OnBnClickedBtnLeft();
				break;
			}
			break;
		case RID_ASE_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleEX_OnBnClickedBtnRight();
				break;
			}
			break;
		case RID_ASE_BTN_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleEX_OnBnClickedBtnUp();
				break;
			}
			break;
		case RID_ASE_BTN_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleEX_OnBnClickedBtnDown();
				break;
			}
			break;		
		case RID_ASE_BTN_GO_FIRST:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleEX_OnBnClickedBtnGoFirst();
				break;
			}
			break;	
		//
		// Speed time setting
		case RID_STS_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnLeft();
				break;
			}
			break;
		case RID_STS_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnRight();
				break;
			}
			break;
		case RID_STS_BTN_SPD_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnSpdUp();
				break;
			}
			break;
		case RID_STS_BTN_SPD_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnSpdDown();
				break;
			}
			break;		
		case RID_STS_BTN_TM_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnTmUp();
				break;
			}
			break;
		case RID_STS_BTN_TM_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnTmDown();
				break;
			}
			break;		
		case RID_STS_BTN_GO_FIRST:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnGoFirst();
				break;
			}
			break;
		//
		//run
		case RID_RN_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnLeft();
				break;
			}
			break;
		case RID_RN_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnRight();
				break;
			}
			break;
		case RID_RN_BTN_PLAY:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnPlayStart();
				break;
			}
			break;
		case RID_RN_BTN_PAUSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnPlay();
				break;
			}
			break;
		case RID_RN_BTN_STOP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnStop();
				break;
			}
			break;
		case RID_RN_BTN_GO_FIRST:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnGoFirst();
				break;
			}
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		uart_putstring((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	uart_putstring(pSndInfo[lParam]);
		else if (wParam == 1) uart_putstring((char *)lParam);
		break;
	case WM_TIMER:
		UI_Timer(wParam);
		break;
	case WM_BACKLIGHT:
		uart_putstring((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}
#endif
void App_Process(void)
{
	if (AppMsgInfo.tail != AppMsgInfo.head) {
		if (AppMsg[AppMsgInfo.tail].hwnd == hParent || AppMsg[AppMsgInfo.tail].hwnd == 0) {																			 
			fnApp_WndProc(AppMsg[AppMsgInfo.tail].hwnd, AppMsg[AppMsgInfo.tail].message, 
					AppMsg[AppMsgInfo.tail].wParam, AppMsg[AppMsgInfo.tail].lParam);
                }
                AppMsgInfo.tail++;
		if (AppMsgInfo.tail >= APP_MSG_BUF_SIZE) AppMsgInfo.tail = 0;
                
	}
	fnUi_Process();
	
}

void App_SetTimer(int id, uint32_t num)
{
	if (id >= APP_TIMER_NUM) return;
	
	AppTimer[id].stop = num;
	AppTimer[id].cnt = 1;
}

void App_KillTimer(int id)
{
	if (id >= APP_TIMER_NUM) return;

	AppTimer[id].cnt = 0;
}

void App_Timer_Init(void)
{
	int i;
	char *p;

	p = (char *)&AppTimer;
	for (i = 0; i < sizeof(sTIMER); i++) {
		p[i] = 0;
	}
}

void App_Timer_Process(void)
{
	int i;
	
	for (i = 0; i < APP_TIMER_NUM; i++) {
		if (AppTimer[i].cnt > 0) {
			AppTimer[i].cnt++;
			if (AppTimer[i].cnt > AppTimer[i].stop) {
				AppTimer[i].cnt = 1;
				APP_SendMessage(hParent, WM_TIMER, i, 0);
			}
		}
	}
}

void App_SetUIProcess(void (*fn)(void))
{
	fnUi_Process = fn;
}

void App_UIProcess_NULL(void)
{
}

void App_SetButtonOption(HMENU rID, uint8_t value)
{
#if 1
	API_WndDisable(hParent, rID, value);
#else
	switch (rID) {
	case RID_PI_BTN_LEFT:
		leftDisable = value;
		break;
	case RID_RN_BTN_PLAY:
		//playDisable= value;
		break;
	case RID_RN_BTN_PAUSE:
		//pauseDisable = value;
		break;
	case RID_RN_BTN_RIGHT:
		rightDisable = value;
		break;
	}
	
#endif
}

void App_Init(void)
{
	uart_putstring("sv 215\r");
	//TLCD_SoundOut("sp wav/system_init.wav\r");
	//uart_putstring("debug\r");

	//hDeskTop = API_CreateWindow("desktop", "i wal06.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
	hDeskTop = API_CreateWindow("desktop", "\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, 
			0, 0, 0);

	API_SetTouchDownFnPtr(App_DrawTouchDown);
	API_SetTouchUpFnPtr(App_DrawTouchUp);
	API_SetTouchMoveFnPtr(App_DrawTouchMove);
	//API_SetSndResFnPtr(App_SetSoundRes);
	//fnApp_WndProc = UI_Window_PatientInfo_Process;
	API_SetAppProcessFnPtr(App_Process);
	App_SetUIProcess(App_UIProcess_NULL);
	App_SetWndProcFnPtr(App_WndProcNull);
	App_Timer_Init();

	UI_SystemInit_Create();

	//playDisable= 0;
	//pauseDisable = 0;
	//leftDisable = 0;
	//rightDisable = 0;
	TouchKeyEvent.longChkCnt = 0;
	TouchKeyEvent.accel = 0;	
	TouchKeyEvent.longPress = 0;
	ButtonKeyEvent.longChkCnt = 0;
	ButtonKeyEvent.accel = 0;	
	ButtonKeyEvent.longPress = 0;
	//UI_PatientInfo_Init();
	//uart_putstring("debug\r");
//	uart_CopyToBuf("sp system_init.wav\r");
}

