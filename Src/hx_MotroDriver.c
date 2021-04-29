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
* Filename		: hx_MotorDriver.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Motor Control Driver
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_MotorDriver.h"
#include "stm32f4xx_hal.h"
#include "Motor_Control.h"
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define HOME_SW						HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define MOTOR_MAX_ANGLE						142
#define MOTOR_MIN_ANGLE						-13//-6
#define MOTOR_OVER_CUR_CHK_CNT				3//20//200(5sec) : 1 is 10ms
#define MOTOR_SENS_CUR_CHK_CNT				1//4//100
#define MOTOR_DRV_IC_CHK_CNT				100//5000 //10ms unit
#define MOTOR_DRV_PWM_VALUE_MAX				14000//20000

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MOTOR_DRIVER_INFO MDrvInfo;

extern uint16_t	FF1, FF2;
//extern int32_t		Target_Angle;
extern uint32_t	ADC_rms;
extern  int32_t     encoder, Target_Angle, Target_counter;
extern uint32_t  ADC_value[10], Pwm_Value, Task_TIMER;
extern uint32_t  Home_flag, Home_TIMER;
extern uint8_t   Home_sensor, Run_mode, Home_mode, Timer_sec, dir;
extern uint16_t  MOTOR_STATE, Total_Counter;
extern TIM_HandleTypeDef htim1;

/* Private function prototypes -----------------------------------------------*/
extern int *UI_GetCurrentMotorAngleVarAddr(void);
extern uint16_t *UI_GetMotorSpeedVarAddr(void);

////////////////////////////Function////////////////////////////////////////
/*
Table 1. Fault Action Table
FF1 FF2    Fault                    Action* 
                               ESF = 0    ESF = 1
0    0  Undervoltage           Disable    Disable
0    0  Overtemperature        No Action  No Action
0    0  Logic Fault            Disable    Disable
1    0  Short to ground        No Action  Disable
1    0  Short to supply        No Action  Disable
1    0  Shorted motor winding  No Action  Disable
0    1  Low load current       No Action  No Action
1    1  None                   No Action  No Action
*/
void MotorDrv_CheckFaultAction(void)
{
#ifdef USE_MOTOR_CTRL_PJG
	FF1 = HAL_GPIO_ReadPin(FF1_GPIO_Port, FF1_Pin);
	FF2 = HAL_GPIO_ReadPin(FF2_GPIO_Port, FF2_Pin);
#endif
	//ESF = 1
	if (FF2 == 0) { //LED of D203 is turned on when FF2 is High
		if (FF1 == 0) {
			//Undervoltage or Logic Fault, Overtemperature, hall connect off
			MDrvInfo.driverICErrCnt++;
			if (MDrvInfo.driverICErrCnt > MOTOR_DRV_IC_CHK_CNT) {
				MDrvInfo.f.b.driverICErr = 1;
			}
		}
		else {
			//Short to ground or Short to supply or Shorted motor winding
		}
	}
	else {
		//Low load current
		if (MDrvInfo.driverICErrCnt) MDrvInfo.driverICErrCnt = 0;//--;
	}
}

void MotorDrv_CheckOverCurrent()
{
	//return;
	if (MDrvInfo.current > MDrvInfo.overCurrent) {
		MDrvInfo.overCurChkCnt++;
	}
	else {
		if (MDrvInfo.overCurChkCnt) {
			MDrvInfo.overCurChkCnt--;
		}
	}
	
	if (MDrvInfo.overCurChkCnt > MOTOR_OVER_CUR_CHK_CNT) {
		MDrvInfo.f.b.overCurrent = 1;
		MDrvInfo.overCurChkCnt = 0;
		MotorDrv_Stop();
	}
	else {
		//MDrvInfo.f.b.overCurrent = 0;
	}
}

void MotorDrv_CheckSensitivityCurrent()
{
#ifdef USE_CHK_SENS_CUR_V2
	short temp;
	
 	//return;
 	if (MotorDrv_GetDirection() == MDD_CCW) {
		temp = (short)(0.0016 * (float)*MDrvInfo.curAngle * (float)*MDrvInfo.curAngle +
				-0.0851 * (float)*MDrvInfo.curAngle + 1.8172);
 	}
	else {
		temp = (short)(-0.0023 * (float)*MDrvInfo.curAngle * (float)*MDrvInfo.curAngle +
				-0.0785 * (float)*MDrvInfo.curAngle + 44.175);
		temp -= 5;
	}
	//pjg<>191106 : bug fix - prevent to occur re-trig sensCureent flag
	if (MDrvInfo.current+temp > MDrvInfo.sensCurrent && 
		MDrvInfo.f.b.sensCurrent == 0 && 
		MDrvInfo.timer.tmp_ms[1] == 0) {
		MDrvInfo.sensChkCnt++;
	}
	else {
		if (MDrvInfo.sensChkCnt) {
			MDrvInfo.sensChkCnt--;
		}
	}
#else
	if (MDrvInfo.current > MDrvInfo.sensCurrent &&
		MDrvInfo.f.b.sensCurrent == 0 &&
		MDrvInfo.timer.tmp_ms[1] == 0) {
		MDrvInfo.sensChkCnt++;
	}
	else {
		if (MDrvInfo.sensChkCnt) {
			MDrvInfo.sensChkCnt--;
		}
	}
#endif
	if (MDrvInfo.sensChkCnt > MOTOR_SENS_CUR_CHK_CNT) {
		MDrvInfo.f.b.sensCurrent = 1;
		MDrvInfo.timer.tmp_ms[1] = 1000;
		//MDrvInfo.sensChkCnt = 0;
	}
	else {
		//MDrvInfo.f.b.sensCurrent = 0;
	}
}

void MotorDrv_CheckSensitivityCurrentOnStandby()
{
	float ftemp, ftemp2;
	
	if (MDrvInfo.SensChk.runOne == 0 || MDrvInfo.timer.tmp_ms[0] != 0) {
		MDrvInfo.SensChk.prev = (float)MDrvInfo.current;//SensChk.cur;
		MDrvInfo.SensChk.runOne++;// = 1;
		MDrvInfo.f.b.standbyCurrent = 0;
		return;
	}
	else {
		ftemp = MDrvInfo.SensChk.alpha*(float)MDrvInfo.current;
		ftemp2 = ((float)1.0-MDrvInfo.SensChk.alpha)*MDrvInfo.SensChk.prev;
		MDrvInfo.SensChk.prev = ftemp+ftemp2;
		MDrvInfo.SensChk.prevCur = (uint32_t)(MDrvInfo.SensChk.prev);
	}

	MDrvInfo.SensChk.diff = MDrvInfo.current-MDrvInfo.SensChk.prevCur;

	if (MDrvInfo.SensChk.diff > MDrvInfo.SensChk.gap) {
		MDrvInfo.standbyChkCnt++;
	}
	else {
		if (MDrvInfo.standbyChkCnt) {
			MDrvInfo.standbyChkCnt--;
		}
	}
	
	if (MDrvInfo.standbyChkCnt > 1) {
		MDrvInfo.f.b.standbyCurrent = 1;
		MDrvInfo.SensChk.runOne = 0;
	}
	else {
		//MDrvInfo.f.b.standbyCurrent = 0;
	}
}


void MotorDrv_SetTargetPosition(int32_t angle)
{
	if (angle > MOTOR_MAX_ANGLE || angle < MOTOR_MIN_ANGLE) return;
	*MDrvInfo.targetAngle = angle;
}

int32_t MotorDrv_GetTargetPosition(void)
{
	return *MDrvInfo.targetAngle;
}

int32_t MotorDrv_GetPosition(void)
{
#ifdef USE_MOTOR_CTRL_PJG
#define MOTOR_PARAM_POLES				7
#define MOTOR_PARAM_HALL_TICKS			6
#define MOTOR_PARAM_HALL_TICK_NUM		(MOTOR_PARAM_POLES*MOTOR_PARAM_HALL_TICKS)

	*MDrvInfo.curAngle = (*MDrvInfo.encoder)/MOTOR_PARAM_HALL_TICK_NUM;
#endif
	//MDrvInfo.curAngle = motorangle;
	return *MDrvInfo.curAngle;
}

void MotorDrv_Run(void)
{
	Run_mode = 1;
	//Home_mode = 1;
	//Home_flag = 0;
	MDrvInfo.mode = MDM_RUN;
	MDrvInfo.sensChkCnt = 0;
	MDrvInfo.overCurChkCnt = 0;
}

//Å¸°Ù À§Ä¡ °ªÀ» ÁÖÁö ¾ÊÀ¸¸é °°Àº ±â´ÉÀ¸·Î ´ëÃ¼
void MotorDrv_Stop(void)
{
	//Run_mode = 0;
	//Run_mode = 1;
	//Home_mode = 1;
	//Home_flag = 0;
	MDrvInfo.mode = MDM_STOP;
}

//pjg++180402
void MotorDrv_StopNoCtrl(void)
{
	Run_mode = 0;
	//Home_mode = 1;
	//Home_flag = 0;
	MDrvInfo.mode = MDM_STOP;
}

void MotorDrv_Pause(void)
{
	//Run_mode = 0;
	//Home_mode = 1;
	//Home_flag = 0;
	Run_mode = 0;
	if (MDrvInfo.f.b.dir == MDD_CCW)
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition()+1); //pjg++180402 quickly stop
	else MotorDrv_SetTargetPosition(MotorDrv_GetPosition());
	Run_mode = 1;		  
	MDrvInfo.mode = MDM_PAUSE;
}

void MotorDrv_GoHome(void)
{
	Run_mode = 1;
	Home_mode = 1;
	//Home_flag = 0;
}

void MotorDrv_StopHome(void)
{
	//Run_mode = 0;
	Home_mode = 0;
	//Home_flag = 0;
}
//»óÀ§´Ü¿¡¼­ Á¦¾î
uint8_t MotorDrv_GetLimitSwitchStatus(void)
{
	int i, j;

	j = 0;
	for (i = 0; i < MDrvInfo.homeInChkCnt; i++) {
#if 0 //ver1
		if (HOME_SW == 1) j++;
#else
		if (HOME_SW == 0) j++;
#endif
		else {
			if (j > 0) j--;
		}
	}
	if (MDrvInfo.homeInChkCnt == j) {
		MDrvInfo.f.b.limitSW = 1;
		return 1;
	}
	return 0;
}

//È¨ÀÎ ½ºÀ§Ä¡ ÃÖÃÊ ÀÎ½Ä ÈÄ ÀÌµ¿ ½Ã°£(mS), »óÀ§´Ü¿¡¼­ Á¦¾î
void MotorDrv_SetLimitSwitchMoveTime(uint32_t time)
{
	MDrvInfo.homeInTime = time;
}

//È¨ÀÎ ½ºÀ§Ä¡ ÃÖÃÊ ÀÎ½Ä ÆÇÁ¤ È½¼ö, »óÀ§´Ü¿¡¼­ Á¦¾î
void MotorDrv_SetLimitSwitchCheckTimes(uint8_t cnt)
{
	MDrvInfo.homeInChkCnt = cnt;
}

uint32_t MotorDrv_GetCurrent(void)
{
	return MDrvInfo.current;
}

void MotorDrv_SetOverCurrent(uint32_t mA)
{
	MDrvInfo.overCurrent = mA;
}

uint32_t MotorDrv_GetOverCurrent(void)
{
	return MDrvInfo.overCurrent;
}

void MotorDrv_SetSensCurrent(uint32_t mA)
{
	MDrvInfo.sensCurrent = mA;
	MDrvInfo.f.b.sensCurrent = 0; //pjg++191106 :
}

uint32_t MotorDrv_GetSensCurrent(void)
{
	return MDrvInfo.sensCurrent;
}

uint8_t MotorDrv_IsOverCurrent(void)
{
	return MDrvInfo.f.b.overCurrent;
}

uint8_t MotorDrv_IsSensCurrent(void)
{
	return MDrvInfo.f.b.sensCurrent;
}

uint8_t MotorDrv_IsStandbyCurrent(void)
{
	return MDrvInfo.f.b.standbyCurrent;
}

void MotorDrv_SetFlagSensCurrent(uint8_t value)
{
	MDrvInfo.f.b.sensCurrent = value;
	MDrvInfo.sensChkCnt = 0;
}

void MotorDrv_SetFlagStandbyCurrent(uint8_t value)
{
	MDrvInfo.f.b.standbyCurrent = value;
}

uint8_t MotorDrv_IsFaultAction(void)
{
	return MDrvInfo.f.b.driverICErr;
}

uint32_t MotorDrv_GetFaultActionValue(void)
{
	return (FF1<<16)|FF2;
}

void MotorDrv_SetPositionCheckTime(uint32_t time)
{
}

//¸ðÅÍ °ü·Ã »óÅÂ º¯¼ö, ¸ðÅÍ´Ü¶ô, ¿ÀÇÂ, °úÀü·ù
uint32_t MotorDrv_Status(void)
{
	return 1;
}

int32_t MotorDrv_GetEncoderVaue(void)
{
	//if (MDrvInfo.encoder != encoder) MDrvInfo.encoder = encoder;
	return *MDrvInfo.encoder;//encoder;
}

void MotorDrv_SetOldEncoderValue(int32_t value)
{
	MDrvInfo.old_encoder = value;
}

int32_t MotorDrv_GetOldEncoderValue(void)
{
	return MDrvInfo.old_encoder;
}

uint16_t MotorDrv_GetMotorState(void)
{
	#ifdef USE_MOTOR_CTRL_PJG
	return (uint16_t)MDrvInfo.f.b.dir;
	#else
	return MOTOR_STATE;
	#endif
}

void MotorDrv_SetMotorState(uint16_t value)
{
	#ifdef USE_MOTOR_CTRL_PJG
	//MotorDrv_SetDirection((uint8_t)value);
	#else
	MOTOR_STATE = value;
	Motor_direction();
	#endif
}

void MotorDrv_SetMotorZeroPosition(void)
{
	Pwm_Value = START_PWM;
	encoder = 0;
	__HAL_TIM_SetCounter(&htim3,0);
	Home_sensor = 1;
	Home_mode = 0;
	//MOTOR_STATE = MOTOR_CW;
	//HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);     // DIR LOW                   
	Target_Angle = -5;
}

uint8_t MotorDrv_CheckLimitSW(void)
{
	//int i;
	//uint8_t senseChkCnt;	
	
	if (MotorDrv_GetLimitSwitchStatus() == 1) {   //gpio read
		MotorDrv_SetMotorZeroPosition();
		return 1;
	}

	return 0;
}

uint8_t MotorDrv_GetFlagLimitSW(void)
{
	return MDrvInfo.f.b.limitSW;
}

void MotorDrv_SetFlagLimitSW(uint8_t value)
{
	MDrvInfo.f.b.limitSW = value;
}

uint8_t MotorDrv_GetFlagHomeIn(void)
{
	return MDrvInfo.f.b.homeIn;
}

void MotorDrv_SetFlagHomeIn(uint8_t value)
{
	MDrvInfo.f.b.homeIn = value;
}

//1~5´Ü°è ¼Óµµ °íÁ¤
void MotorDrv_SetSpeed(uint16_t speed)
{
	switch (speed) {
	case 0:
		MDrvInfo.step = 1;
		break;
	case 1:
		MDrvInfo.step = 10;
		break;
	case 2:
		MDrvInfo.step = 20;
		break;
	case 3:
		MDrvInfo.step = 30;
		break;
	case 4:
		MDrvInfo.step = 40;
		break;
	case 5:
		MDrvInfo.step = 50;
		break;
	default:
		MDrvInfo.step = 1;
		break;
	}
	if (speed < 6) {
		*MDrvInfo.speed = MDrvInfo.step/10;
		#ifdef USE_MOTOR_CTRL_PJG
		if (*MDrvInfo.speed == 0) MDrvInfo.driver.period = 0;
		else MDrvInfo.driver.period = MOTOR_DRV_PWM_VALUE_MAX;//16800 / (*MDrvInfo.speed);
		MotorDrv_SetPwm(MDrvInfo.driver.period);
		#endif
	}
}

uint32_t MotorDrv_GetSpeed(void)
{
	return *MDrvInfo.speed;
}

//À§Ä¡°ªÀÇ ºÎÈ£·Î ¼³Á¤°¡´ÉÀ¸·Î ´ëÃ¼
void MotorDrv_SetDirection(uint8_t dir)
{
	MDrvInfo.f.b.dir = dir;
	#ifdef USE_MOTOR_CTRL_PJG
	if (dir) HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);                                        // DIR LOW
	else HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);                                        // DIR HIGH
	#endif
}

uint8_t MotorDrv_GetDirection(void)
{
	return MDrvInfo.f.b.dir;
}

//ÆÄ¶ó¹ÌÅÍ ÃÊ±âÈ­
void MotorDrv_ClearParameter(void)
{
}

//¸ðÅÍµå¶óÀÌ¹ö ¸®¼Â
void MotorDrv_DriverReset(void)
{
}

void MotorDrv_MoveUp(void)
{
	int32_t gap;
	
	//if (MotorDrv_GetPosition() > *MDrvInfo.targetAngle - MDrvInfo.step) 
	if (*MDrvInfo.curAngle > *MDrvInfo.targetAngle)
		gap = *MDrvInfo.curAngle - *MDrvInfo.targetAngle;
	else 
		gap =  *MDrvInfo.targetAngle - *MDrvInfo.curAngle;
	if (gap < 2) {
		*MDrvInfo.targetAngle -= MDrvInfo.step;
		if (*MDrvInfo.targetAngle < MOTOR_MIN_ANGLE) {
			*MDrvInfo.targetAngle =  MOTOR_MIN_ANGLE;
		}
	}
}

void MotorDrv_MoveDown(void)
{
	int32_t gap;

	//if (MotorDrv_GetPosition() < *MDrvInfo.targetAngle + MDrvInfo.step) 
	if (*MDrvInfo.curAngle > *MDrvInfo.targetAngle)
		gap = *MDrvInfo.curAngle - *MDrvInfo.targetAngle;
	else 
		gap = *MDrvInfo.targetAngle - *MDrvInfo.curAngle;
	if (gap < 2) {
		*MDrvInfo.targetAngle += MDrvInfo.step;
		if (*MDrvInfo.targetAngle > MOTOR_MAX_ANGLE) {
			*MDrvInfo.targetAngle = MOTOR_MAX_ANGLE;
		}
	}
}

void MotorDrv_MoveDownWithValue(uint8_t value)
{
	int32_t gap;

	//if (MotorDrv_GetPosition() < *MDrvInfo.targetAngle + MDrvInfo.step) 
	if (*MDrvInfo.curAngle > *MDrvInfo.targetAngle)
		gap = *MDrvInfo.curAngle - *MDrvInfo.targetAngle;
	else 
		gap = *MDrvInfo.targetAngle - *MDrvInfo.curAngle;
	if (gap < 3) {
		*MDrvInfo.targetAngle += value;
		if (*MDrvInfo.targetAngle > MOTOR_MAX_ANGLE) {
			*MDrvInfo.targetAngle = MOTOR_MAX_ANGLE;
		}
	}
}

void MotorDrv_SetFlagLimitedPause(uint8_t value)
{
	MDrvInfo.f.b.limitPause = value;
}

uint8_t MotorDrv_GetFlagLimitedPause(void)
{
	return MDrvInfo.f.b.limitPause;
}

void MotorDrv_SetVibrationCount(uint8_t value)
{
	MDrvInfo.vibCnt = value;
}

uint8_t MotorDrv_GetVibrationCount(void)
{
	return MDrvInfo.vibCnt;
}

void MotorDrv_SetVibrationDir(uint8_t value)
{
	MDrvInfo.f.b.vibDir = value;
}

uint8_t MotorDrv_GetVibrationDir(void)
{
	return MDrvInfo.f.b.vibDir;
}

void MotorDrv_SetProgressAngle(uint8_t value)
{
	MDrvInfo.prog.angle = value;
}

uint8_t MotorDrv_GetProgressAngle(void)
{
	return MDrvInfo.prog.angle;
}

void MotorDrv_SetFlagRunOne(uint8_t value)
{
	MDrvInfo.f.b.runOne = value;
}

uint8_t MotorDrv_GetFlagRunOne(void)
{
	return MDrvInfo.f.b.runOne;
}

void MotorDrv_SetMotorAngleVarAddr(int *addr)
{
	MDrvInfo.curAngle = addr;
}

void MotorDrv_SetMotorSpeedVarAddr(uint16_t *addr)
{
	MDrvInfo.speed = addr;
}

//void MotorDrv_SetMotorEncoderVarAddr(int32_t *addr)
//{
//	MDrvInfo.encoder = addr;
//}

/*void MotorDrv_SetSensCurrent(uint32_t value)
{
	MDrvInfo.sensCurrent = value;
}*/

uint8_t MotorDrv_GetGapValueOnStandby(void)
{
	return MDrvInfo.SensChk.gap;
}

//pjg++190904
// Coast mode - All gates OFF
void MotorDrv_Release(void)
{
	HAL_GPIO_WritePin(GPIOB, COAST_Pin, GPIO_PIN_RESET); //pjg++190715 : Coast mode - All gates OFF
	HAL_GPIO_WritePin(GPIOB, RESET_Pin, GPIO_PIN_SET);
}

//pjg++190904
// timer off
void MotorDrv_Disable(void)
{
	htim1.Instance->CCER &= (uint32_t)(~(uint32_t)TIM_CCER_CC1E); //pjg++190715 : fix not stop motor, move position : fast stop motor
}

//pjg++190906
// Brake mode - All low-side gates ON
void MotorDrv_Brake(void)
{
	Run_mode = 0;
	MotorDrv_Disable();
	HAL_GPIO_WritePin(GPIOB, COAST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, BRAKE_Pin, GPIO_PIN_RESET);
}

//PWM chop â€“ current decay with both low-side drivers ON
void MotorDrv_PWMChopSlow(void)
{
	Run_mode = 0;
	MotorDrv_Disable();
	HAL_GPIO_WritePin(GPIOB, MODE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, PWM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, BRAKE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, COAST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_Pin, GPIO_PIN_SET);
}

void MotorDrv_PWMChopFast(void)
{
	Run_mode = 0;
	MotorDrv_Disable();
	HAL_GPIO_WritePin(GPIOB, MODE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, PWM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, BRAKE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, COAST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_Pin, GPIO_PIN_SET);
}

//pjg++191114
void MotorDrv_Enable(void)
{
	HAL_GPIO_WritePin(GPIOB, COAST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_Pin, GPIO_PIN_SET);
}

void MotorDrv_SetPwm(uint32_t value)
{
	Pwm_Value = value;
	TIM1->ARR = value;
}

#ifdef USE_MOTOR_CTRL_PJG
//set duty
//param : duty x 10
void MotorDrv_SetDuty(uint32_t duty)
{
	uint32_t temp;
	
	temp = (MDrvInfo.driver.period * (1000-duty))/1000;
	TIM1->CCR1 = temp;
}
#endif

void MotorDrv_SetTimerValue(uint8_t num, uint16_t value)
{
	MDrvInfo.timer.tmp_ms[num] = value;
}

void MotorDrv_SetSensFlagRunOne(uint8_t value)
{
	MDrvInfo.SensChk.runOne = value;
}

uint8_t MotorDrv_GetSensFlagRunOne(void)
{
	return MDrvInfo.SensChk.runOne;
}

void MotorDrv_SensChk_SetAlpha(float value)
{
	MDrvInfo.SensChk.alpha = value;
}

float MotorDrv_SensChk_GetAlpha(void)
{
	return MDrvInfo.SensChk.alpha;
}

void MotorDrv_SensChk_SetGap(uint8_t value)
{
	MDrvInfo.SensChk.gap = value;
}

uint8_t MotorDrv_SensChk_GetGap(void)
{
	return MDrvInfo.SensChk.gap;
}

// run every 10ms
void MotorDrv_Process(void)
{
	int i;

	for (i = 0; i < MDRV_TIMER_MAX_NUM; i++) {
		if (MDrvInfo.timer.tmp_ms[i] >= 10) MDrvInfo.timer.tmp_ms[i] -= 10;
		else MDrvInfo.timer.tmp_ms[i] = 0;
	}
	MDrvInfo.current = MovingAvg_Process(&MDrvInfo.I_mvAvg, ADC_rms);
	#ifndef USE_JIG_TEST_MODE
	MotorDrv_CheckOverCurrent();
	MotorDrv_CheckSensitivityCurrent();
	MotorDrv_CheckFaultAction();
	MotorDrv_GetLimitSwitchStatus(); //201026bg
	MotorDrv_CheckSensitivityCurrentOnStandby();
	#endif
}

#ifdef USE_MOTOR_CTRL_PJG
//void EXTI15_10_IRQHandler(void) // 10,11,12,13,14,15 í•€ ì¸í„°ëŸ½íŠ¸ í•¨ìˆ˜ìž…ë‹ˆë‹¤
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	if (HAL_GPIO_ReadPin(DIRO_GPIO_Port, DIRO_Pin) == GPIO_PIN_RESET) {
		//Bit 4 DIR: Direction
	  	//0: Counter used as upcounter
	  	//1: Counter used as downcounter */
	  	//TIM3->CR1 |= 0x10; // 1
	  	MDrvInfo.encDir = 0;
	}
	else {
		//TIM3->CR1 &= ~0x10; // 0
		MDrvInfo.encDir = 1;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint32_t temp;
	
	if (htim->Instance == TIM3) {
		//temp = TIM3->CNT;
		if (MDrvInfo.encDir == 1) (*MDrvInfo.encoder)--;
		else (*MDrvInfo.encoder)++;
		MDrvInfo.hallSpeed.buf[MDrvInfo.hallSpeed.head] = TIM7->CNT;
		MDrvInfo.hallSpeed.sum += MDrvInfo.hallSpeed.buf[MDrvInfo.hallSpeed.head];
		MDrvInfo.hallSpeed.tail = MDrvInfo.hallSpeed.head + 1;
		if (MDrvInfo.hallSpeed.tail >= MDrV_HALL_SPEED_BUF_NUM) {
			MDrvInfo.hallSpeed.tail = 0;
		}
		//else {
		//	MDrvInfo.hallSpeed.tail = MDrvInfo.hallSpeed.head + 1;
		//}
		MDrvInfo.hallSpeed.head++;
		if (MDrvInfo.hallSpeed.head >= MDrV_HALL_SPEED_BUF_NUM) MDrvInfo.hallSpeed.head = 0;
		MDrvInfo.hallSpeed.sum -= MDrvInfo.hallSpeed.buf[MDrvInfo.hallSpeed.tail];
		MDrvInfo.hallSpeed.speed = MDrvInfo.hallSpeed.sum/MDrV_HALL_SPEED_BUF_NUM;
		TIM7->CNT = 0;
		HAL_GPIO_TogglePin(GPIOB, LED_Pin);
	}
	else if (htim->Instance == TIM7) {
		//temp = TIM3->CNT;
		//*MDrvInfo.encoder = temp;
		//tim7Interval = temp - tim7Old;
		//tim7Old = temp;
		//TIM7->CNT = 0;
	}
}


int32_t oldEncoder;
void MotorDrv_Thread(void)
{
	int32_t curEncoder, temp;

	curEncoder = TIM1->CNT;
	if (MDrvInfo.f.b.dir == MDD_CCW) {
		if (oldEncoder > curEncoder) {
			temp = oldEncoder - curEncoder;
		}
		else {
			
		}
		encoder -= temp;
	}
	else {
	}
	oldEncoder = curEncoder;
	//speed
	//if (MDrvInfo.targetAngle)
}
#endif

void MotorDrv_Smart(void)
{
#ifdef USE_MOTOR_CTRL_PJG
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);                                    // M_RESET LOW
	HAL_Delay(500);
	HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, GPIO_PIN_SET);                                        // MODE HIGH   
	HAL_GPIO_WritePin(COAST_GPIO_Port, COAST_Pin, GPIO_PIN_SET);                                      // COAST HIGH 
	HAL_GPIO_WritePin(BRAKE_GPIO_Port, BRAKE_Pin, GPIO_PIN_SET);                                      // BRAKE HIGH
	HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);                                        // DIR LOW
	HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);                                      // M_RESET HIGH
	HAL_GPIO_WritePin(ESF_GPIO_Port, ESF_Pin, GPIO_PIN_SET);                                          // ESF HIGH   

	GPIO_InitStruct.Pin = DIRO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DIRO_GPIO_Port, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	MotorDrv_SetSpeed(4);
	MotorDrv_SetDuty(100);
	MDrvInfo.f.b.dir = MDD_CCW;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); 
	//HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);                                        // DIR LOW
	//MotorDrv_SetSpeed(1);
	//MotorDrv_SetDuty(50);
	HAL_ADC_Start_DMA(&hadc1,ADC_value,1);
#else
	Motor_Init();
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //pjg++200109
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); //pjg++200109
	HAL_ADC_Start_DMA(&hadc1,ADC_value,1);
	MOTOR_STATE = MOTOR_CCW;	
	Motor_direction();	 
	Target_counter = 0;
	Pwm_Value = START_PWM;
	//Motor_Current = 0;

	Run_mode = 0;
	Home_mode = 0;
	Task_TIMER = 0;
	//HAL_GPIO_WritePin(ESF_GPIO_Port, ESF_Pin, GPIO_PIN_RESET);                                          // ESF LOW  
#endif

}

void MotorDrv_Init2(void)
{
	int i;
	uint8_t *ptr;

	ptr = (uint8_t *)&MDrvInfo;
	for (i = 0; i < sizeof(MOTOR_DRIVER_INFO); i++) {
		*(ptr + i) = 0;
	};

	//MDrvInfo.curAngle = UI_GetCurrentMotorAngleVarAddr();
	MDrvInfo.targetAngle = &Target_Angle;
	MDrvInfo.encoder = &encoder;
	//MDrvInfo.current = &ADC_rms;
	//MDrvInfo.speed = UI_GetMotorSpeedVarAddr();
	MDrvInfo.step = 1;
	MDrvInfo.homeInChkCnt = 10;
	MDrvInfo.SensChk.gap = 3;
	MDrvInfo.SensChk.upOffset = 2;
	MDrvInfo.SensChk.gapBk = MDrvInfo.SensChk.gap;
	MDrvInfo.SensChk.runOne = 0;
	MDrvInfo.SensChk.alpha = 0.09;//0.12;	
	//MDrvInfo.SensChk.mode = 0;
	MotorDrv_SetDirection(MDD_CCW);
	MovingAvg_Init(&MDrvInfo.I_mvAvg, MDrvInfo.I_mvAvgBuf, MTR_MOVE_AVG_BUF_SIZE, 0);
//#ifdef USE_MOTOR_CTRL_PJG
	FF1 = 1;
	FF2 = 1;
//#endif
}


