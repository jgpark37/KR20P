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
* Filename		: hx_Message.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Message box
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "hx_message.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MSG OSMsg[OS_MSG_BUF_SIZE+1];
MSG AppMsg[APP_MSG_BUF_SIZE+1];
MSG_INFO OSMsgInfo;
MSG_INFO AppMsgInfo;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void API_SendMessage(HWND hwnd,  UINT message, WPARAM wParam,  LPARAM lParam)
{
	//POINT pt;
	OSMsg[OSMsgInfo.head].hwnd = hwnd;
	OSMsg[OSMsgInfo.head].message = message;
	OSMsg[OSMsgInfo.head].wParam = wParam;
	OSMsg[OSMsgInfo.head].lParam = lParam;
	OSMsg[OSMsgInfo.head].time = 0;
	//OSMsg[OSMsgInfo.head].pt = pt;
	OSMsgInfo.head++;
	if (OSMsgInfo.head >= OS_MSG_BUF_SIZE) OSMsgInfo.head = 0;
}

void APP_SendMessage(HWND hwnd,  UINT message, WPARAM wParam,  LPARAM lParam)
{
	AppMsg[AppMsgInfo.head].hwnd = hwnd;
	AppMsg[AppMsgInfo.head].message = message;
	AppMsg[AppMsgInfo.head].wParam = wParam;
	AppMsg[AppMsgInfo.head].lParam = lParam;
	AppMsg[AppMsgInfo.head].time = 0;
	//AppMsg[AppMsgInfo.head].pt = pt;
	AppMsgInfo.head++;
	if (AppMsgInfo.head >= APP_MSG_BUF_SIZE) AppMsgInfo.head = 0;
}

void MSG_OS_ClearMessage(void)
{
	OSMsgInfo.head = OSMsgInfo.tail = 0;
}

void MSG_APP_ClearMessage(void)
{
	OSMsgInfo.head = OSMsgInfo.tail = 0;
}

int MSG_OS_GetMessage(MSG *pmsg)
{
	//MSG *temp;
	//char *temp1, *temp2;
	//int i;
	
	if (OSMsgInfo.head == OSMsgInfo.tail) return 0;
	//temp1 = (char *)&pmsg;
	//temp2 = (char *)&OSMsg[OSMsgInfo.tail];
	pmsg = &OSMsg[OSMsgInfo.tail];
	//for (i = 0; i < sizeof(MSG); i++) temp1[i] = temp2[i];
	//pmsg = OSMsg[OSMsgInfo.tail];
	//pmsg.time = 10;
	//pmsg = temp;
	//temp = pmsg;
	//pmsg.hwnd = OSMsg[OSMsgInfo.tail].hwnd;
	//pmsg.message = OSMsg[OSMsgInfo.tail].message;
	//pmsg.wParam = OSMsg[OSMsgInfo.tail].wParam;
	//pmsg.lParam = OSMsg[OSMsgInfo.tail].lParam;
	//pmsg.time = OSMsg[OSMsgInfo.tail].time;
	//pmsg.pt = OSMsg[OSMsgInfo.tail].pt;
	OSMsgInfo.tail++;
	if (OSMsgInfo.tail >= OS_MSG_BUF_SIZE) OSMsgInfo.tail = 0;
	return 1;
}

void MSG_Init(void)
{
	OSMsgInfo.head = OSMsgInfo.tail = 0;
	AppMsgInfo.head = AppMsgInfo.tail = 0;
}


