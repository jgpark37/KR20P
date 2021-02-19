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
* Filename		: hx_UI.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: UI
*******************************************************************************
*
*/

#ifndef _UI_H_
#define _UI_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define USE_FUN_REMOTE

/* Private typedef -----------------------------------------------------------*/
typedef struct _tagUI_TIME {
	struct {
		uint16_t ms;
		uint8_t sec;
		uint8_t min;
		uint32_t hour;
	}time;
	uint32_t tmp_ms;	//variable. use in limit pause
	uint32_t tmp2_ms;	//variable. use in progress
	uint32_t tmp3_ms;	//variable. use in sens current
	uint32_t tmp4_ms;	//variable. use in progress / in anglemeasure / in motor cmd send time
	uint32_t tmp5_ms;	//variable. use in key/touch repeat
	uint32_t tmp_sec;	//variable
}UI_TIME;

enum _LED_MODE {
	LM_NONE,
	LM_BEFORE_BOOT,
	LM_INIT,
	LM_STAND_BY,
	LM_HOME_IN,
	LM_RUN,
	LM_PAUSE,
	LM_STOP,
	LM_ERR_1,
	LM_Complet,
	LM_MAX
};				


#ifdef USE_FUN_REMOTE
typedef struct _tagUI_REMOTE {
	uint8_t buf[10];
	uint8_t head;
	uint8_t tail;
	uint8_t cmd;
}UI_REMOTE;
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern char *pBtnInfo[];
//extern char *pSndInfo[];
extern UI_TIME UI_Time;
#ifdef USE_FUN_REMOTE
extern UI_REMOTE UI_Remote;
#endif

/* Private function prototypes -----------------------------------------------*/
extern void UI_Timer(uint16_t nIDEvent);
extern void UI_Timer_Process(void);
extern void UI_SystemInit_Create(void);
/*
extern void UI_Loading_Init(void);
extern void UI_Loading_OnBnClickedBtnStop(void);
extern void UI_Setup_OnBnClickedBtnSndUp(void);
extern void UI_Setup_OnBnClickedBtnSndDn(void);
extern void UI_Setup_OnBnClickedBtnBLUp(void);
extern void UI_Setup_OnBnClickedBtnBLDn(void);
extern void UI_Setup_OnBnClickedBtnSave(void);
extern void UI_Setup_OnBnClickedBtnCancel(void);
extern void UI_PatientInfo_Init(void);
extern void UI_PatientInfo_OnBnClickedPiBtnLeft(void);
extern void UI_PatientInfo_OnBnClickedPiBtnRight(void);
extern void UI_PatientInfo_OnBnClickedPiBtnUp(void);
extern void UI_PatientInfo_OnBnClickedPiBtnDown(void);
extern void UI_PatientInfo_OnBnClickedPiBtnSetup(void);
extern void UI_PatientInfo_OnBnClickedPiBtnGoHome(void);
extern void UI_AngleEX_OnBnClickedBtnLeft(void);
extern void UI_AngleEX_OnBnClickedBtnRight(void);
extern void UI_AngleEX_OnBnClickedBtnUp(void);
extern void UI_AngleEX_OnBnClickedBtnDown(void);
extern void UI_AngleEX_OnBnClickedBtnGoFirst(void);
extern void UI_SpeedTime_OnBnClickedBtnLeft(void);
extern void UI_SpeedTime_OnBnClickedBtnRight(void);
extern void UI_SpeedTime_OnBnClickedBtnSpdUp(void);
extern void UI_SpeedTime_OnBnClickedBtnSpdDown(void);
extern void UI_SpeedTime_OnBnClickedBtnTmUp(void);
extern void UI_SpeedTime_OnBnClickedBtnTmDown(void);
extern void UI_SpeedTime_OnBnClickedBtnGoFirst(void);
extern void UI_Run_OnBnClickedBtnLeft(void);
extern void UI_Run_OnBnClickedBtnRight(void);
extern void UI_Run_OnBnClickedBtnPlayStart(void);
extern void UI_Run_OnBnClickedBtnPlay(void);
extern void UI_Run_OnBnClickedBtnStop(void);
extern void UI_Run_OnBnClickedBtnGoFirst(void);
*/
extern void UI_LED_Control(uint8_t mode);
extern void UI_Remote_PutData(uint8_t buf);
extern void UI_Remote_Process(void);
extern void UI_Remote_Init(void);

void UI_Loading_Init(void);
void UI_Loading_OnBnClickedBtnStop(void);
void UI_Setup_OnBnClickedBtnSndUp(void);
void UI_Setup_OnBnClickedBtnSndDn(void);
void UI_Setup_OnBnClickedBtnBLUp(void);
void UI_Setup_OnBnClickedBtnBLDn(void);
void UI_Setup_OnBnClickedBtnSave(void);
void UI_Setup_OnBnClickedBtnCancel(void);
void UI_PatientInfo_Init(void);
void UI_PatientInfo_OnBnClickedPiBtnLeft(void);
void UI_PatientInfo_OnBnClickedPiBtnRight(void);
void UI_PatientInfo_OnBnClickedPiBtnUp(void);
void UI_PatientInfo_OnBnClickedPiBtnDown(void);
void UI_PatientInfo_OnBnClickedPiBtnSetup(void);
void UI_PatientInfo_OnBnClickedPiBtnGoHome(void);
void UI_AngleEX_OnBnClickedBtnLeft(void);
void UI_AngleEX_OnBnClickedBtnRight(void);
void UI_AngleEX_OnBnClickedBtnUp(void);
void UI_AngleEX_OnBnClickedBtnDown(void);
void UI_AngleEX_OnBnClickedBtnGoFirst(void);
void UI_SpeedTime_OnBnClickedBtnLeft(void);
void UI_SpeedTime_OnBnClickedBtnRight(void);
void UI_SpeedTime_OnBnClickedBtnSpdUp(void);
void UI_SpeedTime_OnBnClickedBtnSpdDown(void);
void UI_SpeedTime_OnBnClickedBtnTmUp(void);
void UI_SpeedTime_OnBnClickedBtnTmDown(void);
void UI_SpeedTime_OnBnClickedBtnGoFirst(void);
void UI_Run_OnBnClickedBtnLeft(void);
void UI_Run_OnBnClickedBtnRight(void);
void UI_Run_OnBnClickedBtnPlayStart(void);
void UI_Run_OnBnClickedBtnPlay(void);
void UI_Run_OnBnClickedBtnStop(void);
void UI_Run_OnBnClickedBtnGoFirst(void);

void UI_DisplayDecimal(char font, uint8_t pos, int x, int y, short num);
void UI_Run_InitVar(void);
void UI_Setup_SetSoundVolume(uint8_t value);
uint8_t UI_DisplayErrorCode(uint16_t code);
char *UI_GetSndInfo(uint8_t num);
int UI_CheckMotorStatus(void);

/* Private functions ---------------------------------------------------------*/

#endif //_UI_H_
