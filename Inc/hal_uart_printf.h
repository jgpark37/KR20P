#ifndef __HAL_UART_PRINTF_H__
#define __HAL_UART_PRINTF_H__

#ifdef __cplusplus
 extern "C" {
#endif
   
#include "stm32f4xx_hal.h"	 
#include <stdio.h>
	 
extern UART_HandleTypeDef 	huart1;

#define PRINTF_UART_FORT	huart1

size_t __write(int handle, uint8_t *buf, size_t bufSize);

#ifdef __cplusplus
 }
#endif


#endif