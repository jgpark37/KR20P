/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems’s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_MotorDriver.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Motor Control Driver
*******************************************************************************
*
*/

#ifndef _MOTOR_DRIVER_H_
#define _MOTOR_DRIVER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_util.h"

/* Private define ------------------------------------------------------------*/
//#define USE_JIG_TEST_MODE			//no motor check when loading, only check home sensor
//#define USE_CHK_SENS_CUR_V2
//#define USE_MOTOR_CTRL_PJG
//
#define MTR_MOVE_AVG_BUF_SIZE						100
//
#define A3930_FET_UNDERVOLTAGE					0x00000000
#define A3930_FET_OVERTEMP						0x00000000
#define A3930_FET_LOGIC_FAULT					0x00000000
#define A3930_FET_SHORT_TO_GND					0x00010000
#define A3930_FET_SHORT_TO_SUP					0x00010000
#define A3930_FET_SHORTED_WINDING				0x00010000
#define A3930_FET_LOW_LOAD_CURRENT				0x00000001

#define MDRV_TIMER_MAX_NUM						2
#define MDrV_HALL_SPEED_BUF_NUM					5

#define HOME_SW								HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define MOTOR_MAX_ANGLE						142
#define MOTOR_MIN_ANGLE						-6
#define MOTOR_OVER_CUR_CHK_CNT				3//20//200(5sec) : 1 is 10ms
#define MOTOR_SENS_CUR_CHK_CNT				5//100
#define MOTOR_DRV_IC_CHK_CNT				100//5000 //10ms unit
#define MOTOR_DRV_PWM_VALUE_MAX				20000

//

/* Private typedef -----------------------------------------------------------*/
typedef struct _tagMOTOR_DRIVER_INFO {
	uint8_t mode;
	uint16_t *speed;
	uint8_t step;
	uint32_t current;
	uint32_t overCurrent;
	uint16_t overCurChkCnt;
	uint16_t sensChkCnt;
	uint16_t standbyChkCnt;
	uint32_t sensCurrent;	//sensitivity
	uint8_t sensGap;
	int *curAngle;
	int32_t *targetAngle;
	uint32_t homeInTime;
	uint32_t homeInChkCnt;
	int32_t *encoder, old_encoder;
	uint16_t driverICErrCnt;
	//uint16_t limitPauseTime;
	uint8_t vibCnt;
#ifdef USE_MOTOR_CTRL_PJG
	uint8_t encDir;
	struct {
		uint16_t buf[MDrV_HALL_SPEED_BUF_NUM];
		uint8_t head;
		uint8_t tail;
		uint16_t speed;
		uint32_t sum;
	} hallSpeed;
#endif
	struct {
		//uint16_t time;
		uint8_t angle;
	}prog;
	struct {
		float alpha;
		float prev;
		uint32_t cur;
		uint32_t prevCur;
		uint8_t gap; //trigger when power is 6~9N
		uint8_t gapBk;
		uint8_t upOffset; //when motor up current is big than down
		uint8_t runOne;
		int8_t diff;
		//uint8_t mode;
	}SensChk;
	struct {
		uint16_t tmp_ms[MDRV_TIMER_MAX_NUM];
	}timer;
	MOVING_AVG I_mvAvg;
	uint32_t I_mvAvgBuf[MTR_MOVE_AVG_BUF_SIZE];
#ifdef USE_MOTOR_CTRL_PJG
	struct {
		uint32_t period;
		uint32_t duty;
	}driver;
#endif
	union {
		uint16_t all;
		struct {
			unsigned short 		dir					: 1;
			unsigned short 		limitSW				: 1;
			unsigned short 		overCurrent			: 1;
			unsigned short 		sensCurrent			: 1;
			unsigned short 		limitPause			: 1;
			unsigned short 		vibDir				: 1;
			unsigned short 		driverICErr			: 1;
			unsigned short 		runOne				: 1;
			unsigned short 		homeIn				: 1;
			unsigned short 		standbyCurrent		: 1;
			unsigned char 		boot				: 1;
			unsigned char 		reserved			: 5;
		}b;
	}f;
}MOTOR_DRIVER_INFO;

enum MOTOR_DRV_MODE {
	MDM_NONE,
	MDM_RUN,
	MDM_PAUSE,
	MDM_STOP,
	MDM_MAX
};

enum MOTOR_DRV_DIRECTION {
	MDD_CW,
	MDD_CCW,			//내려가는 방향  dmove to down
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern MOTOR_DRIVER_INFO MDrvInfo;

/* Private function prototypes -----------------------------------------------*/
extern void MotorDrv_SetTargetPosition(int32_t angle);
extern int32_t MotorDrv_GetPosition(void);
extern int32_t MotorDrv_GetTargetPosition(void);
extern void MotorDrv_Run(void);
//타겟 위치 값을 주지 않으면 같은 기능으로 대체
extern void MotorDrv_Stop(void);
extern void MotorDrv_StopNoCtrl(void);									  
extern void MotorDrv_Pause(void);
//상위단에서 제어
extern uint8_t MotorDrv_GetLimitSwitchStatus(void);
//홈인 스위치 최초 인식 후 이동 시간(mS), 상위단에서 제어
extern void MotorDrv_SetLimitSwitchMoveTime(uint32_t time);
//홈인 스위치 최초 인식 판정 횟수, 상위단에서 제어
extern void MotorDrv_SetLimitSwitchCheckTimes(uint8_t cnt);
extern uint32_t MotorDrv_GetCurrent(void);
extern void MotorDrv_SetOverCurrent(uint32_t mA);
extern uint32_t MotorDrv_GetOverCurrent(void);
extern void MotorDrv_SetSensCurrent(uint32_t mA);
extern uint32_t MotorDrv_GetSensCurrent(void);
extern void MotorDrv_SetPositionCheckTime(uint32_t time);
//모터 관련 상태 변수, 모터단락, 오픈, 과전류
extern uint32_t MotorDrv_Status(void);
//1~5단계 속도 고정
extern void MotorDrv_SetSpeed(uint16_t speed);
extern uint32_t MotorDrv_GetSpeed(void);
//위치값의 부호로 설정가능으로 대체
extern void MotorDrv_SetDirection(uint8_t dir);
extern uint8_t MotorDrv_GetDirection(void);
//파라미터 초기화
extern void MotorDrv_ClearParameter(void);
//모터드라이버 리셋
extern void MotorDrv_DriverReset(void);
extern uint8_t MotorDrv_IsOverCurrent(void);
extern uint8_t MotorDrv_IsSensCurrent(void);
extern void MotorDrv_MoveUp(void);
extern void MotorDrv_MoveDown(void);
extern void MotorDrv_MoveDownWithValue(uint8_t value);
extern void MotorDrv_SetFlagLimitedPause(uint8_t value);
extern uint8_t MotorDrv_GetFlagLimitedPause(void);
extern void MotorDrv_SetVibrationCount(uint8_t value);
extern uint8_t MotorDrv_GetVibrationCount(void);
extern void MotorDrv_SetVibrationDir(uint8_t value);
extern uint8_t MotorDrv_GetVibrationDir(void);
extern void MotorDrv_SetProgressAngle(uint8_t value);
extern uint8_t MotorDrv_GetProgressAngle(void);
//extern void MotorDrv_SetSensCurrent(uint32_t value);
extern void MotorDrv_Process(void);
extern void MotorDrv_StopHome(void);
extern void MotorDrv_GoHome(void);
extern void MotorDrv_SetFlagSensCurrent(uint8_t value);
extern uint8_t MotorDrv_IsFaultAction(void);
extern uint32_t MotorDrv_GetFaultActionValue(void);
extern void MotorDrv_SetFlagRunOne(uint8_t value);
extern uint8_t MotorDrv_GetFlagRunOne(void);
extern void MotorDrv_CheckOverCurrent(void);
extern int32_t MotorDrv_GetEncoderVaue(void);
extern void MotorDrv_SetEncoderVaue(int32_t data);
extern int32_t MotorDrv_GetOldEncoderValue(void);
extern uint16_t MotorDrv_GetMotorState(void);
extern void MotorDrv_SetMotorState(uint16_t value);
extern void MotorDrv_SetOldEncoderValue(int32_t value);
extern int32_t MotorDrv_GetOldEncoderValue(void);
extern uint8_t MotorDrv_CheckLimitSW(void);
extern uint8_t MotorDrv_GetFlagLimitSW(void);
extern void MotorDrv_SetFlagLimitSW(uint8_t value);
extern uint8_t MotorDrv_GetFlagHomeIn(void);
extern void MotorDrv_SetFlagHomeIn(uint8_t value);
extern void MotorDrv_SetMotorZeroPosition(void);
extern void MotorDrv_SetMotorAngleVarAddr(int *addr);
extern void MotorDrv_SetMotorSpeedVarAddr(uint16_t *addr);
extern void MotorDrv_Release(void);
extern void MotorDrv_Disable(void);
extern void MotorDrv_Brake(void);
extern void MotorDrv_PWMChopSlow(void);
extern void MotorDrv_PWMChopFast(void);
extern void MotorDrv_SetPwm(uint32_t value);
extern void MotorDrv_Smart(void);
extern uint8_t MotorDrv_GetGapValueOnStandby(void);
extern uint8_t MotorDrv_IsStandbyCurrent(void);
extern void MotorDrv_SetFlagStandbyCurrent(uint8_t value);
extern void MotorDrv_SetTimerValue(uint8_t num, uint16_t value);
extern void MotorDrv_SetSensFlagRunOne(uint8_t value);
extern uint8_t MotorDrv_GetSensFlagRunOne(void);
extern void MotorDrv_SetMotorEncoderVarAddr(int32_t *addr);
extern void MotorDrv_SensChk_SetAlpha(float value);
extern float MotorDrv_SensChk_GetAlpha(void);
extern void MotorDrv_SensChk_SetGap(uint8_t value);
extern uint8_t MotorDrv_SensChk_GetGap(void);
extern void MotorDrv_Enable(void);
extern void MotorDrv_Init2(void);
extern void MotorDrv_Thread(void);
#endif //_MOTOR_DRIVER_H_


