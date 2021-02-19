/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                     104, Factory Experiment Bldg, No41.55
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
* Filename		: hx_API.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Touch LCD UI (included graphic driver)
*******************************************************************************
*
*/
 

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_API.h"
#include "hx_RS232.h"
#include "hx_TouchLCD.h"
#include "hx_Message.h"
#include "hx_APP.h"
#include "hx_Key.h"
#include "hx_LED.h"
#include "hx_Error.h"

/* Private define ------------------------------------------------------------*/
#define WINDOW_CLASS_NUM		4         
#define WINDOW_NUM			250

/* Private typedef -----------------------------------------------------------*/
typedef struct tagWNDCLASSEXW {
	//UINT cbSize; /* Win 3.x */
	UINT style;
	WNDPROC lpfnWndProc;
	//int cbClsExtra;
	//int cbWndExtra;
	HINSTANCE hInstance;
	//HICON hIcon;
	//HCURSOR hCursor;
	//HBRUSH hbrBackground;
	LPCWSTR lpszMenuName;
	LPCWSTR lpszClassName; 
	/* Win 4.0 */
	//HICON hIconSm;
} WNDCLASSEXW, *PWNDCLASSEXW;

typedef struct _tagWND_INFO {
	LPCTSTR lpWindowName;
	DWORD dwStyle;
	int x;
	int y;
	int nWidth;
	int nHeight;
	HWND hWndParent; 
	HMENU hMenu;
	//LPVOID lpParam;
	uint8_t disable;	//pjg++170825
}WND_INFO;

typedef union _tagHILO_WORD{
	WORD hi;
	WORD lo;
	DWORD all;
}HILO_WORD;

typedef void (*FnApp_Process)(void);
typedef void (*FnOS_DrawTouchUp)(int rID, POINT pt, DWORD dwStyle);
typedef void (*FnOS_DrawTouchDown)(int rID, POINT pt);
typedef void (*FnOS_DrawTouchMove)(int rID, POINT pt);
typedef void (*FnOS_SetSndRes)(uint16_t sndID);
typedef void (*FnOS_WndProc)(void);

typedef struct _tag_TOUCH_FUNCTION_POINTER {
	FnOS_DrawTouchUp DrawTouchUp;
	FnOS_DrawTouchDown DrawTouchDown;
	FnOS_DrawTouchMove DrawTouchMove;
}TOUCH_FP;

/* Private macro -------------------------------------------------------------*/
#define HIWORD(l) ((WORD)(((DWORD(l)>>16) & 0xFFFF))
#define LOWORD(l) ((WORD)((DWORD)(l) & 0xffff)) 

/* Private variables ---------------------------------------------------------*/
WNDCLASSEXW wndClass[WINDOW_CLASS_NUM];
uint8_t wndClassNum;
WND_INFO wndInfo[WINDOW_NUM];
uint8_t wndNum, parentWndNum;
uint8_t wndCnt;	//created windows number
HWND hDeskTop, hParent, hOldParent, hActiveWnd, hChild[WINDOW_CHILD_NUM];
uint16_t rID_Dn;//, resourceID;
//function pointer
//FnOS_WndProc fnOS_WndProc;
TOUCH_FP  fnOS_Touch;
FnOS_SetSndRes fnOS_SetSndRes;
FnApp_Process fnApp_Process;
OS_TIMER OSTimer[OS_TIMER_NUM];

/* Private function prototypes -----------------------------------------------*/
LRESULT OS_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void USB_Process(void);


//////////////////////////////// API ///////////////////////////////////

HWND API_CreateWindow(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,	HWND hWndParent, HMENU hMenu, //HINSTANCE hInstance, 
	LPVOID lpParam) 
{
	HWND hwnd;
	//POINT pt;

	//pt.x = x; pt.y = y;
	wndInfo[wndNum].lpWindowName = lpWindowName;
	wndInfo[wndNum].dwStyle = dwStyle;
	wndInfo[wndNum].x = x;
	wndInfo[wndNum].y = y;
	wndInfo[wndNum].nWidth = nWidth;
	wndInfo[wndNum].nHeight = nHeight;
	wndInfo[wndNum].hWndParent = hWndParent;
	wndInfo[wndNum].hMenu = hMenu;
	wndInfo[wndNum].disable = 0;	//pjg++170825

	hwnd = &wndInfo[wndNum];
	
	switch (dwStyle) {
	case WS_OVERLAPPEDWINDOW:
		//if (wndClassNum > WINDOW_CLASS_NUM-1) return (void*)0;
		wndClass[wndClassNum].lpfnWndProc = OS_WndProc;
		wndClassNum++;	
		if (wndClassNum >= WINDOW_CLASS_NUM) wndClassNum = 0;
		APP_SendMessage(hwnd, WM_CREATE, 0, 0);
		//API_SendMessage(hwnd, WM_CREATE, (WPARAM)dwStyle, (LPARAM)hMenu);
		parentWndNum = wndNum;
		wndCnt++;
		//break;
	case BS_PUSHBUTTON:
		API_SendMessage(hwnd, WM_CREATE, (WPARAM)dwStyle, (LPARAM)hMenu);
	case BS_CHECKBOX:
		break;
	default:
		break;
	}
	wndNum++;
	if (wndNum >= WINDOW_NUM) wndNum = 1;
	wndInfo[wndNum].hWndParent = 0;
	
	return hwnd;
}

void API_ShowWindow(HWND hwnd, DWORD dwStyle)
{
	
}

HWND API_GetParent(HWND hwnd)
{
	return (void*)&wndInfo[wndNum];
}

void API_SetTouchDownFnPtr(void (*fn)(int rID, POINT pt))
{
	fnOS_Touch.DrawTouchDown = fn;
}

void API_SetTouchUpFnPtr(void (*fn)(int rID, POINT pt, DWORD dwStyle))
{
	fnOS_Touch.DrawTouchUp= fn;
}

void API_SetTouchMoveFnPtr(void (*fn)(int rID, POINT pt))
{
	fnOS_Touch.DrawTouchMove = fn;
}

void API_SetSndResFnPtr(void (*fn)(uint16_t sndID))
{
	fnOS_SetSndRes = fn;
}

void API_SetAppProcessFnPtr(void (*fn)(void))
{
	fnApp_Process = fn;
}

//void API_SetAppWndProcFnPtr(void (*fn)(void))
//{
//	fnOS_WndProc = fn;
//}

void API_SetWndNum(uint8_t num)
{
	if (num >= (WINDOW_NUM)) return;
	wndNum = num;

	wndClassNum = 1;
	//MSG_APP_ClearMessage();
}

void API_ChangeHMenu(HWND hwnd, HMENU oldhMenu, HMENU newhMenu)
{
	int i;
	
	for (i = 0; i < WINDOW_NUM; i++) {
		if (wndInfo[i].hWndParent == hwnd) break;
	}

	for (; i < WINDOW_NUM; i++) {
		if (wndInfo[i].hWndParent == hwnd) {
			if (wndInfo[i].hMenu == oldhMenu) {
				wndInfo[i].hMenu = newhMenu;
				return;
			}
		}
	}
}

//pjg++171122 when pressing long key, if change hmenu, dn event occur
void API_ChangeDnEventedHMenu(HWND hwnd, HMENU newhMenu)
{
	rID_Dn = newhMenu;
}

void API_SetTimer(int id, uint32_t num)
{
	if (id >= OS_TIMER_NUM) return;
	
	OSTimer[id].stop = num;
	OSTimer[id].cnt = 1;
}

void API_KillTimer(int id)
{
	if (id >= OS_TIMER_NUM) return;

	OSTimer[id].cnt = 0;
}

void API_Timer_Init(void)
{
	int i;
	char *p;

	p = (char *)&OSTimer;
	for (i = 0; i < sizeof(OS_TIMER_NUM); i++) {
		p[i] = 0;
	}
}

void API_WndDisable(HWND hwnd, HMENU hMenu, uint8_t value)	//pjg++170825
{
	int i;

	for (i = 0; i < WINDOW_NUM; i++) {
		if (wndInfo[i].hWndParent == hwnd) break;
	}

	for (; i < WINDOW_NUM; i++) {
		if (wndInfo[i].hWndParent == hwnd) {
			if (wndInfo[i].hMenu == hMenu) {
				wndInfo[i].disable= value;
				if (value == BN_DISABLE) rID_Dn = 0xffff;							 
				return;
			}
		}
	}
}

void API_ErrorCode_Init(void)
{
	ErrorInfo.head = ErrorInfo.tail = 0;
	ErrorInfo.buf[0] = 0;
}

uint16_t API_GetErrorCode(void)
{
	uint16_t temp;
	
	if (ErrorInfo.head == ErrorInfo.tail) return 0;

	temp = ErrorInfo.buf[ErrorInfo.tail];
	ErrorInfo.tail++;
	if (ErrorInfo.tail >= OS_ERROR_BUF_SIZE) ErrorInfo.tail = 0;
	return temp;
}

void API_SetErrorCode(uint16_t code, uint8_t type)
{
	ErrorInfo.buf[ErrorInfo.head] = code | (type<<8);
	ErrorInfo.head++;
	if (ErrorInfo.head >= OS_ERROR_BUF_SIZE) ErrorInfo.head = 0;
}

uint8_t API_CheckErrorCode(void)
{
	if (ErrorInfo.head == ErrorInfo.tail) return 0;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Device driver
///////////////////////////////////////////////////////////////////////////////
/*void DeviceDrv_Init(void)
{	

	//UARTDrv_Init();
	TLCD_Driver_Init();
	//KeyDrv_Init();
	//LEDDrv_Init();
	//uart_putchar('\r');
//	uart_putchar('\r');
          printf("\r");
}
*/
//
// OS
//
/*
void OS_Window_Null_Process(void)
{
}

void OS_TouchUp_Null(int rID, POINT pt)
{
}

void OS_TouchDown_Null(int rID, POINT pt)
{
}

void OS_TouchMove_Null(int rID, POINT pt)
{
}

void OS_SetSoundRes_Null(uint16_t sndID)
{
}

void OS_Process_Null(void)
{
}
*/
void OS_Timer_Process(void)
{
	int i;
	
	for (i = 0; i < OS_TIMER_NUM; i++) {
		if (OSTimer[i].cnt > 0) {
			OSTimer[i].cnt++;
			if (OSTimer[i].cnt > OSTimer[i].stop) {
				OSTimer[i].cnt = 1;
				API_SendMessage(0, WM_TIMER, i, 0);
			}
		}
	}

}

void OS_Timer(uint16_t nIDEvent)
{
	switch (nIDEvent) {
	case OS_TIMER_ID_0:			//key timer
		KeyDrv_Process();
		break;
	case OS_TIMER_ID_1:			//led timer
		LEDDrv_Process();
		break;
	case OS_TIMER_ID_2:			//usb detect
		USB_Process();
		break;
	}
}

uint16_t OS_FindResourceAtPoint(POINT pt, DWORD *dwStyle)	//pjg<>170825
{
	int i;
	int left, right2, top, bottom;
	//HMENU h;
	
	i = parentWndNum+1;
	if (i >= WINDOW_NUM) i = 1;
	
	while (parentWndNum != i) {
		if (wndInfo[i].hWndParent == hParent) {
              	if (wndInfo[i].disable != BN_DISABLE) {	//pjg++170825
				left = wndInfo[i].x;
				right2 = left + wndInfo[i].nWidth;
				top = wndInfo[i].y;
				bottom = top + wndInfo[i].nHeight;
				if (left <= pt.x) {
					if (right2 >= pt.x) {
						if (top <= pt.y) {
							if (bottom >= pt.y) {
								*dwStyle = wndInfo[i].dwStyle;
								return wndInfo[i].hMenu;
							}
						}
					}
				}
			}
		}
		else break;//if (wndInfo[i].hWndParent == 0) break;
		i++;
		if (i >= WINDOW_NUM) i = 1;
	}
	return 0xffff;
}

LRESULT OS_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int rID;
	POINT pt;
	//HILO_WORD hilowd;
	DWORD dwStyle;

	//hilowd.all = lParam;
	//pt.y = hilowd.hi;
	//pt.x = hilowd.lo;
	pt.x = (uint16_t)(lParam>>16);
	pt.y = (uint16_t)(lParam);
	
	switch (uMsg) {
	case WM_CREATE:
		uart_putstring((char *)(((WND_INFO *)hWnd)->lpWindowName));
		break;
	case WM_TOUCH_DOWN:
		rID_Dn = OS_FindResourceAtPoint(pt, &dwStyle);
		if (rID_Dn < 0xffff) fnOS_Touch.DrawTouchDown(rID_Dn, pt);
		break;
	case WM_TOUCH_UP:
		if (rID_Dn == 0xffff) break;	//old rID
		rID = OS_FindResourceAtPoint(pt, &dwStyle);
		if (rID == 0xffff || rID_Dn != rID) {
			fnOS_Touch.DrawTouchUp(rID_Dn, pt, ((DWORD)(1<<31))|dwStyle);
		}
		else fnOS_Touch.DrawTouchUp(rID, pt, dwStyle);
		rID_Dn = 0xffff;
		break;
	case WM_TOUCH_MOVE:
		rID = OS_FindResourceAtPoint(pt, &dwStyle);
		if (rID_Dn < 0xffff) {
			if (rID == 0xffff || rID_Dn != rID) {
				fnOS_Touch.DrawTouchUp(rID_Dn, pt, ((DWORD)(1<<31))|dwStyle);
				rID_Dn = 0xffff;
			}
		}
		fnOS_Touch.DrawTouchMove(rID, pt);
		break;
	//pjg++>170523
	case WM_KEYDOWN:
		App_KeyDown(wParam);
		break;
	case WM_KEYUP:
		App_KeyUp(wParam);
		break;
	case WM_KEYLONG:
		App_KeyLong(wParam);
		break;
	//case WM_KEYLONGUP:
	//	App_KeyLongUp(wParam);
	//	break;
	case WM_TIMER: //pjg++170524
		OS_Timer(wParam);
		break;
	default:
		break;
	}

	return 0;
}

//int run_cnt = 0;
void OS_Process(void)
{
	TLCD_Driver_Process();

	//MSG pmsg;

	#if 0
	//pmsg = 0;
	if (MSG_OS_GetMessage(pmsg)) {
		OS_WndProc(pmsg.hwnd, pmsg.message, pmsg.wParam, pmsg.lParam, pmsg.pt);
	}
	//#else 
	if (msgInfo.tail != msgInfo.head) {
		if (msg[msgInfo.tail].message == WM_TOUCH) {
			if (msg[msgInfo.tail].wParam == TOUCH_UP) {
				uart_putstring("i pi-l-up.bmp,41,137\r");
			}
			else if (msg[msgInfo.tail].wParam == TOUCH_DOWN) {
				uart_putstring("i pi-l-dn.bmp,41,137\r");
			}
		}
		msgInfo.tail++;
		if (msgInfo.tail > MSG_BUF_SIZE) msgInfo.tail = 0;
	}
	#endif

	#if 0
	pmsg = 0;
	if (MSG_GetMessage(pmsg)) {
		WndProc(pmsg->hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
	}	
	#else
	if (OSMsgInfo.tail != OSMsgInfo.head) {
		OS_WndProc(OSMsg[OSMsgInfo.tail].hwnd, OSMsg[OSMsgInfo.tail].message, 
			OSMsg[OSMsgInfo.tail].wParam, OSMsg[OSMsgInfo.tail].lParam);
		OSMsgInfo.tail++;
		if (OSMsgInfo.tail >= OS_MSG_BUF_SIZE) OSMsgInfo.tail = 0;
	}
	else {
		fnApp_Process();
	}

	//OS_Timer_Process();
	#endif

	
}

void OS_Init(void)
{
	API_Timer_Init();
	DeviceDrv_Init();

//	uart_putstring("- CPM V1.0 (Hexar System(c)) -\r"); // for touch lcd
//	uart_putstring("- 2017.3.2 -\r"); // for touch lcd
	MSG_Init();

	wndNum = 0;
	parentWndNum = 0;          
	wndClassNum = 0;
	//resourceID = 0;
	wndCnt = 0;	
    API_ErrorCode_Init();   //pjg++171220
	App_Init();
}


