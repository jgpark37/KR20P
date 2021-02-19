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
* Filename		: hx_Key.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Key
*******************************************************************************
*
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "hx_Key.h"
#include "hx_Message.h"
#include "hx_Api.h"

/* Private define ------------------------------------------------------------*/
#define KEY_PLAY_PAUSE					HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)

/* Private typedef -----------------------------------------------------------*/
typedef struct _tagKEY_INFO {
	uint32_t time;
	uint32_t cnt;
	uint8_t value;
	uint8_t disable;
	uint16_t longChkCnt;	//pjg++171115
	uint8_t shortChkCnt;
	//uint32_t longPress;		//pjg++191023
}KEY_INFO;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
KEY_INFO KeyInfo;
uint32_t Key_ChkTimer;

/* Private function prototypes -----------------------------------------------*/
void KeyDrv_Process(void)
{
	if (KeyInfo.disable) return;	//pjg++171019
	
	if (KEY_PLAY_PAUSE == GPIO_PIN_RESET) {
		KeyInfo.value = KEY_STOP;
		KeyInfo.cnt++;
		if (KeyInfo.cnt > KeyInfo.longChkCnt) {
			API_SendMessage(0, WM_KEYLONG, (WPARAM)KEY_STOP, 0);
			KeyInfo.cnt = KeyInfo.shortChkCnt;
			//KeyInfo.longPress++;
		}
		else if (KeyInfo.cnt == KeyInfo.shortChkCnt) {
			API_SendMessage(0, WM_KEYDOWN, (WPARAM)KEY_STOP, 0);
		}
	}
	else {
		if (KeyInfo.cnt >= KeyInfo.shortChkCnt) {
			//if (KeyInfo.longPress == 0) {
				API_SendMessage(0, WM_KEYUP, (WPARAM)KeyInfo.value, 0);
			//}
			//else {// KeyInfo.longPress == 1) {
			//	API_SendMessage(0, WM_KEYLONGUP, (WPARAM)KeyInfo.value, 0);
			//}
		}
		KeyInfo.cnt = 0;
		KeyInfo.time = 0;
		KeyInfo.value = 0;
		//KeyInfo.longPress = 0;
	}
}

void KeyDrv_Disable(uint8_t onoff)
{
	KeyInfo.disable = onoff;
	KeyInfo.cnt = 0;
}

void KeyDrv_SetLongKeyTime(uint16_t value)
{
	//KeyInfo.longChkCnt = value;
}

void KeyDrv_SetShortKeyTime(uint8_t value)
{
	KeyInfo.shortChkCnt = value;
}

uint32_t KeyDrv_GetLongKeyCnt(void)
{
	return 1;//KeyInfo.longPress;
}

void KeyDrv_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	KeyInfo.cnt = 0;
	KeyInfo.time = 0;
	KeyInfo.value = 0;
	KeyInfo.disable = 0;	//pjg++171019
	//KeyInfo.longPress = 0;
	KeyInfo.longChkCnt = KEY_LONG_KEY_CNT;
	KeyInfo.shortChkCnt = KEY_SHORT_KEY_CNT;
	API_SetTimer(OS_TIMER_ID_0, KEY_CHECK_TIME);
}

