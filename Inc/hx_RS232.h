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
* Filename		: hx_RS232.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: 
*******************************************************************************
*
*/

#ifndef _RS232_H_
#define _RS232_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/
#define PRINTF_BUF  100
#define TX_BUF      200

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	uint8_t buf[TX_BUF];
	uint8_t head;
	uint8_t tail;
}sTX_INFO;

//extern sTX_INFO sTx;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
extern void USART3_Init();
extern void uart_putchar(char data);
extern void uart_putstring(char *pbuf);
extern void uart_putstringIRQ(uint8_t *buf);

#endif //_RS232_H_

