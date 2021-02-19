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
* Filename		: hx_message.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Message box
*******************************************************************************
*
*/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "integer.h"

/* Private define ------------------------------------------------------------*/
#define OS_MSG_BUF_SIZE			100
#define APP_MSG_BUF_SIZE			200

//
// Window Message
//
enum _WINDOW_MESSAGE{
	WM_NULL					= 0x0000,
	WM_CREATE,
	WM_DESTROY,
	WM_MOVE               		 	= 0x0003 ,
	WM_SIZEWAIT            		= 0x0004, /* DDK / Win16 */ 
	WM_SIZE		                	= 0x0005,
	WM_ACTIVATE            		= 0x0006 ,
	WM_ENABLE              		= 0x000a,
	WM_CLOSE					= 0x0010,
	//
	//WM_ACTIVATE state values
	WM_PAINT					= 0x000f,
	WM_QUIT					= 0x0012,
	WM_ERASEBKGND          		= 0x0014,
	WM_SHOWWINDOW          	= 0x0018,
	/* Keyboard messages */
	WM_KEYFIRST            		= 0x0100,
	WM_KEYDOWN				= WM_KEYFIRST,
	WM_KEYUP,
	WM_KEYLONG,				//pjg++170523
	WM_KEYLONGUP,			//pjg++191023
	WM_INITDIALOG          		= 0x0110,
	WM_COMMAND				= 0x0111,
	WM_SYSCOMMAND          		= 0x0112,
	WM_TIMER					= 0x0113,
	WM_MOUSEMOVE			= 0x0200,
	WM_LBUTTONDOWN,
	WM_LBUTTONUP,
	WM_LBUTTONDBLCLK,
	WM_RBUTTONDOWN,
	WM_CUT						= 0x0300,
	WM_COPY,
	WM_PASTE,
	WM_CLEAR,
	WM_UNDO,
	//
	// device
	WM_TOUCH_DOWN			= 0x0240,
	WM_TOUCH_UP,
	WM_TOUCH_MOVE,
	WM_GESTURE,
	WM_SOUND,
	WM_BEEP,
	WM_BACKLIGHT,
	//
	//screen
	//
	// resource
	//
	WM_PICTURE,
	WM_BUTTON,
	WM_SPIN_BUTTON,
	//
	// control
	WM_USER					= 0x400,
	WM_MAX		= 0xFFFF
};

// Window Styles
enum _WINDOW_STYLE{
	WS_OVERLAPPED		= 0x0000,
	WS_POPUP			= 0x8000,
	WS_CHILD			= 0x4000,
	WS_MINIMIZE		= 0x2000,
	WS_CAPTION		= 0x00c0,
	WS_VISIBLE			= 0x1000,
	WS_MAXIMIZE		= 0x0100,
	WS_MINIMIZEBOX	= 0x0002,
	WS_MAXIMIZEBOX	= 0x0001,
	WS_MAX
};

enum _WINDOW_SDB_ID{
/*** Standard dialog button IDs ***/
	IDNULL,
	IDOK,
	IDCANCEL, 
	IDABORT,
	IDRETRY,
	IDIGNORE,
	IDYES,
	IDNO,
	IDCLOSE, 
	IDHELP,
	IDTRYAGAIN,
	IDCONTINUE,
	IDTIMEOUT			= 32000
};

#define WS_OVERLAPPEDWINDOW 	(WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW		(WS_POPUP)
#define WS_CHILDWINDOW		(WS_CHILD)


//https://technet.microsoft.com/ko-kr/bb775951
// Button Control Styles
enum _BUTTON_STYLE {
	BS_PUSHBUTTON			= 0x00, // WM_COMMAND 
	BS_DEFPUSHBUTTON		= 0x01,
	BS_CHECKBOX			= 0x02,
	BS_AUTOCHECKBOX		= 0x03,
	BS_RADIOBUTTON		= 0x04,
	BS_BITMAP				= 0x80,
	BS_MAX
};

enum _BUTTON_CONTROL_MESSAGE {
	/* Button states */
	BST_UNCHECKED,
	BST_CHECKED,
	BST_INDETERMINATE,
	BST_PUSHED				= 0x04,
	BST_FOCUS				= 0x08,
	/*** Win32 button control messages ***/
	BM_GETCHECK			= 0xf0,
	BM_SETCHECK,
	BM_GETSTATE,
	BM_SETSTATE,
	BM_SETSTYLE,
	BM_CLICK,
	BM_MAX
};

// User Button Notification Codes
enum _BUTTON_NOTIFICATION {
	BN_CLICKED,
	BN_PAINT,
	BN_HILITE,
	BN_UNHILITE,
	BN_DISABLE,
	BN_DOUBLECLICKED,
	BN_PUSHED,// = BN_HILITE, //pjg<>190816
	BN_LONGPUSHED,
	BN_MOVEPUSHED,
	BN_LONGPUSHEDUP,	//pjg++191023
};

enum _CUSTOM_W {
	CW_USEDEFAULT 	= 0x80,
	CW_MAX
};

/* Private typedef -----------------------------------------------------------*/
#define CONST 			const;
typedef int 				BOOL;
typedef char 				CHAR;
//typedef uint8_t			BYTE;   // decleared in ther usb host
//typedef uint16_t			WORD;
//typedef uint32_t			DWORD;
//typedef uint32_t			UINT;
//typedef long				LONG;
//typedef uint16_t			wchar_t;
//typedef wchar_t 			WCHAR;
typedef UINT 			WPARAM;
typedef LONG 			LPARAM;
typedef uint32_t			NEAR;
typedef uint32_t			FAR;
typedef void 				*PVOID;
typedef PVOID 			HANDLE;
typedef HANDLE 			HWND;
typedef HANDLE 			HBITMAP;
typedef HANDLE 			HBRUSH;
typedef HANDLE 			HICON;
typedef HICON 			HCURSOR;
typedef HANDLE 			HDC;
typedef HANDLE 			HFONT;
//typedef HANDLE 			HMENU;
typedef WORD 			HMENU;
typedef HANDLE 			HINSTANCE;
typedef CHAR 			*LPCSTR;
typedef LPCSTR 			LPCTSTR;
typedef WCHAR 			*LPCWSTR;
typedef LONG			LRESULT;
typedef void				*LPVOID;

typedef struct {
	uint16_t x;
	uint16_t y;
}POINT;

typedef struct { 
	HWND 	hwnd;
	UINT 	message; 
	WPARAM wParam; 
	LPARAM 	lParam;
	DWORD 	time;
	POINT 	pt;
}MSG;

typedef struct {
	uint8_t head;
	uint8_t tail;	
} MSG_INFO;

typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern MSG OSMsg[OS_MSG_BUF_SIZE+1];
extern MSG AppMsg[APP_MSG_BUF_SIZE+1];
extern MSG_INFO OSMsgInfo;
extern MSG_INFO AppMsgInfo;

/* Private function prototypes -----------------------------------------------*/
extern void MSG_Init(void);
extern void API_SendMessage(HWND hwnd,  UINT message, WPARAM wParam,  LPARAM lParam);
extern void APP_SendMessage(HWND hwnd,  UINT message, WPARAM wParam,  LPARAM lParam);
extern int API_GetMessage(MSG pmsg);
extern void API_ClearMessage(void);
extern void APP_ClearMessage(void);

/* Private functions ---------------------------------------------------------*/

#endif //_MESSAGE_H_
