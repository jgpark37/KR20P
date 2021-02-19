#ifndef __Motor_Control_H__
#define __Motor_Control_H__
   
#include "stm32f4xx_hal.h"	 


extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;


#define   MOTOR_CW		0
#define	MOTOR_CCW		1
/*
#define	PID_MODE	          0
#define	PWM_MODE	          1
#define	GAIN_MODE	          5
#define	SETTING_MODE	4

#define	POSITION_MODE	0
#define   SPEED_MODE	1

#define	POSITION_CON_VAL	100

//#define   SET_SPEED           97
*/
#define   START_PWM           18000 
#define   OVER_CURRENT        250

void Motor_Init(void);
void Motor_direction(void);

#endif