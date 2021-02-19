/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems��s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_resource.h
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: Resources
*******************************************************************************
*
*/

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define PI_LOAD_V2							//patient id login screen 

/* Private typedef -----------------------------------------------------------*/
enum _RESOURCE_ID {
	//RID_NONE,
	RID_LD_BTN_STOP,
	RID_LD_BTN_STOPK,
	RID_LD_BTN_STOPC,
	//90%
	RID_MEWR_BTN_OK,
	RID_MEWR_BTN_OKK,
	RID_MEWR_BTN_OKC,
	//FULL
	RID_FULL_BTN_YES,
	RID_FULL_BTN_YESK,
	RID_FULL_BTN_YESC,
	RID_FULL_BTN_NO,
	RID_FULL_BTN_NOK,
	RID_FULL_BTN_NOC,
	//FullCheck
	RID_FUCH_BTN_YES,
	RID_FUCH_BTN_YESK,
	RID_FUCH_BTN_YESC,
	RID_FUCH_BTN_NO,
	RID_FUCH_BTN_NOK,
	RID_FUCH_BTN_NOC,
	//Run
	//RID_RUN_BTN_USER,
	//RID_RUN_BTN_EDIT,
	//RID_RUN_BTN_SETUP,
	//RID_RUN_BTN_PLAY,
	//RID_RUN_BTN_HOME,
	//PWDFORGET
	RID_PWDF_BTN_YES,
	RID_PWDF_BTN_NO,
	//login
	RID_LOG_BTN_NUM,
	RID_LOG_BTN_NUM2,
	RID_LOG_BTN_NUM3,
	RID_LOG_BTN_NUM4,
	RID_LOG_BTN_NUM5,
	RID_LOG_BTN_NUM6,
	RID_LOG_BTN_NUM7,
	RID_LOG_BTN_NUM8,
	RID_LOG_BTN_NUM9,
	RID_LOG_BTN_NUM0,
	RID_LOG_BTN_BACKSP,
	RID_LOG_BTN_OK,
	RID_LOG_BTN_CANCEL,
	//Product
	RID_PROD_BTN_CALI,
	RID_PROD_BTN_PC,
	RID_PROD_BTN_BACK,
	RID_PROD_BTN_ANGCHK,
	RID_PROD_BTN_ANGCHK2,
	//Calibration
	RID_CALI_BTN_CALI0,
	RID_CALI_BTN_CALI5,
	RID_CALI_BTN_CALI10,
	RID_CALI_BTN_CALI15,
	RID_CALI_BTN_CALI20,
	RID_CALI_BTN_RETURN,
	RID_CALI_BTN_BACK,
	//home
	RID_HOME_BTN_MEA,
	RID_HOME_BTN_REHAB,
	//user
	RID_USER_BTN_GUEST,
	RID_USER_BTN_USER,
	//Angle measure
	RID_ANG_BTN_LEFT,
	RID_ANG_BTN_HOME,
	RID_ANG_BTN_HOMEK,
	RID_ANG_BTN_HOMEC,
	RID_ANG_BTN_RIGHT,
	RID_ANG_BTN_EXP,
	RID_ANG_BTN_EXM,
	RID_ANG_BTN_FLP,
	RID_ANG_BTN_FLM,
	RID_ANG_BTN_MEA,		//pjg++190814
	RID_ANG_BTN_MEAK,		//pjg++190814
	RID_ANG_BTN_MEAC,		//pjg++190814
	RID_ANG_BTN_REH,		//pjg++190814
	RID_ANG_BTN_REHK,		//pjg++190814
	RID_ANG_BTN_REHC,		//pjg++190814
	//speed time setting
	RID_STS_BTN_LEFT,
	RID_STS_BTN_HOME,
	RID_STS_BTN_HOMEK,
	RID_STS_BTN_HOMEC,
	RID_STS_BTN_RIGHT,
	RID_STS_BTN_SPDP,
	RID_STS_BTN_SPDM,
	RID_STS_BTN_TMP,
	RID_STS_BTN_TMM,
	//run
	RID_RN_BTN_LEFT,
	RID_RN_BTN_HOME,
	RID_RN_BTN_HOMEK,
	RID_RN_BTN_HOMEC,
	//RID_RN_BTN_RIGHT,
	//RID_RN_BTN_USER,
	RID_RN_BTN_SETUP,
	RID_RN_BTN_PLAY,
	RID_RN_BTN_PLAYING,
	RID_RN_BTN_PAUSE,
	RID_RN_BTN_STOP,
	//RID_RN_BTN_MHOME,
	RID_RN_BTN_PROCHK,
	RID_RN_BTN_LIMCHK,
	RID_RN_BTN_VIBCHK,
	//RID_RN_BTN_ANGLE_CHECK, //pjg++190805
	//RID_RN_BTN_ANGLE_PLAY, //pjg++190805
	//SystemSetup-main
	RID_SYS_BTN_BACK,
	RID_SYS_BTN_USB,
	RID_SYS_BTN_USBK,
	RID_SYS_BTN_USBC,
	RID_SYS_BTN_ADSET,
	RID_SYS_BTN_ADSETK,
	RID_SYS_BTN_ADSETC,
	RID_SYS_BTN_INFO,
	RID_SYS_BTN_INFOK,
	RID_SYS_BTN_INFOC,
	// RID_SYS_SND_CHK,
	RID_SYS_SND_UP,
	RID_SYS_SND_DN,
	//   RID_SYS_BR_CHK,
	RID_SYS_BR_UP,
	RID_SYS_BR_DN,
	RID_SYS_LG_KOR,
	RID_SYS_LG_ENG,
	RID_SYS_LG_CHI,
	//PiLoad
	RID_PLD_BTN_LEFT,
	RID_PLD_BTN_RESET,
	RID_PLD_BTN_RESETK,
	RID_PLD_BTN_RESETC,
	RID_PLD_BTN_LOAD,
	RID_PLD_BTN_LOADK,
	RID_PLD_BTN_LOADC,
	RID_PLD_BTN_CLOSE,
	RID_PLD_BTN_RIGHT,
	RID_PLD_BTN_NUM,
       RID_PLD_BTN_NUM2,
       RID_PLD_BTN_NUM3,
       RID_PLD_BTN_NUM4,
       RID_PLD_BTN_NUM5,
       RID_PLD_BTN_NUM6,
       RID_PLD_BTN_NUM7,
       RID_PLD_BTN_NUM8,
       RID_PLD_BTN_NUM9,
       RID_PLD_BTN_NUM10,
       RID_PLD_BTN_NUM11,
       RID_PLD_BTN_NUM12,
       RID_PLD_BTN_NUM13,
       RID_PLD_BTN_NUM14,
       RID_PLD_BTN_NUM15,
       RID_PLD_BTN_NUM16,
       RID_PLD_BTN_NUM17,
       RID_PLD_BTN_NUM18,
       RID_PLD_BTN_NUM19,
       RID_PLD_BTN_NUM20,
       RID_PLD_BTN_NUM21,
       //
       RID_PILRS_BTN_OK,
       RID_PILRS_BTN_OKK,
       RID_PILRS_BTN_OKC,
       RID_PILRS_BTN_CANCEL,
       RID_PILRS_BTN_CANCELK,
       RID_PILRS_BTN_CANCELC,
       //
       RID_PLD2_BTN_RESET,
       RID_PLD2_BTN_RESETK,
	RID_PLD2_BTN_RESETC,
       RID_PLD2_BTN_LEFT,
	RID_PLD2_BTN_LOAD,
	RID_PLD2_BTN_LOADK,
	RID_PLD2_BTN_LOADC,
	RID_PLD2_BTN_CLOSE,
	RID_PLD2_BTN_RIGHT,
	RID_PLD2_BTN_NUM,
       RID_PLD2_BTN_NUM2,
       RID_PLD2_BTN_NUM3,
       RID_PLD2_BTN_NUM4,
       RID_PLD2_BTN_NUM5,
       RID_PLD2_BTN_NUM6,
       RID_PLD2_BTN_NUM7,
       RID_PLD2_BTN_NUM8,
       RID_PLD2_BTN_NUM9,
       RID_PLD2_BTN_NUM10,
       RID_PLD2_BTN_NUM11,
       RID_PLD2_BTN_NUM12,
       RID_PLD2_BTN_NUM13,
       RID_PLD2_BTN_NUM14,
       RID_PLD2_BTN_NUM15,
       RID_PLD2_BTN_NUM16,
       RID_PLD2_BTN_NUM17,
       RID_PLD2_BTN_NUM18,
       RID_PLD2_BTN_NUM19,
       RID_PLD2_BTN_NUM20,
       RID_PLD2_BTN_NUM21,
       //
       RID_PILRS2_BTN_OK,
       RID_PILRS2_BTN_OKK,
       RID_PILRS2_BTN_OKC,
       RID_PILRS2_BTN_CANCEL,
       RID_PILRS2_BTN_CANCELK,
       RID_PILRS2_BTN_CANCELC,
       //
       RID_PLD3_BTN_RESET,
       RID_PLD3_BTN_RESETK,
	RID_PLD3_BTN_RESETC,
       RID_PLD3_BTN_LEFT,
	RID_PLD3_BTN_LOAD,
	RID_PLD3_BTN_LOADK,
	RID_PLD3_BTN_LOADC,
	RID_PLD3_BTN_CLOSE,
	RID_PLD3_BTN_RIGHT,
	RID_PLD3_BTN_NUM,
       RID_PLD3_BTN_NUM2,
       RID_PLD3_BTN_NUM3,
       RID_PLD3_BTN_NUM4,
       RID_PLD3_BTN_NUM5,
       RID_PLD3_BTN_NUM6,
       RID_PLD3_BTN_NUM7,
       RID_PLD3_BTN_NUM8,
       RID_PLD3_BTN_NUM9,
       RID_PLD3_BTN_NUM10,
       RID_PLD3_BTN_NUM11,
       RID_PLD3_BTN_NUM12,
       RID_PLD3_BTN_NUM13,
       RID_PLD3_BTN_NUM14,
       RID_PLD3_BTN_NUM15,
       RID_PLD3_BTN_NUM16,
       RID_PLD3_BTN_NUM17,
       RID_PLD3_BTN_NUM18,
       RID_PLD3_BTN_NUM19,
       RID_PLD3_BTN_NUM20,
       RID_PLD3_BTN_NUM21,
        //
       RID_PILRS3_BTN_OK,
       RID_PILRS3_BTN_OKK,
       RID_PILRS3_BTN_OKC,
       RID_PILRS3_BTN_CANCEL,
       RID_PILRS3_BTN_CANCELK,
       RID_PILRS3_BTN_CANCELC,
        //
       RID_PLD4_BTN_RESET,
       RID_PLD4_BTN_RESETK,
	RID_PLD4_BTN_RESETC,
       RID_PLD4_BTN_LEFT,
	RID_PLD4_BTN_LOAD,
	RID_PLD4_BTN_LOADK,
	RID_PLD4_BTN_LOADC,
	RID_PLD4_BTN_CLOSE,
	//RID_PLD4_BTN_RIGHT,
	RID_PLD4_BTN_NUM,
       RID_PLD4_BTN_NUM2,
       RID_PLD4_BTN_NUM3,
       RID_PLD4_BTN_NUM4,
       RID_PLD4_BTN_NUM5,
       RID_PLD4_BTN_NUM6,
       RID_PLD4_BTN_NUM7,
       RID_PLD4_BTN_NUM8,
       RID_PLD4_BTN_NUM9,
       RID_PLD4_BTN_NUM10,
       RID_PLD4_BTN_NUM11,
       RID_PLD4_BTN_NUM12,
       RID_PLD4_BTN_NUM13,
       RID_PLD4_BTN_NUM14,
       RID_PLD4_BTN_NUM15,
       RID_PLD4_BTN_NUM16,
       RID_PLD4_BTN_NUM17,
       RID_PLD4_BTN_NUM18,
       RID_PLD4_BTN_NUM19,
       RID_PLD4_BTN_NUM20,
       RID_PLD4_BTN_NUM21,
       //
       RID_PILRS4_BTN_OK,
       RID_PILRS4_BTN_OKK,
       RID_PILRS4_BTN_OKC,
       RID_PILRS4_BTN_CANCEL,
       RID_PILRS4_BTN_CANCELK,
       RID_PILRS4_BTN_CANCELC,
      //patient info
	RID_PI_BTN_PADD, //
    RID_PI_BTN_PCAN,
	//AdSet-progress
	//RID_PRO_BTN_LEFT,
	RID_PRO_BTN_EXIT,
	RID_PRO_BTN_EXITK,
	RID_PRO_BTN_EXITC,
	RID_PRO_BTN_RIGHT,
	RID_PRO_BTN_DEGP,
	RID_PRO_BTN_DEGM,
	RID_PRO_BTN_TMP,
	RID_PRO_BTN_TMM,
	RID_PRO_BTN_CHK,
	//AdSet-limited pause
	RID_LP_BTN_LEFT,
	//RID_LP_BTN_EXIT,
	//RID_LP_BTN_EXITK,
	//RID_LP_BTN_EXITC,
    //RID_LP_BTN_RIGHT,
	RID_LP_BTN_PAUSEP,
    RID_LP_BTN_PAUSEM,
    RID_LP_BTN_CHK,
	//AdSet-vibration
	//RID_VIB_BTN_LEFT,
	//RID_VIB_BTN_EXIT,
	//RID_VIB_BTN_EXITK,
	//RID_VIB_BTN_EXITC,
	//RID_VIB_BTN_RIGHT,
	RID_VIB_BTN_DEGP,
	RID_VIB_BTN_DEGM,
	RID_VIB_BTN_CNTP,
	RID_VIB_BTN_CNTM,
	RID_VIB_BTN_CHK,
	//AdSet-sense/home pos
	//RID_SH_BTN_LEFT,
	//RID_SH_BTN_EXIT,
	//RID_SH_BTN_EXITK,
	//RID_SH_BTN_EXITC,
    //RID_SH_BTN_RIGHT,
	//RID_SH_BTN_SENP,
	//RID_SH_BTN_SENM,
	//setup5
	// same as setup4
	//USB Saving
	RID_USB_BTN_SAVE,
	RID_USB_BTN_SAVEK,
	RID_USB_BTN_SAVEC,
	RID_USB_BTN_CANCEL,
	RID_USB_BTN_CANCELK,
	RID_USB_BTN_CANCELC,
	//SystemSetup-Version
	//RID_VI_BTN_UPDATE,
	RID_VI_BTN_BACK,
	RID_VI_BTN_RESET,
	RID_VI_BTN_RESETK,
	RID_VI_BTN_RESETC,
	RID_VI_BTN_DEF, //default
	RID_VI_BTN_DEFK,
	RID_VI_BTN_DEFC,
	//SystemSetup-Reset
	RID_RST_BTN_RESET,	//OK
	RID_RST_BTN_RESETK,
	RID_RST_BTN_RESETC,
	RID_RST_BTN_RESET_DISABLE,
	RID_RST_BTN_RESET_DISABLEK,
	RID_RST_BTN_RESET_DISABLEC,
	RID_RST_BTN_CANCEL,
	RID_RST_BTN_CANCELK,
	RID_RST_BTN_CANCELC,
	/*RID_RESET_BTN_ALL,
	RID_RESET_BTN_DELETE,
	RID_RESET_BTN_CANCEL,*/
	//training complete
	RID_RC_BTN_OK,
	RID_RC_BTN_OKK,
	RID_RC_BTN_OKC,
	RID_RC_BTN_NO, //pjg++190906
	//init patient info
	//RID_RI_BTN_OK,
	//RID_RI_BTN_NO,
	//measure complete
	RID_MC_BTN_OK,
	RID_MC_BTN_OKK,
	RID_MC_BTN_OKC,
	RID_MC_BTN_RETRY,
	RID_MC_BTN_RETRYK,
	RID_MC_BTN_RETRYC,
	//AdSet-main
	RID_AS_BTN_PRO,
	RID_AS_BTN_LIMIT,
	RID_AS_BTN_VIBR,
	RID_AS_BTN_SENS,
	RID_AS_BTN_SENSK,
	RID_AS_BTN_SENSC,
	RID_AS_BTN_INIT_POS,
	RID_AS_BTN_INIT_POSK,
	RID_AS_BTN_INIT_POSC,
	RID_AS_BTN_MEA_SENS,
	RID_AS_BTN_MEA_SENSK,
	RID_AS_BTN_MEA_SENSC,
	RID_AS_BTN_ANG_STEP,
	RID_AS_BTN_ANG_STEPK,
	RID_AS_BTN_ANG_STEPC,
	RID_AS_BTN_AUTO_ANG,
	RID_AS_BTN_AUTO_ANGK,
	RID_AS_BTN_AUTO_ANGC,
	RID_MAX
};

enum _RESOURCE_PI_TXT_ID {
	RID_PI_TXT_PI,
	RID_PI_TXT_PIID,
	//RID_PI_TXT_SPIID,
	RID_PI_TXT_STTIME,
	RID_PI_TXT_STRPT
	
};

enum _SOUND_ID {
	//SNDID_NONE,
	SNDID_ANGLE_SET,
	SNDID_GO_HOME,
	SNDID_BN_GO_FIRST,
	SNDID_OPERATION_END,
	SNDID_OPERATION_START,
	SNDID_OPERATION_STOP,		//5
	SNDID_PATIENT_INFO,
	SNDID_BN_LEFT_RIGHT,
	SNDID_SPEED_SETTING,
	SNDID_SYSTEM_INIT,
	SNDID_BN_UP_DOWN,			//10
	SNDID_MOTOR_OVERLOAD,
	SNDID_SETUP,
	SNDID_RUN_COMPLETE,
	SNDID_SYSTEM_ERROR,
	SNDID_CALIBRATION_COMPLETE,
	SNDID_MAX
};

enum _LANG_TYPE {
	LT_ENG,				//pjg<>180330 change position to kor
	LT_KOR,
	LT_CHINA,
	LT_MAX
};


enum _RESOURCE_ID_WND_BG {
	WND_INIT,
	WND_LOADING,
	WND_HOME,
	WND_USER,
	WND_ANG_MEA,
	WND_ANGLE,
	WND_SPEED_TIME,
	WND_RUN,
	WND_SETUP_MAIN,
	WND_USER_SEL,
	WND_SETUP_AD_MAIN,
	WND_SETUP_AD_PRO,
	WND_SETUP_AD_LIM,
	WND_SETUP_AD_VIB,
	WND_SETUP_AD_SENS,
	WND_SETUP_AD_HOME,
	WND_SETUP_AD_MEA_SENS,
	WND_SETUP_AD_ANGMS, //angle move step
	WND_SETUP_AD_AUTOAVS, //auto angle value setting
	//popup
	WND_POP_USB_SAVE_REQ,
	WND_POP_USB_SAVING,
	WND_POP_SETUP_SYSINFO,
	WND_POP_USB_CHECK,
	WND_POP_RUN_COMPLETE,
	WND_POP_RESET_REQ,
	WND_POP_DEL_DATA,
	WND_POP_ID_CREATE,
	WND_POP_ID_RIGHT,
	WND_POP_USER_DEL,
	WND_POP_MEA_REPORT,
	WND_POP_MEA_COMPLETE1,
	WND_POP_MEA_COMPLETE2,
	WND_POP_MEA_COMPLETE3,
	WND_POP_MEA_COMPLETE,
	WND_POP_PRO_ON,
	WND_POP_PRO_OFF,
	WND_POP_LP_ON,
	WND_POP_LP_OFF,
	WND_POP_VIB_ON,
	WND_POP_VIB_OFF,
	WND_POP_MEM_USAGE,
	WND_POP_MEM_FULL,
	WND_POP_RESETING,
	WND_MAX,
};

enum _RESOURCE_ID_IMG {
	IMG_REHB_DIS,
	IMG_REHB_EN,
	IMG_MEA_EN,
	IMG_ERROR,
	IMG_MAX,
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern char *pBtnInfo[];
extern char *pPatientInfoTxt[][LT_MAX];
extern char *pMultiLangImg[];
extern char *pLdBkgImg;
extern char *pSndInfo[][LT_MAX];
extern char *pWndBgNameInfo[][LT_MAX];
extern char *pImgNameInfo[][LT_MAX];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#endif //_RESOURCE_H_
