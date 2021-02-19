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
* Filename		: hx_TouchLCD.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Touch LCD UI (included graphic driver), Maker : Ganasys
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_TouchLCD.h"
#include "hx_message.h"
//#include <math.h>

/* Private define ------------------------------------------------------------*/
#define TLCD_CMD_BUF_SIZE		(60)

/* Private typedef -----------------------------------------------------------*/


typedef struct _tagTLCD_INFO {
	uint8_t flag;
	uint16_t buf_cnt;
	uint8_t buf[TLCD_CMD_BUF_SIZE]; //t(343,191)
	POINT point_dn;
	POINT point_up;
	POINT point;
	uint8_t bootOk;
} sTLCD_INFO;


enum {
	TLCD_FLAG_NULL			= 0x00,
	TLCD_FLAG_START 		= 0x01,
	TLCD_FLAG_PREDOWN	= 0x02,
	TLCD_FLAG_DOWN		= 0x04,
	TLCD_FLAG_UP			= 0x08,
	TLCD_FLAG_MOVE		= 0x10,
	TLCD_FLAG_MAX			= 0xff
};	

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sTLCD_Que sTLcd_q;
sTLCD_INFO sTLcd_Info;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint32_t pjg_pow(uint8_t x, uint8_t n)
{
	uint8_t i;
	uint32_t temp;

	temp = 1;
	for (i=0; i < n; i++) {
		temp *= x;
	}
	return temp;
}

uint16_t Ascii2Decimal(uint8_t buf_size, uint8_t *buf)
{
	uint8_t i, temp;
	uint16_t value, v, w;

	if (buf_size > 4) return 0;
	temp = buf_size-1;
	value = 0;
	for (i=0; i < buf_size; i++) {
		if (i==temp) 
			value += (buf[i]-0x30);
		else {
			v = (buf[i]-0x30);
			w = (uint16_t)pjg_pow(10, temp-i);
			value += (v*w);
		}
	}
	return value;
}

POINT TLCD_Driver_GetCoordinate(uint8_t buf_size , uint8_t *buf) 
{
	uint8_t temp[5];
	uint16_t i, cnt;
	POINT point;

	cnt = 0;

	point.x=0; 
	point.y=0;
	if (buf_size < 4) return point;
	if (buf_size >= TLCD_CMD_BUF_SIZE) return point;

	for (i =2 ; i < buf_size; i++) {
		if (buf[i] == ',') {
			point.x = Ascii2Decimal(cnt, temp);
			cnt = 0;
			continue;
		}

		temp[cnt] = buf[i];
		cnt++;
		if (cnt > 4) {
			point.x=0; 
			point.y=0; 
			return point;
		}
	}
	point.y = Ascii2Decimal(cnt, temp);
	return point;
}

uint8_t test_touch=0;
void TLCD_Driver_Parsing_Ganasys(void)
{	
	LPARAM lparam;
	
	switch (sTLcd_q.buf[sTLcd_q.tail]) {
	case '<':
	case 't':
	case '!':
		if (!sTLcd_Info.flag) { // first
			sTLcd_Info.flag = TLCD_FLAG_START;
			sTLcd_Info.buf[0] = sTLcd_q.buf[sTLcd_q.tail];
			sTLcd_Info.buf_cnt = 1;
		}
		else {
			if ((sTLcd_Info.buf[sTLcd_Info.buf_cnt-1] == '!' && // down mark start
				sTLcd_q.buf[sTLcd_q.tail] == '<') ||
				(sTLcd_Info.buf[sTLcd_Info.buf_cnt-1] == '>' && // cood mark start
				sTLcd_q.buf[sTLcd_q.tail] == 't') ||
				(sTLcd_Info.buf[sTLcd_Info.buf_cnt-1] == ')' && // up mark start
				sTLcd_q.buf[sTLcd_q.tail] == '!') ) {
				sTLcd_Info.flag = TLCD_FLAG_START;
				sTLcd_Info.buf[0] = sTLcd_q.buf[sTLcd_q.tail];
				sTLcd_Info.buf_cnt = 1;
			}
			else if (sTLcd_Info.buf[0] == '!' && // up mark
				sTLcd_q.buf[sTLcd_q.tail] == '!'){// && sTLcd_Info.buf_cnt == 2) {
				sTLcd_Info.flag |= TLCD_FLAG_UP;
				sTLcd_Info.point_up = sTLcd_Info.point;
				lparam = (sTLcd_Info.point_up.x<<16)|sTLcd_Info.point_up.y;
				API_SendMessage(0, WM_TOUCH_UP, 0, lparam);
				sTLcd_Info.flag  = TLCD_FLAG_NULL;
                         test_touch = 0;
				//sTLcd_Info.buf[0] = 0;
				//sTLcd_Info.buf_cnt = 0;
				sTLcd_Info.buf[sTLcd_Info.buf_cnt] = sTLcd_q.buf[sTLcd_q.tail];
				if (sTLcd_Info.buf_cnt < TLCD_CMD_BUF_SIZE-1) sTLcd_Info.buf_cnt++;
			}
			else {
				sTLcd_Info.buf[sTLcd_Info.buf_cnt] = sTLcd_q.buf[sTLcd_q.tail];
				if (sTLcd_Info.buf_cnt < TLCD_CMD_BUF_SIZE-1) sTLcd_Info.buf_cnt++;
			}
		}
		break;
	case '>':
	case ')':
		if (sTLcd_Info.buf[0] == '<' && sTLcd_q.buf[sTLcd_q.tail] == '>'){//  && 
			//						sTLcd_Info.buf_cnt == 2) { // down mark			
			sTLcd_Info.flag |= TLCD_FLAG_PREDOWN;
			//sTLcd_Info.buf[sTLcd_Info.buf_cnt] = sTLcd_q.buf[sTLcd_q.tail];
			//if (sTLcd_Info.buf_cnt < TLCD_CMD_BUF_SIZE-1) sTLcd_Info.buf_cnt++;
		}
		else if (sTLcd_Info.buf[0] == 't' && sTLcd_q.buf[sTLcd_q.tail] == ')') { // cood mark		
			sTLcd_Info.point = TLCD_Driver_GetCoordinate(sTLcd_Info.buf_cnt, sTLcd_Info.buf);
			if (sTLcd_Info.flag & TLCD_FLAG_PREDOWN) {
				sTLcd_Info.flag |= TLCD_FLAG_DOWN;
				sTLcd_Info.point_dn = sTLcd_Info.point;
                         test_touch = 1;
			}
			else {
				sTLcd_Info.flag |= TLCD_FLAG_MOVE;
                         test_touch = 2;
			}		 
			//sTLcd_Info.buf[sTLcd_Info.buf_cnt] = sTLcd_q.buf[sTLcd_q.tail];
			//if (sTLcd_Info.buf_cnt < TLCD_CMD_BUF_SIZE-1) sTLcd_Info.buf_cnt++;
		}

		if (sTLcd_Info.flag > TLCD_FLAG_DOWN) {
			if (sTLcd_Info.flag & TLCD_FLAG_DOWN) {
				lparam = (sTLcd_Info.point_dn.x<<16)|sTLcd_Info.point_dn.y;
				API_SendMessage(0, WM_TOUCH_DOWN, 0, lparam);
			}
			else if (sTLcd_Info.flag & TLCD_FLAG_MOVE) {
				lparam = (sTLcd_Info.point.x<<16)|sTLcd_Info.point.y;
				API_SendMessage(0, WM_TOUCH_MOVE, 0, lparam);
			}
			sTLcd_Info.flag  = TLCD_FLAG_NULL;		
		}
		
		sTLcd_Info.buf[0] = 0;
		sTLcd_Info.buf_cnt = 0;
		break;
	default:
		sTLcd_Info.buf[sTLcd_Info.buf_cnt] = sTLcd_q.buf[sTLcd_q.tail];
		if (sTLcd_Info.buf_cnt < TLCD_CMD_BUF_SIZE-1) sTLcd_Info.buf_cnt++;
		break;
	}
}

void TLCD_Driver_Process(void)
{
	if (sTLcd_q.head != sTLcd_q.tail) {
		//USART3_Tx(TLCD_buf[TLCD_buf_tail]);
		TLCD_Driver_Parsing_Ganasys();
		sTLcd_q.tail++;
		if (sTLcd_q.tail >= TLCD_BUF_SIZE) sTLcd_q.tail = 0;
	}
}

void TLCD_Driver_Init(void)
{
	uint16_t i;
	uint8_t *p;

	p = (uint8_t *)&sTLcd_q;
	for (i = 0; i < sizeof(sTLCD_Que); i++)  *(p+i) = 0;
	p = (uint8_t *)&sTLcd_Info;
	for (i = 0; i < sizeof(sTLCD_INFO); i++)  *(p+i) = 0;
	sTLcd_q.head = sTLcd_q.tail = 0;

	 //pjg++170605 wait until lcd is booted
	//for (i = 0; i < 255; i++) {
	//	if (sTLcd_q.head > 2) break;
	//	HAL_Delay(1);
	//}
}

