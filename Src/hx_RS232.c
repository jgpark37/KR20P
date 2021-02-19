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
* Filename		: hx_RS232.c
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
#include "stm32f4xx_hal.h"
//#include "stm32f4xx_usart.h"
#include <stdarg.h> //pjg++161017 for va_list_printf : configuartion->link->use semihost lib
#include "hx_TouchLCD.h"


/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sTX_INFO sTx;
extern UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void uart_SendFromBuf();

// 
// UART Driver
//
/*
void USART3_IRQHandler(void)
{
  

    HAL_UART_Receive_IT(&huart1,&sTLcd_q.buf[sTLcd_q.head],1);
    sTLcd_q.head++;
    if (sTLcd_q.head > TLCD_BUF_SIZE) sTLcd_q.head = 0;
      
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
//              sTLcd_q.buf[sTLcd_q.head] = USART_ReceiveData(USART1);
              sTLcd_q.head++;
              if (sTLcd_q.head > TLCD_BUF_SIZE) sTLcd_q.head = 0;
//              USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//    }

//    if(USART_GetITStatus(USART1, USART_IT_TC) != RESET){
//              USART_ClearITPendingBit(USART1, USART_IT_TC);
              //uart_SendFromBuf();
              if ( sTx.head != sTx.tail) {
                        //USART_SendData(USART3, sTx.buf[sTx.tail]);
                        sTx.tail++;
              }
//    }
  
}
*/
/////////////////////////////////////////////////////////////////////////
//
// UART API
//
/////////////////////////////////////////////////////////////////////////
//#ifdef USE_DEBUG_UART2
void uart_putstring(char *pbuf)
{
	int i;
	char bufchk[100];

	//while (sTx.head != sTx.tail);
	
	for (i = 0; i < PRINTF_BUF; i++) {
		bufchk[i] = pbuf[i];
		if (pbuf[i] == 0) break;
                    printf("%c",pbuf[i]); 
//		uart_putchar(pbuf[i]);
	}
}

void uart_SendFromBuf(void)
{
  if ( sTx.head != sTx.tail) {   
      printf("%c",sTx.buf[sTx.tail]); 
      //USART_SendData(USART3, sTx.buf[sTx.tail]);
      sTx.tail++;
  }
}

void uart_putstringIRQ(uint8_t *buf)
{
	uint8_t i;

	i=0;
	while (buf[i] != 0) {
		sTx.buf[i] = buf[i];
		i++;
		if (i > TX_BUF-1) break;
	}
	sTx.head = i;
	sTx.tail = 0;
//	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	uart_SendFromBuf();
}


//#endif
