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
* Filename		: hx_LED.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: LED
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "hx_LED.h"
#include "hx_Api.h"

/* Private define ------------------------------------------------------------*/
#define LED_GREEN_ON		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET)
#define LED_GREEN_OFF		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET)
#define LED_RED_ON			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
#define LED_RED_OFF			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)
#define LED_BLUE_ON			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET)
#define LED_BLUE_OFF		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET)

/* Private typedef -----------------------------------------------------------*/
enum _LED_DRV_STEP {
	LDS_NONE,
	LDS_START,
	LDS_ON,
	LDS_ONTIME,
	LDS_OFF,
	LDS_OFFTIME,
	LDS_DELAY,
	LDS_ALWAYS_ON,
	LDS_MAX
};

typedef struct _tagLED_DRV{
	struct {
		uint16_t timer;
		uint16_t ontime;
		uint16_t offtime;
		uint16_t step;
		uint16_t repeat;
		uint8_t on;
	}led[LS_MAX];
	uint16_t timer;
	uint16_t ontime[LED_ORDER_BUF_SIZE];
	uint8_t buf[LED_ORDER_BUF_SIZE];
	uint8_t order;
}LED_DRV;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LED_DRV LEDDrv;
//uint32_t LED_ChkTimer;

/* Private function prototypes -----------------------------------------------*/
void LEDDrv_SetParam(uint8_t led, uint8_t on, uint32_t ontime, uint32_t offtime, uint32_t repeat)
{
	int i;

	LEDDrv.led[led].timer = 0;
	LEDDrv.led[led].step = LDS_ON;

	if (on == LED_ON) LEDDrv.led[led].on = LED_ON;
	else LEDDrv.led[led].on = LED_OFF;

	switch (led) {
	case LS_RED:
		LED_RED_OFF;
		break;
	case LS_GREEN:
		LED_GREEN_OFF;
		break;
	case LS_BLUE:
		LED_BLUE_OFF;
		break;
	default:
		break;
	}

	if (LEDDrv.led[led].on == LED_ON) {
		API_SetTimer(OS_TIMER_ID_1, LED_CHECK_TIME);
	}
	else {
		for (i = 0; i < LS_MAX; i++) {
			if (LEDDrv.led[i].on == LED_ON) break;
		}
		if (i >= LS_MAX) API_KillTimer(OS_TIMER_ID_1);
	}
	
	LEDDrv.led[led].repeat = repeat; 
	LEDDrv.led[led].ontime = ontime;
	LEDDrv.led[led].offtime = offtime;
	
}

void LEDDrv_LEDProcess(uint8_t led)
{
	LEDDrv.led[led].timer++;

	switch (LEDDrv.led[led].step) {
	//case LDS_START:
	//	break;
	case LDS_ON:
		if (LEDDrv.led[led].on) {
			if (led == LS_RED) LED_RED_ON;
			else if (led == LS_GREEN) LED_GREEN_ON;
			else if (led == LS_BLUE) LED_BLUE_ON;
		}
		LEDDrv.led[led].step = LDS_ONTIME;
		LEDDrv.led[led].timer = 0;
		break;
	 case LDS_ONTIME:
      		if (LEDDrv.led[led].timer > LEDDrv.led[led].ontime) {
         		LEDDrv.led[led].step = LDS_OFF;
        		if (!LEDDrv.led[led].offtime) {
            			LEDDrv.led[led].step = LDS_ALWAYS_ON;
         		}
      		}
      		break;
	case LDS_OFF:
		if (LEDDrv.led[led].on) {
			if (led == LS_RED) LED_RED_OFF;
			else if (led == LS_GREEN) LED_GREEN_OFF;
			else if (led == LS_BLUE) LED_BLUE_OFF;
		}
		LEDDrv.led[led].step = LDS_OFFTIME;
		LEDDrv.led[led].timer = 0;
		break;
	case LDS_OFFTIME:
       	if (LEDDrv.led[led].timer > LEDDrv.led[led].offtime) {
	     		LEDDrv.led[led].step = LDS_ON;
	      		if (LEDDrv.led[led].repeat < 0xffff) {
	         		LEDDrv.led[led].repeat--;
	         		if (!LEDDrv.led[led].repeat) LEDDrv.led[led].step = LDS_NONE;
	      		}
       	}
      		break;
	//case LDS_DELAY:
	//	break;
	default:
		break;
	}
	
}

void LEDDrv_SetOrder(uint8_t order, uint8_t led, uint16_t ontime)
{
	if (order >= LED_ORDER_BUF_SIZE) return;
	
	LEDDrv.buf[order] = led;
	LEDDrv.ontime[order] = ontime;
	LEDDrv.timer = ontime;
}

void LEDDrv_Process(void)
{
	int i;
	
	if (LEDDrv.ontime[0] < 1) {
		LEDDrv_LEDProcess(LS_RED);
		LEDDrv_LEDProcess(LS_GREEN);
		LEDDrv_LEDProcess(LS_BLUE);
		return;
	}

	// process
	switch (LEDDrv.buf[LEDDrv.order]) {
	case LS_RED:
		LEDDrv_LEDProcess(LS_RED);
		break;
	case LS_GREEN:
		LEDDrv_LEDProcess(LS_GREEN);
		break;
	case LS_BLUE:
		LEDDrv_LEDProcess(LS_BLUE);
		break;
	}
	//check ontime
	if (LEDDrv.timer > 0) {
		LEDDrv.timer--;
		return;
	}

	switch (LEDDrv.buf[LEDDrv.order]) {
	case LS_RED:
		LED_RED_OFF;
		break;
	case LS_GREEN:
		LED_GREEN_OFF;
		break;
	case LS_BLUE:
		LED_BLUE_OFF;
		break;
	}

	i = 0;
	while (i++ < LED_ORDER_BUF_SIZE) {
		LEDDrv.order++;
		if (LEDDrv.order >= LED_ORDER_BUF_SIZE) {
			LEDDrv.order = 0;
		}
		LEDDrv.timer = LEDDrv.ontime[LEDDrv.order];
		if (LEDDrv.timer > 0) {
			break;
		}
	}

	switch (LEDDrv.buf[LEDDrv.order]) {
	case LS_RED:
		LEDDrv.led[LS_RED].step = LDS_ON;
		break;
	case LS_GREEN:
		LEDDrv.led[LS_GREEN].step = LDS_ON;
		break;
	case LS_BLUE:
		LEDDrv.led[LS_BLUE].step = LDS_ON;
		break;
	}
	
}

void LEDDrv_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	int i;
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11|GPIO_PIN_12 , GPIO_PIN_RESET);
	
	/*Configure GPIO pin : PD4 */
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11 , GPIO_PIN_RESET);
	/*Configure GPIO pin : PD4 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	LEDDrv.timer = 0;
	LEDDrv.order = 0;
	for (i = 0; i < LS_MAX; i++) {
		LEDDrv.buf[i] = LS_MAX;
		LEDDrv.ontime[i] = 0;
	}
	
}

