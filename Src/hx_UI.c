/*
*******************************************************************************
*
*                             Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systems°Øs source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_UI.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: UI
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_UI.h"
#include "hx_Message.h"
#include "hx_Resource.h"
#include "hx_API.h"
#include "hx_APP.h"
#include "hx_mcuFlash.h"						
#include "hx_RS232.h"
#include "Motor_Control.h"
#include "hx_Key.h"
#include "hx_LED.h"
#include "hx_EEPROM.h"					  
#include "hx_USBDisk.h"					  
#include "hx_MotorDriver.h"	
#include "hx_Util.h"	
#include "hx_Error.h" //pjg++190906
#include <arm_math.h> //sin,cos

/* Private define ------------------------------------------------------------*/
//#define USE_DEBUG_MODE
#define USE_CALIB_ONLY_0KG					//pjg++200110
//#define USE_QC_LIFE_TEST_SAVE_CUR			//pjg++200131 : current save to usb every 60min, run time max is 999min, display usb check message
//
#define PATIENT_NUM					84
#define PATIENT_INFO_SIZE			2048	// eeprom 256KByte
#define PATIENT_INFO_32_SIZE		2048	//mcu flash
#define EEP_LOGIN_INFO_SAVE_ADDR	0
#define EEP_SETUP_SAVE_ADDR			1024
#define DISP_NUM_BUF_SIZE			17		// "i 0.bmp,123,456\r"
#define DISP_BLANK_NUM_BUF_SIZE		20		// "i bnk.bmp,123,456\r"
#define DISP_BLANK_ALL_NUM_BUF_SIZE	20	// "i bnk2.bmp,123,456\r"
#define DISP_NUM_PLACE_VALUE		3		// 3 place
#define DISP_NUM_DISP_SAME_TIME		(DISP_NUM_PLACE_VALUE*8)
#define DISP_NUM_SBUF_SIZE			18		// "i 50.bmp,123,456\r"
#define DISP_BLANK_NUM_SBUF_SIZE	20		// "i 5bnk.bmp,123,456\r"
//#define DISP_NUM_PLACE_VALUE		3		// 3 place
#define DISP_NUM_SDISP_SAME_TIME	(DISP_NUM_PLACE_VALUE*8)
#define DISP_NUM_9P_100_POS			0
#define DISP_NUM_9P_10_POS			21
#define DISP_NUM_9P_1_POS			(DISP_NUM_9P_10_POS*2)
#define DISP_NUM_7P_100_POS			0
#define DISP_NUM_7P_10_POS			14
#define DISP_NUM_7P_1_POS			(DISP_NUM_9P_10_POS*2)
#define DISP_NUM_5P_100_POS			0
#define DISP_NUM_5P_10_POS			12
#define DISP_NUM_5P_1_POS			(DISP_NUM_5P_10_POS*2)
#define DISP_NUM_5R_9P_1000_POS		0
#define DISP_NUM_5R_9P_100_POS		19
#define DISP_NUM_5R_9P_10_POS		(DISP_NUM_5R_9P_100_POS*2)
#define DISP_NUM_5R_9P_1_POS		(DISP_NUM_5R_9P_100_POS*3)
#define DISP_NUM_5R_5P_1000_POS		12
#define DISP_NUM_5R_5P_100_POS		(DISP_NUM_5R_5P_1000_POS*2)
#define DISP_NUM_5R_5P_10_POS		(DISP_NUM_5R_5P_1000_POS*3)
#define DISP_NUM_5R_5P_1_POS		(DISP_NUM_5R_5P_1000_POS*4)
#define DISP_NUM_ST_Y_POS			129
#define DISP_NUM_RN1_Y_POS			(39+5)
#define DISP_NUM_RN2_Y_POS			(243)
#define DISP_NUM_RC_Y_POS			165+6
#define DISP_NUM_SU1_Y_POS			93+4
#define DISP_NUM_SU2_Y_POS			158+4
#define DISP_NUM_SU3_Y_POS			83+4
#define DISP_NUM_SU4_Y_POS			158+4
#define DISP_NUM_5U_4PS_1000		DISP_NUM_5R_9P_1000_POS+10		
#define DISP_NUM_5U_4PS_100			DISP_NUM_5R_9P_100_POS+10		
#define DISP_NUM_5U_4PS_10			DISP_NUM_5R_9P_10_POS+10
#define DISP_NUM_5U_4PS_1			DISP_NUM_5R_9P_1_POS+10	
#define DISP_NUM_5U_3PS_100			DISP_NUM_5U_4PS_1000+10		
#define DISP_NUM_5U_3PS_10			DISP_NUM_5U_4PS_100+10
#define DISP_NUM_5U_3PS_1			DISP_NUM_5U_4PS_10+10
#define DISP_NUM_5U_2PS_10			DISP_NUM_5U_3PS_100+10
#define DISP_NUM_5U_2PS_1			DISP_NUM_5U_3PS_10+10
#define DISP_NUM_5U_1PS_1			DISP_NUM_5U_4PS_1000+29

#define COMPANY_LENGTH				14
#define MODEL_LENGTH				10
#define PI_INFO_VER					0x14
//sound set
#define BL_LEVEL_NUM				5		//backlight
#define SND_LEVEL_NUM				5		//sound
//#define MOTOR_OC_LEVEL_NUM		4		//over current table num

//#define RUN_SPEED_MAX				3
#define HOME_IN_TOTAL_TIME			70
#define PROD_CAL_BUF_SIZE			5		//product calibration buffer size
#define HOME_IN_INIT_POS			10//15 //pjg<>200205 10 is adjust deg to box package

#define EXERCISE_INFO_NUM			10//375//480//(480:1.6month, 85 person, 256Kbyte memory)2//0

#define RUN_ANGLE_MAX				140
#define RUN_ANGLE_MIN				-5
#define ANGLE_MOVE_GAP				5
#define ANGLE_MIN_GAP				15
#define ANGLE_MIN_DEG				5
#define SAVE_OFFSET_VALUE			5
#define RUN_MIN_EXE_ANGLE			25

#define LOGIN_GUEST_PWD_LEN			4
#define LOGIN_USER_PWD_LEN			4
#define LOGIN_ADMIN_PWD_LEN			5

#define USE_FUN_ANGLE_AUTO_CHECK	//pjg++190805
#define STANDBY_CHK_CURRENT			Motor_OverCurTbl[SL_LEVEL1][MS_SPEED3]
#define STANDBY_SENS_CURRENT		Motor_OverCurTbl[SL_LEVEL2][MS_SPEED3]
#define RUN_CHK_OVERCURRENT			1180 //1000
#define RUN_CHK_MINCURRENT			300
#define USE_FUN_EEPROM_DISK		//pjg++190805
#define UI_WND_DEPTH				3
//
#define ANG_CHK_ANGLE_MAX			135
#define ANG_CHK_ANGLE_MIN			0
#define ANG_MEA_TOTAL_COUNT			3
#define ANG_MEA_MAX_SENS_VALUE		9
#define ANG_MEA_MAX_SENS_STEP		3
#define ANG_MEA_NO_GRAVITY_ANGLE	100
#define USE_ANG_MEA_METHOD_TYPE1		//pjg++191106 : when down - add offset at 110 deg over, when up - add offset all deg
//#define USE_ANG_MEA_METHOD_TYPE2		//pjg++191106 : when down - add offset at 110 deg over, when up - add offset all deg
//#define USE_MOTOR_RUN_TEST			

//
#define PRODUCT_MEA_ANGLE_MAX		49
#define PRODUCT_MEA_ANGLE_MIN		-4
//
#define SAVE_EXE_INFO_MARK			0x55AA
//
#define EEPROMDISK_DRIVE			'0'
#define EEPROMDISK_DIV			    ":/"
#define EEPROMDISK_DIR				"KR20P"
#define EEPROMDISK_PATH				"0:/KR20P"
#define EEPROMDISK_MAX_DATA_SIZE	(240*1024)	//fat area + name area

#define TLCD_DrawPicture		uart_putstring
#define TLCD_CtrlBackLight	uart_putstring
#define TLCD_CtrlSound		uart_putstring
#define TLCD_DrawText		uart_putstring


/* Private typedef -----------------------------------------------------------*/
typedef __packed struct _tagMYDATE_FMT{
	uint16_t year;
	uint8_t month;
	uint8_t day;
}MYDATE_FMT;

typedef __packed struct _tagPATIENT_INFO{
	//char tag[2];	//"pi"
	//uint16_t id;
	uint8_t name[8];	//initial
	MYDATE_FMT birthday;	//pjg++180528++
	uint16_t pwd;	//pjg++180528++ password(max:9999)
	MYDATE_FMT createDate;		//year(255)/month(12)/day(31)
	uint8_t createTime[3];
	MYDATE_FMT date;				//year(255)/month(12)/day(31)
	uint8_t time[3];
	uint32_t totalTime;
	uint32_t totalRepeat;
	//uint32_t *next;	//next addr
} PATIENT_INFO;
		
enum _UI_WINDOW_MODE {
	UI_WND_MODE_DESKTOP,
	UI_WND_MODE_PARENT,
	UI_WND_MODE_INIT,
	UI_WND_MODE_LOADING,
	UI_WND_MODE_USER,
	UI_WND_MODE_HOME,
	UI_WND_MODE_ANGLE_MEA		= 10,
	UI_WND_MODE_ANGLE_MEA_COMPLETE,
	UI_WND_MODE_ANGLE_MEA_REPORT,
	UI_WND_MODE_ANGLE_MEA_LOW_POS,
	UI_WND_MODE_ANGLE_SET_EX,
	UI_WND_MODE_ANGLE_SET_FL,
	UI_WND_MODE_SPEED			= 20,
	UI_WND_MODE_RUN				= 30,
	UI_WND_MODE_RUN_COMPLETE,
	UI_WND_MODE_RUN_PI_INIT,
	UI_WND_MODE_RUN_MODE_VIB_REQ,
	UI_WND_MODE_RUN_MODE_LP_REQ,
	UI_WND_MODE_RUN_MODE_PRO_REQ,
	UI_WND_MODE_SETUP			= 40,
	UI_WND_MODE_SETUP2,
	UI_WND_MODE_SETUP3,
	UI_WND_MODE_ADV_SET,
	UI_WND_MODE_VERSION,
	UI_WND_MODE_SAVING			= 50,
	UI_WND_MODE_DEL_DATA,
	UI_WND_MODE_RESET,
	UI_WND_MODE_DELETING,
	UI_WND_MODE_DEL,
	UI_WND_MODE_DATA_FULL		= 60,
	UI_WND_MODE_DATA_FULL_WARNING,
	UI_WND_MODE_DATA_FULL_CHK,
	UI_WND_MODE_PWD_FORGET		= 70,
	UI_WND_MODE_LOGIN,
	UI_WND_MODE_PRODUCT			= 80,
	UI_WND_MODE_CALIBRATION,
	UI_WND_MODE_PROGRESS		= 90,
	UI_WND_MODE_PAUSE,
	UI_WND_MODE_VIBRATION,
	UI_WND_MODE_SENSE,
	UI_WND_MODE_HOME_POS,
	UI_WND_MODE_AGN_SENSE,
	UI_WND_MODE_AGN_BTN_STEP,
	UI_WND_MODE_AUTO_AGN_VALUE,
	UI_WND_MODE_USB_CHK,
	UI_WND_MODE_PI_USB,
	UI_WND_MODE_ID_CREATE		= 120,
	UI_WND_MODE_ID_RIGHT,
	//child
	UI_WND_MODE_CHILD_NONE		= 150,
	UI_WND_MODE_CHILD_MSG,	
	UI_WND_MODE_CHILD_WND,	
	UI_WND_MODE_MAX,
};

enum _UI_WINDOW_REDRAW {
	UI_WND_REDRAW_OFF,
	UI_WND_REDRAW_NEED,
	UI_WND_REDRAW_ED
};

enum _UI_WINDOW_ANGLE_MODE {
	UI_WND_ANGLE_MODE_EX,
	UI_WND_ANGLE_MODE_FL,
	UI_WND_ANGLE_MODE_MAX
};

enum _UI_DISP_NUM_POS {
	UI_DISP_NUM_1PLACE,
	UI_DISP_NUM_2PLACE	= 1*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_3PLACE	= 2*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_4PLACE	= 3*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_5PLACE	= 4*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_6PLACE	= 5*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_7PLACE	= 6*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_8PLACE	= 7*DISP_NUM_PLACE_VALUE,
	UI_DISP_NUM_MAX
};

enum _UI_DISP_NUM_FONT {
	UI_DISP_NUM_FNT5,
	UI_DISP_NUM_FNT7,
	UI_DISP_NUM_FNT9,
	UI_DISP_NUM_FNT_MAX
};

enum _UI_RUN_MODE {
	UI_RUN_MODE_LOAD,
	UI_RUN_MODE_STOP,
	UI_RUN_MODE_PLAY,
	UI_RUN_MODE_PAUSE,
	UI_RUN_MODE_HOME,
	UI_RUN_MODE_HOME_PAUSE, //pjg++190906
	UI_RUN_MODE_MAX
};

enum _TIMER_NUM {
	TIMER_ID_0,
	TIMER_ID_1,
	TIMER_ID_2,
	TIMER_ID_3,
	TIMER_ID_4,
	TIMER_ID_5,
	TIMER_ID_6,
	TIMER_ID_MAX,
};

enum _LOGIN_TYPE {
	LIT_GUEST,
	LIT_USER,
	LIT_ADMIN1,	// calib
	LIT_ADMIN2,
	LIT_SET_PWD,
	LIT_MAX
};

enum _FULL_TYPE {
	FT_NONE,
	FT_HOME,
	FT_USER,
	FT_PI,
	FT_RUN,
	FT_MEA,
	FT_MAX
};

enum _FULL_STATUS {
	FS_EMPTY,
	FS_90PER,
	FS_FULL,
	FS_MAX
};

enum _UI_FUN_RUN_MODE {
	UI_FRM_NONE,
	UI_FRM_BY_AUTO,
	UI_FRM_GOTO_HOME_POS,
	UI_FRM_NORMAL,
	UI_FRM_LIMITED_PAUSE,
	UI_FRM_PROGRESS,
	UI_FRM_VIBRATION,
	UI_FRM_MAX,	
};

enum _eSENSITIVITY_LEVEL{
	SL_LEVEL1, 	//20kg
	SL_LEVEL2,	//15kg
	SL_LEVEL3,	//10kg
	SL_LEVEL4,	//5kg
	SL_LEVEL_MAX	//0kg
};

enum _eMOTOR_SPEED{
	MS_SPEED1,
	MS_SPEED2,
	MS_SPEED3,
	//MS_SPEED4,
	MS_SPEED_MAX
};

enum _ePRODUCT_MASS_TYPE {
	PMT_1STEP,//0KG,
	PMT_2STEP,//4KG,
	PMT_3STEP,//9KG,
	PMT_4STEP,//14KG,
	PMT_5STEP,//19KG,
	PMT_STEP_MAX,
};

enum _ePASSWORD_TYPE {
	PWDT_PRODUCT,
	PWDT_ADMIN,
	PWDT_MAX
};

enum _eMODE_START_TYPE{
	MST_REHAB,
	MST_MEASURE,
	MST_MAX
};

enum _eAM_SENSITIVITY_LEVEL{
	AMSL_LEVEL1,
	AMSL_LEVEL2,
	AMSL_LEVEL3,
	AMSL_LEVEL_MAX,
};

enum eAM_DIRECTION{
	AMD_FLEXTION,
	AMD_EXTENTION,
	AMD_MAX,
};

enum eAM_VALUE_TYPE{
	AMVT_GAP,
	AMVT_02NOGRAVITY,	//0~100deg
	AMVT_NOGRAVITY2MAX,	//100~150deg
	AMVT_MAX
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char COMPANY_t[COMPANY_LENGTH] = {"HEXAR SYSTEMS"};
const char MODEL_t[MODEL_LENGTH] = {"KR20P"};
//v1.1.0 : hw v2.1
//v1.2.0 : hw v1.8 product 
const uint8_t FW_VER[] = {'1', '2', '2'};//v1.0.Korea Gxx
const uint8_t HW_VER[] = {'1', '8'};									
const MYDATE_FMT CREATE_DATE = {2017, 3, 24};		//2017y, 3m, 24d
const uint8_t CREATE_TIME[3] = {16, 31, 55};		//hour, min, sec

const uint8_t DoubleBufStartCmd[] = {"<D\r"};
const uint8_t DoubleBufEndCmd[] = {"!D\r"};

//const uint16_t SensGap[MS_SPEED_MAX][SL_LEVEL_MAX] = {
//		{16, 91, 126, 157},
//		{24, 116, 176, 223},
//		{32, 139, 213, 283},
//		//{55, 46, 47, 26}
//};

const uint16_t SensDefault[SL_LEVEL_MAX][MS_SPEED_MAX] = {
		{520, 700, 850},	//20kg
		//{540, 640, 800},
		{530, 670, 750},	//15kg
		//{440, 510, 590},
		{470, 510, 640},	//10kg
		//{410, 470, 520},
		{410, 440, 540}		//5kg
		//{400, 420, 420}
};

const uint8_t amSensDefault[AMD_MAX][AMSL_LEVEL_MAX][AMVT_MAX] = {
		//gap  offset(0~105deg), offset(100~135deg),
		//flextion(0~105deg):test deg : 60
		//flextion(100~135deg):test deg : 105
		{
			{4, 0, 13},
			{3, 0, 9}, 
			{2, 0, 8},	
		},
		//extension(0~110deg):test deg : 60
		//extension(100~135deg):test deg : 
		{
			{4, 30, 30},
			{3, 19, 19}, 
			{2, 8, 8},	
		},
};
		
const uint32_t password[PWDT_MAX] = {
	22608, //[9] = {1,2,3,4,5,6,7,8,9}; //product mode
	80622 //hexar make date
};
//const uint8_t AngPicBlue[] = {"i ab000.png,166,70\r"};
//const uint8_t AngPicIndigo[] = {"i ai000.png,166,70\r"};
char CompileDate[20] = __DATE__;
char CompileTime[20] = __TIME__;

char TLCD_BLInfo[] = {"l 80\r"};
char BLInfoBuf[8];
char TLCD_SndInfo[] = {"sv 210\r"};
char SVInfoBuf[8];
uint8_t SndVolTbl[SND_LEVEL_NUM];
uint8_t BLVolTbl[BL_LEVEL_NUM];
uint16_t Motor_OverCurTbl[SL_LEVEL_MAX][MS_SPEED_MAX];//MOTOR_OC_LEVEL_NUM];

struct {
	uint8_t num;//[UI_WND_DEPTH];
	uint8_t prevNum; //pjg++190906
	uint8_t child_num;
	uint8_t child_prevNum;
	uint8_t redraw;
}UI_Wnd;

struct {
	char buf[DISP_NUM_DISP_SAME_TIME][DISP_NUM_BUF_SIZE];
	char blank_buf[DISP_NUM_DISP_SAME_TIME][DISP_BLANK_NUM_BUF_SIZE];
	char blankAll_buf[DISP_NUM_DISP_SAME_TIME][DISP_BLANK_ALL_NUM_BUF_SIZE];
	char preUnitNum[DISP_NUM_DISP_SAME_TIME];	//180320 blink problem 
	//small font buf
	char sbuf[DISP_NUM_SDISP_SAME_TIME][DISP_NUM_SBUF_SIZE];
	char sblank_buf[DISP_NUM_SDISP_SAME_TIME][DISP_BLANK_NUM_SBUF_SIZE];
	//uint8_t pos;
	char sblankAll_buf[DISP_NUM_SDISP_SAME_TIME][DISP_BLANK_NUM_SBUF_SIZE];
}dispNum;

struct _tagPATIENT_INFO_WND{ //»Ø¿⁄¡§∫∏ √¢¿« ∫Øºˆ ¿˙¿Â
	//uint16_t id;
	//PATIENT_INFO pi;//[PATIENT_NUM];
	//uint16_t num;          				 //?????
	uint16_t totalRepeat;				//?? ??????
	uint32_t totalTime;				//?? ?????©£?
}PInfoWnd;

struct _tagPATIENT_INFO_WND2{ //»Ø¿⁄¡§∫∏ √¢¿« ∫Øºˆ ¿˙¿Â
#ifdef PI_LOAD_V2
	uint32_t id;
#else
	uint16_t id;
#endif
	PATIENT_INFO pi;//[PATIENT_NUM];
	//uint16_t num;          				 //?????
	//uint16_t totalRepeat;				//?? ??????
	//uint32_t totalTime;				//?? ?????©£?
}PInfoWnd2;

struct _LOGIN_INFO{
	uint8_t type;	//guest, user, admin1, admin2
	uint8_t pwdBuf[10];	//login æœ»£
	uint8_t cnt; //input count
	uint8_t pwdLen;
	uint8_t trycnt;
}loginInfo;	//pjg++171123

struct _FULL_INFO{
	uint8_t type;
	uint8_t status;
	//uin8_t value;
}FullInfo;

typedef __packed struct _tagBASETUP_INFO{    //∞¢µµº¬∆√ √¢¿« ∫Øºˆ ¿˙¿Â
	short exAngle, flAngle;					//Extension(????) ????(flection) 
	//char mode;  //Ω≈¿¸∞˙ ±º∞Ó ∏µÂ ¿˙¿Â
}BASETUP_INFO;
BASETUP_INFO AngleWnd, AngleWndOld;

typedef __packed struct _tagBASETUP2_INFO{    //º”µµ/Ω√∞£ º¬∆√ √¢¿« ∫Øºˆ ¿˙¿Â
	uint16_t speed;     //º”µµ //smart use
	uint16_t time;      //Ω√∞£ //smart use
}BASETUP2_INFO;
BASETUP2_INFO SpdTmWnd, SpdTmWndOld;

uint16_t SpdTmWnd_bk_speed; //pjg++170530 ¿”Ω√∑Œ ∏∏µÎ.º”µµ∞° 1¿Œ∞ÊøÏ ≥ π´ ¥ æÓº≠ ¿”Ω√¿˚¿∏∑Œ ¿˚øÎ

struct {    //±∏µø √¢¿« ∫Øºˆ ¿˙¿Â
	uint8_t dir;        //∏≈Õ ¿ÃµøπÊ«‚
	uint8_t play;       //±∏µø ªÛ≈¬(play, playing, pause, stop
	uint16_t repeat;    //«ˆ¿Á ±∏µø«— »Ωºˆ
	uint16_t tick;      //«ˆ¿Á ±∏µø«œ¥¬ ≈∏¿Ã∏” ∫Øºˆ
	uint32_t time;      //«ˆ¿Á ±∏µø«œ¥¬ Ω√∞£
	int angle;          //«ˆ¿Á ±∏µøµ«∞Ì ¿÷¥¬ ∏≈Õ ∞¢µµ
	uint8_t complete;	//pjg++190908
}RunWnd;

//pjg++191028
typedef struct _tag_SAVE_ANGLE_MEA_PARAM {
	uint8_t gapBk;
	uint8_t upOffset; //when motor up current is big than down	
	float alpha;
}ANGLE_MEA_PARAM;			 

typedef __packed struct _tagSETUP_INFO{	//?®˙?(???)a?? ???? ????	
	/*
	uint8_t angle;					//??????? ????
	uint8_t speed;					//??????? ???
	uint8_t repeat;					//??????? ???
	uint8_t lsrpt_en;					//??????? ??? ????
	uint8_t lsrpt_en2;										  
	uint8_t stand_by;			   
	uint8_t sndGuid;				 
	*/
	uint8_t vol; // smart use
	uint8_t bright; // smart use
	//uint8_t angle;
	//uint8_t speed;
	//uint8_t repeat;
	//uint8_t lsrpt_en;
	//uint8_t sndGuid;
	//short hiAngle;
}SETUP_INFO;
SETUP_INFO Setup, SetupOld;

typedef __packed struct _tagSETUP2_INFO{
	uint8_t vol;
	uint8_t bright;	
	//short hiAngle;		//home in angle	
	//uint8_t sensitivity;
	//uint8_t led_en;
	//uint8_t pi_reset;
	uint8_t language;
	//uint8_t sndGuid2;	
	//uint8_t lsrpt_en2;
	//uint8_t stand_by;
}SETUP2_INFO;
SETUP2_INFO Setup2, Setup2Old;

typedef __packed struct _tagSETUP3_INFO{
	//uint8_t USB;
	//uint8_t Ver;			   
	uint8_t PDeg;	
	uint8_t PTm;	
	uint8_t ProChk;
	uint8_t LP;
	uint8_t LPChk;
	uint8_t VDeg;	
	uint8_t VCnt;
	uint8_t VibChk;
	//uint8_t Sens;
	uint8_t sensitivity;
	short IPos;
	uint8_t loginType;	//0: input id, 1: load latest id
	//uint8_t fullType;
	//uint8_t AngChkSens;
	uint8_t AngBtnStep;
	uint8_t AutoAngValue;
	uint8_t amSens;	 //pjg++191217
	ANGLE_MEA_PARAM amp;
	uint32_t writeTime;
}SETUP3_INFO;
SETUP3_INFO Setup3, Setup3Old;

typedef struct _tagFLASH_SAVE_PI_PARAM_HEADER {
	char tag[2];	// "pi"
	uint32_t writeTime;
	uint32_t nextNode;
	uint32_t replaceNode;
	char ver[4];
	MYDATE_FMT date;
	char time[3];
	uint16_t idFrom;
	uint16_t idTo;
}PI_PARAM_HEADER;

typedef __packed struct _tagFLASH_SAVE_PI_PARAM {
	//uint16_t id;
	uint8_t id;
	uint8_t ver;	//0x10 : v1.0
	uint32_t writeTime;
	//uint16_t paramSize;	//pjg++171123
	uint8_t paramSize;	//pjg++171123
	uint16_t nextAddr;	//pjg++180528 addr to save exercise info
	PATIENT_INFO pi;
	short exAngle;
	short flAngle;
	//uint16_t speed;
	uint8_t speed;
	//uint16_t time;
	uint8_t time;
	//uint8_t angle;
	//uint8_t repeat;
	uint8_t vol;
	uint8_t bright;
	//uint8_t lsrAngle;		// lsr(limit section repeat
	//uint8_t lsrSpeed;		//lsr
	//uint8_t lsrRepeat;		//lsr
	//uint8_t led_en;	
	//uint8_t hiAngle;
	//uint8_t stand_by;			   
	//uint8_t sndGuid;
	//uint8_t sndGuid2;	
	//uint8_t lsrpt_en;
	//uint8_t lsrpt_en2;		   
	uint8_t language;
	uint8_t PDeg;
	uint8_t PTm;
	uint8_t ProChk;
	uint8_t LP;
	uint8_t LPChk;
	uint8_t VDeg;
	uint8_t VCnt;
	uint8_t VibChk;
	//uint8_t Sens;
	uint8_t sensitivity;
	uint8_t IPos;
}PI_PARAM;

//pjg++180221
typedef __packed struct _tag_SAVE_PRODUCT_PARAM {
	uint16_t Cal_MaxCur[PMT_STEP_MAX][MS_SPEED_MAX];
}PRODUCT_PARAM;			 

typedef __packed struct _tagFLASH_SAVE_SYSTEM_INFO {
	char company[COMPANY_LENGTH];
	char model[MODEL_LENGTH];
	char ver[4];
	MYDATE_FMT createDate;
	char createTime[3];
	MYDATE_FMT modifyDate;
	char modifyTime[3];
	uint32_t writeTime;	//flash re-write times(max 10,000)
	uint32_t nextNode;
	uint32_t replaceNode;
	//BASETUP_INFO AngleWnd;
	//BASETUP2_INFO SpdTmWnd;
	//SETUP_INFO setup;
	SETUP2_INFO setup2;
	SETUP3_INFO setup3;
	PATIENT_INFO pi;
}SYSTEM_INFO;

typedef __packed struct _tagFLASH_SAVE_SYSTEM_BASE_INFO {
	char company[COMPANY_LENGTH];
	char model[MODEL_LENGTH];
	char ver[4];
	uint32_t serialnum; //pjg++180528
	uint32_t adminPwd; //pjg++180615
	MYDATE_FMT createDate;
	char createTime[3];
	MYDATE_FMT modifyDate;
	char modifyTime[3];
	uint32_t writeTime;	//flash re-write times(max 10,000)
	uint32_t nextNode;
	uint32_t replaceNode;
	PRODUCT_PARAM product; //pjg++180221
}SYSTEM_BASE_INFO;

typedef __packed struct _tagSAVE_EXERCISE_INFO {
	uint8_t time;	//minute
	uint8_t exangle; //-5:0~140:145
	uint8_t flangle; //-5:0~140:145
	uint8_t exangle2; //-5:0~140:145, after exercise
	uint8_t flangle2; //-5:0~140:145, after exercise
	uint8_t speed;
	uint16_t count;
	uint16_t runTime;
}EXERCISE_INFO;

typedef __packed struct _tagUSB_DATA_HEADER {
	uint8_t h;
	uint8_t x;
	uint8_t ver;
	uint8_t key;
}USB_DATA_HEADER;

typedef __packed struct _tagUSB_DATA_FMT {
	uint8_t k;
	uint8_t r;
	uint16_t size; // size of SAVE_EXERCISE_INFO_V2
	uint32_t id;
}USB_DATA_FMT;

typedef __packed struct _tagMEASURE_INFO {
	uint8_t exangle; //-5:0~140:145
	uint8_t flangle; //-5:0~140:145
}MEASURE_INFO;

typedef __packed struct _tagSAVE_MEASURE_MODE {
	uint8_t flg; //0: play mode, 1:measure mode
	MEASURE_INFO mi[3];
	uint16_t runTime;
}SAVE_MEASURE_MODE;

typedef __packed struct _tagSAVE_PLAY_MODE {
	uint8_t flg; //0: play mode, 1:measure mode
	MEASURE_INFO mi;
	uint8_t exangle; //-5:0~140:145
	uint8_t flangle; //-5:0~140:145
	uint8_t speed;
	uint8_t time;	//minute
	uint16_t count;
	uint8_t plv; //progressive, limited pause, vibration
	uint16_t runTime;
}SAVE_PLAY_MODE;

typedef __packed union _tagSAVE_EXERCISE_INFO_V2 {
	uint8_t flg; //0: play mode, 1:measure mode
	SAVE_MEASURE_MODE smm;
	SAVE_PLAY_MODE spm;
}SAVE_EXERCISE_INFO_V2;
SAVE_EXERCISE_INFO_V2 SaveExeInfoV2;

UI_TIME UI_Time;

struct {
	uint32_t LastDataAddr;
	uint32_t SaveAddr;
	uint32_t writeTime;
	uint32_t totalsize;
	uint32_t freesize;
}EEPROM;

typedef void (*FnUI_FunRunMode)(void);
FnUI_FunRunMode  fnUi_FunRunMode;

//uint8_t Setup_ChkDataSave;		//check whether setup data save

struct {
	uint8_t PrePressedNum;
	uint8_t PrePressedPageNum;
	uint8_t pageNum;
	uint8_t Pi_Num;
}PiChange_Wnd;

struct {
	uint16_t cnt;
	uint8_t motorSpeed;
	uint16_t maxCurrent[PMT_STEP_MAX][MS_SPEED_MAX];
	int16_t deg[PMT_STEP_MAX][MS_SPEED_MAX];
	uint32_t curCurrent;
	uint32_t oldCurrent;
	uint8_t massStep;
}Product_Calib;				  

//const uint8_t AngPicBlue[] = {"i ab000.png,166,70\r"};
//const uint8_t AngPicIndigo[] = {"i ai000.png,166,70\r"};
uint8_t AngPicBlueBuf[20] = {"i ab000.png,168,74\r"};
uint8_t AngPicIndigoBuf[20] = {"i ai000.png,168,74\r"};
uint32_t loading=0;
//uint16_t longKeyEventCnt = 0;										   
struct {
	uint32_t addr;
	uint16_t dataCnt; //saved data count of user id
}Exercise;
struct {
	uint32_t sn;
	uint32_t adminPwd;
	uint8_t meaCnt; //measure count
	uint32_t writetime;
}SysInfo;

uint32_t udi[3];
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
	//uint8_t newGap;
	//short y;
	short temp;
	//short flex;
	//short ext;
	uint8_t sameCnt;
	short angle;
	uint8_t mode;
}AngChk, *SensChk;
#ifdef USE_FUN_REMOTE
UI_REMOTE UI_Remote;
#endif

struct {
	uint8_t curDisp; //use in remote
	uint32_t copypos; //use in user data copy
	uint8_t usbcopypos; //use in user data copy
	uint8_t draw;
	int temp16;
	int temp162;
}Option;

uint8_t amSensitivity[AMD_MAX][AMSL_LEVEL_MAX][AMVT_MAX];

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
struct {
	uint16_t curBuf[29];
	uint16_t cnt;
	uint8_t fStart;
	uint8_t fSave;
}QCLife;
#endif

extern uint16_t	MOTOR_STATE;
extern uint16_t  Current_Angle, Min_Angle, Max_Angle, Total_Counter;
extern uint8_t   Home_sensor, Error, Run_mode, Home_mode, Timer_sec, Home_flag;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim7;
//extern uint32_t	ADC_rms;
extern char CommonBuf[FLASH_PAGE_SIZE16*2];
//extern    uint32_t  Motor_Current, Over_Current;
//extern    int32_t   Current_Error;	//smart++170616

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void UI_SetWindowMode(uint8_t mode);
void UI_InitVariable(void);
void UI_PopupMemoryWarning_Create(void);
void UI_PopupDataFull_Create(void);
void UI_PopupDataFullCheck_Create(void);
void UI_Login_Create(void);
void UI_Product_Create(void);
void UI_Calibration_Create(void);
void UI_SystemInit_Create(void);
void UI_Loading_Create(void);
void UI_Home_Create(void);
//void UI_Angle_Create(void);
void UI_SpeedTime_Create(void);
void UI_PiLoad_Create(void);
void UI_PiReset_Create(void);
#ifdef PI_LOAD_V1
void UI_PiLoad2_Create(void);
void UI_PiReset2_Create(void);
void UI_PiLoad3_Create(void);
void UI_PiReset3_Create(void);
void UI_PiLoad4_Create(void);
void UI_PiReset4_Create(void);
#endif
void UI_Run_Create(void);
void UI_Setup_Create(void);
void UI_Progress_Create(void);
void UI_Pause_Create(void);
void UI_Vibration_Create(void);
//void UI_SensHp_Create(void);
void UI_PopupUSBCheck_Create(void);
void UI_PiUSB_Create(void);
void UI_PopupSaving_Create(void);
void UI_VersionInfo_Create(void);
//void UI_PushReset_Create(void);
//void UI_PopupDelData_Create(void);
void UI_PiReset_Create(void);
//void UI_Deleting_Create(void);
void UI_Setup_SetBrightness(uint8_t value);
void UI_Setup_SetSoundVolume(uint8_t value);
void UI_Run_Process(void); //pjg++170529
void UI_PopupRunComplete_Create(void);
//void UI_RunPIInit_Create(void);
//void UI_Play_Proecss(void);
void UI_ProcessNull(void); //pjg++170529
//void UI_Run_SetMotorOverCurrent(uint32_t current);
//uint32_t UI_Run_GetMotorOverCurrent(void);
//void UI_Run_PopupMotorOverload(uint32_t level);
//uint32_t UI_Run_GetNowMotorCurrent(void);
void UI_Run_OnKeyLong(uint32_t nChar);
void UI_Run_OnKeyUp(uint32_t nChar);
void UI_AniProgress(uint32_t pos);
void UI_AniProgress2(uint32_t pos);
void UI_SetFunRunMode(uint8_t mode);
void UI_Product_Process_Calibration_GoToStartPos(void);
#ifdef PI_LOAD_V2
void UI_AngleMeasure_OnBnClickedBtnPause(void);
void UI_AngleMeasure_Create(void);
#endif
void UI_AngleMeaSens_Create();
void UI_User_Create(void);
void UI_Sens_Create(void);
void UI_AngleBtnStep_Create(void);
void UI_HomePos_Create(void);
void UI_PopupIDCreate_Create(void);
void UI_PopupIDRight_Create(void);
void UI_AutoAngleValue_Create(void);
void UI_PopupResetReq_Create(void);
void UI_PopupDelReq_Create(void);


//extern void MotorDrv_Init(void);
extern void uart_putchar3(char data);
extern void uart_putstring3(char *pbuf);

//
//////////////////////////////// UI //////////////////////////////////////
#ifdef USE_FUN_REMOTE
void UI_Remote_Init(void)
{
	UI_Remote.head = UI_Remote.tail = 0;
}

void UI_Remote_PutData(uint8_t buf)
{
	UI_Remote.buf[UI_Remote.head] = buf;
	UI_Remote.head++;
	if (UI_Remote.head >= 10) UI_Remote.head = 0;
	if (buf == 0x0d) {
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
	}
	//else uart_putchar3(buf);

	UI_Remote_Process();
}

void UI_Remote_Process(void)
{
	uint8_t temp;
	float ftemp;
	//uint16_t temp16;
	char buf[11];
	
	switch (UI_Remote.buf[UI_Remote.tail]) {
	case '?':
	case 'h':
	//case 'H':
		uart_putstring3("a:alpha\r\n");
		uart_putstring3("g:gap\r\n");
		uart_putstring3("o:up offset\r\n");
		uart_putstring3("c:current disp\r\n");
		uart_putstring3("s:s-alpha\r\n");
		uart_putstring3("d:s-gap\r\n");
		uart_putchar3('>');
		break;
	case 'a':
	//case 'A':
		UI_Remote.cmd = 'a';
		uart_putstring3("set alpha:");
		ConvertFloat2String(AngChk.alpha, buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
		break;		
	case 'g':
	//case 'G':
		UI_Remote.cmd = 'g';
		uart_putstring3("set gap:");
		ConvertULong2String((int32_t)AngChk.gap, (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
		break;		
	case 'o':
	//case 'O':
		UI_Remote.cmd = 'o';
		uart_putstring3("set offset:");
		ConvertULong2String((int32_t)AngChk.upOffset, (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
		break;		
	case 's':
		UI_Remote.cmd = 's';
		uart_putstring3("set s-alpha:");
		ConvertFloat2String(MotorDrv_SensChk_GetAlpha(), buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
		break;		
	case 'd':
		UI_Remote.cmd = 'd';
		uart_putstring3("set s-gap:");
		ConvertULong2String((int32_t)MotorDrv_SensChk_GetGap(), (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');
		break;		
	case '.': //increse data
		if (UI_Remote.cmd == 'a') {
			AngChk.alpha += 0.01;
			ConvertFloat2String(AngChk.alpha, buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'g') {
			AngChk.gap += 1;	
			AngChk.gapBk = AngChk.gap;
			ConvertULong2String((int32_t)AngChk.gap, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'o') {
			AngChk.upOffset += 1;
			ConvertULong2String((int32_t)AngChk.upOffset, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'c') {
			Option.curDisp = 1;
		}
		else if (UI_Remote.cmd == 's') {
			ftemp = MotorDrv_SensChk_GetAlpha();
			ftemp += 0.01;
			ConvertFloat2String(ftemp, buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'd') {
			temp = MotorDrv_SensChk_GetGap();
			temp += 1;
			ConvertULong2String((int32_t)temp, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		break;
	case ',': //decrese data
		if (UI_Remote.cmd == 'a') {
			AngChk.alpha -= 0.01;
			if (AngChk.alpha < 0) AngChk.alpha = 0;
			ConvertFloat2String(AngChk.alpha, buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'g') {
			if (AngChk.gap) AngChk.gap -= 1;
			AngChk.gapBk = AngChk.gap;
			ConvertULong2String((int32_t)AngChk.gap, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'o') {
			if (AngChk.upOffset) AngChk.upOffset -= 1;
			ConvertULong2String((int32_t)AngChk.upOffset, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'c') {
			if (Option.curDisp) Option.curDisp = 0;
			else Option.curDisp = 1;
		}
		else if (UI_Remote.cmd == 's') {
			ftemp = MotorDrv_SensChk_GetAlpha();
			ftemp -= 0.01;
			if (ftemp < 0) ftemp = 0;
			ConvertFloat2String(ftemp, buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		else if (UI_Remote.cmd == 'd') {
			temp = MotorDrv_SensChk_GetGap();
			temp -= 1;
			if (temp) temp -= 1;
			ConvertULong2String((int32_t)temp, (uint8_t *)buf, 11);
			uart_putstring3(buf);
			uart_putchar3(' ');
		}
		break;
	case 'c':
		UI_Remote.cmd = 'c';
		if (Option.curDisp) uart_putstring3("on");
		else uart_putstring3("off");
		uart_putchar3('\r');
		uart_putchar3('\n');
		uart_putchar3('>');	
		break;
	}
	UI_Remote.tail++;
	if (UI_Remote.tail >= 10) UI_Remote.tail = 0;
}
#endif

uint8_t UI_DisplayErrorCode(uint16_t code)
{	
	char buf[18];
	uint8_t errcode;
	//int i;
																									 						 
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = 'r';
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'p';
	buf[6] = 'n';
	buf[7] = 'g';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '7';//9';		   
	buf[11] = '5';
	buf[12] = ',';
	buf[13] = '1';
	buf[14] = '0';//1';
	buf[15] = '3';
	buf[16] = '\r';
	buf[17] = 0;
	
	errcode = (uint8_t)(code&0x00ff);
	TLCD_DrawText(pImgNameInfo[IMG_ERROR][Setup2.language]);
	
	buf[3] = '0' + errcode/100;
	TLCD_DrawText(buf); //1st 
	buf[3] = '0' + (errcode%100)/10;
	buf[9] = '2';//3';	//x coord
	buf[10] = '9';//0';
	buf[11] = '0';
	TLCD_DrawText(buf); //2st 
	buf[3] = '0' + errcode%10;
	buf[9] = '3';	//x coord
	buf[10] = '0';//2';
	buf[11] = '5';
	TLCD_DrawText(buf); //3rd 

	return ((uint8_t)(code>>8));
}

void UI_Timer_Process(void)
{
	if (UI_Time.tmp_ms >= 10) UI_Time.tmp_ms -= 10;
	else UI_Time.tmp_ms = 0;
	if (UI_Time.tmp2_ms >= 10) UI_Time.tmp2_ms -= 10;
	else UI_Time.tmp2_ms = 0;
	if (UI_Time.tmp3_ms >= 10) UI_Time.tmp3_ms -= 10;
	else UI_Time.tmp3_ms = 0;
	if (UI_Time.tmp4_ms >= 10) UI_Time.tmp4_ms -= 10;
	else UI_Time.tmp4_ms = 0;
	if (UI_Time.tmp5_ms >= 10) UI_Time.tmp5_ms -= 10;
	else UI_Time.tmp5_ms = 0;
	UI_Time.time.ms += 10;
	if (UI_Time.time.ms > 999) {
  		UI_Time.time.ms = 0;
		UI_Time.tmp_sec++;
		UI_Time.time.sec++;
		if (UI_Time.time.sec > 59) {
			UI_Time.time.sec = 0;
			UI_Time.time.min++;
			if (UI_Time.time.min > 59) {
				UI_Time.time.min = 0;
				UI_Time.time.hour++;
			}
		}
	}
}

uint16_t GetNumFromString(char *buf, char _char, char findpos)
{
	int i, j;
	int flag;
	int pos;
	char temp[3];
	uint16_t data;

	i = 0;
	j = 0;
	pos = 0;
	temp[0]=0; temp[1]=0; temp[2]=0;
    flag = 0;
	
	while (buf[i] != '\r' || buf[i] == 0) {
		if (buf[i] == _char) {
			flag = 1;
			pos++;
		}
		else if (flag == 1 && pos == findpos) {
			if (j > 2) return 0;
			temp[j] = buf[i];
			j++;
		}
		i++;
		if (i > 100) return 0;
	}

	if (temp[0] == 0) data = 0;
	else if (temp[1] == 0) {
		data = temp[0]-'0';
	}
	else if (temp[2] == 0) {
		data = (temp[0]-'0')*10 + (temp[1]-'0');
	}
	else {
		data = (temp[0]-'0')*100 + (temp[1]-'0')*10 + (temp[2]-'0');
	}
	return data;
}

//pjg++190919
char *MACRO2Buf(char *buf)
{
	return buf;
}

void ID2FileName(uint32_t id, uint8_t *buf)
{
	int i, j, k;
	uint32_t temp, temp2;

	temp = id;
	j = 0;
	buf[j++] = EEPROMDISK_DRIVE;
	buf[j++] = ':';
	buf[j++] = '/';
	buf[j++] = 'K';
	buf[j++] = 'R';
	buf[j++] = '2';
	buf[j++] = '0';
	buf[j++] = 'P';
	buf[j++] = '/';
	k = j;
	for (i = 0; i < 8; i++) {
		temp2 = Multiplier(10, 7-i);
		if (temp2 == 0) buf[k+i] = 0;
        else buf[k+i] = temp/temp2 + '0';
		temp %= temp2;
		j++;
	}
	buf[j++] = '.';
	buf[j++] = 'H'; //hexar
	buf[j++] = 'K'; //kr20p
	buf[j++] = 'E'; //exercise
	buf[j++] = 0;
}

void UI_LED_Control(uint8_t mode)
{
	/*if (Setup2.led_en != BST_CHECKED) {
		LEDDrv_SetParam(LS_RED, 0, 100, 100, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 0, 100, 100, 0xffff);
		LEDDrv_SetParam(LS_BLUE, 0, 100, 100, 0xffff);
		LEDDrv_SetOrder(0, LS_MAX, 0);
		return;
	}*/

	switch (mode) {
	case LM_NONE:
		LEDDrv_SetParam(LS_RED, 0, 100, 100, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 0, 100, 100, 0xffff);
		LEDDrv_SetParam(LS_BLUE, 0, 100, 100, 0xffff);
		break;
	case LM_BEFORE_BOOT:
		LEDDrv_SetOrder(0, LS_RED, 20); // pjg++170606
		//LEDDrv_SetParam(LS_RED, 1, 100, 0, 0xffff);
		LEDDrv_SetParam(LS_RED, 0, 10, 0, 0xffff); //pjg<>190724
		LEDDrv_SetOrder(1, LS_GREEN, 20);
		LEDDrv_SetParam(LS_GREEN, 1, 100, 0, 0xffff);
		LEDDrv_SetOrder(2, LS_BLUE, 20);
		LEDDrv_SetParam(LS_BLUE, 1, 100, 0, 0xffff);
		break;
	case LM_INIT:
		LEDDrv_SetParam(LS_BLUE, 1, 10,0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED, 1, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_RED, 10);
		LEDDrv_SetOrder(1, LS_GREEN, 10);
		LEDDrv_SetOrder(2, LS_BLUE, 10);
		break;
	case LM_HOME_IN:
		LEDDrv_SetParam(LS_BLUE, 0, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED, 1, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_RED, 0);
		break;
	case LM_RUN:
		LEDDrv_SetParam(LS_BLUE, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 0, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED,0, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_BLUE, 0);
		break;
	case LM_PAUSE:
		LEDDrv_SetParam(LS_BLUE, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED,0, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_GREEN, 0);
		break;
	case LM_STOP:
		LEDDrv_SetParam(LS_BLUE, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 0, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED, 1, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_RED, 0);
		break;
	case LM_STAND_BY:
		LEDDrv_SetParam(LS_BLUE, 0, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 1, 10, 0, 0xffff);
		LEDDrv_SetParam(LS_RED, 0, 10, 0, 0xffff);
		LEDDrv_SetOrder(0, LS_RED, 0);
		break;
	case LM_ERR_1:
		LEDDrv_SetParam(LS_BLUE, 0, 10, 10, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 0, 10, 10, 0xffff);
		LEDDrv_SetParam(LS_RED, 1, 10, 10, 0xffff);
		LEDDrv_SetOrder(0, LS_RED, 0);
		break;
	case LM_Complet:
		LEDDrv_SetParam(LS_BLUE, 1, 25, 25, 0xffff);
		LEDDrv_SetParam(LS_GREEN, 1, 25, 25, 0xffff);
		LEDDrv_SetParam(LS_RED,0, 50, 100, 0xffff);
		LEDDrv_SetOrder(0, LS_GREEN, 0);
		break;
	}
}

uint32_t UI_LoadParamFromFlash(char *buf)
{
	SYSTEM_INFO *pFSSysInfo;
	uint32_t addr;
	int i;

	addr = FLASH_SYSTEM_INFO_ADDR;
	for (i = 0; i < FLASH_USER_END_ADDR; i += FLASH_PAGE_SIZE16) {
		FlashDrv_Read(addr, buf, sizeof(SYSTEM_INFO));
		pFSSysInfo = (SYSTEM_INFO *)buf;
		if (pFSSysInfo->company[0] != 'H' ||
			pFSSysInfo->company[1] != 'E' ||
			pFSSysInfo->company[2] != 'X') {
			return 0;
		}
		else {
			if (pFSSysInfo->writeTime > FLASH_ENDURANCE) {
				if (pFSSysInfo->replaceNode == 0xffffffff || 
					pFSSysInfo->replaceNode == 0) return 0;
				else {
					addr = pFSSysInfo->replaceNode;
				}
			}
			else {
				break;
			}
		}
	}

	return addr;
}										 

int UI_SaveParamToFlash(char *buf)
{
	SYSTEM_INFO *pFSSysInfo;
	int i;
	uint32_t addr;

	addr = UI_LoadParamFromFlash(buf);\

	pFSSysInfo = (SYSTEM_INFO *)buf;
//	pFSSysInfo->AngleWnd.exAngle = AngleWnd.exAngle;
//	pFSSysInfo->AngleWnd.flAngle = AngleWnd.flAngle;
//	pFSSysInfo->SpdTmWnd.speed = SpdTmWnd.speed;
//	pFSSysInfo->SpdTmWnd.time = SpdTmWnd.time;
//	pFSSysInfo->setup3.PDeg = Setup3.PDeg;
//	pFSSysInfo->setup3.PTm = Setup3.PTm;
//	pFSSysInfo->setup3.ProChk = Setup3.ProChk;
//	pFSSysInfo->setup3.LP = Setup3.LP;
//	pFSSysInfo->setup3.LPChk = Setup3.LPChk;
//	pFSSysInfo->setup3.VDeg = Setup3.VDeg;
//	pFSSysInfo->setup3.VCnt = Setup3.VCnt;
//	pFSSysInfo->setup3.VibChk = Setup3.VibChk;
//	pFSSysInfo->setup3.sensitivity = Setup3.sensitivity;
//	pFSSysInfo->setup3.IPos = Setup3.IPos;
//	pFSSysInfo->setup2.vol = Setup2.vol;
//	pFSSysInfo->setup2.bright = Setup2.bright;
	//pFSSysInfo->setup.angle = Setup.angle;
	//pFSSysInfo->setup.repeat = Setup.repeat;
	//pFSSysInfo->setup.lsrpt_en = Setup.lsrpt_en;
	//pFSSysInfo->setup2.lsrpt_en2 = Setup2.lsrpt_en2;
	//pFSSysInfo->setup.sndGuid = Setup.sndGuid;
	//pFSSysInfo->setup2.sndGuid2 = Setup2.sndGuid2;										 										 
	//pFSSysInfo->setup2.led_en = Setup2.led_en;
	//pFSSysInfo->setup2.pi_reset = Setup2.pi_reset;
	//pFSSysInfo->setup2.language = Setup2.language;
	//pFSSysInfo->setup2.stand_by= Setup2.stand_by;
	
	

	for (i = 0; i < COMPANY_LENGTH; i++) pFSSysInfo->company[i] = COMPANY_t[i];
	for (i = 0; i < MODEL_LENGTH; i++) pFSSysInfo->model[i] = MODEL_t[i];
	for (i = 0; i < 3; i++) pFSSysInfo->ver[i] = FW_VER[i];
	pFSSysInfo->createDate.year = CREATE_DATE.year;
	pFSSysInfo->createDate.month = CREATE_DATE.month;
	pFSSysInfo->createDate.day = CREATE_DATE.day;
	for (i = 0; i < 3; i++) pFSSysInfo->createTime[i] = CREATE_TIME[i];
	pFSSysInfo->modifyTime[0] = 'a';
	pFSSysInfo->modifyTime[1] = 'b';
	pFSSysInfo->modifyTime[2] = 'c';
	
	pFSSysInfo->nextNode = 0;
	pFSSysInfo->replaceNode = 0;
	pFSSysInfo->replaceNode = 0x12345678;

	if (addr >= FLASH_SYSTEM_INFO_ADDR) {
		pFSSysInfo->writeTime++;
		//if (!FlashDrv_Write(addr, buf, sizeof(SYSTEM_INFO))) return 0;
	}
	else {
		pFSSysInfo->writeTime = 1;
		//i = 128;//sizeof(struct _tagFLASH_SAVE_SYSTEM_INFO);
		//if (!FlashDrv_Write(FLASH_SYSTEM_INFO_ADDR, 
					//buf, sizeof(SYSTEM_INFO))) return 0;
	}

	return 1;
}

uint32_t UI_LoadParamFromEEPROM(char *buf)
{
	SYSTEM_INFO *pFSSysInfo;
	//FILINFO fno;
	//uint32_t addr;
	//int i;

#ifdef USE_FUN_EEPROM_DISK
	if (!EEPROMDisk_Link()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
		return 0;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return 0;
	}

	if (!EEPROMDisk_OpenRead("param.dat")) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		//API_SetErrorCode(EC_EEP_OPEN, EDT_DISP);
		return 0;
	}
	if (!EEPROMDisk_Read((uint8_t *)buf, sizeof(SYSTEM_INFO))) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_READ, EDT_DISP);
		return 0;
	}
	EEPROMDisk_Close();
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();
	
#else
	//addr = 0;
	EEPROMDrv_Read(0, 0, (uint8_t *)buf, sizeof(SYSTEM_INFO));
#endif
	pFSSysInfo = (SYSTEM_INFO *)buf;
	if (pFSSysInfo->company[0] != 'H' ||
		pFSSysInfo->company[1] != 'E' ||
		pFSSysInfo->company[2] != 'X') {
		return 0;
	}

	if (pFSSysInfo->ver[0] != FW_VER[0] ||
		pFSSysInfo->ver[1] != FW_VER[1] ||
		pFSSysInfo->ver[2] != FW_VER[2]) {
		return 0;
	}

	return 1;
}										 

//SYSTEM_BASE_INFO *pFSSysInfo;

//pjg++180222
//	SYSTEM_BASE_INFO *pFSSysInfo;
uint32_t UI_LoadSystemInfoFromFlash(char *buf)
{
	uint32_t addr;
	int i;
	SYSTEM_BASE_INFO *pFSSysInfo;

	addr = FLASH_SYSTEM_INFO_ADDR;
	i = 0;
	//for (i = 0; i < FLASH_USER_END_ADDR; i += FLASH_PAGE_SIZE16) {
		FlashDrv_Read(addr, buf, sizeof(SYSTEM_BASE_INFO));
		pFSSysInfo = (SYSTEM_BASE_INFO *)buf;
		if (pFSSysInfo->company[0] != 'H' ||
			pFSSysInfo->company[1] != 'E' ||
			pFSSysInfo->company[2] != 'X' || 
			(pFSSysInfo->ver[0] != FW_VER[0] || 
			pFSSysInfo->ver[1] != FW_VER[1] || 
			pFSSysInfo->ver[2] != FW_VER[2] )) {
			if (i == 0) return 0;
			//break;
		}
		else {
			if (pFSSysInfo->writeTime > FLASH_ENDURANCE) {
				//if (pFSSysInfo->replaceNode == 0xffffffff || 
				//	pFSSysInfo->replaceNode == 0) return 0;
				//if (pFSSysInfo->replaceNode < FLASH_SYSTEM_INFO_ADDR ||
				//	pFSSysInfo->replaceNode > FLASH_USER_END_ADDR) {
					if (i == 0) return 0;
					//break;
				//}
				//else {
				//	addr = pFSSysInfo->replaceNode;
				//}
			}
			else {
				//break;
			}
		}
	//}

	return addr;
}	
							   
int UI_SaveParamToEEPROM(char *buf)
{
	SYSTEM_INFO *pFSSysInfo;
	int i;
	//uint32_t addr;
	//uint8_t tmpbuf[5];
	FILINFO fno;

	pFSSysInfo = (SYSTEM_INFO *)buf;		
	//addr = UI_LoadParamFromEEPROM(buf);
	//pFSSysInfo = (SYSTEM_INFO *)&buf[0];		
	for (i = 0; i < COMPANY_LENGTH; i++) pFSSysInfo->company[i] = COMPANY_t[i];
	for (i = 0; i < MODEL_LENGTH; i++) pFSSysInfo->model[i] = MODEL_t[i];
	for (i = 0; i < 3; i++) pFSSysInfo->ver[i] = FW_VER[i];

	if (Setup3.writeTime) {
		pFSSysInfo->modifyTime[0] = UI_Time.time.hour;
		pFSSysInfo->modifyTime[1] = UI_Time.time.min;
		pFSSysInfo->modifyTime[2] = UI_Time.time.sec;
		pFSSysInfo->modifyDate.year = 0;
		pFSSysInfo->modifyDate.month = 0;
		pFSSysInfo->modifyDate.day = 0;
		
		pFSSysInfo->nextNode = 0;
		//pFSSysInfo->replaceNode = 0;
		//pFSSysInfo->replaceNode = FLASH_SYSTEM_INFO_ADDR;//0x12345678;
		pFSSysInfo->setup3.writeTime = Setup3.writeTime+1;
		pFSSysInfo->pi.pwd = PInfoWnd2.pi.pwd;
		//pFSSysInfo->writeTime++;
	}
	else {
		pFSSysInfo->createDate.year = CREATE_DATE.year;
		pFSSysInfo->createDate.month = CREATE_DATE.month;
		pFSSysInfo->createDate.day = CREATE_DATE.day;
		for (i = 0; i < 3; i++) pFSSysInfo->createTime[i] = CREATE_TIME[i];
		pFSSysInfo->modifyTime[0] = 0;
		pFSSysInfo->modifyTime[1] = 0;
		pFSSysInfo->modifyTime[2] = 0;
		pFSSysInfo->modifyDate.year = 0;
		pFSSysInfo->modifyDate.month = 0;
		pFSSysInfo->modifyDate.day = 0;
		pFSSysInfo->setup3.writeTime = 1;
		//pFSSysInfo->writeTime = 1;
		//pFSSysInfo->replaceNode = FLASH_SYSTEM_INFO_ADDR;//0x12345678;
	}

//	pFSSysInfo->AngleWnd.exAngle = AngleWnd.exAngle;
//	pFSSysInfo->AngleWnd.flAngle = AngleWnd.flAngle;
//	pFSSysInfo->SpdTmWnd.speed = SpdTmWnd.speed;
//	pFSSysInfo->SpdTmWnd.time = SpdTmWnd.time;
//	pFSSysInfo->setup3.PDeg = Setup3.PDeg;
//	pFSSysInfo->setup3.PTm = Setup3.PTm;
///	pFSSysInfo->setup3.ProChk = Setup3.ProChk;
//	pFSSysInfo->setup3.LP = Setup3.LP;
//	pFSSysInfo->setup3.LPChk = Setup3.LPChk;
//	pFSSysInfo->setup3.VDeg = Setup3.VDeg;
//	pFSSysInfo->setup3.VCnt = Setup3.VCnt;
//	pFSSysInfo->setup3.VibChk = Setup3.VibChk;
	pFSSysInfo->setup3.sensitivity = Setup3.sensitivity;
	pFSSysInfo->setup3.IPos = Setup3.IPos;
	//pFSSysInfo->setup3.AngChkSens = Setup3.AngChkSens;
	pFSSysInfo->setup3.AngBtnStep = Setup3.AngBtnStep;
	pFSSysInfo->setup3.AutoAngValue = Setup3.AutoAngValue;
	//pFSSysInfo->setup3.writeTime = Setup3.writeTime+1;
	pFSSysInfo->setup2.vol = Setup2.vol;
	pFSSysInfo->setup2.bright = Setup2.bright;
	//pFSSysInfo->setup.angle = Setup.angle;	
	//pFSSysInfo->setup.repeat = Setup.repeat;	
	//pFSSysInfo->setup.lsrpt_en = Setup.lsrpt_en;
	//pFSSysInfo->setup2.lsrpt_en2 = Setup2.lsrpt_en2;
	//pFSSysInfo->setup.sndGuid = Setup.sndGuid;
	//pFSSysInfo->setup2.sndGuid2 = Setup2.sndGuid2;
	//pFSSysInfo->setup2.stand_by = Setup2.stand_by;										 										 
	//pFSSysInfo->setup2.led_en = Setup2.led_en;
	//pFSSysInfo->setup2.pi_reset = Setup2.pi_reset;
	pFSSysInfo->setup2.language = Setup2.language;
	pFSSysInfo->setup3.amp.gapBk = AngChk.gapBk;
	pFSSysInfo->setup3.amp.upOffset = AngChk.upOffset;
	pFSSysInfo->setup3.amp.alpha = AngChk.alpha;
	pFSSysInfo->setup3.amSens = Setup3.amSens;

#ifdef USE_FUN_EEPROM_DISK
	if (!EEPROMDisk_Link()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
		return 0;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return 0;
	}
	if (EEPROMDisk_stat("KR20P", &fno) != 1) {
		if (!EEPROMDisk_MkDir("KR20P")) {
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			API_SetErrorCode(EC_EEP_MKDIR, EDT_DISP_HALT);
			return 0;
		}
	}

	if (!EEPROMDisk_OpenWrite("param.dat")) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		API_SetErrorCode(EC_EEP_OPEN, EDT_DISP_HALT);
		return 0;
	}
	if (!EEPROMDisk_lseek(0)) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		API_SetErrorCode(EC_EEP_SEEK, EDT_DISP_HALT);
		return 0;
	}
	
	if (!EEPROMDisk_Write((uint8_t *)buf, sizeof(SYSTEM_INFO))) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		API_SetErrorCode(EC_EEP_WRITE, EDT_DISP_HALT);
		return 0;
	}
	EEPROMDisk_Close();
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();

#else
	for (i = 0 ; i < sizeof(SYSTEM_INFO); i++) {
		tmpbuf[0] = buf[i];
		if (!EEPROMDrv_WriteByte(0, i, tmpbuf[0])) return 0;
		//if (!EEPROMDrv_WriteByte(0, 1, (uint8_t *)buf, sizeof(SYSTEM_INFO))) return 0;
	}
#endif
	return 1;
}

//pjg++180222
int UI_SaveSystemInfoToFlash(char *buf)
{
	SYSTEM_BASE_INFO *pFSSysInfo;
	int i, j;
	//uint32_t addr;

	//addr = UI_LoadSystemInfoFromFlash(buf);
	//addr = FLASH_SYSTEM_INFO_ADDR;

	pFSSysInfo = (SYSTEM_BASE_INFO *)buf;

	for (i = 0; i < COMPANY_LENGTH; i++) pFSSysInfo->company[i] = COMPANY_t[i];
	for (i = 0; i < MODEL_LENGTH; i++) pFSSysInfo->model[i] = MODEL_t[i];
	for (i = 0; i < 3; i++) pFSSysInfo->ver[i] = FW_VER[i];
	
	if (SysInfo.writetime == 0) {
		pFSSysInfo->serialnum = udi[0]+udi[1]+udi[2];
		pFSSysInfo->adminPwd = password[PWDT_ADMIN];
		pFSSysInfo->createDate.year = CREATE_DATE.year;
		pFSSysInfo->createDate.month = CREATE_DATE.month;
		pFSSysInfo->createDate.day = CREATE_DATE.day;
		for (i = 0; i < 3; i++) pFSSysInfo->createTime[i] = CREATE_TIME[i];
		pFSSysInfo->modifyTime[0] = 'a';
		pFSSysInfo->modifyTime[1] = 'b';
		pFSSysInfo->modifyTime[2] = 'c';
		pFSSysInfo->nextNode = 0;
		//pFSSysInfo->replaceNode = 0;
		pFSSysInfo->replaceNode = 0x12345678;
		pFSSysInfo->writeTime = 1;
	}
	else {
		pFSSysInfo->adminPwd = SysInfo.adminPwd;
		pFSSysInfo->modifyDate.year = CREATE_DATE.year;
		pFSSysInfo->modifyDate.month = CREATE_DATE.month;
		pFSSysInfo->modifyDate.day = CREATE_DATE.day;
		pFSSysInfo->modifyTime[0] = (uint8_t)UI_Time.time.hour;
		pFSSysInfo->modifyTime[1] = UI_Time.time.min;
		pFSSysInfo->modifyTime[2] = UI_Time.time.sec;
		pFSSysInfo->writeTime = SysInfo.writetime++;
	}
	
	for (i = 0; i < PMT_STEP_MAX; i++) {
		for (j = 0; j < MS_SPEED_MAX; j++) {
			pFSSysInfo->product.Cal_MaxCur[i][j] = Product_Calib.maxCurrent[i][j];
		}
	}

	//pFSSysInfo->angmea.gapBk = AngChk.gapBk;
	//pFSSysInfo->angmea.upOffset = AngChk.upOffset;
	//pFSSysInfo->angmea.alpha = AngChk.alpha;
	//if (addr >= FLASH_SYSTEM_INFO_ADDR) {
	//	pFSSysInfo->writeTime++;
	//	if (!FlashDrv_Write(addr, buf, sizeof(SYSTEM_BASE_INFO))) return 0;
	//}
	//else {
		//i = 128;//sizeof(struct _tagFLASH_SAVE_SYSTEM_INFO);
		if (!FlashDrv_Write2(FLASH_SYSTEM_INFO_ADDR, 
					buf, sizeof(SYSTEM_BASE_INFO))) return 0;
	//}

	return 1;
}

int UI_LoadPIFromFlash(uint16_t id, char *buf)
{
	PI_PARAM *pFSPI;
	PI_PARAM_HEADER *pFSPIH;
	uint32_t addr;
	uint32_t sector;

	addr = FLASH_PATIENT_DATA_ADDR + id*FLASH_PI_DATA_SIZE;
	sector = GetSector(addr);
	//read header
	FlashDrv_Read(sector*FLASH_PAGE_SIZE16, buf, FLASH_PI_DATA_SIZE);
	pFSPIH = (PI_PARAM_HEADER *)buf;
	if (pFSPIH->tag[0] != 'p' || 
		pFSPIH->tag[1] != 'i' || pFSPIH->writeTime > FLASH_ENDURANCE) {
		addr = FLASH_PATIENT_DATA2_ADDR + id*FLASH_PI_DATA_SIZE;	
		sector = GetSector(addr);
		//read header
		FlashDrv_Read(sector*FLASH_PAGE_SIZE16, buf, FLASH_PI_DATA_SIZE);
		pFSPIH = (PI_PARAM_HEADER *)buf;
		if (pFSPIH->tag[0] != 'p' || 
			pFSPIH->tag[1] != 'i' || pFSPIH->writeTime > 9600) {
			return 0;
		}
	}
	
	//read data to flash
	FlashDrv_Read(addr, buf, FLASH_PI_DATA_SIZE);
	pFSPI = (PI_PARAM *)buf;
	if (pFSPI->id != id) {
		return 0;
	}

	//PInfoWnd.totalTime = pFSPI->totalTime;
       //PInfoWnd.totalRepeat = pFSPI->totalRepeat;
	AngleWnd.exAngle = pFSPI->exAngle;
	AngleWnd.flAngle = pFSPI->flAngle;
	SpdTmWnd.speed = pFSPI->speed;
	SpdTmWnd.time = pFSPI->time;
	Setup3.PDeg = pFSPI->PDeg;
	Setup3.PTm = pFSPI->PTm;
	Setup3.ProChk = pFSPI->ProChk;
	Setup3.LP = pFSPI->LP;								
	Setup3.LPChk = pFSPI->LPChk;
	Setup3.VDeg = pFSPI->VDeg;
	Setup3.VCnt = pFSPI->VCnt;
	Setup3.VibChk = pFSPI->VibChk;
	Setup3.sensitivity = pFSPI->sensitivity;
	Setup3.IPos = pFSPI->IPos;	
	Setup2.vol = pFSPI->vol;
	Setup2.bright = pFSPI->bright;
	//Setup2.sndGuid2 = pFSPI->sndGuid2;
	//Setup2.led_en = pFSPI->led_en;
	//Setup.angle = pFSPI->lsrAngle;
	//Setup.speed = pFSPI->lsrSpeed;
	//Setup.repeat = pFSPI->lsrRepeat;							  
	//Setup.sndGuid = pFSPI->sndGuid;
	//Setup2.sndGuid2 = pFSPI->sndGuid2;
	//Setup2.led_en = pFSPI->led_en;
	//Setup.lsrpt_en = pFSPI->lsrpt_en;
	//Setup2.lsrpt_en2 = pFSPI->lsrpt_en2;								
	//Setup2.language = pFSPI->language;	
								
	return 1;
}


//PI_PARAM FSPIbuf;
int UI_LoadPIFromEEPROM(uint16_t id, uint8_t *buf)
{
	PI_PARAM *pFSPI;
	//PI_PARAM_HEADER *pFSPIH;
	uint32_t addr;
	//int i;

	//addr = id*PATIENT_INFO_SIZE;
	/*
	//read data to flash
	for (i = 0; i < PATIENT_INFO_32_SIZE/PATIENT_INFO_SIZE; i++) {
		if (!EEPROMDrv_Read(0, addr, &read_id, 2)) return 0;
		if (read_id != id) {
			EEPROM_SaveAddr = addr;
			if (i == 0) {
				return 0;
			}
			else {
				break;
			}
		}
		else {
			EEPROM_LastDataAddr = addr;
		}
		addr += PATIENT_INFO_SIZE;
	}
	*/
	EEPROM.writeTime = 0;
	addr = sizeof(SYSTEM_INFO) +
			(sizeof(PI_PARAM) + sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM)*id;
	if (!EEPROMDrv_Read(0, addr, buf, sizeof(PI_PARAM))) {
	//if (!EEPROMDrv_Read(0, addr, (uint8_t *)&FSPIbuf, sizeof(PI_PARAM))) {
          //while(1);
          return 0;
        }
	pFSPI = (PI_PARAM *)buf;
	//pFSPI = &FSPIbuf;
	if (pFSPI->id != id || pFSPI->ver != PI_INFO_VER || pFSPI->paramSize != sizeof(PI_PARAM) || 
		pFSPI->bright > BL_LEVEL_NUM || pFSPI->speed < 1 || pFSPI->flAngle < 1 || pFSPI->time < 1 || 
       	pFSPI->sensitivity > 4 || pFSPI->sensitivity < 1) {
		PInfoWnd2.pi.totalRepeat = 0;
		PInfoWnd2.pi.totalTime = 0;
		Exercise.addr = 0; //pjg++180528
		PInfoWnd2.pi.pwd = 0xffff;
		return 0;
	}
	Exercise.addr = pFSPI->nextAddr; //pjg++180528
		
	EEPROM.writeTime = pFSPI->writeTime;
	//for (i = 0; i < 8; i++) {
	//	pFSPI->pi.name[i] = PInfoWnd2.pi.name[i];
	//}
	PInfoWnd2.pi.totalTime = pFSPI->pi.totalTime;
	PInfoWnd2.pi.totalRepeat = pFSPI->pi.totalRepeat;
	PInfoWnd2.pi.pwd = pFSPI->pi.pwd;
	AngleWnd.exAngle = pFSPI->exAngle;
	AngleWnd.flAngle = pFSPI->flAngle;
	SpdTmWnd.speed = pFSPI->speed;
	SpdTmWnd.time = pFSPI->time;
	Setup3.PDeg = pFSPI->PDeg;
	Setup3.PTm= pFSPI->PTm;
	Setup3.ProChk = pFSPI->ProChk;
	Setup3.LP = pFSPI->LP;								
	Setup3.LPChk = pFSPI->LPChk;
	Setup3.VDeg = pFSPI->VDeg;
	Setup3.VCnt = pFSPI->VCnt;
	Setup3.VibChk = pFSPI->VibChk;
	Setup3.sensitivity = pFSPI->sensitivity;
	Setup3.IPos = pFSPI->IPos;
	Setup2.vol = pFSPI->vol;
	Setup2.bright = pFSPI->bright;
	//Setup2.sndGuid2 = pFSPI->sndGuid2;  //ydy--180523
	//Setup2.led_en = pFSPI->led_en;
	//Setup.angle = pFSPI->lsrAngle;
	//Setup.repeat = pFSPI->lsrRepeat;
	//Setup2.stand_by = pFSPI->stand_by;
	//Setup.sndGuid = pFSPI->sndGuid;
	//Setup.lsrpt_en = pFSPI->lsrpt_en;
	//Setup2.lsrpt_en2 = pFSPI->lsrpt_en2;								
	//Setup2.language = pFSPI->language;
	//if (Setup2.language >= LT_MAX) Setup2.language = LT_KOR;

	UI_Setup_SetSoundVolume(SndVolTbl[Setup2.vol]); //pjg++180605
	UI_Setup_SetBrightness(BLVolTbl[Setup2.bright]); //pjg++180605
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]); //pjg++180605
	return 1;
}

int UI_SavePIToEEPROM(uint16_t id, uint8_t *buf)
{
	PI_PARAM *pFSPI;
	//PI_PARAM_HEADER *pFSPIH;
	uint32_t addr;
	uint32_t i;
	uint8_t tmpbuf[5];

	pFSPI = (PI_PARAM *)&buf[0];
	pFSPI->id = (uint8_t)id;
	pFSPI->ver = PI_INFO_VER;
#if 0 //test
	for (i = 0; i < sizeof(PI_PARAM); i++) {
		buf[4+i] = '0'+i%10;
		
	}
#else
	pFSPI->writeTime = EEPROM.writeTime + 1;
	pFSPI->paramSize = (uint8_t)sizeof(PI_PARAM);
	pFSPI->nextAddr = Exercise.addr; //pjg++180528
	for (i = 0; i < 8; i++) {
		pFSPI->pi.name[i] = PInfoWnd2.pi.name[i];
	}
	pFSPI->pi.birthday.year = PInfoWnd2.pi.birthday.year;
	pFSPI->pi.birthday.month = PInfoWnd2.pi.birthday.month;
	pFSPI->pi.birthday.day = PInfoWnd2.pi.birthday.day;
	//pFSPI->pi.name[0] = 'a';
	//pFSPI->pi.name[1] = 'b';
	//pFSPI->pi.name[2] = 'c';
	//pFSPI->pi.name[3] = 'd';
	//pFSPI->pi.name[4] = 'e';
	//pFSPI->pi.name[5] = 'f';
	//pFSPI->pi.name[6] = 'g';
	//pFSPI->pi.name[7] = 'h';
	if (!EEPROM.writeTime) {
		pFSPI->pi.createTime[0] = UI_Time.time.hour;
		pFSPI->pi.createTime[1] = UI_Time.time.min;
		pFSPI->pi.createTime[2] = UI_Time.time.sec;		
		pFSPI->pi.createDate.year = 2018;  //pjg++180604	
		pFSPI->pi.createDate.month = 6;		
		pFSPI->pi.createDate.day = 4;		
		//pFSPI->pi.pwd = 0; //pjg++180604
		EEPROM.writeTime = 1;
	}
	pFSPI->pi.pwd = PInfoWnd2.pi.pwd;
	pFSPI->pi.time[0] = UI_Time.time.hour;
	pFSPI->pi.time[1] = UI_Time.time.min;
	pFSPI->pi.time[2] = UI_Time.time.sec;
	pFSPI->pi.date.year = 2018;		
	pFSPI->pi.date.month = 6;		
	pFSPI->pi.date.day = 5;		
	pFSPI->pi.totalTime = PInfoWnd2.pi.totalTime;
 	pFSPI->pi.totalRepeat = PInfoWnd2.pi.totalRepeat;
	pFSPI->exAngle = AngleWnd.exAngle;
	pFSPI->flAngle = AngleWnd.flAngle;
	pFSPI->speed = (uint8_t)SpdTmWnd.speed;
	pFSPI->time = (uint8_t)SpdTmWnd.time;
	pFSPI->language = Setup2.language;
	pFSPI->PDeg = Setup3.PDeg;
	pFSPI->PTm = Setup3.PTm;
	pFSPI->ProChk = Setup3.ProChk;
	pFSPI->LP = Setup3.LP;								
	pFSPI->LPChk = Setup3.LPChk;
	pFSPI->VDeg = Setup3.VDeg; 
	pFSPI->VCnt = Setup3.VCnt;
	pFSPI->VibChk = Setup3.VibChk;
	pFSPI->sensitivity = Setup3.sensitivity;
	pFSPI->IPos = Setup3.IPos;
	pFSPI->vol = Setup2.vol;
	pFSPI->bright = Setup2.bright;
	//pFSPI->sndGuid2 = Setup2.sndGuid2;
	//pFSPI->led_en = Setup2.led_en;
	//pFSPI->lsrAngle = Setup.angle;
	//pFSPI->lsrRepeat = Setup.repeat;
	//pFSPI->stand_by = Setup2.stand_by;
	//pFSPI->sndGuid = Setup.sndGuid;
	//pFSPI->lsrpt_en = Setup.lsrpt_en;
	//pFSPI->lsrpt_en2 = Setup2.lsrpt_en2;
#endif
	//save data to flash
	addr = sizeof(SYSTEM_INFO) +
			(sizeof(PI_PARAM) + sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM)*id;
	for (i = 0; i < sizeof(PI_PARAM); i++) {
		tmpbuf[0] = buf[i];
		if (!EEPROMDrv_WriteByte(0, addr+i, tmpbuf[0])) {
		//if (!EEPROMDrv_WriteByte(0, addr, buf, sizeof(PI_PARAM)+20)) {
		//if (!EEPROMDrv_WritePage(0, addr, buf, sizeof(PI_PARAM))) {
			return 0;
		}
	}
	//for (i = 0; i < 100000; i++);  //æ¯¿∏∏È EEPROM ¿˙¿Â¿Ã æ»µ  
	//for (i = 0; i < 150000; i++);  //æ¯¿∏∏È EEPROM ¿˙¿Â¿Ã æ»µ  
	return 1;
}

int UI_DeletePIFromEEPROM(uint32_t id)
{
	uint8_t tmpbuf[25];

#ifdef USE_FUN_EEPROM_DISK
	FILINFO fno;

	if (!EEPROMDisk_Link()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
		return 0;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return 0;
	}
	ID2FileName(id, tmpbuf);
	if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		return 0;
	}
	if (!EEPROMDisk_Delete((char *)tmpbuf)) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		return 0;
	}
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();

#else
	uint8_t buf[2];
	uint32_t addr;
	uint32_t i;

	if (id > PATIENT_NUM) return 0;
	
	buf[0] = 0;
	buf[1] = 0;
	addr = sizeof(SYSTEM_INFO) +
			(sizeof(PI_PARAM) + sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM)*id;

	
	for (i = 0; i < 2; i++) {
		tmpbuf[0] = buf[i];
		if (!EEPROMDrv_WriteByte(0, addr+i, tmpbuf[0])) return 0;
	}
#endif
	return 1;
}

void UI_SavePIToFlash(uint16_t id, char *buf)
{
	PI_PARAM *pFSPI;
	PI_PARAM_HEADER *pFSPIH;
	uint32_t addr;
	uint32_t sector;
	int pos;
	//int i;

	pos = (id%(FLASH_PAGE_SIZE16/FLASH_PI_DATA_SIZE)) * FLASH_PI_DATA_SIZE;
	addr = FLASH_PATIENT_DATA_ADDR + id*FLASH_PI_DATA_SIZE;
	sector = GetSector(addr);
	FlashDrv_Read(sector*FLASH_PAGE_SIZE16, buf, FLASH_PAGE_SIZE16);
	//save data to flash
	pFSPIH = 0;
	pFSPI = (PI_PARAM *)&buf[pos];
	//pFSPI->vol = Setup.vol;
	
	if (pFSPI->id != id) return;

	if (pFSPI->id != id) {
		if (!FlashDrv_Write(FLASH_SYSTEM_INFO_ADDR, 
					 &CommonBuf[0], FLASH_PAGE_SIZE16)) return;
	}
	else {
		if (pFSPIH->writeTime > FLASH_ENDURANCE) {
			pFSPIH->writeTime = 1;
			if (!FlashDrv_Write(FLASH_SYSTEM_INFO2_ADDR, 
						&CommonBuf[0], FLASH_PAGE_SIZE16)) return;
		}
		else {
			pFSPIH->writeTime++;
			if (!FlashDrv_Write(FLASH_SYSTEM_INFO_ADDR, 
					&CommonBuf[0], FLASH_PAGE_SIZE16)) return;
		}
	}
}

int UI_Exercise_LoadAllData(uint8_t id, uint8_t *buf)
{
	uint32_t pi_startAddr;
	//uint32_t ex_startAddr;
	uint16_t exTotalSize;
	//uint32_t posAddr;

	exTotalSize = sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM;
	pi_startAddr = sizeof(SYSTEM_INFO) + (sizeof(PI_PARAM) + exTotalSize)*id;
    if (id == 67)
      id = 1;
	//ex_startAddr = pi_startAddr + sizeof(PI_PARAM);
	//if (!EEPROMDrv_Read(0, ex_startAddr, buf, exTotalSize)) return 0;
	if (!EEPROMDrv_Read(0, pi_startAddr, buf, sizeof(PI_PARAM)+exTotalSize)) return 0;
	return 1;
}

int UI_Exercise_LoadExData(uint32_t addr, uint8_t *buf)
{
	if (!EEPROMDrv_Read(0, addr, buf, sizeof(EXERCISE_INFO))) return 0;
	return 1;
}

int UI_Exercise_LoadLastData(uint32_t id, SAVE_EXERCISE_INFO_V2 *psei)
{
	uint8_t *buf, *buf2;//[4+sizeof(EXERCISE_INFO)];
	uint8_t tmpbuf[30];
	FILINFO fno;
	SAVE_EXERCISE_INFO_V2 *psei_temp;
	uint32_t size;
	uint16_t save_num;
	uint32_t temp;
	uint8_t i;
	
	if (!EEPROMDisk_Link()) {
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
		return -1;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		///App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
		return -1;
	}
	if (EEPROMDisk_stat("0:/KR20P", &fno) != 1) {
		//not init
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_NOFILE, EDT_DISP);
		return -1;
	}
	//file name size is 8byte
	ID2FileName(id, tmpbuf);
	//save_num = 1;
	size = 0;
	if (!EEPROMDisk_OpenRead((char *)tmpbuf)) {
		//no open read file
		return 0;
	}
	else {
		buf = (uint8_t *)CommonBuf;
		if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
			//no read file info
		}
		else size = fno.fsize;//EEPROMDisk_GetSize();
		if (size > EEPROMDISK_MAX_DATA_SIZE) {
			EEPROMDisk_Close();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			//App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_FILE_SIZE, EDT_DISP);
			return 0;
		}
		if (size >= sizeof(SAVE_EXERCISE_INFO_V2)) {
			//move to last data position
			if (!EEPROMDisk_lseek(size-sizeof(SAVE_EXERCISE_INFO_V2))) {
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_SEEK, EDT_DISP);
				return 0;
			}
			//read last data
			if (!EEPROMDisk_Read((uint8_t *)buf, sizeof(SAVE_EXERCISE_INFO_V2))) {
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_READ, EDT_DISP);
				return 0;
			}
			//check last data
			psei_temp = (SAVE_EXERCISE_INFO_V2 *)buf;
			if (psei_temp->flg == MST_REHAB) save_num = psei_temp->spm.runTime;
			else save_num = psei_temp->smm.runTime;
			//check mark
			temp = size / sizeof(SAVE_EXERCISE_INFO_V2);
			if (psei_temp->flg > 1 || save_num != temp) { //data
				//MotorDrv_Release();
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				if (psei->flg > 1) API_SetErrorCode(EC_EEP_DATA_FMT, EDT_DISP);
				else API_SetErrorCode(EC_EEP_DATA, EDT_DISP);
				return 0;
			}
			buf2 = (uint8_t *)psei;
			for (i = 0; i < sizeof(SAVE_EXERCISE_INFO_V2); i++) {
				buf2[i] = buf[i];
			}
			Exercise.dataCnt = temp;
		}
		else {
			EEPROMDisk_Close();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			return 0;
		}
	}
	EEPROMDisk_Close();
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();

	return 1;
}

int UI_Exercise_SaveNextPosAddr(uint8_t id, uint16_t pos)
{
	uint32_t pi_startAddr;
	//uint32_t ex_startAddr;
	uint16_t exTotalSize;
	uint32_t addr;
	uint32_t i;
	//uint8_t *p;
	uint8_t buf[2];
	uint8_t tmpbuf[5];

	exTotalSize = sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM;
	pi_startAddr = sizeof(SYSTEM_INFO) + (sizeof(PI_PARAM) + exTotalSize)*id;
	addr = pi_startAddr + 9;
	buf[0] = (uint8_t)(pos>>8);
	buf[1] = (uint8_t)(pos);
	for (i = 0; i < 2; i++) {
		tmpbuf[0] = buf[i];
		if (!EEPROMDrv_WriteByte(0, addr+i, tmpbuf[0])) return 0;
	}
	return 1;
	
}

int UI_Exercise_SaveData(uint32_t id, uint8_t read, uint16_t p_pos)
{
	//uint16_t pos;
	//uint32_t posAddr;
	//uint32_t pi_startAddr;
	//uint32_t ex_startAddr;
	//uint32_t exTotalSize;
	//int i;
	//EXERCISE_INFO *pexinfo;
	uint8_t *buf;//[4+sizeof(EXERCISE_INFO)];
	uint8_t tmpbuf[30];
	FILINFO fno;
	SAVE_EXERCISE_INFO_V2 *psei;
	uint32_t size;
	uint16_t save_num;
	uint32_t temp;
	
#ifdef USE_FUN_EEPROM_DISK
	if (!EEPROMDisk_Link()) {
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
		return -1;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		///App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
		return -1;
	}
	if (EEPROMDisk_stat("0:/KR20P", &fno) != 1) {
		//not init
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_NOFILE, EDT_DISP);
		return -1;
	}
	//file name size is 8byte
	ID2FileName(id, tmpbuf);
	save_num = 1;
	size = 0;
	if (!EEPROMDisk_OpenRead((char *)tmpbuf)) {
		//no read file
	}
	else {
		buf = (uint8_t *)CommonBuf;
		if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
			//no file
		}
		else {
			size = fno.fsize;//EEPROMDisk_GetSize();
		}
		if (size > EEPROMDISK_MAX_DATA_SIZE) {
			EEPROMDisk_Close();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			//App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_FILE_SIZE, EDT_DISP);
			return 0;
		}
		if (size >= sizeof(SAVE_EXERCISE_INFO_V2)) {
			if (!EEPROMDisk_lseek(size-sizeof(SAVE_EXERCISE_INFO_V2))) {
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_SEEK, EDT_DISP);
				return 0;
			}
			if (!EEPROMDisk_Read((uint8_t *)buf, sizeof(SAVE_EXERCISE_INFO_V2))) {
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_READ, EDT_DISP);
				return 0;
			}
			//check last data
			psei = (SAVE_EXERCISE_INFO_V2 *)buf;
			if (psei->flg == MST_REHAB) save_num = psei->spm.runTime + 1;
			else save_num = psei->smm.runTime + 1;
			//check mark
			temp = size / sizeof(SAVE_EXERCISE_INFO_V2);
			if (psei->flg > 1 || (save_num-1) != temp) {
				//MotorDrv_Release();
				EEPROMDisk_Close();
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				//App_SetUIProcess(UI_ProcessNull);
				if (psei->flg > 1) API_SetErrorCode(EC_EEP_DATA_FMT, EDT_DISP);
				else API_SetErrorCode(EC_EEP_DATA, EDT_DISP);
				return 0;
			}
		}
		else {
			//no data
		}
	}
	EEPROMDisk_Close();
	
	if (!EEPROMDisk_OpenWrite((char *)tmpbuf)) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_OPEN, EDT_DISP);
		return -1;
	}

	if (!EEPROMDisk_lseek(size)) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_SEEK, EDT_DISP);
		return -1;
	}
	if (SaveExeInfoV2.flg == MST_REHAB) {
		SaveExeInfoV2.spm.time = (uint8_t)SpdTmWnd.time;
		SaveExeInfoV2.spm.exangle = (uint8_t)(AngleWnd.exAngle+5);
		SaveExeInfoV2.spm.flangle = (uint8_t)(AngleWnd.flAngle+5);
		SaveExeInfoV2.spm.speed = (uint8_t)SpdTmWnd.speed;
		SaveExeInfoV2.spm.count = RunWnd.repeat;
		SaveExeInfoV2.spm.plv = 0;
		if (Setup3.ProChk == BST_CHECKED) SaveExeInfoV2.spm.plv |= 0x04; 
		if (Setup3.LPChk == BST_CHECKED) SaveExeInfoV2.spm.plv |= 0x02; 
		if (Setup3.VibChk == BST_CHECKED) SaveExeInfoV2.spm.plv |= 0x01; 
		SaveExeInfoV2.spm.runTime = save_num;
	}
	else {
		SaveExeInfoV2.smm.runTime = save_num;
	}

	buf = (uint8_t *)&SaveExeInfoV2;
	if (!EEPROMDisk_Write(buf, sizeof(SAVE_EXERCISE_INFO_V2))) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_WRITE, EDT_DISP);
		return 0;
	}
	EEPROMDisk_Close();
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();
#else
	//uint8_t *p;

	if (id > PATIENT_NUM) return -1;

	buf = (uint8_t *)CommonBuf;
	if (read) pos = Exercise.addr;
	else pos = p_pos;

	//if (pos >= EXERCISE_INFO_NUM) return -1;

	exTotalSize = sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM;
	//pi_startAddr = sizeof(SYSTEM_INFO);
	//pi_startAddr = sizeof(PI_PARAM);
	pi_startAddr = sizeof(SYSTEM_INFO) + (sizeof(PI_PARAM) + exTotalSize)*id;
	ex_startAddr = pi_startAddr + sizeof(PI_PARAM);
	posAddr = sizeof(EXERCISE_INFO)*pos;
	if (posAddr >= exTotalSize) return 0;
	//p = (uint8_t *)&exinfo;
	
	//read
	if (!pos) {
		posAddr = sizeof(EXERCISE_INFO)*(EXERCISE_INFO_NUM-1);
	}
	else {
		posAddr = sizeof(EXERCISE_INFO)*(pos-1);
	}
	posAddr += ex_startAddr;
	pexinfo = (EXERCISE_INFO *)&buf[0];
	if (!UI_Exercise_LoadExData(posAddr, (uint8_t *)pexinfo)) return -1;
	//if (!EEPROMDrv_Read(0, posAddr, (uint8_t *)&exinfo, sizeof(EXERCISE_INFO))) return -1;
	if (pexinfo->exangle-5 > RUN_ANGLE_MAX) pexinfo->runTime = 0;
	
	//save
	posAddr = sizeof(EXERCISE_INFO)*pos;
	posAddr += (ex_startAddr);
	pexinfo->time = (uint8_t)SpdTmWnd.time;
	pexinfo->exangle = (uint8_t)(AngleWnd.exAngle+5);
	pexinfo->flangle = (uint8_t)(AngleWnd.flAngle+5);
	pexinfo->speed = (uint8_t)SpdTmWnd.speed;
	pexinfo->count = RunWnd.repeat;
	pexinfo->runTime++;
	for (i = 0; i < sizeof(EXERCISE_INFO); i++) {
		tmpbuf[0] = buf[i];
		//if (!EEPROMDrv_WriteByte(0, posAddr, (uint8_t *)&exinfo, sizeof(EXERCISE_INFO))) return -1;
		if (!EEPROMDrv_WriteByte(0, posAddr+i, tmpbuf[0])) return -1;
		//if (!EEPROMDrv_WritePage(0, posAddr, buf, sizeof(EXERCISE_INFO))) return -1;
	}
	//for (i = 0; i < 150000; i++);  //æ¯¿∏∏È EEPROM ¿˙¿Â¿Ã æ»µ  

	Exercise.addr = pos+1;
	//save next addr
	if (!UI_Exercise_SaveNextPosAddr(id, Exercise.addr)) return -1;
//	UI_SavePIToEEPROM(id, (uint8_t *)CommonBuf);
#endif
	return 1;
}

int UI_SaveExerciseData(uint8_t afterMemFull)
{
	int rv;

#ifdef PI_LOAD_V2
	rv = UI_Exercise_SaveData(PInfoWnd2.id, 0, 0);
	if (rv == -1) {
		//save error
		return -1;
	}

#else
	if (PInfoWnd2.id > PATIENT_NUM) return -1;

	if (!afterMemFull) {
		rv = UI_Exercise_SaveData(PInfoWnd2.id, 1, 0);
		if (rv == -1) {
			//save error
			return -1;
		}
		else if (rv == 1) return 1;

		if (PInfoWnd2.id < PATIENT_NUM) {
			//run memory full windows
			//UI_PopupDataFull_Create();
			return 0;
		}
	}
	
	rv = UI_Exercise_SaveData(PInfoWnd2.id, 0, 0);
	if (rv == -1) {
		//save error
		return -1;
	}
#endif
	else if (rv == 0) return 0;
	return 1;
}

//pjg++180806
void UI_SetMotorErrorCode(void)
{
	uint32_t rv;

	rv = MotorDrv_GetFaultActionValue();
	if (rv == A3930_FET_LOGIC_FAULT) { //pjg++180806
		API_SetErrorCode(EC_MDIC_HALL, EDT_DISP_HALT);
	}
	else if (rv == A3930_FET_SHORT_TO_GND) { //pjg++180806
		API_SetErrorCode(EC_MDIC_MOTOR_SHORT, EDT_DISP_HALT);
	}
	//else ;//low load current
}			 
void UI_DisplayMemoryUsage(void)
{	
	char buf[18];
	int _10unit, _1unit, temp;
#ifdef USE_FUN_EEPROM_DISK
	temp = (EEPROM.freesize*100)/EEPROM.totalsize;
	_10unit = temp/10;
	_1unit = temp%10;
#else
	_10unit = Exercise.addr *10  / EXERCISE_INFO_NUM;
	_1unit = Exercise.addr * 100 / EXERCISE_INFO_NUM - 90;
#endif																									 						 
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = 'r'; //red color number
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'p';
	buf[6] = 'n';
	buf[7] = 'g';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '9';		   
	buf[11] = '0';
	buf[12] = ',';
	buf[13] = '0';
	buf[14] = '8';
	buf[15] = '8';
	buf[16] = '\r';
	buf[17] = 0;
	
	buf[3] = '0' + _10unit;
	TLCD_DrawText(buf); //1st 
	buf[3] = '0' + _1unit;
	buf[9] = '3';	//x coord
	buf[10] = '0';
	buf[11] = '5';
	TLCD_DrawText(buf); //2st 
}

void UI_SaveSystemParam(void)
{
#if 0 //fat of eeprom test
	//USBDisk_UnMount();

	EEPROMDrv_Init(&hspi3);
	if (!EEPROMDisk_Link()) {
		//App_SetUIProcess(UI_ProcessNull);
		//API_SetErrorCode(EC_EEP_WRITE, EDT_DISP_HALT);
		return 0;
	}
	if (!EEPROMDisk_Mount()) {
		return 0;
	}
	if (!EEPROMDisk_OpenDir("kr50p")) {
		if (!EEPROMDisk_Format()) {
			//App_SetUIProcess(UI_ProcessNull);
			//API_SetErrorCode(EC_EEP_DISK_FAIL, EDT_DISP_HALT);
			return 0;
		}
        if (!EEPROMDisk_Mount()) {
            return 0;
        }
		EEPROMDisk_MkDir("kr50p");
	}

	if (!EEPROMDisk_OpenRead("/kr50p/00000001.dat")) {
		if (EEPROMDisk_OpenWrite("/kr50p/00000001.dat")) {
			for (int i = 0; i < 1024; i++) {
				CommonBuf[i] = i;
			}
			EEPROMDisk_Write(CommonBuf, 1024);
			EEPROMDisk_Close();
		}
	}
	for (int i = 0; i < 1024; i++) {
		CommonBuf[i] = 0;
	}
	EEPROMDisk_Read(CommonBuf, 1024);============================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================
#endif
}

void UI_GoBackWnd(void)
{
	switch (UI_Wnd.prevNum) {
	case UI_WND_MODE_RUN:
		UI_Run_Create();
		break;
	case UI_WND_MODE_ANGLE_MEA:
		UI_AngleMeasure_Create();
		break;
	case UI_WND_MODE_ANGLE_SET_EX:
		//UI_Angle_Create();
		break;
	case UI_WND_MODE_SPEED:
		UI_SpeedTime_Create();
		break;
	case UI_WND_MODE_HOME:
		UI_Home_Create();
		break;
	case UI_WND_MODE_SETUP:
		UI_Setup_Create();
		break;
	case UI_WND_MODE_USER:
		UI_User_Create();
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void UI_PopupMemoryWarning_OnBnClickedBtnOK(void)
{
#ifdef PI_LOAD_V2
	FILINFO fno;
	uint8_t tmpbuf[25];
	uint32_t temp;//, temp2;

	temp = (EEPROM.freesize*100)/EEPROM.totalsize;
	if (temp >= 99) return;

	//if (loginInfo.type == LIT_USER){
		if (!loginInfo.cnt) return;
		PInfoWnd2.id = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
	//}
	if (!EEPROMDisk_Link()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
		return;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return;
	}
	ID2FileName(PInfoWnd2.id, tmpbuf);
	if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//are you create id
		UI_PopupIDCreate_Create();
		return;
	}
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();
	//is right id
	UI_PopupIDRight_Create();

#else
	if(FullInfo.type == FT_PI){
		loginInfo.type = LIT_USER;
		UI_Login_Create();
	}
	else{
		UI_Run_Create();
	}
#endif
}

void UI_PopupMemoryWarning_Init(void)
{
	API_CreateWindow("", pBtnInfo[(RID_MEWR_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_MEWR_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_MEWR_BTN_OK+Setup2.language)*2], ',', 2),
			108,45, hParent, RID_MEWR_BTN_OK+Setup2.language, 0);

	UI_DisplayMemoryUsage();
	UI_LED_Control(LM_NONE);
}

LRESULT UI_PopupMemoryWarning_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupMemoryWarning_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_MEWR_BTN_OK:
		case RID_MEWR_BTN_OKK:
		case RID_MEWR_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupMemoryWarning_OnBnClickedBtnOK();
				break;
			}
			return 0;                               
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_PopupMemoryWarning_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupMemoryWarning_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_MEM_USAGE][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DATA_FULL_WARNING;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupDataFull_OnBnClickedBtnYes(void)
{
	//UI_PopupDataFullCheck_Create();
	UI_PiLoad_Create();
}

void UI_PopupDataFull_OnBnClickedBtnNo(void)
{
	//if(FullInfo.type == FT_PI){
		#ifdef PI_LOAD_V1
		if(PiChange_Wnd.PrePressedNum < 21)	UI_PiLoad_Create();
		else if(PiChange_Wnd.PrePressedNum < 42) UI_PiLoad2_Create();
		else if(PiChange_Wnd.PrePressedNum < 63) UI_PiLoad3_Create();
		else UI_PiLoad4_Create();
		#else
		UI_PiLoad_Create();
		#endif
	//}
	//else UI_SpeedTime_Create();
}

void UI_PopupDataFull_Init(void)
{
	API_CreateWindow("", pBtnInfo[(RID_FULL_BTN_YES+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FULL_BTN_YES+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FULL_BTN_YES+Setup2.language)*2], ',', 2),
			108,45, hParent, RID_FULL_BTN_YES+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[(RID_FULL_BTN_NO*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[(RID_FULL_BTN_NO*2], ',', 1),
	//		GetNumFromString(pBtnInfo[(RID_FULL_BTN_NO*2], ',', 2),
	//		108,45, hParent, RID_FULL_BTN_NO, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i r1.png,285,89\r");
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i r0.png,300,89\r");
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i r0.png,315,89\r");

	UI_LED_Control(LM_NONE);
}

LRESULT UI_PopupDataFull_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupDataFull_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_FULL_BTN_YES:
		case RID_FULL_BTN_YESK:
		case RID_FULL_BTN_YESC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDataFull_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_FULL_BTN_NO:
		case RID_FULL_BTN_NOK:
		case RID_FULL_BTN_NOC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDataFull_OnBnClickedBtnNo();
				break;
			}
			return 0;                                   
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_PopupDataFull_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupDataFull_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_MEM_FULL][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DATA_FULL;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
void UI_PopupDataFullCheck_OnBnClickedBtnYes(void)
{
	if(FullInfo.type == FT_PI){
		Exercise.addr = 0;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
		loginInfo.type = LIT_USER;
		UI_Login_Create();
		UI_LED_Control(LM_STAND_BY);
	}
	else{
		Exercise.addr = 0;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
		UI_Run_Create();
	}
}

void UI_PopupDataFullCheck_OnBnClickedBtnNo(void)
{
	if(FullInfo.type == FT_PI){
		#ifdef PI_LOAD_V1
		if(PiChange_Wnd.PrePressedNum < 21)	UI_PiLoad_Create();
		else if(PiChange_Wnd.PrePressedNum < 42)	UI_PiLoad2_Create();
		else if(PiChange_Wnd.PrePressedNum < 63)	UI_PiLoad3_Create();
		else	UI_PiLoad4_Create();
		#else
		UI_PiLoad_Create();
		#endif
	}
	else UI_SpeedTime_Create();
}

void UI_PopupDataFullCheck_Init(void)
{
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 2),
			108,45, hParent, RID_FUCH_BTN_YES+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 2),
			108,45, hParent, RID_FUCH_BTN_NO+Setup2.language, 0);
	UI_LED_Control(LM_NONE);
}

LRESULT UI_PopupDataFullCheck_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupDataFullCheck_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_FUCH_BTN_YES:
		case RID_FUCH_BTN_YESK:
		case RID_FUCH_BTN_YESC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDataFullCheck_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_FUCH_BTN_NO:
		case RID_FUCH_BTN_NOK:
		case RID_FUCH_BTN_NOC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDataFullCheck_OnBnClickedBtnNo();
				break;
			}
			return 0;                                   
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_PopupDataFullCheck_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupDataFullCheck_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i fuchbg.png,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DATA_FULL_CHK;
}
#endif

void UI_Display_Init(uint8_t value)
{
	int i, j;
        //return;
	//
	//font 9
	// = {"i 0.bmp,259,127\r"};
	dispNum.buf[0][0] = 'i';	dispNum.buf[0][1] = ' ';	dispNum.buf[0][2] = '0';	dispNum.buf[0][3] = '.';	
	dispNum.buf[0][4] = 'b';	dispNum.buf[0][5] = 'm';	dispNum.buf[0][6] = 'p';	dispNum.buf[0][7] = ',';	
	dispNum.buf[0][8] = '2';	dispNum.buf[0][9] = '5';	dispNum.buf[0][10] = '9';	dispNum.buf[0][11] = ',';	
	dispNum.buf[0][12] = '1';	dispNum.buf[0][13] = '2';	dispNum.buf[0][14] = '7';	dispNum.buf[0][15] = '\r';	
	dispNum.buf[0][16] = 0;	
	for (i = 1; i < DISP_NUM_DISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_NUM_BUF_SIZE; j++) dispNum.buf[i][j] = dispNum.buf[0][j];
	}
	//"i bnk.bmp,259,127\r"
	dispNum.blank_buf[0][0] = 'i';	dispNum.blank_buf[0][1] = ' ';	dispNum.blank_buf[0][2] = 'b';
	dispNum.blank_buf[0][3] = 'n';	dispNum.blank_buf[0][4] = 'k';	dispNum.blank_buf[0][5] = '.';	
	dispNum.blank_buf[0][6] = 'b';	dispNum.blank_buf[0][7] = 'm';	dispNum.blank_buf[0][8] = 'p';	
	dispNum.blank_buf[0][9] = ',';	dispNum.blank_buf[0][10] = '2';	dispNum.blank_buf[0][11] = '5';	
	dispNum.blank_buf[0][12] = '9';	dispNum.blank_buf[0][13] = ',';	dispNum.blank_buf[0][14] = '1';	
	dispNum.blank_buf[0][15] = '2';	dispNum.blank_buf[0][16] = '7';	dispNum.blank_buf[0][17] = '\r';	
	dispNum.blank_buf[0][18] = 0;
	for (i = 1; i < DISP_NUM_DISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_BLANK_NUM_BUF_SIZE; j++) dispNum.blank_buf[i][j] = dispNum.blank_buf[0][j];
	}
	//"i bnk2.bmp,259,127\r"
	dispNum.blankAll_buf[0][0] = 'i';		dispNum.blankAll_buf[0][1] = ' ';		dispNum.blankAll_buf[0][2] = 'b';
	dispNum.blankAll_buf[0][3] = 'n';	dispNum.blankAll_buf[0][4] = 'k';	dispNum.blankAll_buf[0][5] = '2';
	dispNum.blankAll_buf[0][6] = '.';		dispNum.blankAll_buf[0][7] = 'b';	dispNum.blankAll_buf[0][8] = 'm';	
	dispNum.blankAll_buf[0][9] = 'p';	dispNum.blankAll_buf[0][10] = ',';	dispNum.blankAll_buf[0][11] = '2';	
	dispNum.blankAll_buf[0][12] = '5';	dispNum.blankAll_buf[0][13] = '9';	dispNum.blankAll_buf[0][14] = ',';	
	dispNum.blankAll_buf[0][15] = '1';	dispNum.blankAll_buf[0][16] = '2';	dispNum.blankAll_buf[0][17] = '7';	
	dispNum.blankAll_buf[0][18] = '\r';	dispNum.blankAll_buf[0][19] = 0;
	for (i = 1; i < DISP_NUM_DISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_BLANK_ALL_NUM_BUF_SIZE; j++) dispNum.blankAll_buf[i][j] = dispNum.blankAll_buf[0][j];
	}
	
	for (i = 1; i < DISP_NUM_DISP_SAME_TIME; i++)
	{
		dispNum.preUnitNum[i] = 0;
	}

#if 0
	//
	//font 7
	// = {"i 70.bmp,259,127\r"};
	dispNum.sbuf[0][0] = 'i';	dispNum.sbuf[0][1] = ' ';	dispNum.sbuf[0][2] = '7';	dispNum.sbuf[0][3] = '0';
	dispNum.sbuf[0][4] = '.';	dispNum.sbuf[0][5] = 'b';	dispNum.sbuf[0][6] = 'm';	dispNum.sbuf[0][7] = 'p';	
	dispNum.sbuf[0][8] = ',';	dispNum.sbuf[0][9] = '2';	dispNum.sbuf[0][10] = '5';	dispNum.sbuf[0][11] = '9';
	dispNum.sbuf[0][12] = ',';	dispNum.sbuf[0][13] = '1';	dispNum.sbuf[0][14] = '2';	dispNum.sbuf[0][15] = '7';	
	dispNum.sbuf[0][16] = '\r';	dispNum.sbuf[0][17] = 0;	
	for (i = 1; i < DISP_NUM_SDISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_NUM_SBUF_SIZE; j++) dispNum.sbuf[i][j] = dispNum.sbuf[0][j];
	}
	//"i 0bnk.bmp,259,127\r"
	dispNum.sblank_buf[0][0] = 'i';	dispNum.sblank_buf[0][1] = ' ';	dispNum.sblank_buf[0][2] = '7';
	dispNum.sblank_buf[0][3] = 'b';	dispNum.sblank_buf[0][4] = 'n';	dispNum.sblank_buf[0][5] = 'k';	
	dispNum.sblank_buf[0][6] = '.';	dispNum.sblank_buf[0][7] = 'b';	dispNum.sblank_buf[0][8] = 'm';	
	dispNum.sblank_buf[0][9] = 'p';	dispNum.sblank_buf[0][10] = ',';	dispNum.sblank_buf[0][11] = '2';	
	dispNum.sblank_buf[0][12] = '5';	dispNum.sblank_buf[0][13] = '9';	dispNum.sblank_buf[0][14] = ',';	
	dispNum.sblank_buf[0][15] = '1';	dispNum.sblank_buf[0][16] = '2';	dispNum.sblank_buf[0][17] = '7';	
	dispNum.sblank_buf[0][18] = '\r';	dispNum.sblank_buf[0][19] = 0;
	for (i = 1; i < DISP_NUM_SDISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_BLANK_NUM_SBUF_SIZE; j++) dispNum.sblank_buf[i][j] = dispNum.sblank_buf[0][j];
	}
#endif
	//
	//font 5
	// = {"i 50.bmp,259,127\r"};
	dispNum.sbuf[0][0] = 'i';	dispNum.sbuf[0][1] = ' ';	dispNum.sbuf[0][2] = '0';	dispNum.sbuf[0][3] = '0';
	dispNum.sbuf[0][4] = '.';	dispNum.sbuf[0][5] = 'p';	dispNum.sbuf[0][6] = 'n';	dispNum.sbuf[0][7] = 'g';	
	dispNum.sbuf[0][8] = ',';	dispNum.sbuf[0][9] = '2';	dispNum.sbuf[0][10] = '5';	dispNum.sbuf[0][11] = '9';
	dispNum.sbuf[0][12] = ',';	dispNum.sbuf[0][13] = '1';	dispNum.sbuf[0][14] = '2';	dispNum.sbuf[0][15] = '7';	
	dispNum.sbuf[0][16] = '\r';	dispNum.sbuf[0][17] = 0;	
	for (i = 1; i < DISP_NUM_SDISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_NUM_SBUF_SIZE; j++) dispNum.sbuf[i][j] = dispNum.sbuf[0][j];
	}
	//"i 0bnk.bmp,259,127\r"
	dispNum.sblank_buf[0][0] = 'i';	dispNum.sblank_buf[0][1] = ' ';	dispNum.sblank_buf[0][2] = 'n';
	dispNum.sblank_buf[0][3] = 'b';	dispNum.sblank_buf[0][4] = 'n';	dispNum.sblank_buf[0][5] = 'k';	
	dispNum.sblank_buf[0][6] = '.';	dispNum.sblank_buf[0][7] = 'b';	dispNum.sblank_buf[0][8] = 'm';	
	dispNum.sblank_buf[0][9] = 'p';	dispNum.sblank_buf[0][10] = ',';	dispNum.sblank_buf[0][11] = '2';	
	dispNum.sblank_buf[0][12] = '5';	dispNum.sblank_buf[0][13] = '9';	dispNum.sblank_buf[0][14] = ',';	
	dispNum.sblank_buf[0][15] = '1';	dispNum.sblank_buf[0][16] = '2';	dispNum.sblank_buf[0][17] = '7';	
	dispNum.sblank_buf[0][18] = '\r';	dispNum.sblank_buf[0][19] = 0;
	for (i = 1; i < DISP_NUM_SDISP_SAME_TIME; i++)  {
		for (j = 0; j < DISP_BLANK_NUM_SBUF_SIZE; j++) dispNum.sblank_buf[i][j] = dispNum.sblank_buf[0][j];
	}

	//dispNumPos = value;
}

// clear all area
//param - font : font select
//             x : x pos
//             y : y pos
//             pos : display position
void UI_DisplayClearNumber(char font, int x, int  y, uint8_t pos)
{
	short temp, temp2, temp3, temp4;
	//return;
        
	temp = x%100;
	temp2 = temp%10;
	temp3 = y%100;
	temp4 = temp3%10;
	
	if (font == UI_DISP_NUM_FNT5) {
		dispNum.sblankAll_buf[pos][11] = x/100+'0'; 	dispNum.sblankAll_buf[pos][12] = temp/10+'0'; 	dispNum.sblankAll_buf[pos][13] = temp2+'0';	// x axis
		dispNum.sblankAll_buf[pos][15] = y/100+'0'; 	dispNum.sblankAll_buf[pos][16] = temp3/10+'0'; dispNum.sblankAll_buf[pos][17] = temp4+'0';	// y axis
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.sblankAll_buf[pos]);
	}
	else {
		dispNum.blankAll_buf[pos][11] = x/100+'0'; 	dispNum.blankAll_buf[pos][12] = temp/10+'0'; 	dispNum.blankAll_buf[pos][13] = temp2+'0';	// x axis
		dispNum.blankAll_buf[pos][15] = y/100+'0'; 	dispNum.blankAll_buf[pos][16] = temp3/10+'0'; 	dispNum.blankAll_buf[pos][17] = temp4+'0';	// y axis
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.blankAll_buf[pos]);
	}
}

// ∆˘∆Æ9≈©±‚¿« º˝¿⁄ 1¿⁄ «¶«ˆ
// param : font - font 5 or 9
//             x - x coordinate
//             y - y coordinate
//             n - 10 : erase, - : - mark
//             pos - display line
void UI_DisplayNumber(char font, int x, int  y, int num, uint8_t pos)
{
	short temp, temp2, temp3, temp4;
	
        //return;
        
	temp = x%100;
	temp2 = temp%10;
	temp3 = y%100;
	temp4 = temp3%10;

	if (num < 10) {
		if (font == UI_DISP_NUM_FNT5) {
			if (num >= 0) dispNum.sbuf[pos][3] = num+'0';
			else dispNum.sbuf[pos][3] = '-';

			dispNum.sbuf[pos][9] = x/100+'0'; 	dispNum.sbuf[pos][10] = temp/10+'0'; 	dispNum.sbuf[pos][11] = temp2+'0';	// x axis
			dispNum.sbuf[pos][13] = y/100+'0'; dispNum.sbuf[pos][14] = temp3/10+'0'; 	dispNum.sbuf[pos][15] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.sbuf[pos]);
		}
		else if (font == UI_DISP_NUM_FNT7) {
			if (num >= 0) dispNum.sbuf[pos][3] = num+'0';
			else dispNum.sbuf[pos][3] = '-';

			dispNum.sbuf[pos][9] = x/100+'0'; 	dispNum.sbuf[pos][10] = temp/10+'0'; 	dispNum.sbuf[pos][11] = temp2+'0';	// x axis
			dispNum.sbuf[pos][13] = y/100+'0'; dispNum.sbuf[pos][14] = temp3/10+'0'; 	dispNum.sbuf[pos][15] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.sbuf[pos]);
		}
		else {
			if (num >= 0) dispNum.buf[pos][2] = num+'0';
			else dispNum.buf[pos][2] = '-';

			dispNum.buf[pos][8] = x/100+'0'; 	dispNum.buf[pos][9] = temp/10+'0'; 	dispNum.buf[pos][10] = temp2+'0';	// x axis
			dispNum.buf[pos][12] = y/100+'0'; 	dispNum.buf[pos][13] = temp3/10+'0'; 	dispNum.buf[pos][14] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.buf[pos]);
		}
	}
	else {
		if (font == UI_DISP_NUM_FNT5) {
			dispNum.sblank_buf[pos][11] = x/100+'0'; 	dispNum.sblank_buf[pos][12] = temp/10+'0'; 	dispNum.sblank_buf[pos][13] = temp2+'0';	// x axis
			dispNum.sblank_buf[pos][15] = y/100+'0'; 	dispNum.sblank_buf[pos][16] = temp3/10+'0'; dispNum.sblank_buf[pos][17] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.sblank_buf[pos]);
		}
		else if (font == UI_DISP_NUM_FNT7) {
			dispNum.sblank_buf[pos][11] = x/100+'0'; 	dispNum.sblank_buf[pos][12] = temp/10+'0'; 	dispNum.sblank_buf[pos][13] = temp2+'0';	// x axis
			dispNum.sblank_buf[pos][15] = y/100+'0'; 	dispNum.sblank_buf[pos][16] = temp3/10+'0'; dispNum.sblank_buf[pos][17] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.sblank_buf[pos]);
		}
		else {
			dispNum.blank_buf[pos][10] = x/100+'0'; 	dispNum.blank_buf[pos][11] = temp/10+'0'; 	dispNum.blank_buf[pos][12] = temp2+'0';	// x axis
			dispNum.blank_buf[pos][14] = y/100+'0'; 	dispNum.blank_buf[pos][15] = temp3/10+'0'; 	dispNum.blank_buf[pos][16] = temp4+'0';	// y axis
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)dispNum.blank_buf[pos]);
		}
	}
}
//º˝¿⁄ √÷¥Î 3¿⁄∏Æ «•«ˆ(-∆˜«‘)
void UI_DisplayDecimal(char font, uint8_t pos, int x, int y, short num)
{
	char temp, temp2, temp3;
	
	if (font == UI_DISP_NUM_FNT5) {	
		if (pos > DISP_NUM_SDISP_SAME_TIME) return;
	}
	else {
		if (pos > DISP_NUM_DISP_SAME_TIME) return;
	}

	if(num > 999){
		temp = num%1000;
		temp2 = temp%100;
		temp3 = temp2%10;
		UI_DisplayNumber(font, x, y, num/1000, pos+3);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS*1, y, temp/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS*2, y, temp2/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS*3, y, temp3, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS*1, y, temp/100, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS*2, y, temp2/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS*3, y, temp3, pos+0);
		}
	}
	else if (num < -99) {
	}          
	else if (num > 99) {
		temp = num%100;
		temp2 = temp%10;
		UI_DisplayNumber(font, x, y, num/100, pos+2);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp2, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp2, pos+0);
		}
	}
	else if (num > 9) {
		temp = num%10;
		UI_DisplayNumber(font, x, y, num/10, pos+1);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, temp, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, temp, pos+0);
		}
	}
	else if (num >= 0) {
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x, y, num, pos+0);
		}
		else {
			UI_DisplayNumber(font, x, y, num, pos+0);
		}
	}
	else {
		if (num < -9) {
			temp = num%10;
			UI_DisplayNumber(font, x, y, -1, pos+2);
			if (font == UI_DISP_NUM_FNT5) {
				UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp*-1, pos+0);
			}
			else {
				UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp*-1, pos+0);
			}
		}
		else {
			//UI_DisplayNumber(font, x, y, 10, pos+2);
			if (font == UI_DISP_NUM_FNT5) {
				UI_DisplayNumber(font, x, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, num*-1, pos+0);
			}
			else {
				UI_DisplayNumber(font, x, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, num*-1, pos+0);
			}
		}
	}
}

void UI_DisplayDecimalSel(char font, uint8_t pos, int x, int y, short num, uint8_t unit_num)
{
	int temp, temp2, temp3;
	
	if (font == UI_DISP_NUM_FNT5) {	
		if (pos > DISP_NUM_SDISP_SAME_TIME) return;
	}
	else {
		if (pos > DISP_NUM_DISP_SAME_TIME) return;
	}

	if(num > 9999){
		return;
	}
	else if(num > 999){
		temp = num%1000;
		temp2 = temp%100;
		temp3 = temp2%10;
		UI_DisplayNumber(font, x, y, num/1000, pos+3);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, temp/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp2/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS+DISP_NUM_5P_10_POS, y, temp3, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, temp/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp2/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS+DISP_NUM_9P_10_POS, y, temp3, pos+0);
		}
	}
	else if (num < -99) {
	}          
	else if (num > 99) {
		temp = num%100;
		temp2 = temp%10;
		UI_DisplayNumber(font, x, y, num/100, pos+2);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp2, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp2, pos+0);
		}
	}
	else if (num > 9) {
		temp = num%10;
		if (unit_num > 2) UI_DisplayNumber(font, x, y, 10, pos+2);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, num/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, num/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp, pos+0);
		}
	}
	else if (num >= 0) {
		if (unit_num > 2) UI_DisplayNumber(font, x, y, 10, pos+2);
		if (font == UI_DISP_NUM_FNT5) {
			if (unit_num > 1) UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, 10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, num, pos+0);
		}
		else {
			if (unit_num > 1) UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, 10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, num, pos+0);
		}
	}
	else {
		if (num < -9) {
			temp = num%10;
			UI_DisplayNumber(font, x, y, -1, pos+2);
			if (font == UI_DISP_NUM_FNT5) {
				UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, temp*-1, pos+0);
			}
			else {
				UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, temp*-1, pos+0);
			}
		}
		else {
			if (font == UI_DISP_NUM_FNT5) {
				if (unit_num > 1) UI_DisplayNumber(font, x+DISP_NUM_5P_10_POS, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5P_1_POS, y, num*-1, pos+0);
			}
			else {
				if (unit_num > 1) UI_DisplayNumber(font, x+DISP_NUM_9P_10_POS, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_9P_1_POS, y, num*-1, pos+0);
			}
		}
	}
}

//center align display
void UI_DisplayDecimal_5Unit(char font, uint8_t pos, int x, int y, short num)
{
	char temp, temp2;
        short temp3, temp4;

	if (font == UI_DISP_NUM_FNT5) {	
		if (pos > DISP_NUM_SDISP_SAME_TIME) return;
	}
	else {
		if (pos > DISP_NUM_DISP_SAME_TIME) return;
	}

	if(num > 32766){
		return;
	}
	else if(num < -9999){
	}
	else if (num > 9999) {
		temp4 = num%10000;
		temp3 = temp4%1000;
		temp = temp3%100;
		temp2 = temp%10;
		UI_DisplayNumber(font, x, y, num/10000, pos+4);
		dispNum.preUnitNum[pos] = 5;
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, temp4/1000, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, temp3/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp2, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1000_POS, y, temp4/1000, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_100_POS, y, temp3/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1_POS, y, temp2, pos+0);
		}
	}
	else if(num < -999){
	}
	else if (num > 999) {
		temp3 = num%1000;
		temp = temp3%100;
		temp2 = temp%10;
		//UI_DisplayNumber(font, x, y, 10, pos+4);
		if (dispNum.preUnitNum[pos] != 4) {
			UI_DisplayClearNumber(font, x, y, pos);
			dispNum.preUnitNum[pos] = 4;
		}
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, num/1000, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, temp3/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp2, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_1000, y, num/1000, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_100, y, temp3/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_10, y, temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_1, y, temp2, pos+0);
		}
	}
	else if (num < -99) {
	}          
	else if (num > 99) {
		temp = num%100;
		temp2 = temp%10;
		//UI_DisplayNumber(font, x, y, 10, pos+3);
		if (dispNum.preUnitNum[pos] != 3) {
			UI_DisplayClearNumber(font, x, y, pos);
			dispNum.preUnitNum[pos] = 3;
		}
		if (font == UI_DISP_NUM_FNT5) {
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, 10, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, num/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y,temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp2, pos+0);
		}
		else {
			//UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_1000, y, 10, pos+3);
			UI_DisplayNumber(font, x+DISP_NUM_5U_3PS_100, y, num/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5U_3PS_10, y,temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5U_3PS_1, y, temp2, pos+0);
		}
	}
	else if (num > 9) {
		temp = num%10;
		//UI_DisplayNumber(font, x, y, 10, pos+2);
		if (dispNum.preUnitNum[pos] != 2) {
			UI_DisplayClearNumber(font, x, y, pos);
			dispNum.preUnitNum[pos] = 2;
		}
		if (font == UI_DISP_NUM_FNT5) {
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, 10, pos+3);
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, 10, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, num/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp, pos+0);
		}
		else {
			//UI_DisplayNumber(font, x+DISP_NUM_5U_4PS_1000, y, 10, pos+3);
			//UI_DisplayNumber(font, x+DISP_NUM_5U_3PS_100, y, 10, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5U_2PS_10, y, num/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5U_2PS_1, y, temp, pos+0);
		}
	}
	else if (num >= 0) {
		//UI_DisplayNumber(font, x, y, 10, pos+2);
		if (dispNum.preUnitNum[pos] != 1) {
			UI_DisplayClearNumber(font, x, y, pos);
			dispNum.preUnitNum[pos] = 1;
		}
		if (font == UI_DISP_NUM_FNT5) {
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, 10, pos+3);
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, 10, pos+2);
			//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, 10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, num, pos+0);
		}
		else {
			//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1000_POS, y, 10, pos+3);
			//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_100_POS, y, 10, pos+2);
			//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1_POS, y, 10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5U_1PS_1, y, num, pos+0);
		}
	}
	else {
		if (num < -9) {
			temp = num%10;
			//UI_DisplayNumber(font, x, y, 10, pos+2);
			if (dispNum.preUnitNum[pos] != 0) {
			UI_DisplayClearNumber(font, x, y, pos);
			dispNum.preUnitNum[pos] = 0;
		}
			if (font == UI_DISP_NUM_FNT5) {
				//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, 10, pos+3);
				UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, -1, pos+2);
				UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp*-1, pos+0);
			}
			else {
				//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1000_POS, y, 10, pos+3);
				UI_DisplayNumber(font, x+DISP_NUM_5R_9P_100_POS, y, -1, pos+2);
				UI_DisplayNumber(font, x+DISP_NUM_5R_9P_10_POS, y, (num*-1)/10, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1_POS, y, temp*-1, pos+0);
			}
		}
		else {
			UI_DisplayNumber(font, x, y, 10, pos+2);
			if (font == UI_DISP_NUM_FNT5) {
				//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1000_POS, y, 10, pos+3);
				//UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, 10, pos+2);
				UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, num*-1, pos+0);
			}
			else {
				//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1000_POS, y, 10, pos+3);
				//UI_DisplayNumber(font, x+DISP_NUM_5R_9P_100_POS, y, 10, pos+2);
				UI_DisplayNumber(font, x+DISP_NUM_5R_9P_10_POS, y, -1, pos+1);
				UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1_POS, y, num*-1, pos+0);
			}
		}
	}
}

void UI_DisplayDecimal_3UnitZero(char font, uint8_t pos, int x, int y, short num)
{
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,91,10\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,103,10\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,115,10\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,127,10\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,139,10\r"); 
	char temp, temp2;
        //short temp3, temp4;

	if (font == UI_DISP_NUM_FNT5) {	
		if (pos > DISP_NUM_SDISP_SAME_TIME) return;
	}
	else {
		if (pos > DISP_NUM_DISP_SAME_TIME) return;
	}

	if(num > 32766){
		return;
		}
	else if (num < 1000) {
		temp = num%100;
		temp2 = temp%10;
		//UI_DisplayNumber(font, x, y, 0, pos+3);
		if (font == UI_DISP_NUM_FNT5) {
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, num/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_10_POS, y,temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_1_POS, y, temp2, pos+0);
		}
		else {
			UI_DisplayNumber(font, x+DISP_NUM_5R_5P_100_POS, y, num/100, pos+2);
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_10_POS, y,temp/10, pos+1);
			UI_DisplayNumber(font, x+DISP_NUM_5R_9P_1_POS, y, temp2, pos+0);
		}
	}
	else if (num > 999) {
	}
	else if(num < -9999){
		}
	else if(num < -999){
		}

}

void UI_InitSetupVariable(void)
{
	const uint8_t defaultName[] = "no name ";
	int i;
	
	Setup3.ProChk = BST_UNCHECKED;
	Setup3.LPChk = BST_UNCHECKED;
	Setup3.VibChk = BST_UNCHECKED;
	Setup2.vol = 4;		// 4 max
	Setup2.bright = 4;		// 4 max				
	//Setup.speed = 3;
	//Setup2.sndGuid2  = BST_CHECKED;
	//Setup2.led_en = BST_CHECKED;						   							 
	Setup3.PDeg = 5;	
	Setup3.PTm = 5;//1;	//pjg<>200205
	Setup3.LP = 3;
	Setup3.VDeg = 3;
	Setup3.VCnt = 3;
	Setup3.sensitivity = 2;//4; //15kg
	//Setup2.sensitivity = 4;	
	//Setup2.hiAngle = 0;
	#ifdef USE_DEBUG_MODE
	Setup3.IPos = -5;
	#else
	Setup3.IPos = HOME_IN_INIT_POS; //pjg<>190830 : when man press machine, motor move too fast. so gear is broken.
	#endif
	//Setup3.AngChkSens = 2;
    Setup3.amp.gapBk = 2;
    Setup3.amp.upOffset = 6;//2;
    Setup3.amp.alpha = 0.019;//0.08;
	AngChk.gap = Setup3.amp.gapBk;
	AngChk.gapBk = Setup3.amp.gapBk;
	AngChk.upOffset = Setup3.amp.upOffset;
	AngChk.alpha = Setup3.amp.alpha;
	Setup3.AngBtnStep = 1;//5; //pjg<>200902 by ask park su hyun
	Setup3.AutoAngValue = 5;
	Setup3.amSens = 2; //pjg++191217
    //Setup2.language = LT_ENG;
	//Setup_ChkDataSave = 0;
	PiChange_Wnd.PrePressedNum = PATIENT_NUM;//255;
	PInfoWnd2.id = 0;//PATIENT_NUM;
	PInfoWnd2.pi.pwd = 0xffff;
	PiChange_Wnd.pageNum = 0;
	PiChange_Wnd.PrePressedPageNum	= 255;
	for (i = 0; i < 8; i++) PInfoWnd2.pi.name[i] = defaultName[i];
	PInfoWnd2.pi.birthday.year = 2018;
	PInfoWnd2.pi.birthday.month = 6;
	PInfoWnd2.pi.birthday.day = 22;
}

void UI_InitSystemVariable(void)
{
	int i,j;
	uint8_t *ptr;
	
	for (i = 0; i < sizeof(TLCD_BLInfo); i++) 
		BLInfoBuf[i] = TLCD_BLInfo[i];
	BLInfoBuf[7] = 0;
	for (j = 0; j < sizeof(TLCD_SndInfo); j++) 
		SVInfoBuf[j] = TLCD_SndInfo[j];
	SVInfoBuf[7] = 0;

	BLVolTbl[0] = 15;
	BLVolTbl[1] = 30;
	BLVolTbl[2] = 50;
	BLVolTbl[3] = 65;
	BLVolTbl[4] = 80;
	
	//SndVolTbl[0] = 0;
	SndVolTbl[0] = 195;
	SndVolTbl[1] = 210;
	SndVolTbl[2] = 230;
	SndVolTbl[3] = 245;
	SndVolTbl[4] = 255;

	UI_Time.time.ms = 0;
	UI_Time.time.sec = 0;
	UI_Time.time.min = 0;
	UI_Time.time.hour = 0;
	UI_Time.tmp_ms = 0;
	UI_Time.tmp_sec = 0;

	SpdTmWnd_bk_speed = 0x7fff;

	//for (i = 0; i < sizeof(AngPicBlue); i++) 
	//	AngPicBlueBuf[i] = AngPicBlue[i];

	//for (i = 0; i < sizeof(AngPicIndigo); i++) 
	//	AngPicIndigoBuf[i] = AngPicIndigo[i];

	ptr = (uint8_t *)&loginInfo;
	for (i = 0; i < sizeof(loginInfo); i++) {
		ptr[i] = 0;
	}
	for (i = 0; i < 10; i++) {
		loginInfo.pwdBuf[i] = 0xff;
	}
}

void UI_InitVariable(void)
{
	//int i;
	
  //»Ø¿⁄¡§∫∏ √¢¿« ∫Øºˆ √ ±‚»≠
	//PInfoWnd2.id = 1;
	PInfoWnd2.pi.totalRepeat = 0;
	PInfoWnd2.pi.totalTime = 0;
    //∞¢µµº¬∆√¿« ∫Øºˆ √ ±‚»≠
	AngleWnd.exAngle = 0;
	AngleWnd.flAngle = 40;
    //º”µµ/Ω√∞£ º¬∆√¿« ∫Øºˆ √ ±‚»≠
	SpdTmWnd.speed = MS_SPEED_MAX;
	SpdTmWnd.time = 30;
    //±∏µø √¢¿« ∫Øºˆ √ ±‚»≠
	RunWnd.repeat = 0;
       Total_Counter = 0;
	RunWnd.play = UI_RUN_MODE_STOP;
	RunWnd.dir = 0;
	//RunWnd.angle = AngleWnd.exAngle;
   //    RunWnd.angle = Current_Angle - 5;
          
	RunWnd.tick = 0;
	RunWnd.time = 0;
	//Setup2.vol = 5;
	//Setup2.sensitivity = 1;        
       //Setup.speed = 3;    
 	//loginInfo.type = LIT_GUEST;
	Exercise.dataCnt = 0;
	
}

void UI_PIVariable(void)
{
	AngleWnd.exAngle = 0;
	AngleWnd.flAngle = 40;
	SpdTmWnd.speed = 3;
	SpdTmWnd.time = 30;
	Setup3.ProChk = BST_UNCHECKED;
	Setup3.LPChk = BST_UNCHECKED;
	Setup3.VibChk = BST_UNCHECKED;
	Setup2.vol = 4;		
	Setup2.bright = 4;										   							 
	Setup3.PDeg = 5;	
	Setup3.PTm = 1;	
	Setup3.LP = 3;
	Setup3.VDeg = 3;
	Setup3.VCnt = 3;
	Setup3.sensitivity = 4;
	Setup3.IPos = HOME_IN_INIT_POS;
	Setup3.writeTime = 0;
	Setup3.amSens = 3;
}

void UI_InitSensitivityValue(void)
{
	int temp;
	int i, j;

	temp = 0;
	/*Motor_OverCurTbl[0][0] = 540-temp;//500;//400;	//20kg
	Motor_OverCurTbl[0][1] = 440-temp;//410;//340;	//15kg
	Motor_OverCurTbl[0][2] = 410-temp;//350;//260;	//10kg
	Motor_OverCurTbl[0][3] = 400-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[0][4] = 980-temp;//980;//500;

	Motor_OverCurTbl[1][0] = 640-temp;//500;//400;	//20kg
	Motor_OverCurTbl[1][1] = 510-temp;//410;//340;	//15kg
	Motor_OverCurTbl[1][2] = 470-temp;//350;//260;	//10kg
	Motor_OverCurTbl[1][3] = 420-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[1][4] = 980-temp;//980;//500;

	Motor_OverCurTbl[2][0] = 800-temp;//500;//400;	//20kg
	Motor_OverCurTbl[2][1] = 590-temp;//410;//340;	//15kg
	Motor_OverCurTbl[2][2] = 520-temp;//350;//260;	//10kg
	Motor_OverCurTbl[2][3] = 420-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[2][4] = 980-temp;//980;//500;

	//not use from 171221
	Motor_OverCurTbl[3][0] = 750-temp;//500;//400;	//20kg
	Motor_OverCurTbl[3][1] = 670-temp;//410;//340;	//15kg
	Motor_OverCurTbl[3][2] = 530-temp;//350;//260;	//10kg
	Motor_OverCurTbl[3][3] = 540-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[3][4] = 980-temp;//980;//500;

	Motor_OverCurTbl[4][0] = 840-temp;//500;//400;	//20kg
	Motor_OverCurTbl[4][1] = 770-temp;//410;//340;	//15kg
	Motor_OverCurTbl[4][2] = 660-temp;//350;//260;	//10kg
	Motor_OverCurTbl[4][3] = 540-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[4][4] = 980-temp;//980;//500;
	*/
	for (i = 0 ; i < SL_LEVEL_MAX; i++) {
		for (j = 0 ; j < MS_SPEED_MAX; j++) {
			Motor_OverCurTbl[i][j] = SensDefault[i][j]-temp;
		}
	}

}

//pjg++190820
void UI_InitOneTime(void)
{
	uint8_t *ptr;
	int i,j,k;

	AngChk.gap = 2;
	AngChk.upOffset = 6;//2;
	AngChk.gapBk = AngChk.gap;
	//AngChk.prev = 0;
	AngChk.runOne = 0;
	AngChk.alpha = 0.019;//0.029;//0.08;	
	AngChk.mode = 0;
	for (i = 0; i < AMD_MAX; i++) {
		for (j = 0; j < AMSL_LEVEL_MAX; j++) {
			for (k = 0; k < AMVT_MAX; k++) {
				amSensitivity[i][j][k] = amSensDefault[i][j][k];
			}
		}
	}
	//SensChk = &AngChk;
	Option.curDisp = 1;
	RunWnd.complete = 0;
	loading = 0;
	ptr = (uint8_t *)&SaveExeInfoV2;
	for (i = 0; i < sizeof(SAVE_EXERCISE_INFO_V2); i++) ptr[i] = 0;
	EEPROM.freesize = 0;
	EEPROM.totalsize = 1;
	FullInfo.type = FT_PI;
	FullInfo.status = FS_EMPTY;
	Setup3.writeTime = 0;
	Setup3.amSens = 3;
}

char *UI_GetSndInfo(uint8_t num)
{
	return pSndInfo[num][0];
}

//pjg++180312
void UI_SetSensitivityByCalibrationValue(uint16_t  __packed value[][MS_SPEED_MAX])
{
	//uint16_t temp;
	int i, j;

	//temp = 0;

	/*/
	//speed 1
	//Motor_OverCurTbl[0][0] = (value[MS_SPEED1]-gap)-temp;//500;//400;	
	Motor_OverCurTbl[MS_SPEED1][SL_LEVEL1] = (value[MS_SPEED1]-36)-temp; //20kg
	Motor_OverCurTbl[MS_SPEED1][SL_LEVEL2] = (value[MS_SPEED1]-9)-temp;	//15kg
	Motor_OverCurTbl[MS_SPEED1][SL_LEVEL3] = (value[MS_SPEED1]-19)-temp;	//10kg
	Motor_OverCurTbl[MS_SPEED1][SL_LEVEL4] = (value[MS_SPEED1]-14)-temp;//5kg
	
	//speed 2
	Motor_OverCurTbl[MS_SPEED2][SL_LEVEL1] = (value[MS_SPEED2]-43)-temp;	//20kg
	Motor_OverCurTbl[MS_SPEED2][SL_LEVEL2] = (value[MS_SPEED2]-32)-temp;	//15kg
	Motor_OverCurTbl[MS_SPEED2][SL_LEVEL3] = (value[MS_SPEED2]-23)-temp;	//10kg
	Motor_OverCurTbl[MS_SPEED2][SL_LEVEL4] = (value[MS_SPEED2]-13)-temp;	//5kg

	//speed 3
	Motor_OverCurTbl[MS_SPEED3][SL_LEVEL1] = (value[MS_SPEED3]-47)-temp;	//20kg
	Motor_OverCurTbl[MS_SPEED3][SL_LEVEL2] = (value[MS_SPEED3]-35)-temp;	//15kg
	Motor_OverCurTbl[MS_SPEED3][SL_LEVEL3] = (value[MS_SPEED3]-37)-temp;	//10kg
	Motor_OverCurTbl[MS_SPEED3][SL_LEVEL4] = (value[MS_SPEED3]-19)-temp;	//5kg

	//speed 4
	//not use from 171221
	Motor_OverCurTbl[MS_SPEED4][SL_LEVEL1] = (value[MS_SPEED4]-55)-temp;	//20kg
	Motor_OverCurTbl[MS_SPEED4][SL_LEVEL2] = (value[MS_SPEED4]-46)-temp;	//15kg
	Motor_OverCurTbl[MS_SPEED4][SL_LEVEL3] = (value[MS_SPEED4]-47)-temp;	//10kg
	Motor_OverCurTbl[MS_SPEED4][SL_LEVEL4] = (value[MS_SPEED4]-26)-temp;	//5kg
	
	Motor_OverCurTbl[4][0] = 840-temp;//500;//400;	//20kg
	Motor_OverCurTbl[4][1] = 770-temp;//410;//340;	//15kg
	Motor_OverCurTbl[4][2] = 660-temp;//350;//260;	//10kg
	Motor_OverCurTbl[4][3] = 540-temp;//270;//200;	//5kg
	//Motor_OverCurTbl[4][4] = 980-temp;//980;//500;
	*/

#ifdef USE_CALIB_ONLY_0KG
	for (j = 0; j < MS_SPEED_MAX; j++) {
        Product_Calib.maxCurrent[0][j] = value[0][j];
    }

	for (i = 0; i < SL_LEVEL_MAX; i++) {
		for (j = 0; j < MS_SPEED_MAX; j++) {
			#if 1 //200129 : avg of 10 set
			if (j == 0) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)19.717*(float)(i+1)*(float)(i+1) +
														25.997*(float)(i+1) + 3.25);
			}
			else if (j == 1) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)11.333*(float)(i+1)*(float)(i+1) +
														93.627*(float)(i+1) + -24.9);
			}
			else if (j == 2) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)5.2667*(float)(i+1)*(float)(i+1) +
														139.6*(float)(i+1) + -37.8);
			}
			/*200125
			if (j == 0) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)19.125*(float)(i+1)*(float)(i+1) +
														21.325*(float)(i+1) + 3.125);
			}
			else if (j == 1) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)15.688*(float)(i+1)*(float)(i+1) +
														69.137*(float)(i+1) + -9.9375);
			}
			else if (j == 2) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)9.5*(float)(i+1)*(float)(i+1) +
														118.3*(float)(i+1) + -28.375);
			}
			*/
			/*200110
			if (j == 0) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)12.143*(float)(i+2)*(float)(i+2) +
														17.543*(float)(i+2) + -33.6);
			}
			else if (j == 1) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)9.6429*(float)(i+2)*(float)(i+2) +
														63.443*(float)(i+2) + -79.0);
			}
			else if (j == 2) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)12.071*(float)(i+2)*(float)(i+2) +
														73.071*(float)(i+2) + -88.6);
			}*/
			#else //old version set(no foot board, aliminum link)
			if (j == 0) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)11*(float)(i+2)*(float)(i+2) +
														-14.6*(float)(i+2) + 4);
			}
			else if (j == 1) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)9*(float)(i+2)*(float)(i+2) +
														23.0*(float)(i+2) + -32.0);
			}
			else if (j == 2) {
				Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[0][j] + (uint16_t)((float)5.75*(float)(i+2)*(float)(i+2) +
														56.15*(float)(i+2) + -62.25);
			}
			#endif
               Product_Calib.maxCurrent[i+1][j] = Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j];
		}
	}
#else
	for (i = 0; i < SL_LEVEL_MAX; i++) {
		for (j = 0; j < MS_SPEED_MAX; j++) {
			Product_Calib.maxCurrent[i][j] = value[i][j];
			if (i != 0) Motor_OverCurTbl[SL_LEVEL_MAX-i-1][j] = value[i][j];//-temp;
		}
	}
#endif

}

void UI_PINumber_Display(void)
{
#ifdef PI_LOAD_V2
	uint8_t buf[20];
	uint32_t temp;//, temp2;

	if (loginInfo.type == LIT_GUEST) {
		if(Setup2.language == LT_KOR)
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guek.bmp,93,6\r");
		else if(Setup2.language == LT_CHINA)
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guec.bmp,87,6\r");
		else
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gue.bmp,93,11\r");
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,91,10\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,103,10\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,115,10\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,127,10\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,139,10\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,151,10\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,163,10\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i wbnk.bmp,176,10\r");
		temp = PInfoWnd2.id;
		buf[7] = temp/10000000;
		temp = temp%10000000;
		buf[6] = temp/1000000;
		temp = temp%1000000;
		buf[5] = temp/100000;
		temp = temp%100000;
		buf[4] = temp/10000;
		temp = temp%10000;
		buf[3] = temp/1000;
		temp = temp%1000;
		buf[2] = temp/100;
		temp = temp%100;
		buf[1] = temp/10;
		temp = temp%10;
		buf[0] = temp;
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS, 10, buf[7], 0);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*2, 10, buf[6], 1);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*3, 10, buf[5], 2);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*4, 10, buf[4], 3);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*5, 10, buf[3], 4);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*6, 10, buf[2], 5);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*7, 10, buf[1], 6);
		UI_DisplayNumber(UI_DISP_NUM_FNT5, 93+DISP_NUM_5R_5P_1000_POS*8, 10, buf[0], 7);
	}

#else
	if (PiChange_Wnd.pageNum == 0){
		if(PInfoWnd2.id == PATIENT_NUM)
		{
			if(Setup2.language == LT_KOR)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guek.bmp,93,6\r");
			else if(Setup2.language == LT_CHINA)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guec.bmp,87,6\r");
			else
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gue.bmp,93,11\r");
		}
		else
			UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PInfoWnd2.id+1);
	}
	else if (PiChange_Wnd.pageNum == 1){
		if(PInfoWnd2.id == PATIENT_NUM)
		{
			if(Setup2.language == LT_KOR)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guek.bmp,93,6\r");
			else if(Setup2.language == LT_CHINA)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guec.bmp,87,6\r");
			else
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gue.bmp,93,11\r");
		}
		else
			UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PInfoWnd2.id+31);
	}
		
	else if (PiChange_Wnd.pageNum == 2){
		if(PInfoWnd2.id == PATIENT_NUM)
		{
			if(Setup2.language == LT_KOR)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guek.bmp,93,6\r");
			else if(Setup2.language == LT_CHINA)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guec.bmp,87,6\r");
			else
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gue.bmp,93,11\r");
		}
		else
			UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PInfoWnd2.id+61);
	}
	
	else{
		if(PInfoWnd2.id == PATIENT_NUM)
		{
			if(Setup2.language == LT_KOR)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guek.bmp,93,6\r");
			else if(Setup2.language == LT_CHINA)
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i guec.bmp,87,6\r");
			else
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gue.bmp,93,11\r");
		}
		else
			UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PInfoWnd2.id+91);
	}
#endif
}

void UI_ProcessNull(void)
{
}

void UI_Pwdforget_OnBnClickedBtnYes(void)
{
	loginInfo.type = LIT_ADMIN2;
	UI_Login_Create();
}

void UI_Pwdforget_OnBnClickedBtnNo(void)
{
#ifdef PI_LOAD_V1
	if(PiChange_Wnd.PrePressedNum < 21)	UI_PiLoad_Create();
	else if(PiChange_Wnd.PrePressedNum < 42)	UI_PiLoad2_Create();
	else if(PiChange_Wnd.PrePressedNum < 63)	UI_PiLoad3_Create();
	else	UI_PiLoad4_Create();
#else
	UI_PiLoad_Create();
#endif
}


void UI_Pwdforget_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_PWDF_BTN_YES*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PWDF_BTN_YES*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PWDF_BTN_YES*2], ',', 2),
			113, 46, hParent, RID_PWDF_BTN_YES, 0);
	API_CreateWindow("", pBtnInfo[RID_PWDF_BTN_NO*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PWDF_BTN_NO*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PWDF_BTN_NO*2], ',', 2),
			113, 46, hParent, RID_PWDF_BTN_NO, 0);
	UI_LED_Control(LM_NONE);
}

LRESULT UI_Pwdforget_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Pwdforget_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_PWDF_BTN_YES:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pwdforget_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_PWDF_BTN_NO:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pwdforget_OnBnClickedBtnNo();
				break;
			}
			return 0;                                        
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_Pwdforget_Create(void)
{
	App_SetWndProcFnPtr(UI_Pwdforget_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i fogpwdbg.png,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.num = UI_WND_MODE_PWD_FORGET;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

////////////////////////////////////////login//////////////////////////////////////////

int UI_Login_SavePassword(uint8_t id, uint8_t *buf, uint8_t len)
{
	uint32_t pi_startAddr;
	uint16_t exTotalSize;
	uint32_t addr;
	uint16_t pwd;
	uint8_t tbuf[5];
	uint8_t tmpbuf[2];
	int i;

	if (id > PATIENT_NUM) return 0;
	//if (type >= LIT_MAX) return 0;
	
	exTotalSize = sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM;
	pi_startAddr = sizeof(SYSTEM_INFO) + (sizeof(PI_PARAM) + exTotalSize)*id;
	addr = pi_startAddr + 21;
	pwd = 0;
	for (i = 0; i < len; i++) {
		pwd += buf[i]*Multiplier(10, len-i-1);
	}
	tmpbuf[0] = (uint8_t)(pwd&0xff);
	tmpbuf[1] = (uint8_t)(pwd>>8);
	for (i = 0; i < 2; i++) {
		tbuf[0] = tmpbuf[i];
		//if (!EEPROMDrv_WriteByte(0, addr, tbuf, 2)) return 0;
		if (!EEPROMDrv_WriteByte(0, addr+i, tbuf[0])) return 0;
	}
	PInfoWnd2.pi.pwd = pwd;
	return 1;
}

void UI_Login_DisplayPassword()
{    
	if(loginInfo.type == LIT_USER){	
		if(loginInfo.cnt == 0){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,122\r");
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,122,loginInfo.pwdBuf[0],1);
		}
		if(loginInfo.cnt == 1){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,122,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,58,122\r");	
		}	
		if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,122,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,82,122\r");
		}
		if (loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,88,122,loginInfo.pwdBuf[3],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,106,122\r");
		}
	}
	else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
		if(loginInfo.cnt == 0){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,49,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,73,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,97,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,121,122\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,145,122\r");
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,6,122,loginInfo.pwdBuf[0],1);
		}
		if(loginInfo.cnt == 1){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,30,122,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,48,122\r");	
		}	
		if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,54,122,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,72,122\r");
		}
		if (loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,78,122,loginInfo.pwdBuf[3],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,96,122\r");
		}
		if(loginInfo.cnt == 4){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,102,122,loginInfo.pwdBuf[4],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,120,122\r");	
		}	
	}
	else{
		if(loginInfo.cnt == 0){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,88\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,88\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,88\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,88\r");
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,88,loginInfo.pwdBuf[0],1);
		}
		if(loginInfo.cnt == 1){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,88,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,58,88\r");	
		}	
		if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,88,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,82,88\r");
		}
		if (loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,88,88,loginInfo.pwdBuf[3],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,106,88\r");
		}
		if(loginInfo.cnt == 4){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,155\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,155\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,155\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,155\r");
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,155,loginInfo.pwdBuf[4],1);
		}	
		if(loginInfo.cnt == 5){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,155,loginInfo.pwdBuf[5],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,58,155\r");
		}
		if (loginInfo.cnt == 6){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,155,loginInfo.pwdBuf[6],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,82,155\r");
		}
		if (loginInfo.cnt == 7){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,88,155,loginInfo.pwdBuf[7],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdd.bmp,106,155\r");
		}
	}
}

void UI_Login_OnBnClickedBtnNUM1(void)
{
	if(loginInfo.trycnt == 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 1;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
	/*if(loginInfo.cnt < 5)
		loginInfo.cnt = loginInfo.cnt+1;
	else
		loginInfo.cnt = 0;
		
	if(loginInfo.cnt == 1)
		loginInfo.pwdBuf[0] = 1;
	else if(loginInfo.cnt == 2)
		loginInfo.pwdBuf[1] = 1;
	else if(loginInfo.cnt == 3)
		loginInfo.pwdBuf[2] = 1;
	else
		loginInfo.pwdBuf[3] = 1;*/
}

void UI_Login_OnBnClickedBtnNUM2(void)
{	
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 2;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
	
}

void UI_Login_OnBnClickedBtnNUM3(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 3;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM4(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 4;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM5(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 5;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM6(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 6;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM7(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 7;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM8(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 8;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM9(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 9;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnNUM0(void)
{
	if(loginInfo.trycnt > 5)
	{
		if(loginInfo.type == LIT_USER)	UI_Pwdforget_Create();
		else{
			API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
			return;
		}
	}
	else{
		loginInfo.pwdBuf[loginInfo.cnt] = 0;
		UI_Login_DisplayPassword();
		loginInfo.cnt++;
		if(loginInfo.type == LIT_USER){
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN) loginInfo.cnt = LOGIN_USER_PWD_LEN;
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if (loginInfo.cnt >= LOGIN_ADMIN_PWD_LEN) loginInfo.cnt = LOGIN_ADMIN_PWD_LEN;
		}
		else{
			if (loginInfo.cnt >= LOGIN_USER_PWD_LEN*2) loginInfo.cnt = LOGIN_USER_PWD_LEN*2;
		}
	}
}

void UI_Login_OnBnClickedBtnBackspace(void)
{
	if(loginInfo.type == LIT_USER){
		if(loginInfo.cnt == 4){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,122,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,122\r");
			loginInfo.cnt = 3;
		}
		else if(loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,122,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,122\r");
			loginInfo.cnt = 2;
		}
		else if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,122,loginInfo.pwdBuf[0],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,122\r");
			loginInfo.cnt = 1;
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,122\r");
			loginInfo.cnt = 0;
		}
	}
	else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
	 	if(loginInfo.cnt == 5){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,78,122,loginInfo.pwdBuf[3],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,145,122\r");
			loginInfo.cnt = 4;
		}
		else if(loginInfo.cnt == 4){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,54,122,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,121,122\r");
			loginInfo.cnt = 3;
		}
		else if(loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,30,122,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,97,122\r");
			loginInfo.cnt = 2;
		}
		else if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,6,122,loginInfo.pwdBuf[0],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,73,122\r");
			loginInfo.cnt = 1;
			
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,122\r");
			loginInfo.cnt = 0;
		}
	}
	else{
		// 1π¯¡Ÿ 
		if(loginInfo.cnt == 4){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,88,loginInfo.pwdBuf[2],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,88\r");
			loginInfo.cnt = 3;
		}
		else if(loginInfo.cnt == 3){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,88,loginInfo.pwdBuf[1],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,88\r");
			loginInfo.cnt = 2;
		}
		else if(loginInfo.cnt == 2){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,88,loginInfo.pwdBuf[0],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,88\r");
			loginInfo.cnt = 1;	
		}
		else if(loginInfo.cnt == 1){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,88\r");
			loginInfo.cnt = 0;	
		}
		// 2π¯¡Ÿ 
		else if(loginInfo.cnt == 5){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,88,88,loginInfo.pwdBuf[3],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,155\r");
			loginInfo.cnt = 4;
		}
		else if(loginInfo.cnt == 6){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,16,155,loginInfo.pwdBuf[4],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,155\r");
			loginInfo.cnt = 5;
		}
		else if(loginInfo.cnt == 7){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,40,155,loginInfo.pwdBuf[5],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,155\r");
			loginInfo.cnt = 6;
		}
		else if(loginInfo.cnt == 8){
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,64,155,loginInfo.pwdBuf[6],1);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,155\r");
			loginInfo.cnt = 7;
		}
	}
}

void UI_Login_OnBnClickedBtnOK(void)
{
	int i;
	uint32_t pwd;
	uint8_t is_same;

	pwd = 0;
	if(loginInfo.trycnt > 5){
		API_SetErrorCode(EC_LOGIN_PWD_ERR, EDT_DISP_HALT);
		return;
	}
	if(loginInfo.cnt == 0)
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i inpwd.bmp,10,186\r");
	else{
		if(loginInfo.type == LIT_USER){
			if(loginInfo.cnt == 4){
				for (i = 0; i < LOGIN_USER_PWD_LEN; i++) {
					pwd += loginInfo.pwdBuf[i]*Multiplier(10, LOGIN_USER_PWD_LEN-i-1);
				}
				if(PInfoWnd2.pi.pwd == (uint32_t)pwd)	UI_Home_Create();//UI_Angle_Create();
				else{
					loginInfo.cnt = 0;
					loginInfo.trycnt++;

					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i icpwd.bmp,10,186\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,122\r");
					
				}
			}
			else if(loginInfo.cnt > 0 || loginInfo.cnt < 4){
				loginInfo.cnt = 0;
				loginInfo.trycnt++;

				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i icpwd.bmp,10,186\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,122\r");
			}
		}
		else if(loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2){
			if(loginInfo.cnt == 5){
				for (i = 0; i < LOGIN_ADMIN_PWD_LEN; i++) {
					pwd += loginInfo.pwdBuf[i]*Multiplier(10, LOGIN_ADMIN_PWD_LEN-i-1);
				}
				is_same = 0;
				if(loginInfo.type == LIT_ADMIN1) {
					if(password[PWDT_PRODUCT] == pwd){
						UI_Product_Create();
						is_same = 1;
					}
				}
				else{
					if(SysInfo.adminPwd == pwd){
						loginInfo.type = LIT_SET_PWD;
						UI_Login_Create();
						is_same = 1;
					}
				}		
				if (!is_same) {
					loginInfo.cnt = 0;
					loginInfo.trycnt++;

					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i icpwd.bmp,10,186\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,49,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,73,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,97,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,121,122\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,145,122\r");
				}
			}
			else if(loginInfo.cnt > 0 || loginInfo.cnt < 5){
				loginInfo.cnt = 0;
				loginInfo.trycnt++;

				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i icpwd.bmp,10,186\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,49,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,73,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,97,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,121,122\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,145,122\r");
			}
		}
		else{ //LIT_SET_PWD
			if(loginInfo.cnt == 8){
				if((loginInfo.pwdBuf[0] == loginInfo.pwdBuf[4]) && (loginInfo.pwdBuf[1] == loginInfo.pwdBuf[5]) && 
				   (loginInfo.pwdBuf[2] == loginInfo.pwdBuf[6]) && (loginInfo.pwdBuf[3] == loginInfo.pwdBuf[7])){
				   	loginInfo.type = LIT_USER;
					/*loginInfo.pwdBuf[0] = 1;
					loginInfo.pwdBuf[1] = 2;
					loginInfo.pwdBuf[2] = 3;
					loginInfo.pwdBuf[3] = 4;*/
					//UI_Login_SavePassword(PInfoWnd2.id, (uint8_t *)loginInfo.pwdBuf, 4);
					pwd = 0;
					for (i = 0; i < LOGIN_USER_PWD_LEN; i++) {
						pwd += loginInfo.pwdBuf[i]*Multiplier(10, LOGIN_USER_PWD_LEN-i-1);
					}
					SysInfo.adminPwd = (uint16_t)pwd;
					//UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
					//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
					UI_SaveSystemInfoToFlash((char *)CommonBuf);
					UI_Login_Create();
				}
				else
				{
					loginInfo.cnt = 0;
					loginInfo.trycnt++;
			
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmpwd.bmp,10,186\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,88\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,88\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,88\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,88\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,155\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,155\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,155\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,155\r");
				}
			}
			else if(loginInfo.cnt > 0 || loginInfo.cnt < 8){
				loginInfo.cnt = 0;
				loginInfo.trycnt++;
		
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmpwd.bmp,10,186\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,88\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,88\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,88\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,88\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,59,155\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,83,155\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,107,155\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,131,155\r");
			}
		}
		for(int j = 1; j<7; j++)
		{
			loginInfo.pwdBuf[j] = 0;
		}
	}
}


void UI_Login_OnBnClickedBtnCancel(void)
{
	if(loginInfo.type == LIT_ADMIN1){
		//longKeyEventCnt = 0;
	   	UI_SystemInit_Create();
	}
	else{
		#ifdef PI_LOAD_V1
		if(PiChange_Wnd.PrePressedNum < 21)	UI_PiLoad_Create();
		else if(PiChange_Wnd.PrePressedNum < 42)	UI_PiLoad2_Create();
		else if(PiChange_Wnd.PrePressedNum < 63)	UI_PiLoad3_Create();
		else	UI_PiLoad4_Create();
		#else
		UI_PiLoad_Create();
		#endif
	}
	loginInfo.cnt = 0;
}


void UI_Login_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_NUM0*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM0*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_NUM0*2], ',', 2),
			46, 53, hParent, RID_LOG_BTN_NUM0, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_BACKSP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_BACKSP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_BACKSP*2], ',', 2),
			93, 49, hParent, RID_LOG_BTN_BACKSP, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_OK*2], ',', 2),
			113, 46, hParent, RID_LOG_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_LOG_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LOG_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LOG_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_LOG_BTN_CANCEL, 0);
	
	if (loginInfo.type == LIT_USER) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usft.bmp,36,87\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwlcd.bmp,36,117\r");
		UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
		loginInfo.pwdBuf[0] = '0';
	}
	else if (loginInfo.type == LIT_ADMIN1 || loginInfo.type == LIT_ADMIN2) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i admft.bmp,36,87\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwlcd.bmp,36,117\r");
		loginInfo.pwdBuf[0] = '0';
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i spwdft.bmp,36,58\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwlcd.bmp,36,83\r");	
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i scpwdft.bmp,36,127\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwlcd.bmp,36,150\r");
		loginInfo.pwdBuf[0] = '0';
	}
	UI_LED_Control(LM_NONE);
	loginInfo.cnt = 0;
	loginInfo.trycnt = 0;
	UI_PINumber_Display();
}

LRESULT UI_Login_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Login_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_LOG_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push
				UI_Login_OnBnClickedBtnNUM1();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM2();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM3();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM4();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM5();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM6();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push
				UI_Login_OnBnClickedBtnNUM7();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM8();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM9();
				break;
			}
			return 0;
		case RID_LOG_BTN_NUM0:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnNUM0();
				break;
			}
			return 0;
		case RID_LOG_BTN_BACKSP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnBackspace();
				break;
			}
			return 0;
		case RID_LOG_BTN_OK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnOK();
				break;
			}
			return 0;
		case RID_LOG_BTN_CANCEL:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Login_OnBnClickedBtnCancel();
				break;
			}
			return 0;                                        
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_Login_Create(void)
{
	App_SetWndProcFnPtr(UI_Login_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i pwdbg.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_LOGIN;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

/////////////////////////////////Product////////////////////////////////////////
void UI_Product_OnBnClickedBtnCali(void)
{
	UI_Calibration_Create();
}

void UI_Product_OnBnClickedBtnPCconnect(void)
{

}

void UI_Product_OnBnClickedBtnBack(void)
{
	//longKeyEventCnt = 0;
	UI_SystemInit_Create();
}

void UI_Product_OnBnClickedBtnAngChkChange(void)
{
}

void UI_Product_OnBnClickedBtnEEPROMFormat(void)
{
	EEPROMDisk_Link();
	EEPROMDisk_Mount();
	uart_putstring("f Format start(wait 12sec),50,210\n");
	if (!EEPROMDisk_Format()) {
		uart_putstring("f Format fail,50,230\n");
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_FORMAT, EDT_DISP_HALT);
	}
	else {
		uart_putstring("f Format complete,50,230\n");
	}

	if (!EEPROMDisk_MkDir(EEPROMDISK_DIR)) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MKDIR, EDT_DISP_HALT);
		//break;
	}
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();
}

void UI_Product_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_PROD_BTN_CALI*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PROD_BTN_CALI*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PROD_BTN_CALI*2], ',', 2),
			108,45, hParent, RID_PROD_BTN_CALI, 0);
	//API_CreateWindow("", pBtnInfo[RID_PROD_BTN_PC*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PROD_BTN_PC*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PROD_BTN_PC*2], ',', 2),
	//		108,45, hParent, RID_PROD_BTN_PC, 0);
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_BACK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_BACK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_BACK*2], ',', 2),
			108,30, hParent, RID_CALI_BTN_BACK, 0);
	API_CreateWindow("", pBtnInfo[RID_PROD_BTN_ANGCHK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PROD_BTN_ANGCHK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PROD_BTN_ANGCHK*2], ',', 2),
			154,47, hParent, RID_PROD_BTN_ANGCHK, 0);
	API_CreateWindow("", pBtnInfo[RID_PROD_BTN_ANGCHK2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PROD_BTN_ANGCHK2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PROD_BTN_ANGCHK2*2], ',', 2),
			154,47, hParent, RID_PROD_BTN_ANGCHK2, 0);

	UI_LED_Control(LM_NONE);
	//FlashDrv_SetTempBuf(&CommonBuf[FLASH_PAGE_SIZE16]);
	//FlashDrv_SetParam(FLASH_PAGE_SIZE16);												   
	//UI_SaveSystemInfoToFlash(CommonBuf); //test
	//CommonBuf[0] = 0;
	//CommonBuf[1] = 0;
	//CommonBuf[2] = 0;
	//UI_LoadSystemInfoFromFlash(CommonBuf);
}

LRESULT UI_Product_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Product_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_PROD_BTN_CALI:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnCali();
				break;
			}
			return 0;
		case RID_PROD_BTN_PC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnPCconnect();
				break;
			}
			return 0;
		case RID_CALI_BTN_BACK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnBack();
				break;
			}
			return 0;                                        
		case RID_PROD_BTN_ANGCHK:
			switch(lParam) {
			case BN_CLICKED: // push 
				//UI_Product_OnBnClickedBtnBack();
				break;
			}
			return 0;										 
		case RID_PROD_BTN_ANGCHK2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnEEPROMFormat();
				break;
			}
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_Product_Create(void)
{
	App_SetWndProcFnPtr(UI_Product_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i prodbg.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_PRODUCT;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

////////////////////////////Calibration///////////////////////////////////////////
void UI_Calibration_Timer(uint16_t nIDEvent)
{
	switch (Product_Calib.massStep) {
	case PMT_1STEP:
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 233,52, 
			Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		break;
	case PMT_2STEP:
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 233,97, 
			Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		break;
	case PMT_3STEP:
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 233,142, 
			Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		break;
	case PMT_4STEP:
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 233,187, 
			Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		break;
	case PMT_5STEP:
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 233,232, 
			Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		break;
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 380,143, RunWnd.angle, 2);
	
}

void UI_Product_Calibration_SetButtonStatus(void)
{
	Product_Calib.massStep++;
	switch (Product_Calib.massStep) {
	case PMT_1STEP:
		break;
	case PMT_2STEP:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,85\r");
			App_SetButtonOption(RID_CALI_BTN_CALI5, BN_PUSHED);
		break;
	case PMT_3STEP:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,131\r");
			App_SetButtonOption(RID_CALI_BTN_CALI10, BN_PUSHED);
		break;
	case PMT_4STEP:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,177\r");
			App_SetButtonOption(RID_CALI_BTN_CALI15, BN_PUSHED);
		break;
	case PMT_5STEP:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,224\r");
			App_SetButtonOption(RID_CALI_BTN_CALI20, BN_PUSHED);
		break;
	}
}
void UI_Product_Calibration_ButtonDisable(void)
{
	if(Product_Calib.massStep == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,85\r");
		App_SetButtonOption(RID_CALI_BTN_CALI5, BN_DISABLE);
	}
	else if(Product_Calib.massStep == 2)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,131\r");
		App_SetButtonOption(RID_CALI_BTN_CALI10, BN_DISABLE);
	}
	else if(Product_Calib.massStep == 3)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,177\r");
		App_SetButtonOption(RID_CALI_BTN_CALI15, BN_DISABLE);
	}
	else if(Product_Calib.massStep == 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,224\r");
		App_SetButtonOption(RID_CALI_BTN_CALI20, BN_DISABLE);
	}
}
	
void UI_Product_Process_Calibration_Complete(void)
{
	if (!UI_CheckMotorStatus()) return;

	if (MotorDrv_GetPosition() <= -2) {
		//complete msg display
		MotorDrv_Stop();
		//FlashDrv_SetTempBuf(&CommonBuf[FLASH_PAGE_SIZE16]);
		FlashDrv_SetTempBuf(CommonBuf);
		FlashDrv_SetParam(FLASH_PAGE_SIZE16);												   
		//UI_LoadSystemInfoFromFlash(CommonBuf);
		//HAL_GetUID(udi);
		udi[0] = HAL_GetUIDw0();
		udi[1] = HAL_GetUIDw1();
		udi[2] = HAL_GetUIDw2();
		UI_SaveSystemInfoToFlash(CommonBuf);
		//UI_Calibration_DisplayValue();
		UI_LED_Control(LM_Complet);
		uart_putstring("f complete,350,230\n");
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_CALIBRATION_COMPLETE);
		App_SetUIProcess(UI_ProcessNull);
	}
}

//#define TEST_CURRENT_CHECK
#ifdef TEST_CURRENT_CHECK
uint32_t test_cur_buf[5000];
int32_t test_angle_buf[5000];
uint32_t test_test_time[5000];
uint32_t test_cur_cnt =0;
uint32_t test_test_time1;
uint32_t test_check_cnt=0;
#define TEST_CHECK_TIME		12 // ms
#endif
void UI_Product_Process_Calibration_Measure(void)
{	
#if 1 //average
	if (!UI_Time.tmp4_ms) {
		Product_Calib.curCurrent += MotorDrv_GetCurrent();
		Product_Calib.cnt++;
		if (Product_Calib.cnt >= PROD_CAL_BUF_SIZE) {
			Product_Calib.curCurrent /= Product_Calib.cnt;
			//Product_Calib.oldCurrent = Product_Calib.curCurrent;
			if (Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1] < Product_Calib.curCurrent) {
				Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1] = Product_Calib.curCurrent;
				Product_Calib.deg[Product_Calib.massStep][Product_Calib.motorSpeed-1] = MotorDrv_GetPosition();
			}
			Product_Calib.curCurrent = 0;
			Product_Calib.cnt = 0;
		}
	}
#else //same value find
	if (MotorDrv_GetCurrent() > Product_Calib.maxCurrent[Product_Calib.motorSpeed-1]) {
		Product_Calib.curCurrent = MotorDrv_GetCurrent();
		if (Product_Calib.curCurrent == Product_Calib.oldCurrent) {
			if (Product_Calib.sameCnt < 0xfffe) Product_Calib.sameCnt++;
			if (Product_Calib.sameCnt > 20) {
				if (Product_Calib.maxCurrent[Product_Calib.motorSpeed-1] < Product_Calib.curCurrent)
				Product_Calib.maxCurrent[Product_Calib.motorSpeed-1] = Product_Calib.curCurrent;
			}
		}
		Product_Calib.oldCurrent = Product_Calib.curCurrent;
	}
#endif

	if (MotorDrv_GetPosition() <= -2) {
		MotorDrv_SetTargetPosition(-2);
		//MotorDrv_StopNoCtrl();
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition()); //pjg++180402 quickly stop
		MotorDrv_Stop();		

		if (Product_Calib.motorSpeed == 1) {
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 155,10, 
				Product_Calib.deg[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 185,10, 
				Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		}
		else if (Product_Calib.motorSpeed == 2) {
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 235,10, 
				Product_Calib.deg[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_4PLACE, 265,10, 
				Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		}
		else if (Product_Calib.motorSpeed == 3) {
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_5PLACE, 315,10, 
				Product_Calib.deg[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
			UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_6PLACE, 345,10, 
				Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1]);
		}
		Product_Calib.motorSpeed++;
		if (Product_Calib.motorSpeed > MS_SPEED_MAX) {
			Product_Calib.motorSpeed = 1;
			#ifdef USE_CALIB_ONLY_0KG
			App_SetUIProcess(UI_Product_Process_Calibration_Complete);
			App_KillTimer(TIMER_ID_5);
			#else
			if (Product_Calib.massStep >= PMT_5STEP) {
				App_SetUIProcess(UI_Product_Process_Calibration_Complete);
				App_KillTimer(TIMER_ID_5);
			}
			else {
				UI_Product_Calibration_SetButtonStatus();
				App_SetUIProcess(UI_ProcessNull);
			}
			#endif
			return;
		}
		else {
			MotorDrv_SetTargetPosition(50);
			MotorDrv_SetSpeed(4); //fast move up
			App_SetUIProcess(UI_Product_Process_Calibration_GoToStartPos);
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 370,208, Product_Calib.motorSpeed, 1);
			return;
		}
		//App_SetUIProcess(UI_ProcessNull);
		//UI_Calibration_DisplayValue();
#ifdef TEST_CURRENT_CHECK
		test_test_time1 = 100000 - UI_Time.tmp4_ms;
#endif
	}

#ifdef TEST_CURRENT_CHECK
	//test_check_cnt++;
	if (!UI_Time.tmp3_ms) {
		test_test_time[test_cur_cnt] = 100000-UI_Time.tmp4_ms;
		test_angle_buf[test_cur_cnt] = MotorDrv_GetPosition();
		test_cur_buf[test_cur_cnt] = MotorDrv_GetCurrent();
		test_cur_cnt++;
		if (test_cur_cnt > 4999) test_cur_cnt = 0;
		test_check_cnt = 0;
		UI_Time.tmp3_ms = TEST_CHECK_TIME;
	}
#endif	

}

void UI_Product_Process_Calibration_GoToStartPos(void)
{
	//if (!UI_CheckMotorStatus()) return;

	if (MotorDrv_GetPosition() >= PRODUCT_MEA_ANGLE_MAX) {
		MotorDrv_SetSpeed(Product_Calib.motorSpeed);
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition());
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 370,208, Product_Calib.motorSpeed, 1);
		HAL_Delay(500);
		MotorDrv_SetTargetPosition(PRODUCT_MEA_ANGLE_MIN);
		MotorDrv_SetDirection(MDD_CW);		
		App_SetUIProcess(UI_Product_Process_Calibration_Measure);
		Product_Calib.maxCurrent[Product_Calib.massStep][Product_Calib.motorSpeed-1] = 0;
		Product_Calib.curCurrent = 0;
		Product_Calib.cnt = 0;
		UI_Time.tmp4_ms = 3000;
#ifdef TEST_CURRENT_CHECK
		UI_Time.tmp4_ms = 100000;
		UI_Time.tmp3_ms = TEST_CHECK_TIME;
		test_check_cnt = 0;
		test_cur_cnt = 0;
#endif
	}	
	//UI_Calibration_DisplayValue();
}

void UI_Product_Process_Calibration_HomeIn(void)
{
	//home-in switch break
	if (UI_Time.tmp_sec > HOME_IN_TOTAL_TIME) {
		MotorDrv_StopHome();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_HOMEIN, EDT_DISP_HALT);
		return;
	}

	if (MotorDrv_GetDirection() == MDD_CW) {
		if (MotorDrv_GetLimitSwitchStatus() == 0) {
			MotorDrv_SetMotorState(MOTOR_CCW);
			MotorDrv_SetDirection(MDD_CCW);
			MotorDrv_SetFlagLimitSW(0);
		}
	}
	else {
		if(MotorDrv_GetFlagLimitSW() == 0){
			if (MotorDrv_GetLimitSwitchStatus() == 1) {
				MotorDrv_SetMotorZeroPosition();
				MotorDrv_StopHome();
				MotorDrv_SetFlagLimitSW(0);
				MotorDrv_SetTargetPosition(50);
				MotorDrv_SetDirection(MDD_CCW);
				Product_Calib.motorSpeed = 1;
				Product_Calib.massStep = PMT_1STEP;
				//Product_Calib.maxCurrent[Product_Calib.motorSpeed-1] = 0;
				MotorDrv_SetSpeed(4);
				App_SetUIProcess(UI_Product_Process_Calibration_GoToStartPos);
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ret.bmp,362,46\r");
				App_SetButtonOption(RID_CALI_BTN_RETURN, BN_PUSHED);
				App_SetTimer(TIMER_ID_5, 30);
				//Product_Calib.massStep = PMT_5STEP;//test
			}
		}
		else {
			//if (UI_GotoHomePosition()) {
			//	MotorDrv_StopHome();
			//	MotorDrv_SetFlagLimitSW(0);
			//	App_SetUIProcess(UI_Product_Process_Calibration_GoToStartPos);
			//}
		}
	}
	//UI_Calibration_DisplayValue();
}

void UI_Product_Process_Calibration_GoToStandby(void)
{
	if (MotorDrv_GetPosition() <= -2) {
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition()); //pjg++180402 quickly stop
		MotorDrv_Stop();		  
		App_SetUIProcess(UI_ProcessNull);
	}	
	//UI_Calibration_DisplayValue();
}

void UI_Product_Process_FactorySetting(void)
{
}

void UI_Product_OnBnClickedBtnCalibration(void)
{	
	App_SetUIProcess(UI_Product_Process_Calibration_HomeIn);
	//App_SetTimer(TIMER_ID_5, 30);
	UI_Time.tmp_sec = 0;//70;//limit s/w search time
	if (MotorDrv_GetLimitSwitchStatus() == 1) {
		MotorDrv_SetMotorState(MOTOR_CW);
		MotorDrv_SetDirection(MDD_CW);
	}
	else {
		MotorDrv_SetMotorState(MOTOR_CCW);
		MotorDrv_SetDirection(MDD_CCW);
	}
	MotorDrv_SetFlagLimitSW(0);
	MotorDrv_GoHome();
	UI_LED_Control(LM_RUN);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,40\r");
	App_SetButtonOption(RID_CALI_BTN_CALI0, BN_DISABLE);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 370,208, Product_Calib.motorSpeed+1, 1);

	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 155,5, 0);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 165,5, 1);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 175,5, 2);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 185,5, 3);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 195,5, 4);	
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 205,5, 5);
	
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 235,5, 6);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 245,5, 7);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 255,5, 8);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 265,5, 9);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 275,5, 10);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 285,5, 11);
	
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 315,5, 12);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 325,5, 13);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 340,10, 14);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 350,10, 15);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 360,10, 16);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 370,10, 17);
}

void UI_Product_OnBnClickedBtnCalibration1(void)
{	
	if (Product_Calib.massStep >= PMT_STEP_MAX) return;
	
	MotorDrv_SetTargetPosition(50);
	MotorDrv_SetDirection(MDD_CCW);
	MotorDrv_SetSpeed(4);			
	App_SetUIProcess(UI_Product_Process_Calibration_GoToStartPos);
	UI_Product_Calibration_ButtonDisable();
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ret.bmp,362,46\r");
	App_SetButtonOption(RID_CALI_BTN_RETURN, BN_PUSHED);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 370,208, 4, 1);

	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 155,5, 0);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 165,5, 1);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 175,5, 2);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 185,5, 3);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 195,5, 4);	
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 205,5, 5);
	
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 235,5, 6);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 245,5, 7);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 255,5, 8);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 265,5, 9);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 275,5, 10);
	UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 285,5, 11);
	
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 315,5, 12);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 325,5, 13);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 340,10, 14);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 350,10, 15);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 360,10, 16);
	//UI_DisplayClearNumber(UI_DISP_NUM_FNT7, 370,10, 17);
}

void UI_Calibration_OnBnClickedBtnReturn(void)
{
	if(Product_Calib.massStep == PMT_1STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,40\r");
		App_SetButtonOption(RID_CALI_BTN_CALI0, BN_PUSHED);
		Product_Calib.massStep = PMT_1STEP;
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nret.bmp,362,46\r");
		App_SetButtonOption(RID_CALI_BTN_RETURN, BN_DISABLE);
	}
	else if(Product_Calib.massStep == PMT_2STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,40\r");
		App_SetButtonOption(RID_CALI_BTN_CALI0, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,85\r");
		App_SetButtonOption(RID_CALI_BTN_CALI5, BN_DISABLE);
		Product_Calib.massStep = PMT_1STEP;
	}
	else if(Product_Calib.massStep == PMT_3STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,85\r");
		App_SetButtonOption(RID_CALI_BTN_CALI5, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,131\r");
		App_SetButtonOption(RID_CALI_BTN_CALI10, BN_DISABLE);
		Product_Calib.massStep = PMT_2STEP;
	}
	else if(Product_Calib.massStep == PMT_4STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,131\r");
		App_SetButtonOption(RID_CALI_BTN_CALI10, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,177\r");
		App_SetButtonOption(RID_CALI_BTN_CALI15, BN_DISABLE);
		Product_Calib.massStep = PMT_3STEP;
	}
	else if(Product_Calib.massStep == PMT_5STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i cali.bmp,49,177\r");
		App_SetButtonOption(RID_CALI_BTN_CALI15, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,224\r");
		App_SetButtonOption(RID_CALI_BTN_CALI20, BN_DISABLE);
		Product_Calib.massStep = PMT_4STEP;
	}
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nret.bmp,362,46\r");
	App_SetButtonOption(RID_CALI_BTN_RETURN, BN_DISABLE);
	MotorDrv_SetTargetPosition(-2);
	MotorDrv_SetDirection(MDD_CW);					
	Product_Calib.motorSpeed = 1;
	//Product_Calib.massStep = PMT_1STEP;
	//Product_Calib.maxCurrent[Product_Calib.motorSpeed-1] = 0;
	MotorDrv_SetSpeed(4);
	App_SetUIProcess(UI_Product_Process_Calibration_GoToStandby);
	App_SetTimer(TIMER_ID_5, 30);
}

void UI_Calibration_OnBnClickedBtnBack(void)
{
	MotorDrv_SetTargetPosition(MotorDrv_GetPosition()); //pjg++180402 quickly stop
	MotorDrv_Stop();		  
	App_SetUIProcess(UI_ProcessNull);
	UI_Product_Create();
}

void UI_Calibration_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_CALI0*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI0*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI0*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_CALI0, 0);
#ifndef USE_CALIB_ONLY_0KG
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_CALI5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI5*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_CALI5, 0);
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_CALI10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI10*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_CALI10, 0);
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_CALI15*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI15*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI15*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_CALI15, 0);
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_CALI20*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI20*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_CALI20*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_CALI20, 0);
#endif
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_RETURN*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_RETURN*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_RETURN*2], ',', 2),
			108,45, hParent, RID_CALI_BTN_RETURN, 0);
	API_CreateWindow("", pBtnInfo[RID_CALI_BTN_BACK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_CALI_BTN_BACK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_CALI_BTN_BACK*2], ',', 2),
			108,30, hParent, RID_CALI_BTN_BACK, 0);
	/*uart_putstring("RR 50,50,201,41,11,0,0,0,0\r\n");
	uart_putstring("RR 50,50,200,40,10,0,255,0,1\r\n");
	uart_putstring("f Calibration,60,60\r\n");
	uart_putstring("f Put on 20kg mass,60,70\r\n");*/
	/*
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,85\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,131\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,177\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ncali.bmp,49,224\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nret.bmp,362,46\r");
	*/
	App_SetButtonOption(RID_CALI_BTN_CALI0, BN_PUSHED);
	//App_SetButtonOption(RID_CALI_BTN_CALI5, BN_DISABLE);
	//App_SetButtonOption(RID_CALI_BTN_CALI10, BN_DISABLE);
	//App_SetButtonOption(RID_CALI_BTN_CALI15, BN_DISABLE);
	//App_SetButtonOption(RID_CALI_BTN_CALI20, BN_DISABLE);
	App_SetButtonOption(RID_CALI_BTN_RETURN, BN_DISABLE);
	
	UI_Time.tmp_sec = 0;//70;//limit s/w search time

	UI_LED_Control(LM_STAND_BY);
	UI_Calibration_Timer(0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"f Speed,380,180\n");
	MotorDrv_SetOverCurrent(RUN_CHK_OVERCURRENT); //pjg++191126
	/*
        if (MotorDrv_GetLimitSwitchStatus() == 1) {
		MotorDrv_SetMotorState(MOTOR_CW);
		MotorDrv_SetDirection(MDD_CW);
	}
	else {
		MotorDrv_SetMotorState(MOTOR_CCW);
		MotorDrv_SetDirection(MDD_CCW);
	}
	MotorDrv_SetFlagLimitSW(0);
	MotorDrv_GoHome();
        */
}

LRESULT UI_Calibration_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Calibration_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_CALI_BTN_CALI0:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnCalibration();
				break;
			}
			return 0;
		case RID_CALI_BTN_CALI5:
		case RID_CALI_BTN_CALI10:
		case RID_CALI_BTN_CALI15:
		case RID_CALI_BTN_CALI20:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Product_OnBnClickedBtnCalibration1();
				break;
			}
			return 0;
		case RID_CALI_BTN_RETURN:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Calibration_OnBnClickedBtnReturn();
				break;
			}
			return 0;	
		case RID_CALI_BTN_BACK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Calibration_OnBnClickedBtnBack();
				break;
			}
			return 0;
		/*case RID_HOME_BTN_USER:
			switch(lParam) {
			case BN_CLICKED: // push 
				//UI_Home_OnBnClickedBtnUser();
				break;
			}
			return 0;*/
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_TIMER:
		UI_Calibration_Timer(wParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_Calibration_Create(void)
{
	App_SetWndProcFnPtr(UI_Calibration_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i calibg.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_CALIBRATION;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

uint8_t UI_GotoHomePosition(void)
{
	//if (!UI_CheckMotorStatus()) return 0;

	if (MotorDrv_GetPosition() > (int)Setup3.IPos) {//+1) {
		MotorDrv_MoveUp();
	}
	else if (MotorDrv_GetPosition() < (int)Setup3.IPos) {//-1) {
		MotorDrv_MoveDown();
	}
	else {
		MotorDrv_SetTargetPosition(Setup3.IPos);
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//pjg++190916
void UI_SystemInit_Timer(uint16_t nIDEvent)
{
	SYSTEM_INFO *fssi;	
	SYSTEM_BASE_INFO *fssbi;		
	FILINFO fno;
	uint16_t temp;
	//extern const uint16_t metainfo[16];
	int fr;
	uint8_t *ptr, *ptr2, i, j;

	loading++;
	
	switch (loading) {
		case 1:
		  	//temp = metainfo[4];
			MotorDrv_Init2(); //home-in error because lcd wait time;
			MotorDrv_SetOverCurrent(RUN_CHK_OVERCURRENT); //prevent that error is occur in booting
			//for (fr = 500; fr < 900; fr++)
			//	ConvertLong2String(fr, &CommonBuf[0], 5);
			//USBDisk_UnMount();
			//USBDisk_UnLink();
			//USBDisk_Link();
			//USBDisk_Mount();
			EEPROMDrv_Init(&hspi3);
			break;
		case 2:
#ifndef USE_JIG_TEST_MODE
			if (!EEPROMDisk_Link()) {
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
				break;
			}
			if (!EEPROMDisk_Mount()) {
				EEPROMDisk_UnLink();
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
				break;
			}
			#if 0 //*test: eeprom data full write 
			if (1) {
				uint8_t filename[12];
				filename[0] = 't'; filename[1] = 'e'; filename[2] = 's'; filename[3] = 't';
				filename[4] = '0'; filename[5] = '0'; 
				filename[6] = '.'; filename[7] = 't'; filename[8] = 'x'; filename[9] = 't'; 
				filename[10] = 0;
				for (temp = 0; temp < 16*1024; temp++) CommonBuf[temp] = temp;
				for (temp = 0; temp < 99; temp++) {
					filename[4] = '0'+temp/10; filename[5] = '0'+temp%10; 
					EEPROMDisk_OpenWrite(filename);
					EEPROMDisk_Write(CommonBuf, 512);
					EEPROMDisk_Close();
					EEPROMDisk_GetFree("0:/");
				}
			}
			else {
				EEPROMDisk_OpenRead("test.txt");
				EEPROMDisk_Read(CommonBuf, 16*1024);
				EEPROMDisk_Close();
			}
			#endif
			//EEPROMDisk_GetFree("0:/");
			fr = EEPROMDisk_stat("0:/KR20P", &fno);
			//fr = -1; //eeprom format
			if (fr == -1) {
				uart_putstring("f Format start(wait 12sec),50,210\n");
				if (!EEPROMDisk_Format()) {
					uart_putstring("f Format fail,50,230\n");
					App_SetUIProcess(UI_ProcessNull);
					API_SetErrorCode(EC_EEP_FORMAT, EDT_DISP_HALT);
				}
				else {
					uart_putstring("f Format complete,50,230\n");
				}
			}
			else {
				if (fr == 0) { // no exist dir, first use
					if (!EEPROMDisk_MkDir(EEPROMDISK_DIR)) {
						App_SetUIProcess(UI_ProcessNull);
						API_SetErrorCode(EC_EEP_MKDIR, EDT_DISP_HALT);
						//break;
					}
				}
			}
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
#endif
			break;
		case 3:
			UI_InitSetupVariable(); 
			//FlashDrv_SetTempBuf(&CommonBuf[FLASH_PAGE_SIZE16]);
			//FlashDrv_SetParam(FLASH_PAGE_SIZE16);
			//if (0) { //init save eeprom param data
#ifndef USE_JIG_TEST_MODE
			if (UI_LoadParamFromEEPROM(CommonBuf)) {
				fssi = (SYSTEM_INFO *)CommonBuf;
				Setup2.vol = fssi->setup2.vol;
				if (Setup2.vol > 5) Setup2.vol = 5; //pjg++170609 bug fix:flash save err
				Setup2.bright = fssi->setup2.bright;
				if (Setup2.bright > 5) Setup2.bright = 5;
				//Setup.sndGuid = fssi->setup.sndGuid;
				//Setup.lsrpt_en = fssi->setup.lsrpt_en;
				//Setup2.lsrpt_en2 = fssi->setup2.lsrpt_en2;
				//Setup2.sndGuid2 = fssi->setup2.sndGuid2;		
				//Setup2.led_en = fssi->setup2.led_en;
				Setup2.language = fssi->setup2.language;
				if(Setup2.language >= LT_MAX) Setup2.language = LT_ENG;
		//		Setup3.PDeg = fssi->setup3.PDeg;
		//		Setup3.PTm = fssi->setup3.PTm;
		//		Setup3.ProChk = fssi->setup3.ProChk;
				//Setup2.stand_by = fssi->setup2.stand_by;
		//		Setup3.LPChk = fssi->setup3.LPChk;
		//		Setup3.VibChk = fssi->setup3.VibChk;
				//Setup.angle = fssi->setup.angle;
		//		Setup3.VCnt = fssi->setup3.VCnt;
				Setup3.IPos = fssi->setup3.IPos; //pjg<>190813 : ignore at boot time
				if (Setup3.IPos < 0 || Setup3.IPos > 140) Setup3.IPos = HOME_IN_INIT_POS;
				Setup3.sensitivity = fssi->setup3.sensitivity;
				//Setup3.AngChkSens = fssi->setup3.AngChkSens;
				//if (Setup3.AngChkSens > ANG_MEA_MAX_SENS_VALUE) Setup3.AngChkSens = 2;
				Setup3.amSens = fssi->setup3.amSens;
				if (Setup3.amSens > 3) Setup3.amSens = 3;
				
				Setup3.amp.gapBk = fssi->setup3.amp.gapBk;
				if (Setup3.amp.gapBk > 10) Setup3.amp.gapBk = 2;
				AngChk.gapBk = Setup3.amp.gapBk;
				AngChk.gap = AngChk.gapBk;
				//if (((uint32_t)fssi->setup3.amp.alpha) > 0xf0000000)   fssi->setup3.amp.alpha = 0.08;
				Setup3.amp.alpha = fssi->setup3.amp.alpha;
				if (Setup3.amp.alpha > 1.0 || Setup3.amp.alpha <= 0.0) Setup3.amp.alpha = 0.019;//0.08;
				AngChk.alpha = 0.019;//Setup3.amp.alpha;
				Setup3.amp.upOffset = fssi->setup3.amp.upOffset;
				if (Setup3.amp.upOffset > 10) Setup3.amp.upOffset = 2;
				AngChk.upOffset = Setup3.amp.upOffset;
				
				Setup3.AngBtnStep = fssi->setup3.AngBtnStep;
				if (Setup3.AngBtnStep > 100) Setup3.AngBtnStep = 1;//5; //pjg<>200902 by ask park su hyun
				Setup3.writeTime = fssi->setup3.writeTime;
				if (Setup3.writeTime > 100100) Setup3.writeTime = 0;
			}
			else {
				Setup3.writeTime = 0;
				UI_SaveParamToEEPROM(CommonBuf);
			}
#endif
			break;
		case 4: 
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pWndBgNameInfo[WND_INIT][Setup2.language]);
			UI_AniProgress(1);
			UI_AniProgress(2);
			UI_AniProgress(3);
			UI_AniProgress(4);
			break;
		case 5:	
			//FlashDrv_SetTempBuf(&CommonBuf[FLASH_PAGE_SIZE16]);
			FlashDrv_SetTempBuf(CommonBuf);
			FlashDrv_SetParam(FLASH_PAGE_SIZE16);
			//if (0) //init save flash param data
			if (UI_LoadSystemInfoFromFlash(CommonBuf))
            { //pjg++180221
				fssbi = (SYSTEM_BASE_INFO *)CommonBuf;
				//Product_Calib.maxCurrent = fssbi->product.Cal_MaxCur;
				//ptr = &fssbi->product.Cal_MaxCur[0][0];
				UI_SetSensitivityByCalibrationValue(fssbi->product.Cal_MaxCur);
				temp = 0;
				for (i = 0; i < SL_LEVEL_MAX; i++) {
					for (j = 0; j < MS_SPEED_MAX; j++) {
						if (Motor_OverCurTbl[i][j] < RUN_CHK_MINCURRENT || 
							Motor_OverCurTbl[i][j] > RUN_CHK_OVERCURRENT) {
							temp = 1;
							break;
						}
					}
					if (temp) break;
				}
				if (temp) { //copy to default data
					Product_Calib.maxCurrent[0][0] = 386;
					Product_Calib.maxCurrent[0][1] = 394;
					Product_Calib.maxCurrent[0][2] = 408;
					for (i = 0; i < SL_LEVEL_MAX; i++) {
						for (j = 0; j < MS_SPEED_MAX; j++) {
							Product_Calib.maxCurrent[i+1][j] = SensDefault[SL_LEVEL_MAX-i-1][j];
							Motor_OverCurTbl[i][j] = SensDefault[i][j];
						}
					}
				}
				SysInfo.sn = fssbi->serialnum;
				SysInfo.adminPwd = fssbi->adminPwd;
				SysInfo.writetime = fssbi->writeTime;
			}	
			else {
				SysInfo.sn = 1234567890;
				SysInfo.writetime = 0;
				Product_Calib.maxCurrent[0][0] = 386;
				Product_Calib.maxCurrent[0][1] = 394;
				Product_Calib.maxCurrent[0][2] = 408;
				for (i = 0; i < SL_LEVEL_MAX; i++) {
					for (j = 0; j < MS_SPEED_MAX; j++) {
						Product_Calib.maxCurrent[i+1][j] = SensDefault[SL_LEVEL_MAX-i-1][j];
						Motor_OverCurTbl[i][j] = SensDefault[i][j];
					}
				}
#ifndef USE_JIG_TEST_MODE
				UI_SaveSystemInfoToFlash(CommonBuf);
#endif
			}
			break;
		case 6: 
			UI_Setup_SetSoundVolume(SndVolTbl[Setup2.vol]);
			UI_Setup_SetBrightness(BLVolTbl[Setup2.bright]);
			#ifdef USE_JIG_TEST_MODE
			uart_putstring("sp wav/noise.wav\r");
			HAL_Delay(3000);
			#endif
			//if (Setup2.sndGuid2 == BST_CHECKED)
			APP_SendMessage(hParent, WM_SOUND, 0, SNDID_SYSTEM_INIT);
			break;
		case 7: 
			//UI_LED_Control(LM_INIT);
			UI_Display_Init(0);
			KeyDrv_SetLongKeyTime(70);
			break;
		case 8: 
			MotorDrv_SetMotorAngleVarAddr(&RunWnd.angle); //pjg++180102
			MotorDrv_SetMotorSpeedVarAddr((uint16_t *)&SpdTmWnd.speed); //pjg++180102
			//MotorDrv_SetOverCurrent(1000); //ori
			MotorDrv_SetSensCurrent(STANDBY_SENS_CURRENT);
			MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //prevent that error is occur in booting
			break;
		case 9:
			UI_InitVariable();
			
			ptr = (uint8_t *)&Setup2;
			ptr2 = (uint8_t *)&Setup2Old;
			for (i = 0; i < sizeof(SETUP2_INFO); i++) {
				ptr2[i] = ptr[i];
			}
			ptr = (uint8_t *)&Setup3;
			ptr2 = (uint8_t *)&Setup3Old;
			for (i = 0; i < sizeof(SETUP3_INFO); i++) {
				ptr2[i] = ptr[i];
			}
			//
			//test
			//UI_SavePIToEEPROM(3, CommonBuf);
			//for (i = 0; i < sizeof(PI_PARAM); i++) CommonBuf[i] = 0;
			//UI_LoadPIFromEEPROM(3, CommonBuf);
			//UI_Exercise_SaveData(3, 0, 3);
			//exTotalSize = sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM;
			//pi_startAddr = sizeof(SYSTEM_INFO) + (sizeof(PI_PARAM) + exTotalSize)*3;
			//ex_startAddr = pi_startAddr + sizeof(PI_PARAM);
			//posAddr = sizeof(EXERCISE_INFO)*3;
			//posAddr += ex_startAddr;
			//UI_Exercise_LoadExData(posAddr, (uint8_t *)&buf);
			MotorDrv_Smart();
			//MotorDrv_Run();
			break;
		case 10:
			//home_switch = MotorDrv_GetLimitSwitchStatus();
			if (App_GetLongButtonCnt() > 3) 
			{
				//break;//return;
			}
			//if (1) UI_Calibration_Create(); //pjg++180104
			//else UI_Home_Create();
			else {
				App_KillTimer(TIMER_ID_0);
				UI_Loading_Create();
				//else UI_Run_Create();
				loading = 0;
                return;
			}
			break;
		default:
			if (App_GetLongButtonCnt() > 7) 
			{
				App_KillTimer(TIMER_ID_0);
				loginInfo.type = LIT_ADMIN1;
				UI_Login_Create(); //pjg++180104
				//UI_Product_Create(); //pjg++180104
				return;
			}
			else {
				if (loading < 50) break;
				App_KillTimer(TIMER_ID_0);
				UI_Loading_Create();
				//else UI_Run_Create();
				loading = 0;
				return;
			}
			break;
	}

	UI_AniProgress(loading);
    // Timer_sec = 0;
}

//Ω√Ω∫≈€ √ ±‚»≠ »≠∏È ±∏º∫
//uint8_t key2[256];
void UI_SystemInit_Init(void)
{          
	//uint32_t *ptr;
	//uint32_t exTotalSize;
	//uint32_t pi_startAddr;
	//uint32_t posAddr;
	//uint32_t ex_startAddr;
	//EXERCISE_INFO buf;
	//int i;
	
	// button
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ldb.bmp,99,190\r");
	//APP_SendMessage(0, WM_SOUND, 0, SNDID_SYSTEM_INIT);
	
	App_SetTimer(TIMER_ID_0, 30);
	UI_InitSystemVariable();
	UI_InitSensitivityValue();
	UI_InitOneTime();

	#if 0 //fat of eeprom test
	if (!USBDisk_Mount()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return;
	}
	#endif
	#if 0
	float ftemp;
	uint8_t key;//, key2[256];
	uint8_t i;
	uint32_t dwtemp;

	for (i = 0; i < 256; i++) {
		ftemp = arm_sin_f32((float)i) + arm_sin_f32((float)i/2) + arm_sin_f32((float)i/3);
		dwtemp = (DWORD)((3.0-ftemp) * 100);
		key2[i] = (BYTE)(dwtemp%256);
        if (key2[i] == 0) key2[i] = i/2;
	}
	#endif
        
}

//≈Õƒ°≥™ ≈∏¿Ã∏”µÓ¿« µø¿€ ±∏µø «¡∑Œººº≠
LRESULT UI_SystemInit_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_SystemInit_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		UI_SystemInit_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;
	case WM_KEYUP:
		//longKeyEventCnt = 0;	 				  
		break;
	case WM_KEYLONG:
		//longKeyEventCnt++;
		break;
        case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

//Ω√Ω∫≈€ √ ±‚»≠ √¢ ª˝º∫
void UI_SystemInit_Create(void)
{	
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_SystemInit_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i load.jpg,0,0\r", WS_OVERLAPPEDWINDOW,
	//hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_INIT][Setup2.language], WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)"i boot.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_INIT;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

//////////////////////////////////////////////////////////////////////////////
//
//Home in
void UI_Loading_Timer(uint16_t nIDEvent)
{
	loading++;
//      HAL_Delay(10);
   	//Home_mode = 1;
    // Home_flag = 0;
//      MOTOR_STATE = 2;                 
	/*if (loading > 9) {
        App_KillTimer(TIMER_ID_1);
        UI_Angle_Create();
        return;
	}*/
	/*  if(Home_sensor == 1){
		//Home_mode = 0;	//pjg+171020
		//Home_flag = 1;	//pjg+171020
		App_KillTimer(TIMER_ID_1);
		if (SpdTmWnd_bk_speed < 3) {
			SpdTmWnd.speed = SpdTmWnd_bk_speed;
		}
        UI_Home_Create();
        return;
	}
	Timer_sec = 0;   
	UI_AniProgress(loading);*/

	//home_swticht = MotorDrv_GetHomeInSwitchStatus();
	//return;
	
	//Home In error
	/*if (!MotorDrv_GetFlagRunOne()) {
		if (MotorDrv_GetEncoderVaue() > 5000 && loading > 6) MotorDrv_SetFlagRunOne(1);
	}
	if (MotorDrv_GetHomeInSwitchStatus() == 0) {
		if (!MotorDrv_GetFlagLimitedPause()) MotorDrv_SetFlagLimitedPause(1);
	}
	//move to +0 deg
	if (MotorDrv_GetFlagRunOne() ){//&& !MotorDrv_GetFlagLimitedPause()) {
		//MOTOR_STATE = 1;
		MotorDrv_StopHome();
		API_SetErrorCode(EC_HOMEIN, EDT_DISP_HALT);
	}
	// over -0 deg and over move
	if (MotorDrv_GetHomeInSwitchStatus() == 0 && MotorDrv_GetEncoderVaue()) {
		if (MotorDrv_GetHomeInSwitchStatus() == 0) {
			MotorDrv_SetOldEncoderValue(MotorDrv_GetEncoderVaue());
		}
		if (MotorDrv_GetEncoderVaue() - MotorDrv_GetOldEncoderValue() > 700) {
			MotorDrv_StopHome();
			API_SetErrorCode(EC_HOMEIN, EDT_DISP_HALT);
			return;
		}
	}
	//over current
	MotorDrv_CheckOverCurrent();
	if (MotorDrv_IsOverCurrent()) {
		MotorDrv_StopHome();
		App_KillTimer(TIMER_ID_1);
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_HOMEIN, EDT_DISP);
		API_SetErrorCode(EC_MOTOR_OVERLOAD, EDT_DISP_HALT);
		return;
	}
	//home-in switch break
	if (UI_Time.tmp_sec > 70) {
		MotorDrv_StopHome();
		App_KillTimer(TIMER_ID_1);
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_HOMEIN, EDT_DISP_HALT);
		return;
	}
	*/
	if (MotorDrv_GetFlagHomeIn()) 
	{
		MotorDrv_SetTargetPosition(Setup3.IPos);
		MotorDrv_StopHome();
		App_KillTimer(TIMER_ID_1);
		if(SpdTmWnd_bk_speed < 3){
			SpdTmWnd.speed = SpdTmWnd_bk_speed;
		}
		RunWnd.play = UI_RUN_MODE_PAUSE;
		UI_Time.tmp3_ms = 2000;
		MotorDrv_SetFlagStandbyCurrent(0);
		MotorDrv_SetTimerValue(0, 5000);
		MotorDrv_SetSensFlagRunOne(0);
		UI_Home_Create();
		return;
	}

	Timer_sec = 0;
	UI_AniProgress(loading);
}

void  UI_Loading_Process(void)
{
	//UI_LimitSWCheck();
	//int i;
	//uint8_t senseChkCnt;	
	//if (!UI_CheckMotorStatus()) return;
	
	//home-in switch break
	#ifndef USE_JIG_TEST_MODE
	if (UI_Time.tmp_sec > HOME_IN_TOTAL_TIME) {
		MotorDrv_StopHome();
		//App_KillTimer(TIMER_ID_1);
		//UI_SetFunRunMode(UI_FRM_NONE);
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_HOMEIN, EDT_DISP_HALT);
		return;
	}
	#endif

	if (MotorDrv_GetDirection() == MDD_CW) {
		if (MotorDrv_GetLimitSwitchStatus() == 0) {
			MotorDrv_SetMotorState(MOTOR_CCW);
			MotorDrv_SetDirection(MDD_CCW);
			MotorDrv_SetFlagLimitSW(0);
		}
	}
	else {
		if(MotorDrv_GetFlagLimitSW() == 0){
			if (MotorDrv_GetLimitSwitchStatus() == 1) {
				MotorDrv_SetMotorZeroPosition();
				MotorDrv_SetDirection(MDD_CCW);
				//MotorDrv_StopHome();
				//App_SetUIProcess(UI_ProcessNull);
				//CommonBuf[0] = (uint8_t)Setup3.IPos; //pjg++190813 : backup home pos
				//Setup3.IPos = 0;//pjg++190813
				#ifdef USE_JIG_TEST_MODE
				MotorDrv_StopHome();
				MotorDrv_SetFlagLimitSW(0);
				MotorDrv_SetFlagHomeIn(1);
				App_SetUIProcess(UI_ProcessNull);
				#endif
			}
		}
		else {
			if (UI_GotoHomePosition()) {
				MotorDrv_StopHome();
				MotorDrv_SetFlagLimitSW(0);
				MotorDrv_SetFlagHomeIn(1);
				App_SetUIProcess(UI_ProcessNull);
				//Setup3.IPos = (uint8_t)CommonBuf[0];//pjg++190813
			}
		}
	}
}


//»®¿Œ µø¿€ ¡ﬂ¡ˆ«œ∞Ì »Ø¿⁄¡§∫∏ √¢¿∏∑Œ ¿Ãµø«œ∞‘ «‘
void UI_Loading_OnBnClickedBtnStop(void)
{
	//UI_Home_Create();
	if (Option.temp16 == 0) {
		MotorDrv_StopHome();
		MotorDrv_StopNoCtrl();
		MotorDrv_Release();
		App_KillTimer(TIMER_ID_1);
		App_SetUIProcess(UI_ProcessNull);
		Option.temp16 = 1;
	}
	else {
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
		MotorDrv_Enable();
		//UI_LED_Control(LM_HOME_IN);
		App_SetTimer(TIMER_ID_1, 50);//150000);
		App_SetUIProcess(UI_Loading_Process);
		UI_Time.tmp_sec = 0;//70;//limit s/w search time
		//if (MotorDrv_GetLimitSwitchStatus() == 1) {
		//	MotorDrv_SetMotorState(MOTOR_CW);
		//	MotorDrv_SetDirection(MDD_CW);
		//}
		//else {
		//	MotorDrv_SetMotorState(MOTOR_CCW);
		//	MotorDrv_SetDirection(MDD_CCW);
		//}
		//MotorDrv_SetFlagLimitSW(0);
		MotorDrv_GoHome();
		RunWnd.play = UI_RUN_MODE_LOAD;
		Option.temp16 = 0;
	}
}

void UI_Loading_OnKeyUp(void)
{
	UI_Loading_OnBnClickedBtnStop();
}

//»®¿Œ »≠∏È ±∏º∫
void UI_Loading_Init(void)
{
	// button
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ldb.bmp,99,190\r"); 
	API_CreateWindow("", pBtnInfo[(RID_LD_BTN_STOP+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_LD_BTN_STOP+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_LD_BTN_STOP+Setup2.language)*2], ',', 2),
			80, 50, hParent, RID_LD_BTN_STOP+Setup2.language, 0);
	
	//UI_LED_Control(LM_STAND_BY);
	
	//if (Setup2.sndGuid2 == BST_CHECKED)
	APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
	UI_LED_Control(LM_HOME_IN);
	/*if (!SysInfo.sn) {
		API_SetErrorCode(EC_EMPTY_SYSINFO, EDT_DISP_HALT);
		return;
	}*/ // TEST »ƒ «ÿ¡¶ 
	App_SetTimer(TIMER_ID_1, 50);//150000);
	App_SetUIProcess(UI_Loading_Process);
	loading = 0;
	UI_Time.tmp_sec = 0;//70;//limit s/w search time
	if (MotorDrv_GetLimitSwitchStatus() == 1) {
		MotorDrv_SetMotorState(MOTOR_CW);
		MotorDrv_SetDirection(MDD_CW);
	}
	else {
		MotorDrv_SetMotorState(MOTOR_CCW);
		MotorDrv_SetDirection(MDD_CCW);
	}
	MotorDrv_SetFlagLimitSW(0);
	MotorDrv_GoHome();
	RunWnd.play = UI_RUN_MODE_LOAD;
	Option.temp16 = 0;
}

LRESULT UI_Loading_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Loading_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_LD_BTN_STOP:
		case RID_LD_BTN_STOPK:
		case RID_LD_BTN_STOPC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Loading_OnBnClickedBtnStop();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		UI_Loading_Timer(wParam);
		break;
	case WM_KEYUP:  //pjg++170928
		if (UI_Time.tmp5_ms) break; //prevent 
		UI_Time.tmp5_ms = 700;
		UI_Loading_OnBnClickedBtnStop();
		break;
	default:
		break;
	}

	return 0;
}
//»®¿Œ »≠∏È ª˝º∫
void UI_Loading_Create(void)
{	
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_Loading_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i inpo.jpg,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_LOADING][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_LOADING;
}

//////////////////////////////////////////////////////////////////////////////
void UI_User_OnBnClickedBtnGuest(void)
{
	//uint32_t totalTimeBk;
	//uint32_t totalRepeatBk;
	
	//PInfoWnd2.id = PATIENT_NUM;
	//PiChange_Wnd.PrePressedNum = PATIENT_NUM;
   
	//CommonBuf[0] = (uint8_t)Setup3.IPos;
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf); //pjg++180529, pjg--190806
	/*if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	else {*/
		//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
		//totalTimeBk = PInfoWnd2.pi.totalTime;
		//totalRepeatBk = PInfoWnd2.pi.totalRepeat;
		//UI_InitSetupVariable();
		//UI_InitSystemVariable();
		UI_InitVariable();
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
		SpdTmWnd.time = 9999;//8*60;
		AngleWnd.flAngle = 140;
#endif
		//PInfoWnd2.pi.totalTime = totalTimeBk;
		//PInfoWnd2.pi.totalRepeat = totalRepeatBk;
	//}
	//Setup3.IPos = (uint8_t)CommonBuf[0];
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf); //pjg++190806 move pos, pjg--190813 : home pos is cleared
	//UI_Angle_Create();
	loginInfo.type = LIT_GUEST;
	UI_AngleMeasure_Create();
}

void UI_User_OnBnClickedBtnUser(void)
{
#ifdef PI_LOAD_V2
	loginInfo.type = LIT_USER;
	UI_InitVariable();
#else
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;

	if(loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	else {
		UI_InitSetupVariable();
		UI_InitSystemVariable();
		UI_InitVariable();
	}
#endif	
	//FullInfo.type = FT_PI;
	UI_PiLoad_Create();
}

void UI_User_OnBnClickedBtnSetup(void)
{
	UI_Setup_Create();
}

void UI_User_Init(void)
{
	//API_SetErrorCode(EC_MOTOR_OVERLOAD, EDT_DISP_HALT);
	// button
	/*API_CreateWindow("", pBtnInfo[(RID_HOME_BTN_GUEST+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_HOME_BTN_GUEST+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_HOME_BTN_GUEST+Setup2.language)*2], ',', 2),
			113, 113, hParent, RID_HOME_BTN_GUEST+Setup2.language, 0);*/
	API_CreateWindow("", pBtnInfo[RID_USER_BTN_GUEST*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_USER_BTN_GUEST*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_USER_BTN_GUEST*2], ',', 2),
			113, 113, hParent, RID_USER_BTN_GUEST, 0);
	
	API_CreateWindow("", pBtnInfo[RID_USER_BTN_USER*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_USER_BTN_USER*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_USER_BTN_USER*2], ',', 2),
			113, 113, hParent, RID_USER_BTN_USER, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_SETUP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_SETUP, 0);
	App_SetUIProcess(UI_ProcessNull); //pjg++190905
	UI_LED_Control(LM_STAND_BY);
	//MotorDrv_PWMChopSlow();
	//MotorDrv_PWMChopFast();
	//MotorDrv_SetPwm(10);
	//MotorDrv_Brake();
	//MotorDrv_Release();
	RunWnd.complete = 0;
	SysInfo.meaCnt = 0;
}

LRESULT UI_User_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_User_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_USER_BTN_GUEST:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_User_OnBnClickedBtnGuest();
				break;
			}
			return 0;
		case RID_USER_BTN_USER:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_User_OnBnClickedBtnUser();
				break;
			}
			return 0;
				default:
			return 0;
		case RID_RN_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_User_OnBnClickedBtnSetup();
				break;
			}
			return 0;	
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_User_Create(void)
{
	App_SetWndProcFnPtr(UI_User_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i homedp.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_USER][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_USER;
}

//////////////////////////////////////////////////////////////////////////////
//»® »≠∏È 
void UI_Home_OnBnClickedBtnMeasure(void)
{
	int i;
	
	SaveExeInfoV2.flg = MST_MEASURE;
	for (i = 0; i < 3; i++) {
		SaveExeInfoV2.smm.mi[i].exangle = 0;
		SaveExeInfoV2.smm.mi[i].flangle = 0;
	}
	FullInfo.type = FT_MEA;
	UI_User_Create();
}

void UI_Home_OnBnClickedBtnRehab(void)
{
	SaveExeInfoV2.flg = MST_REHAB;
	SaveExeInfoV2.spm.mi.exangle = 0;
	SaveExeInfoV2.spm.mi.flangle = 0;
	FullInfo.type = FT_RUN;
	UI_User_Create();
}

void UI_Home_OnBnClickedBtnSetup(void)
{
	UI_Setup_Create();
}

void UI_Home_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_HOME_BTN_MEA*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_HOME_BTN_MEA*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_HOME_BTN_MEA*2], ',', 2),
			113, 113, hParent, RID_HOME_BTN_MEA, 0);
	
	API_CreateWindow("", pBtnInfo[RID_HOME_BTN_REHAB*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_HOME_BTN_REHAB*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_HOME_BTN_REHAB*2], ',', 2),
			113, 113, hParent, RID_HOME_BTN_REHAB, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_SETUP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_SETUP, 0);
	App_SetUIProcess(UI_ProcessNull); //pjg++190905
	UI_LED_Control(LM_STAND_BY);
	#ifdef USE_MOTOR_RUN_TEST
	uint32_t appJumpAddress;
	void (*GoToApp)(void);	
	appJumpAddress = 0x;
	NVIC_SystemReset(); 
	#endif

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	uart_putstring("fc 255,0,0\r"); 	  
	uart_putstring("f USB checking... ,5,50\r");
	if (!USBDisk_Link()) {
		return;
	}
	if (!USBDisk_Mount()) {
		USBDisk_UnLink();
		return;
	}

	for (loading = 0; loading < 30000; loading++) {
		if (USBDisk_CheckDetect() == 1) break;
		HAL_Delay(1);
	}
	if (USBDisk_GetReadyFlag() == 1) {
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB detected ,5,240\r");
	}
	else {
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB not detected ,5,240\r");
	}
	USBDisk_UnMount();
	USBDisk_UnLink();
#endif
}

LRESULT UI_Home_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Home_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_HOME_BTN_MEA:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Home_OnBnClickedBtnMeasure();
				break;
			}
			return 0;
		case RID_HOME_BTN_REHAB:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Home_OnBnClickedBtnRehab();
				break;
			}
			return 0;
				default:
			return 0;
		case RID_RN_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Home_OnBnClickedBtnSetup();
				break;
			}
			return 0;	
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_Home_Create(void)
{
	App_SetWndProcFnPtr(UI_Home_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i homedp.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_HOME][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_HOME;
}
///////////////////////////////////////////////////////////////////////////////
#if 0
//∞¢µµ º≥¡§ 
void UI_Angle_SetAngle_Pic(short angle,short angle2)
{      
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i angleb-.bmp,166,70\r");
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ab001.png,276,70\r");
	if (!(angle%5)) {
		if (angle < 0) {
			AngPicBlueBuf[4] = '-';
			AngPicBlueBuf[5] = '0';
			AngPicBlueBuf[6] = ((angle%10)*-1) + '0';
		}
		else if (angle < 10) {
			AngPicBlueBuf[4] = '0';
			AngPicBlueBuf[5] = '0';
			AngPicBlueBuf[6] = (angle%10) + '0';
		}
		else if (angle < 100) {
			AngPicBlueBuf[4] = '0';
			AngPicBlueBuf[5] = (angle/10) + '0';
			AngPicBlueBuf[6] = (angle%10) + '0';
		}
		else {
		  	AngPicBlueBuf[4] = '1';
		  	AngPicBlueBuf[5] = (angle%100)/10 + '0';
		  	AngPicBlueBuf[6] = (angle%10) + '0';
		}
	}

	if (!(angle2%5)) {
		if (angle2 < 0) {
			AngPicIndigoBuf[4] = '-';
			AngPicIndigoBuf[5] = '0';
			AngPicIndigoBuf[6] = ((angle2%10)*-1) + '0';
		}
		else if (angle2 < 10) {
			AngPicIndigoBuf[4] = '0';
			AngPicIndigoBuf[5] = '0';
			AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
		else if (angle2 < 100) {
			AngPicIndigoBuf[4] = '0';
			AngPicIndigoBuf[5] = (angle2/10) + '0';
			AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
		else {
		  	AngPicIndigoBuf[4] = '1';
		  	AngPicIndigoBuf[5] = (angle2%100)/10 + '0';
		  	AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
	}

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)AngPicIndigoBuf);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)AngPicBlueBuf);
    APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd); 
        
	
}

void UI_Angle_Process(void)
{

}

void UI_Angle_OnBnClickedBtnLeft(void)
{
   	UI_User_Create();
}

void UI_Angle_OnBnClickedBtnHome(void)
{	
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	else{
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
		}
	
   	UI_Home_Create();
}

void UI_Angle_OnBnClickedBtnRight(void)
{
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
   	UI_SpeedTime_Create();
}

void UI_Angle_OnBnClickedBtnExUp(void)
{
	loading = 0;
	if (AngleWnd.exAngle >= RUN_ANGLE_MAX) return;
	//if(AngleWnd.exAngle < RUN_ANGLE_MAX-ANGLE_MOVE_GAP) AngleWnd.exAngle++;
	if(AngleWnd.exAngle < RUN_ANGLE_MAX) AngleWnd.exAngle += Setup3.AngBtnStep;
	if (AngleWnd.exAngle > RUN_ANGLE_MAX) AngleWnd.exAngle = RUN_ANGLE_MAX;
	//{
		if(AngleWnd.exAngle > RUN_ANGLE_MIN){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,133\r");
			App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
		}
		if(AngleWnd.flAngle <= (AngleWnd.exAngle+25))
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
		}
	//}
	//else return;
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,94,AngleWnd.exAngle,3);
}

void UI_Angle_OnBnClickedBtnExDown(void)
{
	if (AngleWnd.exAngle <= RUN_ANGLE_MIN) return;
	//if(AngleWnd.exAngle > RUN_ANGLE_MIN)  AngleWnd.exAngle--;
	if (AngleWnd.exAngle > RUN_ANGLE_MIN)  AngleWnd.exAngle -= Setup3.AngBtnStep;
	if (AngleWnd.exAngle < RUN_ANGLE_MIN) AngleWnd.exAngle = RUN_ANGLE_MIN;
	//{
		if(AngleWnd.exAngle == RUN_ANGLE_MIN){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
			App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
		}
		if(AngleWnd.flAngle>(AngleWnd.exAngle+25))
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
		}
     //}
	//else return;
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,38,94,AngleWnd.exAngle,3);
	
}

//unsigned char 
void UI_Angle_OnBnLongClickedBtnExUp(void)
{
	if (AngleWnd.exAngle >= RUN_ANGLE_MAX) return;
	if(AngleWnd.flAngle<=(AngleWnd.exAngle+25))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}	
	else if(AngleWnd.flAngle<=(AngleWnd.exAngle+30) && AngleWnd.flAngle>(AngleWnd.exAngle+25))
		AngleWnd.exAngle++;
	else {
		AngleWnd.exAngle += ANGLE_MOVE_GAP;
		if (AngleWnd.exAngle%5) { //pjg++190814
			AngleWnd.exAngle -= AngleWnd.exAngle%5;
		}
	}
	if(AngleWnd.exAngle > RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,133\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
	}
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5, (AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,94,AngleWnd.exAngle,3);
}

void UI_Angle_OnBnLongClickedBtnExDown(void)
{
	if (AngleWnd.exAngle <= RUN_ANGLE_MIN) return;
	if(AngleWnd.flAngle>(AngleWnd.exAngle+25))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}
	if (AngleWnd.exAngle > -1) 
	{
		AngleWnd.exAngle -= ANGLE_MOVE_GAP;	
		if (AngleWnd.exAngle%5) { //pjg++190814
			AngleWnd.exAngle -= AngleWnd.exAngle%5;
		}
	}
	else if(AngleWnd.exAngle >= -1  || AngleWnd.exAngle > RUN_ANGLE_MIN)
		AngleWnd.exAngle --;
	//else return;
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.exAngle == RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	}
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,94,AngleWnd.exAngle,3);
}

void UI_Angle_OnBnClickedBtnFlUp(void)
{
	if (AngleWnd.flAngle >= RUN_ANGLE_MAX) return;
	//if(AngleWnd.flAngle < RUN_ANGLE_MAX) AngleWnd.flAngle ++;
	if (AngleWnd.flAngle < RUN_ANGLE_MAX) AngleWnd.flAngle += Setup3.AngBtnStep;
	//{
		if(AngleWnd.flAngle == RUN_ANGLE_MAX){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,134\r");
			App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
		}
		if(AngleWnd.exAngle<(AngleWnd.flAngle-25))
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		}
	//}
	//else return;
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);

																		
 
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,94,AngleWnd.flAngle,3);
}

void UI_Angle_OnBnClickedBtnFlDown(void){
	if (AngleWnd.flAngle <= RUN_ANGLE_MIN) return;
	//if(AngleWnd.flAngle > 20) AngleWnd.flAngle--;
	if(AngleWnd.flAngle > 20) AngleWnd.flAngle -= Setup3.AngBtnStep;
	//{
		if(AngleWnd.flAngle < RUN_ANGLE_MAX){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
		}
		if(AngleWnd.exAngle>=(AngleWnd.flAngle-25))
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		}
	//}
	//else return;
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,94,AngleWnd.flAngle,3);
	
}

void UI_Angle_OnBnLongClickedBtnFlUp(void)
{	
	if(AngleWnd.exAngle<(AngleWnd.flAngle-25))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}
	
	if (AngleWnd.flAngle < 136){ //AngleWnd.exAngle = 0;
		AngleWnd.flAngle += ANGLE_MOVE_GAP;
		if (AngleWnd.flAngle%5) { //pjg++190814
			AngleWnd.flAngle -= AngleWnd.flAngle%5;
		}
	}
	else if(AngleWnd.flAngle <= 136 || AngleWnd.flAngle < RUN_ANGLE_MAX)
		AngleWnd.flAngle++;
	//else return;

	//if(AngleWnd.exAngle%5 <= 0)
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.flAngle == RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,134\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	}
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE,363,94,AngleWnd.flAngle,3);
}

void UI_Angle_OnBnLongClickedBtnFlDown(void)
{
		if(AngleWnd.exAngle>=(AngleWnd.flAngle-25)||AngleWnd.flAngle==20)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
		}	
		else if(AngleWnd.exAngle>=(AngleWnd.flAngle-30) && AngleWnd.exAngle<(AngleWnd.flAngle-25))											
			AngleWnd.flAngle--;
		else {
			AngleWnd.flAngle -= ANGLE_MOVE_GAP;
			if (AngleWnd.flAngle%5) { //pjg++190814
				AngleWnd.flAngle -= AngleWnd.flAngle%5;
			}
		}
	
	//else return;
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.flAngle < RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,133\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
	}
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,94,AngleWnd.flAngle,3);
	
}


void UI_Angle_Init(void)
{		
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_LEFT*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_ANG_BTN_HOME*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_HOME*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_HOME*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_HOME, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_HOME+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_RIGHT*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_RIGHT, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_EXP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXP*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_EXP, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_EXM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXM*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_EXM, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_FLP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLP*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_FLP, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_FLM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLM*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_FLM, 0);
	if(AngleWnd.exAngle > RUN_ANGLE_MIN){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,133\r");
			App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	}
	if(AngleWnd.flAngle < RUN_ANGLE_MAX){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,133\r");
			App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,134\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	}
	if(AngleWnd.flAngle<=(AngleWnd.exAngle+25))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}
	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,94,AngleWnd.exAngle,3);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,94,AngleWnd.flAngle,3);

//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	UI_Angle_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_PINumber_Display();
}

LRESULT UI_Angle_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Angle_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		/*case RID_ANG_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				//MessageBox(hWnd,TEXT("Hello"),TEXT("Button"),MB_OK);
				UI_Angle_OnBnClickedBtnLeft();
				break;
			}
			return 0;*/
		case RID_ANG_BTN_HOME:
		case RID_ANG_BTN_HOMEK:
		case RID_ANG_BTN_HOMEC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Angle_OnBnClickedBtnHome();
				break;
			}
			return 0;
		case RID_ANG_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Angle_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_ANG_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Angle_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_ANG_BTN_EXP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Angle_OnBnClickedBtnExUp();
                break;
            case BN_LONGPUSHED: //long push 
                UI_Angle_OnBnLongClickedBtnExUp();
                break;                                       
			}
			return 0;
		case RID_ANG_BTN_EXM:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Angle_OnBnClickedBtnExDown();
                break;
            case BN_LONGPUSHED: //long push 
                UI_Angle_OnBnLongClickedBtnExDown();
                break;                                        
			}
			return 0;
		case RID_ANG_BTN_FLP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Angle_OnBnClickedBtnFlUp();
                break;
            case BN_LONGPUSHED: //long push 
                UI_Angle_OnBnLongClickedBtnFlUp();
                break;                                       
			}
			return 0;
		case RID_ANG_BTN_FLM:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Angle_OnBnClickedBtnFlDown();
                break;
            case BN_LONGPUSHED: //long push 
                UI_Angle_OnBnLongClickedBtnFlDown();
                break;                                        
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_TIMER:  //pjg++170928
		//UI_Timer(wParam);
		break;
	case WM_KEYUP:  //pjg++170928
		break;
	case WM_KEYLONG:  //pjg++170928
		//UI_Run_OnKeyLong(wParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_Angle_Create(void)
{		
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_Angle_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i angb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_ANGLE][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_User_Process);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_ANGLE_SET_EX;
}
#endif

void UI_SpeedTime_SetSpTm_Pic(short Speed,short Time)
{         
	//TLCD_DrawPicture("<D\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 164,100,angle);
 	//TLCD_DrawPicture("i angleb.bmp,30,70\r");
      //APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i angle-b-22.png,276,70\r");101112
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ab001.png,276,70\r");
    if (Speed == 1) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,190,66\r");
    }
    else if (Speed == 2) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,190,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp01.bmp,190,96\r");
    }
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,190,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp01.bmp,190,96\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp02.bmp,190,66\r");
    }	
    /*    else {
	//	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,51,41\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp01.bmp,190,98\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp01.bmp,190,111\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sp02.bmp,190,66\r");
     }*/

	 if (Time < 10) {
	 	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,93\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
     }
     else if (Time < 20) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,93\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
    }
	else if (Time < 30) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,93\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,125\r");
    }
	else if (Time < 40) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,93\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,125\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,111\r");
	
    }
	else if (Time < 50) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,93\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,125\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,111\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,97\r");
    }
	else if (Time < 60) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,74\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,125\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,111\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,97\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,83\r");
    }
    else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,74\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmwh.bmp,259,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,139\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,125\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,111\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,97\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm01.bmp,259,83\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i tm02.bmp,259,66\r");
    }

	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)SpTmPicSpeedBuf);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)SpTmPicTimeBuf);
	
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sptmic.png,68,74\r");
	
 	//TLCD_DrawPicture((char *)AngPicIndigoBuf);
	//TLCD_DrawPicture((char *)AngPicBlueBuf);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	//TLCD_DrawPicture("!D\r");       
        
	
}

void UI_SpeedTime_OnBnClickedBtnLeft(void)
{
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
   	UI_AngleMeasure_Create();
}

void UI_SpeedTime_OnBnClickedBtnHome(void)
{
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	else {
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
		}
	
   	UI_Home_Create();
}

void UI_SpeedTime_OnBnClickedBtnRight(void)
{
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_Run_InitVar();
	
	//if((EXERCISE_INFO_NUM*19/20) < Exercise.addr && Exercise.addr < EXERCISE_INFO_NUM)
	//	UI_PopupMemoryWarning_Create();
	//else if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	//else	
		UI_Run_Create();
}

void UI_SpeedTime_OnBnClickedBtnSpdUp(void)
{
	int pos;
	
	if (Setup2.language == LT_ENG) pos = 47;
	else pos = 27;
		
	if (SpdTmWnd.speed < MS_SPEED_MAX) SpdTmWnd.speed++;
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,94, SpdTmWnd.speed,1);
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
	if(SpdTmWnd.speed > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDM, BN_PUSHED);
	}
	if(SpdTmWnd.speed == MS_SPEED_MAX)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
		App_SetButtonOption(RID_STS_BTN_SPDP, BN_DISABLE);
	}	

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

//∏≈Õ º”µµ ¥‹∞Ë ∞™ ∞®º“
void UI_SpeedTime_OnBnClickedBtnSpdDown(void)
{
	int pos;
	
	if (Setup2.language == LT_ENG) pos = 47;
	else pos = 27;
	
	if (SpdTmWnd.speed > 1) SpdTmWnd.speed--;
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,94, SpdTmWnd.speed,1);
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
	if(SpdTmWnd.speed == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDM, BN_DISABLE);
	}
	if(SpdTmWnd.speed < MS_SPEED_MAX)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDP, BN_PUSHED);	
	}

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

void UI_SpeedTime_OnBnLongClickedBtnSpdUp(void)
{	
	int pos;
	
	if (Setup2.language == LT_ENG) pos = 47;
	else pos = 27;
	
	if (SpdTmWnd.speed < MS_SPEED_MAX) SpdTmWnd.speed++;
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,94, SpdTmWnd.speed,1);
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
	
	if(SpdTmWnd.speed > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDM, BN_PUSHED);
	}
	if(SpdTmWnd.speed == MS_SPEED_MAX)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
		App_SetButtonOption(RID_STS_BTN_SPDP, BN_DISABLE);
	}	
	
	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

void UI_SpeedTime_OnBnLongClickedBtnSpdDown(void)
{
	int pos;
	
	if (Setup2.language == LT_ENG) pos = 47;
	else pos = 27;
	
	if (SpdTmWnd.speed > 1) SpdTmWnd.speed--;
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,94, SpdTmWnd.speed,1);
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
	
	if(SpdTmWnd.speed == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDM, BN_DISABLE);
	}
	if(SpdTmWnd.speed < MS_SPEED_MAX)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDP, BN_PUSHED);	
	}

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

//±∏µøΩ√∞£ ¡ı∞°
void UI_SpeedTime_OnBnClickedBtnTmUp(void)
{
	  
	//if(SpdTmWnd.time >= 60)  SpdTmWnd.time = 60;
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	if(SpdTmWnd.time < 9999)  SpdTmWnd.time++;
#else
	if(SpdTmWnd.time < 60)  SpdTmWnd.time++;
#endif
	{
		if(Setup3.ProChk == BST_CHECKED){
			if(SpdTmWnd.time > Setup3.PTm){
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
				App_SetButtonOption(RID_STS_BTN_TMM, BN_PUSHED);
			}
		}
		if(Setup3.ProChk == BST_UNCHECKED){
			if(SpdTmWnd.time > 1){
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
				App_SetButtonOption(RID_STS_BTN_TMM, BN_PUSHED);
			}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
			if(SpdTmWnd.time == 9999){
#else
			if(SpdTmWnd.time == 60){
#endif
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,134\r");
				App_SetButtonOption(RID_STS_BTN_TMP, BN_DISABLE);
			}
		}
	}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,4);
#else
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,2);
#endif

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);

}
//±∏µøΩ√∞£ ∞®º“
void UI_SpeedTime_OnBnClickedBtnTmDown(void)
{
	if(SpdTmWnd.time > 1) SpdTmWnd.time--;
	{
		if(Setup3.ProChk == BST_CHECKED){
			if(SpdTmWnd.time <= Setup3.PTm)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
				App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
			}
		}
		if(SpdTmWnd.time == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
		}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
		if(SpdTmWnd.time < 9999)
#else
		if(SpdTmWnd.time < 60)
#endif
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,133\r");
			App_SetButtonOption(RID_STS_BTN_TMP, BN_PUSHED);
		}
	}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,4);
#else
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,2);
#endif
	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);

}

void UI_SpeedTime_OnBnLongClickedBtnTmUp(void)
{	
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	if(SpdTmWnd.time < 9999)
	{	
		if (SpdTmWnd.time < (9999-5))  SpdTmWnd.time += 5;
		else// if(SpdTmWnd.time >= 56) 
			SpdTmWnd.time++;
	}
#else
	if(SpdTmWnd.time < 60)
	{	
		if (SpdTmWnd.time < 56)  SpdTmWnd.time += 5;
		else// if(SpdTmWnd.time >= 56) 
			SpdTmWnd.time++;
	}
#endif
	if(Setup3.ProChk == BST_CHECKED){
		if(SpdTmWnd.time > Setup3.PTm){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
			App_SetButtonOption(RID_STS_BTN_TMM, BN_PUSHED);
		}
	}
	if(Setup3.ProChk == BST_UNCHECKED){
		if(SpdTmWnd.time > 1){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
			App_SetButtonOption(RID_STS_BTN_TMM, BN_PUSHED);
		}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
		if(SpdTmWnd.time == 9999){
#else
		if(SpdTmWnd.time == 60){
#endif
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,134\r");
			App_SetButtonOption(RID_STS_BTN_TMP, BN_DISABLE);
		}
	}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,4);
#else
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,2);
#endif

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

void UI_SpeedTime_OnBnLongClickedBtnTmDown(void)
{	
	if(SpdTmWnd.time > 1)
	{
		if(SpdTmWnd.time < (Setup3.PTm+5))// && SpdTmWnd.time < Setup3.PTm)
			SpdTmWnd.time--;
		else
			SpdTmWnd.time -= 5;
		/*if(SpdTmWnd.time > 1)
		{
			if (SpdTmWnd.time > 7) SpdTmWnd.time -= 5;
			else if(SpdTmWnd.time <= 7 )SpdTmWnd.time --;
		}*/
	}
	
	if(Setup3.ProChk == BST_CHECKED){
		if(SpdTmWnd.time <= Setup3.PTm)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
		}
	}
	if(SpdTmWnd.time == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
		App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
	}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	if(SpdTmWnd.time < 9999)
#else
	if(SpdTmWnd.time < 60)
#endif
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,133\r");
		App_SetButtonOption(RID_STS_BTN_TMP, BN_PUSHED);
	}

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,4);
#else
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,2);
#endif

	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
}

void UI_SpeedTime_OnBnClickedBtnSetup(void)
{
	UI_Setup_Create();
}

void UI_SpeedTime_Process(void)
{
}

void UI_SpeedTime_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_ENG) pos = 47;
	else pos = 27;
	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	// button
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_LEFT*2], ',', 2),
			116, 45, hParent, RID_STS_BTN_LEFT, 0);
	/*API_CreateWindow("", pBtnInfo[RID_STS_BTN_HOME*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_HOME*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_HOME*2], ',', 2),
			116, 45, hParent, RID_STS_BTN_HOME, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_STS_BTN_HOME+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_STS_BTN_HOME+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_STS_BTN_HOME+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_STS_BTN_HOME+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_RIGHT*2], ',', 2),
			116, 45, hParent, RID_STS_BTN_RIGHT, 0);
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_SPDP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_SPDP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_SPDP*2], ',', 2),
			58, 46, hParent, RID_STS_BTN_SPDP, 0);
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_SPDM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_SPDM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_SPDM*2], ',', 2),
			58, 46, hParent, RID_STS_BTN_SPDM, 0);
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_TMP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_TMP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_TMP*2], ',', 2),
			58, 46, hParent, RID_STS_BTN_TMP, 0);
	API_CreateWindow("", pBtnInfo[RID_STS_BTN_TMM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_STS_BTN_TMM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_STS_BTN_TMM*2], ',', 2),
			58, 46, hParent, RID_STS_BTN_TMM, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_SETUP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_SETUP, 0);
	
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	
	FullInfo.type = FT_RUN;

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,94, SpdTmWnd.speed,1);
	#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,4);
	#else
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 355, 94, SpdTmWnd.time,2);
	#endif

	UI_PINumber_Display();
	
	if(Setup3.ProChk == BST_CHECKED){
		if(SpdTmWnd.time <= Setup3.PTm){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
			App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
		}
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,133\r");
		App_SetButtonOption(RID_STS_BTN_TMM, BN_PUSHED);
	}
	if(SpdTmWnd.speed == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,78,133\r");
		App_SetButtonOption(RID_STS_BTN_SPDM, BN_DISABLE);
	}
	else if(SpdTmWnd.speed == MS_SPEED_MAX)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,15,134\r");
		App_SetButtonOption(RID_STS_BTN_SPDP, BN_DISABLE);	
	}
	if(SpdTmWnd.time == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,133\r");
		App_SetButtonOption(RID_STS_BTN_TMM, BN_DISABLE);
	}
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	else if(SpdTmWnd.time == 9999)
#else
	else if(SpdTmWnd.time == 60)
#endif
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,133\r");
		App_SetButtonOption(RID_STS_BTN_TMP, BN_DISABLE);
	}
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);		
	UI_SpeedTime_SetSpTm_Pic(SpdTmWnd.speed,SpdTmWnd.time);
	UI_LED_Control(LM_STAND_BY);
}

LRESULT UI_SpeedTime_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_SpeedTime_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_STS_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				//MessageBox(hWnd,TEXT("Hello"),TEXT("Button"),MB_OK);
				UI_SpeedTime_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_STS_BTN_HOME:
		case RID_STS_BTN_HOMEK:
		case RID_STS_BTN_HOMEC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnHome();
				break;
			}
			return 0;
		case RID_STS_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_STS_BTN_SPDP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnSpdUp();
				break;
			case BN_LONGPUSHED: //long push 
				UI_SpeedTime_OnBnLongClickedBtnSpdUp();
				break;
			}
			return 0;
		case RID_STS_BTN_SPDM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnSpdDown();
				break;
			case BN_LONGPUSHED: //long push 
				UI_SpeedTime_OnBnLongClickedBtnSpdDown();
				break;
			}
			return 0;
		case RID_STS_BTN_TMP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnTmUp();
				break;
			case BN_LONGPUSHED: //long push 
				UI_SpeedTime_OnBnLongClickedBtnTmUp();
				break;
			}
			return 0;
		case RID_STS_BTN_TMM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SpeedTime_OnBnClickedBtnTmDown();
				break;
			case BN_LONGPUSHED: //long push 
				UI_SpeedTime_OnBnLongClickedBtnTmDown();
				break;    
			}
			return 0;
		case RID_RN_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_SpeedTime_OnBnClickedBtnSetup();
                break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_SpeedTime_Create(void)
{
	App_SetWndProcFnPtr(UI_SpeedTime_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i sptmb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SPEED_TIME][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_SPEED;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupRunModeVibReq_OnBnClickedBtnYes(void)
{	
	if (Setup3.VibChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nv.bmp,410,2\r");
		Setup3.VibChk = BST_UNCHECKED;
		Setup3Old.VibChk = BST_UNCHECKED; //pjg++200205
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i v.bmp,410,2\r");
		Setup3.VibChk = BST_CHECKED;
		Setup3Old.VibChk = BST_CHECKED; //pjg++200205
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i np.bmp,342,2\r");
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	}
	UI_SetFunRunMode(UI_FRM_BY_AUTO);
	RunWnd.repeat = 0; //pjg++200205
	RunWnd.time = SpdTmWnd.time; //pjg++200205
	UI_Run_Create();
}

void UI_PopupRunModeVibReq_OnBnClickedBtnNo(void)
{	
	UI_Run_Create();
}

//±∏µøøœ∑· »≠∏È ±∏º∫
void UI_PopupRunModeVibReq_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_YES+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_NO+Setup2.language, 0);
}

LRESULT UI_PopupRunModeVibReq_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupRunModeVibReq_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_FUCH_BTN_YES:
		case RID_FUCH_BTN_YESK:
		case RID_FUCH_BTN_YESC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeVibReq_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_FUCH_BTN_NO:
		case RID_FUCH_BTN_NOK:
		case RID_FUCH_BTN_NOC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeVibReq_OnBnClickedBtnNo();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}
//±∏µøøœ∑· »≠∏È ª˝º∫
void UI_PopupRunModeVibReq_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupRunModeVibReq_WndProc);
	if (Setup3.VibChk == BST_CHECKED) {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_VIB_OFF][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	else {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_VIB_ON][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_RUN_MODE_VIB_REQ;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupRunModeLPauseReq_OnBnClickedBtnYes(void)
{	
	if (Setup3.LPChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nl.bmp,376,2\r");
		Setup3.LPChk = BST_UNCHECKED;
		Setup3Old.LPChk = BST_UNCHECKED; //pjg++200205
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i l.bmp,376,2\r");
		Setup3.LPChk = BST_CHECKED;
		Setup3Old.LPChk = BST_CHECKED; //pjg++200205
	}
	UI_SetFunRunMode(UI_FRM_BY_AUTO);
	RunWnd.repeat = 0; //pjg++200205
	RunWnd.time = SpdTmWnd.time; //pjg++200205
	UI_Run_Create();
}

void UI_PopupRunModeLPauseReq_OnBnClickedBtnNo(void)
{	
	UI_Run_Create();
}

void UI_PopupRunModeLPauseReq_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_YES+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_NO+Setup2.language, 0);
}

LRESULT UI_PopupRunModeLPauseReq_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupRunModeLPauseReq_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_FUCH_BTN_YES:
		case RID_FUCH_BTN_YESK:
		case RID_FUCH_BTN_YESC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeLPauseReq_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_FUCH_BTN_NO:
		case RID_FUCH_BTN_NOK:
		case RID_FUCH_BTN_NOC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeLPauseReq_OnBnClickedBtnNo();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PopupRunModeLPauseReq_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupRunModeLPauseReq_WndProc);
	if (Setup3.LPChk == BST_CHECKED) {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_LP_OFF][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	else {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_LP_ON][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_RUN_MODE_LP_REQ;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupRunModeProgressReq_OnBnClickedBtnYes(void)
{	
	if (Setup3.ProChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i np.bmp,342,2\r");
		Setup3.ProChk = BST_UNCHECKED;
		Setup3Old.ProChk = BST_UNCHECKED; //pjg++200205
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i p.bmp,342,2\r");
		Setup3.ProChk = BST_CHECKED;
		Setup3Old.ProChk = BST_CHECKED; //pjg++200205
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nv.bmp,410,2\r");
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	}
	UI_SetFunRunMode(UI_FRM_BY_AUTO);
	RunWnd.repeat = 0; //pjg++200205
	RunWnd.time = SpdTmWnd.time; //pjg++200205
	UI_Run_Create();
}

void UI_PopupRunModeProgressReq_OnBnClickedBtnNo(void)
{	
	UI_Run_Create();
}

void UI_PopupRunModeProgressReq_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_YES+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_YES+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_FUCH_BTN_NO+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_FUCH_BTN_NO+Setup2.language, 0);
}

LRESULT UI_PopupRunModeProgressReq_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupRunModeProgressReq_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_FUCH_BTN_YES:
		case RID_FUCH_BTN_YESK:
		case RID_FUCH_BTN_YESC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeProgressReq_OnBnClickedBtnYes();
				break;
			}
			return 0;
		case RID_FUCH_BTN_NO:
		case RID_FUCH_BTN_NOK:
		case RID_FUCH_BTN_NOC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunModeProgressReq_OnBnClickedBtnNo();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PopupRunModeProgressReq_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupRunModeProgressReq_WndProc);
	if (Setup3.ProChk == BST_CHECKED) {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_PRO_OFF][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	else {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_PRO_ON][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_RUN_MODE_PRO_REQ;
}

//////////////////////////////////////////////////////////////////////////////
void UI_Run_InitVar(void)
{
//          Total_Counter = 0;
	RunWnd.play = UI_RUN_MODE_STOP;     //±∏µøªÛ≈¬
	RunWnd.dir = 0;                     //±∏µø πÊ«‚
//	RunWnd.angle = AngleWnd.exAngle;    //∏≈Õ¿« ±∏µø∞¢µµ ¿˙¿Â
	RunWnd.repeat = 0;                    //±∏µø π›∫π»∏ºˆ √ ±‚»≠
//       RunWnd.angle = 0;//Current_Angle - 5;	//pjg<>171222
	RunWnd.tick = 0;                    //∏≈Õ ±∏µøΩ√∞£ ∆Ω(Ω√∞£)∞™
	RunWnd.time = SpdTmWnd.time;        //±∏µø«— Ω√∞£(∫– ¥‹¿ß
	//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, RunWnd.angle);
}

void UI_Run_DisplayValue(void)
{
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, RunWnd.angle, 3);
	UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 372,85, RunWnd.repeat);
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 370,160, RunWnd.time, 4);
#else
	 UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 370,160, RunWnd.time, 3);
#endif
	 //Íµ¨Îèô Î∞òÎ≥µÌïú ÌöüÏàò ÌëúÌòÑ
	//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 372,83, RunWnd.repeat);
       //Î™®ÌÑ∞ Í∞ÅÎèÑ ÌëúÌòÑ
	//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 372,157, RunWnd.angle);
	//Íµ¨Îèô ÏãúÍ∞Ñ ÌëúÌòÑ
	//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 370,232, RunWnd.time);
       //UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 233, DISP_NUM_RN2_Y_POS, Total_Counter);
    //ÌòÑÏû¨ÍπåÏßÄ Íµ¨ÎèôÌïú Ï¥ù ÏãúÍ∞Ñ ÌëúÌòÑ
//	PInfoWnd.totalTime++;
	//UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 140, DISP_NUM_RN2_Y_POS, PInfoWnd2.pi.totalTime);
	//if (PInfoWnd.totalTime > 999) PInfoWnd.totalTime = 0;
}

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
void UI_Run_SaveCurrent(void)
{
	FILINFO fno;
	//uint16_t *p;
	int i, cnt;
	uint8_t buf[20];

#if 1
	if (!USBDisk_Link()) {
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB link error... ,5,50\r");
		return;
	}
	if (!USBDisk_Mount()) {
		USBDisk_UnLink();
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB mount error... ,5,50\r");
		return;
	}
	if (!USBDisk_OpenAlwaysWrite((char *)"0:/cur.csv")) {
		USBDisk_UnMount();
		USBDisk_UnLink();
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB open error... ,5,50\r");
		return;
	}	
	if (EEPROMDisk_stat((char *)"0:/cur.csv", &fno) != 1) {
		USBDisk_UnMount();
		USBDisk_UnLink();
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB stat error... ,5,50\r");
		return;
	}
	if (!USBDisk_lseek(fno.fsize)) {
		USBDisk_Close();
		USBDisk_UnMount();
		USBDisk_UnLink();
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB lseek error... ,5,50\r");
		return;
	}

	uart_putstring("fc 255,255,255\r"); 	  
	buf[0] = 'f';
	buf[1] = ' ';
	buf[2] = QCLife.cnt/100+'0';
	cnt = QCLife.cnt%100;
	buf[3] = cnt/10+'0';
	buf[4] = cnt%10+'0';
	buf[5] = ',';
	buf[6] = '3';
	buf[7] = '0';
	buf[8] = '0';
	buf[9] = ',';
	buf[10] = '1';
	buf[11] = '9';
	buf[12] = '0';
	buf[13] = '\r';
	buf[14] = 0;
	uart_putstring((char *)buf);
	QCLife.cnt++;
	cnt = 0;
	cnt = ConvertLong2String(QCLife.cnt, &CommonBuf[cnt], 5);
	CommonBuf[cnt] = ',';
	cnt++;
	for (i = 0; i < 29; i++) {
		cnt += ConvertLong2String(QCLife.curBuf[i], &CommonBuf[cnt], 5);
		CommonBuf[cnt] = ',';
		cnt++;
	}
	CommonBuf[cnt++] = 0x0d;
	CommonBuf[cnt++] = 0x0a;
	if (!USBDisk_Write((uint8_t *)CommonBuf, cnt)) {
		USBDisk_Close();
		USBDisk_UnMount();
		USBDisk_UnLink();
		uart_putstring("fc 255,0,0\r"); 	  
		uart_putstring("f USB write error... ,5,50\r");
		return;
	}
	USBDisk_Close();
	USBDisk_UnMount();
	USBDisk_UnLink();
#else
	uart_putstring("fc 255,255,255\r"); 	  
	buf[0] = 'f';
	buf[1] = ' ';
	buf[2] = QCLife.cnt/100+'0';
	cnt = QCLife.cnt%100;
	buf[3] = cnt/10+'0';
	buf[4] = cnt%10+'0';
	buf[5] = ',';
	buf[6] = '3';
	buf[7] = '0';
	buf[8] = '0';
	buf[9] = ',';
	buf[10] = '1';
	buf[11] = '9';
	buf[12] = '0';
	buf[13] = '\r';
	buf[14] = 0;
	uart_putstring((char *)buf);
	QCLife.cnt++;
#endif	
	//uart_putstring("f          ,300,190\r");
	uart_putstring("fc 0,0,0\r"); 	  
	uart_putstring("f Save Cnt:,150,190\r");
	buf[0] = 'f';
	buf[1] = ' ';
	buf[2] = QCLife.cnt/100+'0';
	cnt = QCLife.cnt%100;
	buf[3] = cnt/10+'0';
	buf[4] = cnt%10+'0';
	buf[5] = ',';
	buf[6] = '3';
	buf[7] = '0';
	buf[8] = '0';
	buf[9] = ',';
	buf[10] = '1';
	buf[11] = '9';
	buf[12] = '0';
	buf[13] = '\r';
	buf[14] = 0;
	uart_putstring((char *)buf);
}
#endif

void UI_Run_Timer(uint16_t nIDEvent)
{
	switch (nIDEvent) {
		case TIMER_ID_2:
			if (RunWnd.time > 0) {
				if(RunWnd.play != UI_RUN_MODE_HOME) {
					if(Timer_sec >= 6){ //1min //pjg<>200103
						RunWnd.time--;
						PInfoWnd2.pi.totalTime++;
						Timer_sec = 0;
						#ifdef USE_QC_LIFE_TEST_SAVE_CUR
						loading++;
						if (loading >= 60) {
							loading = 0;
							QCLife.fStart = 0;
							QCLife.fSave = 0;
							for (int i = 0; i < 29; i++) QCLife.curBuf[i] = 0;
						}
						#endif
					}
					//PInfoWnd2.pi.totalRepeat += RunWnd.repeat;//Total_Counter;
				}
			}
			break;
	}
	//UI_Run_Process();
	UI_Run_DisplayValue();
}

/* pjg--180102
int *UI_GetCurrentMotorAngleVarAddr(void)
{
	return &RunWnd.angle;
}

uint16_t *UI_GetMotorSpeedVarAddr(void)
{
	return &SpdTmWnd.speed;
}
*/

int UI_Run_CheckHomePos(void)
{
	if (Home_sensor == 1) return 1;
	else return 0;
}

void UI_Run_SetGotoHomeCmd(void)
{
	//Home_mode = 1;
	//Home_flag = 0;
	MotorDrv_Run();
  	//MotorDrv_SetTargetPosition(Setup3.IPos-1);
}

void UI_Run_Process_HomeIn(void)
{
	//pjg++>180806
	//if (!UI_CheckMotorStatus()) return;
	//pjg<++180806	
	
#if 1 //pjg<>190904
	if (UI_GotoHomePosition()) {
		//MotorDrv_StopHome();
		//MotorDrv_SetFlagLimitSW(0);
		App_SetUIProcess(UI_ProcessNull);
		
		//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, 0);
		UI_Run_DisplayValue();
		API_ChangeHMenu(hParent, RID_RN_BTN_STOP, RID_RN_BTN_PLAY);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
		App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_PUSHED);
		//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_LEFT, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_SETUP, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_PLAY, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_STOP, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
		KeyDrv_Disable(0);
		UI_Run_InitVar();
		App_KillTimer(TIMER_ID_2);
		RunWnd.play = UI_RUN_MODE_PAUSE;
		//MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent thtat motor is break when fast press machine
		MotorDrv_SetSensCurrent(STANDBY_SENS_CURRENT);
	}
#else
	if (MotorDrv_GetDirection() == MDD_CW) {
		if (MotorDrv_GetLimitSwitchStatus() == 0) {
			MotorDrv_SetMotorState(MOTOR_CCW);
			MotorDrv_SetDirection(MDD_CCW);
			MotorDrv_SetFlagLimitSW(0);
		}
	}
	else {
		if(MotorDrv_GetFlagLimitSW() == 0){
			if (MotorDrv_GetLimitSwitchStatus() == 1) {
				MotorDrv_SetMotorZeroPosition();
				//MotorDrv_StopHome();
				//App_SetUIProcess(UI_ProcessNull);
			}
		}
		else {
			if (UI_GotoHomePosition()) {
				MotorDrv_StopHome();
				MotorDrv_SetFlagLimitSW(0);
				App_SetUIProcess(UI_ProcessNull);
				
				UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, 0);
				API_ChangeHMenu(hParent, RID_RN_BTN_STOP, RID_RN_BTN_PLAY);
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
				App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_PUSHED);
				//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_LEFT, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_SETUP, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_PLAY, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_STOP, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
				App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
				KeyDrv_Disable(0);
		 		//App_SetUIProcess(UI_ProcessNull); //pjg++170529
				//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
				UI_Run_InitVar();
				App_KillTimer(TIMER_ID_2);
				MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent thtat motor is break when fast press machine
			}
		}
	}
#endif

	//if (UI_Run_CheckHomePos())
	/*if (UI_GotoHomePosition())
	{
		MotorDrv_Stop();
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, 0);
		API_ChangeHMenu(hParent, RID_RN_BTN_STOP, RID_RN_BTN_PLAY);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
		App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_RIGHT, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_LEFT, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_SETUP, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_PLAY, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_STOP, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
		KeyDrv_Disable(0);
 		App_SetUIProcess(UI_ProcessNull); //pjg++170529
            	if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
		UI_Run_InitVar();
		App_KillTimer(TIMER_ID_2);
    }*/
}

/*
void UI_Run_Process_HomeIn_Complete(void)
{
	if (UI_Run_CheckHomePos())
	{
		UI_Run_InitVar();
		App_KillTimer(TIMER_ID_2);
		App_SetUIProcess(UI_ProcessNull); //pjg++170529
		UI_PopupRunComplete_Create();
	}
}
*/

void UI_Run_OnBnClickedBtnLeft(void)
{
	//Run_mode = 0;
	MotorDrv_Stop();
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//PInfoWnd2.pi.totalRepeat = Total_Counter;
		//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
	RunWnd.play = 0;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); 
   	UI_SpeedTime_Create();
}

void UI_Run_OnBnClickedBtnHome(void)
{
	//Run_mode = 0;
	MotorDrv_Stop();
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//PInfoWnd2.pi.totalRepeat = Total_Counter;
		//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	}
	RunWnd.play = 0;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); 
   	UI_Home_Create();
}

void UI_Run_OnBnClickedBtnRight(void)
{
   	
}

/*void UI_Run_OnBnClickedBtnUser(void)
{	
	Run_mode = 0;
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//PInfoWnd2.pi.totalRepeat = Total_Counter;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
	RunWnd.play = 0;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); 
   	UI_PiLoad_Create();
}*/

void UI_Run_OnBnClickedBtnSetup(void)
{
	 //Run_mode = 0;
	//MotorDrv_Stop();
	//if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//PInfoWnd2.pi.totalRepeat = Total_Counter;
		//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//}
	//RunWnd.play = 0;
	//API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
	//App_KillTimer(TIMER_ID_2);
	//App_SetUIProcess(UI_ProcessNull); //pjg++170529
	UI_Setup_Create();
}

//pjg++191101
void UI_Run_OnBnClickedBtnPlayHome(void)
{
	if (RunWnd.play == UI_RUN_MODE_HOME) {
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition());
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_STOP);
		App_KillTimer(TIMER_ID_2);
		App_SetUIProcess(UI_ProcessNull);
		UI_LED_Control(LM_PAUSE);
		RunWnd.play = UI_RUN_MODE_HOME_PAUSE;
		return;
	}
	else if (RunWnd.play == UI_RUN_MODE_HOME_PAUSE) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_STOP*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
		App_SetTimer(TIMER_ID_2, 30);
		if (RunWnd.time > 0) App_SetUIProcess(UI_Run_Process_HomeIn);
		else App_SetUIProcess(UI_Run_Process);
		UI_LED_Control(LM_RUN);
		RunWnd.play = UI_RUN_MODE_HOME;
		return;
	}

}

//uint8_t shortkey_one=0;
void UI_Run_OnBnClickedBtnPlayStart(void)
{
	//if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	//else
	{
		MotorDrv_Run();
		UI_SetFunRunMode(UI_FRM_BY_AUTO);
		//RunWnd.time = SpdTmWnd.time; //pjg--200205
		RunWnd.play = UI_RUN_MODE_PLAY;
		API_ChangeHMenu(hParent, RID_RN_BTN_PLAY, RID_RN_BTN_PLAYING);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAYING*2]);
		//if (Setup2.sndGuid2 == BST_CHECKED)
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_START);
		//App_SetTimer(1, 150000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed(150000)
		App_SetTimer(TIMER_ID_2, 30);//00000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed
		App_SetUIProcess(UI_Run_Process); //pjg++170529
		UI_LED_Control(LM_RUN);
		//Total_Counter = PInfoWnd2.pi.totalRepeat;
		Timer_sec = 0; //pjg++170608 buf fix: complete right now
		App_SetButtonOption(RID_RN_BTN_LEFT, BN_DISABLE);
		//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_SETUP, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_PLAY, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_STOP, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_DISABLE);
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
		KeyDrv_Disable(0);	
		UI_Time.tmp_sec = 0;
		MotorDrv_SetOverCurrent(RUN_CHK_OVERCURRENT); //pjg++190904 : to prevent that motor is break when fast press machine
		MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
	}
}

//uint8_t longkey_one=0;
void UI_Run_OnBnLongClickedBtnPlayHome(void)
{
	if (RunWnd.play == UI_RUN_MODE_PLAY) return;

	UI_Run_SetGotoHomeCmd();
  	UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);

	RunWnd.play = UI_RUN_MODE_HOME;
	App_SetTimer(TIMER_ID_2, 30);//00000);
	API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_STOP);
	API_ChangeDnEventedHMenu(hParent, RID_RN_BTN_STOP);
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_STOP);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_STOP*2]);
	//API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PAUSE);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
	//if (Setup2.sndGuid2 == BST_CHECKED)
	APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
	
	App_SetButtonOption(RID_RN_BTN_LEFT, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_SETUP, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_PLAY, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_STOP, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	//KeyDrv_Disable(1);
	App_SetUIProcess(UI_Run_Process_HomeIn); //pjg++171018
	MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent that motor is break when fast press machine
}

//playing->pause
void UI_Run_OnBnClickedBtnPause(void)
{
	MotorDrv_Pause();
	//UI_SetFunRunMode(UI_FRM_BY_AUTO);
	RunWnd.play = UI_RUN_MODE_PAUSE;
	API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PAUSE);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
	//if (Setup2.sndGuid2 == BST_CHECKED)
       APP_SendMessage(0, WM_SOUND, 0, SNDID_OPERATION_STOP);
	//App_SetTimer(1, 150000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed(150000)
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); //pjg++170529
	UI_LED_Control(LM_PAUSE);
	//PInfoWnd2.pi.totalRepeat = Total_Counter;

	App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_LEFT, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_SETUP, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_PLAY, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_STOP, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
	App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
	KeyDrv_Disable(0);
	//MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent thtat motor is break when fast press machine
	MotorDrv_SetSensCurrent(STANDBY_SENS_CURRENT);
	MotorDrv_SetTimerValue(0, 2000);
	MotorDrv_SetSensFlagRunOne(0);
}

//¿œΩ√¡§¡ˆø°º≠ «√∑π¿Ãπˆ∆∞¿ª ¥ŸΩ√ ¥©«¬ ∞ÊøÏ
//pause->playing
void UI_Run_OnBnClickedBtnPlay(void)
{
      // Run_mode = 1;
//	MotorDrv_Run();
	UI_SetFunRunMode(UI_FRM_BY_AUTO);
	RunWnd.play = UI_RUN_MODE_PLAY;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAYING);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAYING*2]);
	//if (Setup2.sndGuid2 == BST_CHECKED)
	APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_START);
	App_SetTimer(TIMER_ID_2, 30);//00000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed
	App_SetUIProcess(UI_Run_Process); //pjg++170529
	UI_LED_Control(LM_RUN);
	//PInfoWnd2.pi.totalRepeat = Total_Counter;
	Timer_sec = 0; //pjg++170608 buf fix: complete right now

	//UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf); //pjg--180806

	App_SetButtonOption(RID_RN_BTN_LEFT, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_HOME+Setup2.language, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_SETUP, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_PLAY, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_STOP, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	KeyDrv_Disable(0);
	//MotorDrv_SetOverCurrent(RUN_CHK_OVERCURRENT); //pjg++190904 : to prevent that motor is break when fast press machine
	MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
} 

void UI_Run_OnBnClickedBtnStop(void)
{	
	if (RunWnd.play == UI_RUN_MODE_HOME) {
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition());
		//API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_STOP);
		API_ChangeDnEventedHMenu(hParent, RID_RN_BTN_PLAY);
		//API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_STOP);
		API_ChangeHMenu(hParent, RID_RN_BTN_STOP, RID_RN_BTN_PLAY);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_STOP);
		App_KillTimer(TIMER_ID_2);
		App_SetUIProcess(UI_ProcessNull);
		UI_LED_Control(LM_PAUSE);
		RunWnd.play = UI_RUN_MODE_HOME_PAUSE;
	}
	else {
		API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_STOP);
		API_ChangeDnEventedHMenu(hParent, RID_RN_BTN_STOP);
		API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_STOP);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_STOP*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
		App_SetTimer(TIMER_ID_2, 30);
		if (RunWnd.time > 0) App_SetUIProcess(UI_Run_Process_HomeIn);
		else App_SetUIProcess(UI_Run_Process);
		UI_LED_Control(LM_RUN);
		RunWnd.play = UI_RUN_MODE_HOME;
	}
}

void UI_Run_OnBnClickedKeyBtnPlayHome(void)
{	
	if (RunWnd.play == UI_RUN_MODE_HOME) {
		MotorDrv_SetTargetPosition(MotorDrv_GetPosition());
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_STOP);
		App_KillTimer(TIMER_ID_2);
		App_SetUIProcess(UI_ProcessNull);
		UI_LED_Control(LM_PAUSE);
		RunWnd.play = UI_RUN_MODE_HOME_PAUSE;
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_STOP*2]);
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
		App_SetTimer(TIMER_ID_2, 30);
		if (RunWnd.time > 0) App_SetUIProcess(UI_Run_Process_HomeIn);
		else App_SetUIProcess(UI_Run_Process);
		UI_LED_Control(LM_RUN);
		RunWnd.play = UI_RUN_MODE_HOME;
	}
}

//∏ÿ√„πˆ∆∞(stop)¿ª ¥©∏•∞ÊøÏ
/*
void UI_Run_OnBnClickedBtnStop(void)
{	
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//RunWnd.play = UI_RUN_MODE_STOP;
		API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
		API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PLAY);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
		if (Setup2.sndGuid2 == BST_CHECKED)
			APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_END);
		App_KillTimer(TIMER_ID_2);
//		UI_Run_PopupMotorOverload(0);
		UI_Run_InitVar();	
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, RunWnd.angle);
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 372,85, RunWnd.repeat);
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 370,160, SpdTmWnd.time);
		//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 67, DISP_NUM_RN1_Y_POS, RunWnd.time);
		//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 197, DISP_NUM_RN1_Y_POS, AngleWnd.exAngle);
		//UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 327, DISP_NUM_RN1_Y_POS, RunWnd.repeat);
		App_SetUIProcess(UI_ProcessNull); //pjg++170529
		UI_LED_Control(LM_PAUSE);
		PInfoWnd2.pi.totalRepeat = Total_Counter;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
}*/

//void UI_Run_OnBnClickedBtnInit(void)
//{
	
	//UI_RunPIInit_Create();
//}

//uint32_t snesedCnt = 0;
//uint32_t snesedCnt2 = 0;
void UI_CheckCurrentSensitivity()
{
	if (MotorDrv_IsSensCurrent() && UI_Time.tmp3_ms == 0) {
		MotorDrv_SetFlagSensCurrent(0);
		UI_Time.tmp3_ms = 5000;
		if (MotorDrv_GetDirection() == MDD_CCW) {
			MotorDrv_SetDirection(MDD_CW);
			MotorDrv_MoveUp();
		}
		else {
			MotorDrv_SetDirection(MDD_CCW);
			MotorDrv_MoveDown();
		}
	}
}

/*
#define TEST_BUF_SIZE			((FLASH_PAGE_SIZE16*2)/4)
struct {
	float buf[TEST_BUF_SIZE/2];
	float buf2[TEST_BUF_SIZE/2];
	uint8_t *ang;//[TEST_BUF_SIZE];
	uint32_t cnt;
}test;
*/

void UI_Run_NormalMode(void)
{
	//uint32_t temp;
	//char buf[20];
	
	//if (!UI_CheckMotorStatus()) return;
	
	if (MotorDrv_GetFlagLimitSW()) {
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_OVER_RANGE, EDT_DISP_HALT);
		return;
	}

	UI_CheckCurrentSensitivity();
	if (MotorDrv_GetDirection() == MDD_CCW) {
		if (MotorDrv_GetPosition() >= AngleWnd.flAngle) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.flAngle);
				//MotorDrv_Stop();
				UI_Time.tmp4_ms = 200;
			}
			if (!UI_Time.tmp4_ms) {
				MotorDrv_SetDirection(MDD_CW);
				MotorDrv_SetFlagRunOne(0);
				MotorDrv_MoveUp();
			}
			//MotorDrv_SetTargetPosition(AngleWnd.exAngle-2);
		}
		else {
			//if (!UI_Time.tmp4_ms) {
				MotorDrv_MoveDown();
				//UI_Time.tmp4_ms = 10;
			//}
		}
	}
	else {
		if (MotorDrv_GetPosition() <= AngleWnd.exAngle) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				UI_Time.tmp4_ms = 500;//000;
			}
			if (!UI_Time.tmp4_ms) {
				MotorDrv_SetDirection(MDD_CCW);
				MotorDrv_SetFlagRunOne(0);
				MotorDrv_MoveDown();
				RunWnd.repeat++;
				PInfoWnd2.pi.totalRepeat++; //pjg++180316
			}
			//MotorDrv_SetTargetPosition(AngleWnd.flAngle+2);
		}
		else {
			//if (!UI_Time.tmp4_ms) {
				MotorDrv_MoveUp();
				//UI_Time.tmp4_ms = 10;
			//}
		}
	}

	if (!RunWnd.time) {
		UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
	}
}

void UI_Run_LimitedPauseMode(void)
{
	//if (!UI_CheckMotorStatus()) return;
	if (MotorDrv_GetFlagLimitSW()) {
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_OVER_RANGE, EDT_DISP_HALT);
		return;
	}
	UI_CheckCurrentSensitivity();
	
	if (MotorDrv_GetDirection() == MDD_CCW) {
		if (MotorDrv_GetPosition() >= AngleWnd.flAngle) {
			if (!MotorDrv_GetFlagLimitedPause()) {
				MotorDrv_SetFlagLimitedPause(1);
				MotorDrv_SetTargetPosition(AngleWnd.flAngle);
				UI_Time.tmp_ms = Setup3.LP*1000;
			}
			if (!UI_Time.tmp_ms) {
				MotorDrv_SetFlagLimitedPause(0);
				MotorDrv_SetDirection(MDD_CW);
				//MotorDrv_SetTargetPosition(AngleWnd.exAngle-2);
				MotorDrv_MoveUp();
			}
		}
		else {
			MotorDrv_MoveDown();
			//MotorDrv_SetTargetPosition(AngleWnd.flAngle+2);
		}
	}
	else {
		if (MotorDrv_GetPosition() <= AngleWnd.exAngle) {
			if (!MotorDrv_GetFlagLimitedPause()) {
				MotorDrv_SetFlagLimitedPause(1);
				MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				UI_Time.tmp_ms = Setup3.LP*1000;
			}
			if (!UI_Time.tmp_ms) {
				MotorDrv_SetFlagLimitedPause(0);
				MotorDrv_SetDirection(MDD_CCW);
				//MotorDrv_SetTargetPosition(AngleWnd.flAngle+2);
				MotorDrv_MoveDown();
				RunWnd.repeat++;
				PInfoWnd2.pi.totalRepeat++; //pjg++180316
			}
		}
		else {
			MotorDrv_MoveUp();
			//MotorDrv_SetTargetPosition(AngleWnd.exAngle-2);
		}
	}

	if (!RunWnd.time) {
		UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
	}
}

void UI_Run_ProgressMode(void)
{
	if ( Setup3.PDeg <= MotorDrv_GetProgressAngle()) { //progress angle end
		if (Setup3.LPChk) UI_Run_LimitedPauseMode();
		else UI_Run_NormalMode();
	}
	else {
		//if (!UI_CheckMotorStatus()) return;
		if (MotorDrv_GetFlagLimitSW()) {
			MotorDrv_Stop();
			UI_SetFunRunMode(UI_FRM_NONE);
			API_SetErrorCode(EC_OVER_RANGE, EDT_DISP_HALT);
			return;
		}
		UI_CheckCurrentSensitivity();

		if (MotorDrv_GetDirection() == MDD_CCW) {
			if (MotorDrv_GetPosition() > 
				(AngleWnd.flAngle - Setup3.PDeg + MotorDrv_GetProgressAngle())) {
				//MotorDrv_SetTargetPosition(AngleWnd.flAngle - Setup3.PDeg + MotorDrv_GetProgressAngle());
				if (Setup3.LPChk) {	//limited pause
					if (!MotorDrv_GetFlagLimitedPause()) {
						MotorDrv_SetFlagLimitedPause(1);
						UI_Time.tmp_ms = Setup3.LP*1000;
					}
					if (!UI_Time.tmp_ms) {
						MotorDrv_SetDirection(MDD_CW);
						//MotorDrv_SetTargetPosition(AngleWnd.exAngle + Setup3.PDeg - MotorDrv_GetProgressAngle()-1);
						MotorDrv_SetFlagLimitedPause(0);
						MotorDrv_MoveUp();
					}
				}
				else {
					if (!MotorDrv_GetFlagRunOne()) {
						MotorDrv_SetFlagRunOne(1);
						UI_Time.tmp4_ms = 400;//200;
					}
					if (!UI_Time.tmp4_ms) {
						MotorDrv_SetFlagRunOne(0);
						MotorDrv_SetDirection(MDD_CW);
						//MotorDrv_SetTargetPosition(AngleWnd.exAngle-2);
						MotorDrv_MoveUp();
					}
				}
			}
			else MotorDrv_MoveDown();
		}
		else {	//Motor CW
			if (MotorDrv_GetPosition() <
				(AngleWnd.exAngle + Setup3.PDeg - MotorDrv_GetProgressAngle())) {
				MotorDrv_SetTargetPosition(AngleWnd.exAngle + Setup3.PDeg - MotorDrv_GetProgressAngle());
				if (Setup3.LPChk) {	//limited pause
					if (!MotorDrv_GetFlagLimitedPause()) {
						MotorDrv_SetFlagLimitedPause(1);
						UI_Time.tmp_ms = Setup3.LP*1000;
					}
					if (!UI_Time.tmp_ms) {
						RunWnd.repeat++;
						MotorDrv_SetDirection(MDD_CCW);
						//MotorDrv_SetTargetPosition(AngleWnd.flAngle - Setup3.PDeg + MotorDrv_GetProgressAngle()+1);
						MotorDrv_SetFlagLimitedPause(0);
						MotorDrv_MoveDown();
					}
				}
				else {
					if (!MotorDrv_GetFlagRunOne()) {
						MotorDrv_SetFlagRunOne(1);
						UI_Time.tmp4_ms = 200;
					}
					if (!UI_Time.tmp4_ms) {
						MotorDrv_SetFlagRunOne(0);
						RunWnd.repeat++;
						PInfoWnd2.pi.totalRepeat++; //pjg++180316
						MotorDrv_SetDirection(MDD_CCW);
						//MotorDrv_SetTargetPosition(AngleWnd.flAngle+2);
						MotorDrv_MoveDown();
					}
				}
			}
			else MotorDrv_MoveUp();
		}

		if (MotorDrv_GetPosition() >= AngleWnd.exAngle && MotorDrv_GetPosition() <= AngleWnd.flAngle) {
			if (Setup3.LPChk) {		
				if (!UI_Time.tmp2_ms) {
					UI_Time.tmp2_ms = (Setup3.PTm*60*1000)/Setup3.PDeg +
														(Setup3.LP*2)*1000;			
					MotorDrv_SetProgressAngle(MotorDrv_GetProgressAngle()+1);
				}
			}
			else {
				if (!UI_Time.tmp2_ms) {
					UI_Time.tmp2_ms = (Setup3.PTm*60*1000)/Setup3.PDeg;		
					MotorDrv_SetProgressAngle(MotorDrv_GetProgressAngle()+1);
				}
			}
		}
	}
	
	if (!RunWnd.time) {
		UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
	}
}

void UI_Run_SubVibrationDownMode(void)
{
	if (MotorDrv_GetPosition() >= AngleWnd.flAngle && MotorDrv_GetDirection() == MDD_CCW) {
		//if (!MotorDrv_GetVibrationCount())
		//	MotorDrv_SetTargetPosition(AngleWnd.flAngle);
		if (Setup3.LPChk) {	//limited pause
			if (MotorDrv_GetVibrationCount() == 0) {
				if (!MotorDrv_GetFlagLimitedPause()) {
					MotorDrv_SetFlagLimitedPause(1);
					UI_Time.tmp_ms = Setup3.LP*1000;
					MotorDrv_SetTargetPosition(AngleWnd.flAngle);
				}
			}
			
		}

		if (!UI_Time.tmp_ms) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.flAngle);
				//MotorDrv_Stop();
				UI_Time.tmp4_ms = 300;
			}
			if (!UI_Time.tmp4_ms) {
				if (MotorDrv_GetDirection() == MDD_CCW) {
					MotorDrv_SetFlagRunOne(0);
					MotorDrv_SetDirection(MDD_CW);
					//MotorDrv_SetTargetPosition(AngleWnd.flAngle-Setup3.VDeg-3);
					//MotorDrv_MoveUp();
				}
				else 
                             	MotorDrv_MoveUp();
                             	//MotorDrv_MoveUp();
			}
		}
	}
	else {
		if (MotorDrv_GetDirection() == MDD_CW) {
			if(MotorDrv_GetPosition() <= AngleWnd.flAngle-Setup3.VDeg)
			{
				if (!MotorDrv_GetFlagRunOne()) {
					MotorDrv_SetFlagRunOne(1);
					MotorDrv_SetTargetPosition(AngleWnd.flAngle-Setup3.VDeg);
					//MotorDrv_Stop();
					UI_Time.tmp4_ms = 900;
				}
				if (!UI_Time.tmp4_ms) {
					MotorDrv_SetFlagRunOne(0);
					MotorDrv_SetDirection(MDD_CCW);
					//MotorDrv_Stop();
					//MotorDrv_SetTargetPosition(AngleWnd.flAngle-Setup3.VDeg);
					MotorDrv_MoveDown();
					MotorDrv_SetVibrationCount(MotorDrv_GetVibrationCount()+1);
					//MotorDrv_Run();
					//UI_Time.tmp_ms = 300;
				}
			}
			else MotorDrv_MoveUp();
		
		}
		else MotorDrv_MoveDown();
	}
}

void UI_Run_SubVibrationUpMode(void)
{
	if (MotorDrv_GetPosition() <=  AngleWnd.exAngle && MotorDrv_GetDirection() == MDD_CW) {
		//if (!MotorDrv_GetVibrationCount())
		//	MotorDrv_SetTargetPosition(AngleWnd.exAngle);
		if (Setup3.LPChk) {	//limited pause
			if (MotorDrv_GetVibrationCount() == 0) {
				if (!MotorDrv_GetFlagLimitedPause()) {
					MotorDrv_SetFlagLimitedPause(1);
					UI_Time.tmp_ms = Setup3.LP*1000;
					MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				}
			}
		}
		if (!UI_Time.tmp_ms) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				UI_Time.tmp4_ms = 1000;
			}
			if (!UI_Time.tmp4_ms) {
				if (MotorDrv_GetDirection() == MDD_CW) {
					MotorDrv_SetFlagRunOne(0);
					MotorDrv_SetDirection(MDD_CCW);
					//MotorDrv_SetTargetPosition(AngleWnd.exAngle+Setup3.VDeg+3);
					//MotorDrv_MoveDown();
				}
				else MotorDrv_MoveDown();
			}
		}
	}
	else {
		if (MotorDrv_GetDirection() == MDD_CCW) {
			if(MotorDrv_GetPosition() >=  AngleWnd.exAngle+Setup3.VDeg)
			{
				if (!MotorDrv_GetFlagRunOne()) {
					MotorDrv_SetFlagRunOne(1);
					MotorDrv_SetTargetPosition(AngleWnd.exAngle+Setup3.VDeg);
					//MotorDrv_Stop();
					UI_Time.tmp4_ms = 400;
				}
				if (!UI_Time.tmp4_ms) {
					MotorDrv_SetFlagRunOne(0);
					MotorDrv_SetDirection(MDD_CW);
					//MotorDrv_SetTargetPosition(AngleWnd.exAngle+Setup3.VDeg);
					MotorDrv_MoveUp();
					MotorDrv_SetVibrationCount(MotorDrv_GetVibrationCount()+1);
				}
			}
			else MotorDrv_MoveDown();
		}
		else MotorDrv_MoveUp();
	}
}

void UI_Run_VibrationMode(void)
{
	//if (!UI_CheckMotorStatus()) return;
	if (MotorDrv_GetFlagLimitSW()) {
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_OVER_RANGE, EDT_DISP_HALT);
		return;
	}
	UI_CheckCurrentSensitivity();

	if (MotorDrv_GetVibrationDir() == MDD_CCW) {
		if(MotorDrv_GetVibrationCount() < Setup3.VCnt){
			UI_Run_SubVibrationDownMode();
		}
		else {
			MotorDrv_SetVibrationDir(MDD_CW);
			MotorDrv_SetVibrationCount(0);
			MotorDrv_SetDirection(MDD_CW);
			//MotorDrv_SetTargetPosition(AngleWnd.exAngle-2);
			MotorDrv_SetFlagLimitedPause(0);
			MotorDrv_SetFlagRunOne(0);
			MotorDrv_MoveUp();
		}
	}
	else {
		if(MotorDrv_GetVibrationCount() < Setup3.VCnt){
			UI_Run_SubVibrationUpMode();
		}
		else {
			RunWnd.repeat++;
			PInfoWnd2.pi.totalRepeat++; //pjg++180316
			MotorDrv_SetVibrationDir(MDD_CCW);
			MotorDrv_SetVibrationCount(0);
			MotorDrv_SetDirection(MDD_CCW);
			//MotorDrv_SetTargetPosition(AngleWnd.flAngle+2);
			MotorDrv_SetFlagLimitedPause(0);
			MotorDrv_SetFlagRunOne(0);
			MotorDrv_MoveDown();
              }
	}
	
	if (!RunWnd.time) {
		UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
	}
}

void UI_Run_OnBnClickedBtnProgressChk(void)	 
{
	if (Setup3.VibChk != BST_CHECKED)
		UI_PopupRunModeProgressReq_Create();
}

void UI_Run_OnBnClickedBtnLPChk(void)	 
{
	UI_PopupRunModeLPauseReq_Create();
}

void UI_Run_OnBnClickedBtnVibrationChk(void)		 
{
	if (Setup3.ProChk != BST_CHECKED)
		UI_PopupRunModeVibReq_Create();
}

void UI_SetFunRunMode(uint8_t mode)
{
	uint8_t temp_mode;
	//int32_t targetPos;
	
	if (mode == UI_FRM_BY_AUTO) {
		if(Setup3.ProChk){
			temp_mode = UI_FRM_PROGRESS;
		}
		else if(Setup3.VibChk){
			temp_mode = UI_FRM_VIBRATION;
		}
		else if(Setup3.LPChk){
			temp_mode = UI_FRM_LIMITED_PAUSE;
		}
		else{
			temp_mode = UI_FRM_NORMAL;
		}
	}
	else {
		temp_mode = mode;
	}

	if (MotorDrv_GetPosition() > MotorDrv_GetTargetPosition()) {
		if (RunWnd.play != UI_RUN_MODE_PAUSE) {	//pjg++180116
			MotorDrv_SetDirection(MDD_CW);
		}
		//targetPos = AngleWnd.exAngle-2;
		MotorDrv_SetVibrationDir(MDD_CW); 
	}
	else {
		if (RunWnd.play != UI_RUN_MODE_PAUSE) {	//pjg++180116
			MotorDrv_SetDirection(MDD_CCW);
		}
		//targetPos = AngleWnd.flAngle+2;
		MotorDrv_SetVibrationDir(MDD_CCW); 
	}

	MotorDrv_SetFlagRunOne(0);
	
	switch (temp_mode) {
	case UI_FRM_GOTO_HOME_POS:
		fnUi_FunRunMode = UI_ProcessNull;
		//MotorDrv_SetTargetPosition(-2);
		break;
	case UI_FRM_NORMAL:
		fnUi_FunRunMode = UI_Run_NormalMode;
		//MotorDrv_SetTargetPosition(targetPos);
		break;
	case UI_FRM_LIMITED_PAUSE:
		fnUi_FunRunMode = UI_Run_LimitedPauseMode;
		MotorDrv_SetFlagLimitedPause(0);
		//MotorDrv_SetTargetPosition(targetPos);
		break;
	case UI_FRM_PROGRESS:
		fnUi_FunRunMode = UI_Run_ProgressMode;
		MotorDrv_SetProgressAngle(0);
		//MotorDrv_SetTargetPosition(targetPos);//AngleWnd.flAngle - Setup3.PDeg+2);
		UI_Time.tmp_ms = 0;
		UI_Time.tmp2_ms = (Setup3.PTm*60*1000)/Setup3.PDeg;	
		break;
	case UI_FRM_VIBRATION:
		fnUi_FunRunMode = UI_Run_VibrationMode;
		//MotorDrv_SetTargetPosition(targetPos);
		break;
	default:
		fnUi_FunRunMode = UI_ProcessNull;
		break;
	}

}

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
void UI_Run_QCLifeTest_SaveCurAtDeg(void)
{
	if (MotorDrv_GetPosition() == 0) {
		QCLife.fStart = 1;
	}

	Product_Calib.curCurrent += MotorDrv_GetCurrent();
	Product_Calib.cnt++;
	if (Product_Calib.cnt >= PROD_CAL_BUF_SIZE) {
		Product_Calib.curCurrent /= Product_Calib.cnt;
		Product_Calib.oldCurrent = Product_Calib.curCurrent;
		Product_Calib.curCurrent = 0;
		Product_Calib.cnt = 0;
	}
	//else return;

	if (QCLife.fStart == 0) return;
	if (QCLife.fSave == 1) return;

	if (MotorDrv_GetPosition() == 1) {
		if (QCLife.curBuf[0] == 0) {
			QCLife.curBuf[0] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28] = Product_Calib.oldCurrent;
			if (QCLife.fSave == 0) {
				QCLife.fSave = 1;
				UI_Run_SaveCurrent();
			}
		}
	}
	else if (MotorDrv_GetPosition() == 10) {
		if (QCLife.curBuf[1] == 0) {
			QCLife.curBuf[1] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-1] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-1] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 20) {
		if (QCLife.curBuf[2] == 0) {
			QCLife.curBuf[2] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-2] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-2] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 30) {
		if (QCLife.curBuf[3] == 0) {
			QCLife.curBuf[3] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-3] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-3] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 40) {
		if (QCLife.curBuf[4] == 0) {
			QCLife.curBuf[4] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-4] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-4] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 50) {
		if (QCLife.curBuf[5] == 0) {
			QCLife.curBuf[5] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-5] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-5] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 60) {
		if (QCLife.curBuf[6] == 0) {
			QCLife.curBuf[6] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-6] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-6] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 70) {
		if (QCLife.curBuf[7] == 0) {
			QCLife.curBuf[7] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-7] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-7] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 80) {
		if (QCLife.curBuf[8] == 0) {
			QCLife.curBuf[8] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-8] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-8] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 90) {
		if (QCLife.curBuf[9] == 0) {
			QCLife.curBuf[9] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-9] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-9] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 100) {
		if (QCLife.curBuf[10] == 0) {
			QCLife.curBuf[10] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-10] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-10] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 110) {
		if (QCLife.curBuf[11] == 0) {
			QCLife.curBuf[11] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-11] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-11] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 120) {
		if (QCLife.curBuf[12] == 0) {
			QCLife.curBuf[12] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-12] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-12] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 130) {
		if (QCLife.curBuf[13] == 0) {
			QCLife.curBuf[13] = Product_Calib.oldCurrent;
		}
		else if (QCLife.curBuf[28-13] == 0 && MotorDrv_GetDirection() == MDD_CW) {
			QCLife.curBuf[28-13] = Product_Calib.oldCurrent;
		}
	}
	else if (MotorDrv_GetPosition() == 139) {
		if (QCLife.curBuf[14] == 0) {
			QCLife.curBuf[14] = Product_Calib.oldCurrent;
		}
		//else if (QCLife.curBuf[28-14] == 0 && MotorDrv_GetDirection() == MDD_CW) {
		//	QCLife.curBuf[28-14] = Product_Calib.oldCurrent;
		//}
	}
}
#endif

//±∏µø¡ﬂø° Ω««‡µ«¥¬ «‘ºˆ
void UI_Run_Process(void)
{
	//uint32_t temp;
	//char buf[30];
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	//uint16_t *p;
#endif

   //∏≈Õ ±∏µø¿Ã øœ∑· µ«æ˙¿ª ∞ÊøÏ
	if (RunWnd.time == 0) {
		if (RunWnd.play == UI_RUN_MODE_PLAY) {
			API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
			API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PLAY);
			if (loginInfo.type == LIT_USER) {
				UI_Exercise_SaveData(PInfoWnd2.id, 1, 0);
			}
			RunWnd.play = UI_RUN_MODE_HOME;
			UI_Run_SetGotoHomeCmd();
			UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
			APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
		}

		if (UI_GotoHomePosition()) {
			App_KillTimer(TIMER_ID_2);
			App_SetUIProcess(UI_ProcessNull); //pjg++170529
			RunWnd.complete = 1;
			UI_PopupRunComplete_Create();
		}
		return;
	}
	fnUi_FunRunMode();

#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	UI_Run_QCLifeTest_SaveCurAtDeg();
#endif	

#if 0 // current monitor
	if (Option.curDisp) 
	{
		temp = MotorDrv_GetCurrent();
		ConvertULong2String((int16_t)temp, (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3(',');
		ConvertULong2String((int16_t)RunWnd.angle, (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3(',');
		ConvertULong2String((int16_t)UI_Time.tmp_sec, (uint8_t *)buf, 11);
		uart_putstring3(buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
	}
#endif
}
//±∏µø¡ﬂø° Ω««‡µ«¥¬ «‘ºˆ
void UI_Run_OnKeyUp(uint32_t nChar)
{
	switch (nChar) {
	case KEY_STOP: 	
		if (RunWnd.play == UI_RUN_MODE_STOP) {
			UI_Run_OnBnClickedBtnPlayStart();
		}
		else if (RunWnd.play == UI_RUN_MODE_PLAY) {
			UI_Run_OnBnClickedBtnPause();
		}
		else if (RunWnd.play == UI_RUN_MODE_PAUSE) {
			UI_Run_OnBnClickedBtnPlay();
		}
		else if (RunWnd.play == UI_RUN_MODE_HOME) {
			UI_Run_OnBnClickedKeyBtnPlayHome();
		}
		else if (RunWnd.play == UI_RUN_MODE_HOME_PAUSE) {
			UI_Run_OnBnClickedKeyBtnPlayHome();
		}
		break;
	default:
		break;
	}
}

void UI_Run_OnKeyLong(uint32_t nChar)
{
	switch (nChar) {
	case KEY_STOP:
		UI_Run_OnBnLongClickedBtnPlayHome();
        break;
	default:
		break;
	}
}

void	UI_Run_Init(void)
{       
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	int i;
#endif

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	// button
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_LEFT*2], ',', 2),
			116, 45, hParent, RID_RN_BTN_LEFT, 0);
	API_CreateWindow("", pBtnInfo[(RID_RN_BTN_HOME+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RN_BTN_HOME+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RN_BTN_HOME+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_RN_BTN_HOME+Setup2.language, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_RN_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_RIGHT*2], ',', 2),
			116, 45, hParent, RID_RN_BTN_RIGHT, 0);*/
	/*API_CreateWindow("", pBtnInfo[RID_RN_BTN_USER*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_USER*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_USER*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_USER, 0);*/
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_SETUP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_SETUP, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_PLAY*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_PLAY*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_PLAY*2], ',', 2),
			138, 139, hParent, RID_RN_BTN_PLAY, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_PROCHK*2], BS_CHECKBOX, 
			342,2,
			30, 30, hParent, RID_RN_BTN_PROCHK, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_LIMCHK*2], BS_CHECKBOX, 
			376,2,
			30, 30, hParent, RID_RN_BTN_LIMCHK, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_VIBCHK*2], BS_CHECKBOX, 
			410,2,
			30, 30, hParent, RID_RN_BTN_VIBCHK, 0);

	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	//RunWnd.angle = 0; //1deg bug
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 6,87, AngleWnd.exAngle,3);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 71,87, AngleWnd.flAngle,3);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i flxbg3.png,70,79\r");
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 30,160, RunWnd.angle, 3);
	UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 372,85, RunWnd.repeat);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 370,160, RunWnd.time, 3);//SpdTmWnd.time, 3);
	
	UI_PINumber_Display();
	
	if (Setup3.ProChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i p.bmp,342,2\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nv.bmp,410,2\r");
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i np.bmp,342,2\r");
		App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
	}
	if (Setup3.LPChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i l.bmp,376,2\r");
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nl.bmp,376,2\r");
	}
	if (Setup3.VibChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i v.bmp,410,2\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i np.bmp,342,2\r");
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nv.bmp,410,2\r");
		App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
	}

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	MotorDrv_SetSensCurrent(STANDBY_SENS_CURRENT);
#ifdef USE_QC_LIFE_TEST_SAVE_CUR
	for (i = 0; i < 29; i++) QCLife.curBuf[i] = 0;
	QCLife.fStart = 0;
	QCLife.fSave = 0;
	QCLife.cnt = 0;
	loading = 0;
	Product_Calib.oldCurrent = 0;
	
#endif
}

LRESULT UI_Run_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Run_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// PI
		case RID_RN_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				//MessageBox(hWnd,TEXT("Hello"),TEXT("Button"),MB_OK);
				UI_Run_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_RN_BTN_HOME:
		case RID_RN_BTN_HOMEK:
		case RID_RN_BTN_HOMEC:
			switch(lParam) {
				case BN_CLICKED: // push 
					UI_Run_OnBnClickedBtnHome();
				break;
			}
			return 0;	
		/*case RID_RN_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnRight();
				break;
			}
			return 0;*/
		/*case RID_RN_BTN_USER:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnUser();
				break;
			}
			return 0;*/
		case RID_RN_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnSetup();
				break;
			}
			return 0;	
		case RID_RN_BTN_PLAY:
			switch(lParam) {
			case BN_CLICKED: // push 
				if (RunWnd.play == UI_RUN_MODE_HOME || RunWnd.play == UI_RUN_MODE_HOME_PAUSE) {
					UI_Run_OnBnClickedBtnPlayHome();
				}
				else UI_Run_OnBnClickedBtnPlayStart();
				break;
			case BN_LONGPUSHED: //long push 
				if (App_GetLongTouchCnt() > 1) break;
                UI_Run_OnBnLongClickedBtnPlayHome();
                break; 
			}
			return 0;
		case RID_RN_BTN_PLAYING:
			switch(lParam) {
			case BN_CLICKED: // push    
				if (UI_Time.tmp5_ms) break;
				UI_Time.tmp5_ms = 700;
				UI_Run_OnBnClickedBtnPause();
				break;
			case BN_LONGPUSHED: //long push 
				//if (App_GetLongTouchCnt() > 1) break;
				//UI_Run_OnBnLongClickedBtnPlayHome();
	            break; 
			}
			return 0;
		case RID_RN_BTN_PAUSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				if (UI_Time.tmp5_ms) break; //pjg++191028 : to prevent fast start
				UI_Time.tmp5_ms = 700;
				UI_Run_OnBnClickedBtnPlay();
				break;
			case BN_LONGPUSHED: //long push 
				if (App_GetLongTouchCnt() > 1) break;
                UI_Run_OnBnLongClickedBtnPlayHome();
                break; 
			}
			return 0;
	    case RID_RN_BTN_STOP:
			switch(lParam) {
			case BN_CLICKED: // push 
				if (UI_Time.tmp5_ms) break; //pjg++191028 : to prevent fast start
				UI_Time.tmp5_ms = 700;
				UI_Run_OnBnClickedBtnStop();
				break;
			}
			return 0;
		/*case RID_RN_BTN_MHOME:
			switch(lParam) {
			case BN_CLICKED: // push                               
				UI_Run_OnBnLongClickedBtnPlayHome();
				break;
			}
			return 0;*/
		case RID_RN_BTN_PROCHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                //Run_mode = 0;    
                // MotorDrv_Stop();
				UI_Run_OnBnClickedBtnProgressChk();
				break;
			}
			return 0;
		case RID_RN_BTN_LIMCHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                //Run_mode = 0;     
               	//MotorDrv_Stop();
				UI_Run_OnBnClickedBtnLPChk();
				break;
			}
			return 0;	
		case RID_RN_BTN_VIBCHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                //Run_mode = 0;    
                //MotorDrv_Stop();
				UI_Run_OnBnClickedBtnVibrationChk();
				break;
			}
			return 0;
		/*
		case RID_RN_BTN_ANGLE_PLAY:
			switch(lParam) {
			case BN_CLICKED: // push 
				if (RunWnd.play == UI_RUN_MODE_PLAY) UI_Run_OnBnClickedBtnPause();
				else UI_Run_OnBnClickedBtnAutoCheckAngle();
				break;
			}
			return 0;	
		case RID_RN_BTN_ANGLE_CHECK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SetAngleOfAngleCheck();
				break;
			}
			return 0;	
		*/
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_TIMER:  //pjg++170928
		UI_Run_Timer(wParam);
		break;
	case WM_KEYUP:  //pjg++170928
		if (UI_Time.tmp5_ms) break; //prevent 
		UI_Time.tmp5_ms = 700;
		UI_Run_OnKeyUp(wParam);
		break;
	case WM_KEYLONG:  //pjg++170928
		if (App_GetLongButtonCnt() > 1) break;
		UI_Run_OnKeyLong(wParam);
		break;
	default:
		break;
	}

	return 0;
}

//∏ﬁ¿Œ »≠∏È ª˝º∫
void UI_Run_Create(void)
{	
	App_SetWndProcFnPtr(UI_Run_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i runb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_RUN][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);

	App_SetUIProcess(UI_ProcessNull);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_RUN;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}

//////////////////////////////////////////////////////////////////////////////
// run complete(±∏µøøœ∑·)
void UI_PopupRunComplete_OnBnClickedBtnOk(void)
{	
	//UI_AngleMeasure_Create();
	//UI_Run_Create();
	UI_LED_Control(LM_STAND_BY);
	//Total_Counter = PInfoWnd2.pi.totalRepeat;
	//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	UI_Home_Create();
}

void UI_PopupRunComplete_OnBnClickedBtnNo(void)
{	
	UI_Run_Create();
	UI_LED_Control(LM_STAND_BY);
	//Total_Counter = PInfoWnd2.pi.totalRepeat;
	if (loginInfo.type == LIT_USER) {
		UI_Exercise_SaveData(PInfoWnd2.id, 1, 0);
	}
	//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	RunWnd.repeat = 0;                    //±∏µø π›∫π»∏ºˆ √ ±‚»≠
}

//±∏µøøœ∑· »≠∏È ±∏º∫
void UI_PopupRunComplete_Init(void)
{
	//int i;
	//uint32_t temp;
	//uint32_t temp2;
	
	// button
	API_CreateWindow("", pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_RC_BTN_OK+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[(RID_RC_BTN_NO*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[(RID_RC_BTN_NO*2], ',', 1),
	//		GetNumFromString(pBtnInfo[(RID_RC_BTN_NO*2], ',', 2),
	//		124, 51, hParent, RID_RC_BTN_NO, 0);
	
	//if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	

	//if(PInfoWnd2.id == PATIENT_NUM)
	//	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i gueb.bmp,70,143\r");
	//else
	//	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 55,143,PInfoWnd2.id+1);
	//temp = PInfoWnd2.id;
	//for (i = 0; i < 8; i++) {
	//	temp2 = Multiplier(10, 7-i);
	//	UI_DisplayNumber(UI_DISP_NUM_FNT9, 55+i*24,143,temp/temp2, UI_DISP_NUM_1PLACE);
	//	temp %= temp2;
	//}
	if (Setup2.language == LT_ENG) {
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 268, 117, RunWnd.repeat);
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 268, 146, SpdTmWnd.time);
	}
	else if (Setup2.language == LT_KOR) {
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 268, 117, RunWnd.repeat);
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 268, 146, SpdTmWnd.time);
	}
	else {
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 268, 117, RunWnd.repeat);
		UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 268, 146, SpdTmWnd.time);
	}
	
	UI_LED_Control(LM_Complet);
    //if (Setup2.sndGuid2 == BST_CHECKED)
       APP_SendMessage(hParent, WM_SOUND, 0, SNDID_RUN_COMPLETE);
}

LRESULT UI_PopupRunComplete_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupRunComplete_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_RC_BTN_OK:
		case RID_RC_BTN_OKK:
		case RID_RC_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunComplete_OnBnClickedBtnOk();
				break;
			}
			return 0;
		case RID_RC_BTN_NO:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunComplete_OnBnClickedBtnOk();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}
//±∏µøøœ∑· »≠∏È ª˝º∫
void UI_PopupRunComplete_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_PopupRunComplete_WndProc);
	//hParent = API_CreateWindow("main", (LPCSTR)"i runc.png,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_RUN_COMPLETE][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_Run_Process);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_RUN_COMPLETE;
	
}

///////////////////////////////////////////////////////////////////////////////
void UI_AdSet_OnBnClickedBtnExit(void)
{	
	UI_Setup_Create();
}

void UI_AdSet_OnBnClickedBtnProgress(void)
{	
	UI_Progress_Create();
}

void UI_AdSet_OnBnClickedBtnLimitPause(void)
{	
	UI_Pause_Create();
}

void UI_AdSet_OnBnClickedBtnVibration(void)
{	
	UI_Vibration_Create();
}

void UI_AdSet_OnBnClickedBtnSensitivity(void)
{	
	UI_Sens_Create();
}

void UI_AdSet_OnBnClickedBtnInitPosition(void)
{	
	UI_HomePos_Create();
}

void UI_AdSet_OnBnClickedBtnMeaSensitivity(void)
{	
	UI_AngleMeaSens_Create();
}

void UI_AdSet_OnBnClickedBtnAngleMoveStep(void)
{	
	UI_AngleBtnStep_Create();
}

void UI_AdSet_OnBnClickedBtnAutoAngleValue(void)
{	
	UI_AutoAngleValue_Create();
}

void UI_AdSet_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[RID_SYS_BTN_BACK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BTN_BACK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BTN_BACK*2], ',', 2),
			47,45, hParent, RID_SYS_BTN_BACK, 0);
	API_CreateWindow("", pBtnInfo[RID_AS_BTN_PRO*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_AS_BTN_PRO*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_AS_BTN_PRO*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_PRO, 0);
	API_CreateWindow("", pBtnInfo[RID_AS_BTN_LIMIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_AS_BTN_LIMIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_AS_BTN_LIMIT*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_LIMIT, 0);
	API_CreateWindow("", pBtnInfo[RID_AS_BTN_VIBR*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_AS_BTN_VIBR*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_AS_BTN_VIBR*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_VIBR, 0);
	API_CreateWindow("", pBtnInfo[(RID_AS_BTN_SENS+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_AS_BTN_SENS+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_AS_BTN_SENS+Setup2.language)*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_SENS+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_AS_BTN_INIT_POS+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_AS_BTN_INIT_POS+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_AS_BTN_INIT_POS+Setup2.language)*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_INIT_POS+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_AS_BTN_MEA_SENS+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_AS_BTN_MEA_SENS+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_AS_BTN_MEA_SENS+Setup2.language)*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_MEA_SENS+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_AS_BTN_ANG_STEP+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_AS_BTN_ANG_STEP+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_AS_BTN_ANG_STEP+Setup2.language)*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_ANG_STEP+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_AS_BTN_AUTO_ANG+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_AS_BTN_AUTO_ANG+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_AS_BTN_AUTO_ANG+Setup2.language)*2], ',', 2),
			224, 42, hParent, RID_AS_BTN_AUTO_ANG+Setup2.language, 0);
	
}

LRESULT UI_AdSet_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_AdSet_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_SYS_BTN_BACK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_AS_BTN_PRO:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnProgress();
				break;
			}
			return 0;
		case RID_AS_BTN_LIMIT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnLimitPause();
				break;
			}
			return 0;
		case RID_AS_BTN_VIBR:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnVibration();
				break;
			}
			return 0;
		case RID_AS_BTN_SENS:
		case RID_AS_BTN_SENSK:
		case RID_AS_BTN_SENSC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnSensitivity();
				break;
			}
			return 0;
		case RID_AS_BTN_INIT_POS:
		case RID_AS_BTN_INIT_POSK:
		case RID_AS_BTN_INIT_POSC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnInitPosition();
				break;
			}
			return 0;
		case RID_AS_BTN_MEA_SENS:
		case RID_AS_BTN_MEA_SENSK:
		case RID_AS_BTN_MEA_SENSC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnMeaSensitivity();
				break;
			}
			return 0;
		case RID_AS_BTN_ANG_STEP:
		case RID_AS_BTN_ANG_STEPK:
		case RID_AS_BTN_ANG_STEPC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnAngleMoveStep();
				break;
			}
			return 0;
		case RID_AS_BTN_AUTO_ANG:
		case RID_AS_BTN_AUTO_ANGK:
		case RID_AS_BTN_AUTO_ANGC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AdSet_OnBnClickedBtnAutoAngleValue();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_AdSet_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_AdSet_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_MAIN][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_Run_Process);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_ADV_SET;
}

///////////////////////////////////////////////////////////////////////////////
void UI_Setup_Timer(uint16_t nIDEvent)
{
	USBDisk_Process();
}

//Ω√Ω∫≈€ º≥¡§ √¢ 
void UI_Setup_OnBnClickedBtnExit(void)
{ 
	uint8_t *ptr, *ptr2, i, flag;

	App_KillTimer(1);
	API_SetTimer(OS_TIMER_ID_2, 200);
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else
	flag = 0;
	ptr = (uint8_t *)&Setup2;
	ptr2 = (uint8_t *)&Setup2Old;
	for (i = 0; i < sizeof(SETUP2_INFO); i++) {
		if (ptr[i] != ptr2[i]) {
			flag = 1;
			break;
		}
	}
	ptr = (uint8_t *)&Setup3;
	ptr2 = (uint8_t *)&Setup3Old;
	for (i = 0; i < sizeof(SETUP3_INFO); i++) {
		if (ptr[i] != ptr2[i]) {
			flag = 1;
			break;
		}
	}
	if (flag) {
		if ((Setup3.LPChk != Setup3Old.LPChk) || //pjg++200205
			(Setup3.LP != Setup3Old.LP) ||
			(Setup3.ProChk != Setup3Old.ProChk) ||
			(Setup3.PDeg != Setup3Old.PDeg) ||
			(Setup3.PTm != Setup3Old.PTm) ||
			(Setup3.VDeg != Setup3Old.VDeg) ||
			(Setup3.VCnt != Setup3Old.VCnt) ||
			(Setup3.VibChk != Setup3Old.VibChk))
		{
			RunWnd.repeat = 0;
			RunWnd.time = SpdTmWnd.time;
		}
		ptr = (uint8_t *)&Setup2;
		ptr2 = (uint8_t *)&Setup2Old;
		for (i = 0; i < sizeof(SETUP2_INFO); i++) {
			ptr2[i] = ptr[i];
		}
		ptr = (uint8_t *)&Setup3;
		ptr2 = (uint8_t *)&Setup3Old;
		for (i = 0; i < sizeof(SETUP3_INFO); i++) {
			ptr2[i] = ptr[i];
		}
		UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	}
	
	UI_GoBackWnd();
}

void UI_Setup_OnBnClickedBtnUSB(void)
{ 
	App_KillTimer(1);
	UI_PopupUSBCheck_Create();
	//UI_PiUSB_Create();
	//USBDisk_Process();
}

void UI_Setup_OnBnClickedBtnAdSet(void)
{ 
	//UI_Progress_Create();
	App_KillTimer(1);
	UI_AdSet_Create();
}

void UI_Setup_OnBnClickedBtnSysInfo(void)
{ 
	App_KillTimer(1);
	UI_VersionInfo_Create();
}

//¿Ωº∫≈©±‚ ¡∂¡§
void UI_Setup_SetSoundVolume(uint8_t value)
{
	int temp;

	if (value < 10) {
		SVInfoBuf[3] = value+'0';
		SVInfoBuf[4] = '\r';
		SVInfoBuf[5] = 0;
		APP_SendMessage(0, WM_SOUND, 1, (LPARAM)SVInfoBuf);
	}
	else if (value < 100) {
		SVInfoBuf[3] = value/10+'0';
		SVInfoBuf[4] = value%10+'0';
		SVInfoBuf[5] = '\r';
		SVInfoBuf[6] = 0;
		APP_SendMessage(0, WM_SOUND, 1, (LPARAM)SVInfoBuf);
	}
	else {//if (Setup.bright < 256) {
		temp = value%100;
		SVInfoBuf[3] = value/100+'0';
		SVInfoBuf[4] = temp/10+'0';
		SVInfoBuf[5] = temp%10+'0';
		SVInfoBuf[6] = '\r';
	//	SVInfoBuf[7] = 0;
		APP_SendMessage(0, WM_SOUND, 1, (LPARAM)SVInfoBuf);
	}

}					 

//LCDπ‡±‚ ¡∂¡§
void UI_Setup_SetBrightness(uint8_t value)
{
	if (value < 10) {
		BLInfoBuf[2] = value+'0';
		BLInfoBuf[3] = '\r';
		BLInfoBuf[4] = 0;
		APP_SendMessage(0, WM_BACKLIGHT, 0, (LPARAM)BLInfoBuf);
	}
	else if (value <= 80) {
		BLInfoBuf[2] = value/10+'0';
		BLInfoBuf[3] = value%10+'0';
		BLInfoBuf[4] = '\r';
		APP_SendMessage(0, WM_BACKLIGHT, 0, (LPARAM)BLInfoBuf);
	}
}

void UI_Setup_BRset(uint8_t bright)
{
	if(bright == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i llb.bmp,161,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,212,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,164,79\r");		
		}
	else if(bright == 2)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i llb.bmp,161,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,259,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,164,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,212,79\r");
		}
	else if(bright == 3)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i llb.bmp,161,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,308,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,164,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,214,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,263,79\r");
		}
	else if(bright == 4)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i llb.bmp,161,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,357,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,164,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,214,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,265,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,314,79\r");
		}
	else if(bright == 5)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i llb.bmp,161,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,164,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,214,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,265,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,316,79\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lrb.bmp,363,79\r");
			
		}
}

void UI_Setup_Sndset(uint8_t vol)
{
	if(vol == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i slb.bmp,161,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,212,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,164,134\r");
			
		}
	else if(vol == 2)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i slb.bmp,161,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,259,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,164,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,212,134\r");
			
		}
	else if(vol == 3)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i slb.bmp,161,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,308,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,164,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,214,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,263,134\r");
		}
	else if(vol == 4)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i slb.bmp,161,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i clb.bmp,357,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,164,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,214,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,265,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,314,134\r");
		}
	else if(vol == 5)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i slb.bmp,161,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,164,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,214,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,265,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,316,134\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srb.bmp,363,134\r");
			
		}
}
//¿Ωº∫º“∏Æ ≈∞øÚ
void UI_Setup_OnBnClickedBtnSndUp(void)
{

	if (Setup2.vol < SND_LEVEL_NUM-1)Setup2.vol++;
	UI_Setup_SetSoundVolume(SndVolTbl[Setup2.vol]);	
	UI_Setup_Sndset(Setup2.vol+1);
	if(Setup2.vol == SND_LEVEL_NUM-1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npsnd.bmp,428,114\r");
		App_SetButtonOption(RID_SYS_SND_UP, BN_DISABLE);
	}
	if(Setup2.vol > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i msnd.bmp,102,114\r");
		App_SetButtonOption(RID_SYS_SND_DN, BN_PUSHED);
	}
	
}

//¿Ωº∫º“∏Æ ¡Ÿ¿”
void UI_Setup_OnBnClickedBtnSndDn(void)
{
	if (Setup2.vol > 0)Setup2.vol--;
	UI_Setup_SetSoundVolume(SndVolTbl[Setup2.vol]);
	UI_Setup_Sndset(Setup2.vol+1);
	if(Setup2.vol == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmsnd.bmp,102,114\r");
		App_SetButtonOption(RID_SYS_SND_DN, BN_DISABLE);
	}
	if(Setup2.vol < SND_LEVEL_NUM-1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i psnd.bmp,428,114\r");
		App_SetButtonOption(RID_SYS_SND_UP, BN_PUSHED);
	}
}

/*void UI_Setup_OnBnClickedBtnSndChk(void)
		 
{
	if (Setup2.sndGuid2 == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nsndshk.bmp,107,122\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmsnd.bmp,177,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npsnd.bmp,428,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbars.bmp,231,131\r");
		App_SetButtonOption(RID_SYS_SND_UP, BN_DISABLE);
		App_SetButtonOption(RID_SYS_SND_DN, BN_DISABLE);
		Setup2.sndGuid2= BST_UNCHECKED;
	}
	else { 
		
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sndshk.bmp,107,122\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i msnd.bmp,177,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i psnd.bmp,428,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bars.bmp,231,131\r");
		UI_Setup_Sndset(Setup2.vol+1);
		App_SetButtonOption(RID_SYS_SND_UP, BN_PUSHED);
		App_SetButtonOption(RID_SYS_SND_DN, BN_PUSHED);
		Setup2.sndGuid2= BST_CHECKED;
		if(Setup2.vol == 4)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npsnd.bmp,428,114\r");
			App_SetButtonOption(RID_SYS_SND_UP, BN_DISABLE);
		}
		if(Setup2.vol == 0)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmsnd.bmp,177,114\r");
			App_SetButtonOption(RID_SYS_SND_DN, BN_DISABLE);
		}
	}
}*/

//LCD π‡∞‘
void UI_Setup_OnBnClickedBtnBRUp(void)
{
	if (Setup2.bright < BL_LEVEL_NUM-1)Setup2.bright++;
	UI_Setup_SetBrightness(BLVolTbl[Setup2.bright]);
	UI_Setup_BRset(Setup2.bright+1);

	if(Setup2.bright == BL_LEVEL_NUM-1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npbr.bmp,428,60\r");
		App_SetButtonOption(RID_SYS_BR_UP, BN_DISABLE);
	}
	if(Setup2.bright > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i mbr.bmp,102,60\r");
		App_SetButtonOption(RID_SYS_BR_DN, BN_PUSHED);
	}
	/*if (Setup.bright < 80) Setup.bright += 5;							  

	else return;																		   

	UI_Setup_SetBrightness(Setup2.bright);*/
}
//LCD æÓµ”∞‘
void UI_Setup_OnBnClickedBtnBRDn(void)
{
	if (Setup2.bright > 0)Setup2.bright--;
	UI_Setup_SetBrightness(BLVolTbl[Setup2.bright]);
	UI_Setup_BRset(Setup2.bright+1);

	if(Setup2.bright == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmbr.bmp,102,60\r");
		App_SetButtonOption(RID_SYS_BR_DN, BN_DISABLE);
	}
	if(Setup2.bright < BL_LEVEL_NUM-1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pbr.bmp,428,60\r");
		App_SetButtonOption(RID_SYS_BR_UP, BN_PUSHED);
	}
	/*if (Setup.bright > 9) Setup.bright -= 5;
	else return;
	
	UI_Setup_SetBrightness(Setup.bright);*/
}

void UI_Setup_OnBnClickedBtnLangKor(void)
{	if(Setup2.language == LT_ENG)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssen.bmp,283,169\r");
		App_SetButtonOption(RID_SYS_LG_ENG, BN_PUSHED);
	}
	else if(Setup2.language == LT_CHINA)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssch.bmp,379,169\r");
		App_SetButtonOption(RID_SYS_LG_CHI, BN_PUSHED);
	}
	Setup2.language = LT_KOR;
	UI_Setup_Create();
}

void UI_Setup_OnBnClickedBtnLangEng(void)
{
	if(Setup2.language == LT_KOR)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssko.bmp,186,169\r");
		App_SetButtonOption(RID_SYS_LG_KOR, BN_PUSHED);
	}
	else if(Setup2.language == LT_CHINA)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssch.bmp,379,169\r");
		App_SetButtonOption(RID_SYS_LG_CHI, BN_PUSHED);
	}
	Setup2.language = LT_ENG;
	UI_Setup_Create();
}

void UI_Setup_OnBnClickedBtnLangChi(void)
{
	if(Setup2.language == LT_KOR)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssko.bmp,186,169\r");
		App_SetButtonOption(RID_SYS_LG_KOR, BN_PUSHED);
	}
	else if(Setup2.language == LT_ENG)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ssen.bmp,283,169\r");
		App_SetButtonOption(RID_SYS_LG_ENG, BN_PUSHED);
	}
	Setup2.language = LT_CHINA;
	UI_Setup_Create();
}

/*void UI_Setup_OnBnClickedBtnBRChk(void)
		 
{			
	if (Setup2.led_en == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbrchk.bmp,108,68\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmbr.bmp,177,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npbr.bmp,428,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbarb.bmp,231,77\r");
		App_SetButtonOption(RID_SYS_BR_UP, BN_DISABLE);
		App_SetButtonOption(RID_SYS_BR_DN, BN_DISABLE);
		UI_LED_Control(LM_NONE);
		
		Setup2.led_en= BST_UNCHECKED;
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i brchk.bmp,108,68\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i mbr.bmp,177,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pbr.bmp,428,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barb.bmp,231,77\r");
		UI_Setup_BRset(Setup2.bright+1);
		App_SetButtonOption(RID_SYS_BR_UP, BN_PUSHED);
		App_SetButtonOption(RID_SYS_BR_DN, BN_PUSHED);
		Setup2.led_en= BST_CHECKED;
		UI_LED_Control(LM_STAND_BY);
		if(Setup2.bright == 4)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npbr.bmp,428,60\r");
			App_SetButtonOption(RID_SYS_BR_UP, BN_DISABLE);
		}
		if(Setup2.bright == 0)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmbr.bmp,177,60\r");
			App_SetButtonOption(RID_SYS_BR_DN, BN_DISABLE);
		}
	}
}*/

void UI_Setup_Init(void)
{
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	// button
	API_CreateWindow("", pBtnInfo[RID_SYS_BTN_BACK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BTN_BACK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BTN_BACK*2], ',', 2),
			47,45, hParent, RID_SYS_BTN_BACK, 0);
	/*API_CreateWindow("", pBtnInfo[(RID_SYS_BTN_BACK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_BACK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_BACK+Setup2.language)*2], ',', 2),
			118, 46, hParent, RID_SYS_BTN_BACK+Setup2.language, 0);*/
	/*API_CreateWindow("", pBtnInfo[RID_SYS_BTN_USB*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BTN_USB*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BTN_USB*2], ',', 2),
			153, 46, hParent, RID_SYS_BTN_USB, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_SYS_BTN_USB+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_USB+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_USB+Setup2.language)*2], ',', 2),
			153, 46, hParent, RID_SYS_BTN_USB+Setup2.language, 0);
	/*API_CreateWindow("", pBtnInfo[RID_SYS_BTN_ADSET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BTN_ADSET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BTN_ADSET*2], ',', 2),
			154, 46, hParent, RID_SYS_BTN_ADSET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_SYS_BTN_ADSET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_ADSET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_ADSET+Setup2.language)*2], ',', 2),
			153, 46, hParent, RID_SYS_BTN_ADSET+Setup2.language, 0);
	/*API_CreateWindow("", pBtnInfo[RID_SYS_BTN_INFO*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BTN_INFO*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BTN_INFO*2], ',', 2),
			153, 46, hParent, RID_SYS_BTN_INFO, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_SYS_BTN_INFO+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_INFO+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_SYS_BTN_INFO+Setup2.language)*2], ',', 2),
			153, 46, hParent, RID_SYS_BTN_INFO+Setup2.language, 0);
	/*API_CreateWindow("", pBtnInfo[RID_SYS_SND_CHK*2], BS_CHECKBOX, 
			107,122,
			46, 27, hParent, RID_SYS_SND_CHK, 0);*/
	API_CreateWindow("", pBtnInfo[RID_SYS_SND_UP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_SND_UP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_SND_UP*2], ',', 2),
			47, 46, hParent, RID_SYS_SND_UP, 0);
	API_CreateWindow("", pBtnInfo[RID_SYS_SND_DN*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_SND_DN*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_SND_DN*2], ',', 2),
			47, 46, hParent, RID_SYS_SND_DN, 0);
	/*API_CreateWindow("", pBtnInfo[RID_SYS_BR_CHK*2], BS_CHECKBOX, 
			108,68,
			46, 27, hParent, RID_SYS_BR_CHK, 0);*/
	// bl button
	API_CreateWindow("", pBtnInfo[RID_SYS_BR_UP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BR_UP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BR_UP*2], ',', 2),
			47, 45, hParent, RID_SYS_BR_UP, 0);
	API_CreateWindow("", pBtnInfo[RID_SYS_BR_DN*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_BR_DN*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_BR_DN*2], ',', 2),
			47, 45, hParent, RID_SYS_BR_DN, 0);
	//Language
	API_CreateWindow("", pBtnInfo[RID_SYS_LG_KOR*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_LG_KOR*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_LG_KOR*2], ',', 2),
			93, 44, hParent, RID_SYS_LG_KOR, 0);
	API_CreateWindow("", pBtnInfo[RID_SYS_LG_ENG*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_SYS_LG_ENG*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_SYS_LG_ENG*2], ',', 2),
			93, 44, hParent, RID_SYS_LG_ENG, 0);
	//API_CreateWindow("", pBtnInfo[RID_SYS_LG_CHI*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_SYS_LG_CHI*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_SYS_LG_CHI*2], ',', 2),
	//		93, 44, hParent, RID_SYS_LG_CHI, 0);

	UI_LED_Control(LM_STAND_BY);

	//UI_Setup_Sndset(Setup2.vol+1);
	//UI_Setup_BRset(Setup2.bright+1);	
	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_Setup_Sndset(Setup2.vol+1);
	UI_Setup_BRset(Setup2.bright+1);

	if(Setup2.language == LT_KOR)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nssko.bmp,186,169\r");
		App_SetButtonOption(RID_SYS_LG_KOR, BN_DISABLE);
	}
	//else if(Setup2.language == LT_CHINA)
	//{
	//	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nssch.bmp,379,169\r");
	//	App_SetButtonOption(RID_SYS_LG_CHI, BN_DISABLE);
	//}
	else
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nssen.bmp,283,169\r");
		App_SetButtonOption(RID_SYS_LG_ENG, BN_DISABLE);
	}
	
	
	/*if (Setup2.sndGuid2 == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sndshk.bmp,107,122\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i msnd.bmp,177,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i psnd.bmp,428,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bars.bmp,231,131\r");
		UI_Setup_Sndset(Setup2.vol+1);

		App_SetButtonOption(RID_SYS_SND_UP, BN_PUSHED);
		App_SetButtonOption(RID_SYS_SND_DN, BN_PUSHED); 
		
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nsndshk.bmp,107,122\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmsnd.bmp,177,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npsnd.bmp,428,114\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbars.bmp,231,131\r");

		App_SetButtonOption(RID_SYS_SND_UP, BN_DISABLE);
		App_SetButtonOption(RID_SYS_SND_DN, BN_DISABLE);
		
	}
	if (Setup2.led_en == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i brchk.bmp,108,68\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i mbr.bmp,177,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pbr.bmp,428,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barb.bmp,231,77\r");
		UI_Setup_BRset(Setup2.bright+1);
		UI_LED_Control(LM_STAND_BY);

		App_SetButtonOption(RID_SYS_BR_UP, BN_PUSHED);
		App_SetButtonOption(RID_SYS_BR_DN, BN_PUSHED);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbrchk.bmp,108,68\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmbr.bmp,177,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npbr.bmp,428,60\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nbarb.bmp,231,77\r");

		UI_LED_Control(LM_NONE);

		App_SetButtonOption(RID_SYS_BR_UP, BN_DISABLE);
		App_SetButtonOption(RID_SYS_BR_DN, BN_DISABLE);
	}*/
	if(Setup2.vol == 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npsnd.bmp,428,114\r");
		App_SetButtonOption(RID_SYS_SND_UP, BN_DISABLE);
	}
	else if(Setup2.vol == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmsnd.bmp,102,114\r");
		App_SetButtonOption(RID_SYS_SND_DN, BN_DISABLE);
	}
	
	if(Setup2.bright == 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npbr.bmp,428,60\r");
		App_SetButtonOption(RID_SYS_BR_UP, BN_DISABLE);
	}
	else if(Setup2.bright == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nmbr.bmp,102,60\r");
		App_SetButtonOption(RID_SYS_BR_DN, BN_DISABLE);
	}
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	//App_SetUIProcess(UI_Setup_Process);
	App_SetTimer(1, 20);
	API_KillTimer(OS_TIMER_ID_2);
}

LRESULT UI_Setup_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (hParent != hWnd) return 0;
	
	switch (uMsg) {
	case WM_CREATE:
		UI_Setup_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// ASE
		case RID_SYS_BTN_BACK:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnExit();
                break;
			}
			return 0; 
		case RID_SYS_BTN_USB:
		case RID_SYS_BTN_USBK:
		case RID_SYS_BTN_USBC:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnUSB();
                break;
			}
			return 0;
			case RID_SYS_BTN_ADSET:
			case RID_SYS_BTN_ADSETK:
			case RID_SYS_BTN_ADSETC:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnAdSet();
                break;
			}
			return 0;
		case RID_SYS_BTN_INFO:
		case RID_SYS_BTN_INFOK:
		case RID_SYS_BTN_INFOC:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnSysInfo();
                break;
			}
			return 0;
		/*case RID_SYS_SND_CHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnSndChk();
                break;
			}
			return 0; */
		case RID_SYS_SND_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnSndUp();
                break;
			}
			return 0; 
		case RID_SYS_SND_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnSndDn();
                break;
			}
			return 0; 
		/*case RID_SYS_BR_CHK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Setup_OnBnClickedBtnBRChk();
				break;
			default:
				break;
			}
			return 0;*/
		case RID_SYS_BR_UP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnBRUp();
                break;
			}
			return 0; 
		case RID_SYS_BR_DN:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnBRDn();
                break;
			}
			return 0;
		case RID_SYS_LG_KOR:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnLangKor();
                break;
			}
			return 0;
		case RID_SYS_LG_ENG:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnLangEng();
                break;
			}
			return 0;
		case RID_SYS_LG_CHI:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Setup_OnBnClickedBtnLangChi();
                break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;              
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	case WM_TIMER:
		UI_Setup_Timer(wParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_Setup_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_Setup_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i sysbg.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_MAIN][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	if (UI_Wnd.num != UI_WND_MODE_SETUP) UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_SETUP;
} 

///////////////////////////////////////////////////////////////////////////////////////////////
void UI_PopupIDCreate_OnBnClickedBtnOk(void)
{
#if 0
	FILINFO fno;
	uint8_t tmpbuf[25];

	if (!EEPROMDisk_Link()) {
		return;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		return;
	}
	ID2FileName(PInfoWnd2.id, tmpbuf);
	if (!EEPROMDisk_OpenWrite((char *)tmpbuf)) {
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		return;
	}
#endif
	UI_AngleMeasure_Create();
}

void UI_PopupIDCreate_OnBnClickedBtnCancel(void)
{
	UI_PiLoad_Create();
}

void UI_PopupIDCreate_Init(void)
{
	int i,j,k;
	uint32_t temp, temp2, pos, posmod;
	char *buf;
	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	loading = 0;
	API_CreateWindow("", pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_OK+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_CANCEL+Setup2.language, 0);
    buf = CommonBuf;
	//id disp
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'p';
	buf[6] = 'n';
	buf[7] = 'g';
	buf[8] = ',';
	if (Setup2.language == LT_ENG) {
		buf[9] = '1';
		buf[10] = '5';
		buf[11] = '5';
		buf[12] = ',';
		buf[13] = '1';
		buf[14] = '3';
		buf[15] = '4';
		pos = 155;
	}
	else if (Setup2.language == LT_KOR) {
		buf[9] = '2';
		buf[10] = '3';
		buf[11] = '2';
		buf[12] = ',';
		buf[13] = '1';
		buf[14] = '0';
		buf[15] = '7';
		pos = 232;
	}
	else {
		buf[9] = '2';
		buf[10] = '3';
		buf[11] = '2';
		buf[12] = ',';
		buf[13] = '1';
		buf[14] = '0';
		buf[15] = '7';
		pos = 232;
	}
	buf[16] = '\r';
	buf[17] = 0;
	//i = 18;
	k = 0;
	temp = PInfoWnd2.id;
    temp2 = 0;
	for (i = 0; i < 8; i++) {
		temp2 = Multiplier(10, 7-i);
        if (temp2 == 0) buf[3] = 0;
		else buf[3] = temp/temp2 + '0';
		posmod = pos%100;
		buf[9] = pos/100 + '0';
		buf[10] = posmod/10 + '0';
		buf[11] = posmod%10 + '0';
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[20*k]); 
		temp %= temp2;
		k++;
		for (j = 0; j < 18; j++) CommonBuf[20*k+j] = buf[j];
  	  	buf = &CommonBuf[20*k];
		pos += 11;
	}
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
}

LRESULT UI_PopupIDCreate_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupIDCreate_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_PILRS_BTN_OK:
		case RID_PILRS_BTN_OKK:
		case RID_PILRS_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupIDCreate_OnBnClickedBtnOk();
				break;
			}
			return 0; 
		case RID_PILRS_BTN_CANCEL:
		case RID_PILRS_BTN_CANCELK:
		case RID_PILRS_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupIDCreate_OnBnClickedBtnCancel();
				break;
			}
			return 0; 
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PopupIDCreate_Create(void)
{	

	App_SetWndProcFnPtr(UI_PopupIDCreate_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_ID_CREATE][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_ID_CREATE;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void UI_PopupIDRight_OnBnClickedBtnOk(void)
{
	SAVE_EXERCISE_INFO_V2 sei;
	uint8_t temp;//, temp2;
#if 1	
	if (UI_Exercise_LoadLastData(PInfoWnd2.id, &sei)) {
		if (Exercise.dataCnt > 0) {
			if (sei.flg < 2) {
				if (sei.flg == MST_REHAB) {
					SaveExeInfoV2.spm.mi.exangle = sei.spm.exangle;
					SaveExeInfoV2.spm.mi.flangle = sei.spm.flangle;
				}
				else {
					temp = (sei.smm.mi[0].exangle + sei.smm.mi[1].exangle + sei.smm.mi[2].exangle)/3;
					SaveExeInfoV2.spm.mi.exangle = temp;
					temp = (sei.smm.mi[0].flangle + sei.smm.mi[1].flangle + sei.smm.mi[2].flangle)/3;
					SaveExeInfoV2.spm.mi.flangle = temp;
				}
			}
		}
	}
#endif
	UI_AngleMeasure_Create();
}

void UI_PopupIDRight_OnBnClickedBtnCancel(void)
{
	UI_PiLoad_Create();
}

void UI_PopupIDRight_Init(void)
{
	uint8_t *buf;
	uint8_t i,j,k;
	uint32_t temp, temp2, pos, posmod;

	loading = 0;
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	API_CreateWindow("", pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_OK+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_OK+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_CANCEL+Setup2.language, 0);

    buf = (uint8_t *)CommonBuf;
	//id disp
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'p';
	buf[6] = 'n';
	buf[7] = 'g';
	buf[8] = ',';
	if (Setup2.language == LT_ENG) {
		buf[9] = '1';
		buf[10] = '5';
		buf[11] = '4';
		buf[12] = ',';
		buf[13] = '1';
		buf[14] = '2';
		buf[15] = '1';
		buf[16] = '\r';
		buf[17] = 0;
		pos = 154;
	}
	else if (Setup2.language == LT_KOR) {
		buf[9] = '2';
		buf[10] = '3';
		buf[11] = '2';
		buf[12] = ',';
		buf[13] = '9';
		buf[14] = '3';
		buf[15] = '\r';
		buf[16] = 0;
		pos = 232;
	}
	else {
		buf[9] = '2';
		buf[10] = '3';
		buf[11] = '2';
		buf[12] = ',';
		buf[13] = '9';
		buf[14] = '3';
		buf[15] = '\r';
		buf[16] = 0;
		pos = 232;
	}
	//i = 18;
	k = 0;
	temp = PInfoWnd2.id;
    temp2 = 0;
	for (i = 0; i < 8; i++) {
		temp2 = Multiplier(10, 7-i);
		if (temp2 == 0) buf[3] = 0;
        else buf[3] = temp/temp2 + '0';
		posmod = pos%100;
		buf[9] = pos/100 + '0';
		buf[10] = posmod/10 + '0';
		buf[11] = posmod%10 + '0';
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[20*k]); 
		temp %= temp2;
		k++;
		for (j = 0; j < 18; j++) CommonBuf[20*k+j] = buf[j];
  	  	buf = (uint8_t *)&CommonBuf[20*k];
		pos += 11;
	}
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
}

LRESULT UI_PopupIDRight_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupIDRight_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_PILRS_BTN_OK:
		case RID_PILRS_BTN_OKK:
		case RID_PILRS_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupIDRight_OnBnClickedBtnOk();
				break;
			}
			return 0; 
		case RID_PILRS_BTN_CANCEL:
		case RID_PILRS_BTN_CANCELK:
		case RID_PILRS_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupIDRight_OnBnClickedBtnCancel();
				break;
			}
			return 0; 
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PopupIDRight_Create(void)
{	
	App_SetWndProcFnPtr(UI_PopupIDRight_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_ID_RIGHT][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_ID_RIGHT;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void UI_PiLoad_DisplayID(void)
{   
	int i;
	
	if(loginInfo.cnt == 0){
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,118,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,136,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,154,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,172,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,190,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,208,88\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,226,88\r");
	}

	i = 66 + loginInfo.cnt*18;//20;//18;
	UI_DisplayNumber(UI_DISP_NUM_FNT9, i,115,loginInfo.pwdBuf[loginInfo.cnt], UI_DISP_NUM_3PLACE);
}

void UI_PiLoad_DisplayDel(int pos)
{   
	switch (pos) {
		case 0:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,66,115\r");
			break;
		case 1:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,84,115\r");
			break;
		case 2:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,102,115\r");
			break;
		case 3:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,120,115\r");
			break;
		case 4:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,138,115\r");
			break;
		case 5:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,156,115\r");
			break;
		case 6:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,174,115\r");
			break;
		case 7:
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,192,115\r");
			break;
	}
}

void UI_PiReset_Enable(void)
{
	if (PInfoWnd2.id == PATIENT_NUM)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESET, BN_PUSHED);
			App_SetButtonOption(RID_PLD2_BTN_RESET, BN_PUSHED);
			App_SetButtonOption(RID_PLD3_BTN_RESET, BN_PUSHED);
			App_SetButtonOption(RID_PLD4_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETK, BN_PUSHED);
			App_SetButtonOption(RID_PLD2_BTN_RESETK, BN_PUSHED);
			App_SetButtonOption(RID_PLD3_BTN_RESETK, BN_PUSHED);
			App_SetButtonOption(RID_PLD4_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETC, BN_PUSHED);
			App_SetButtonOption(RID_PLD2_BTN_RESETC, BN_PUSHED);
			App_SetButtonOption(RID_PLD3_BTN_RESETC, BN_PUSHED);
			App_SetButtonOption(RID_PLD4_BTN_RESETC, BN_PUSHED);
		}
	}
}

void UI_PiLoad_OnBnClickedBtnClose(void)
{
#ifdef PI_LOAD_V1
	PInfoWnd2.id = PATIENT_NUM;
#endif
	UI_User_Create();
}
	
void UI_PiLoad_OnBnClickedBtnLeft(void)
{
	UI_User_Create();
}

void UI_PiLoad_OnBnClickedBtnLoad(void)
{
#ifdef PI_LOAD_V2
	FILINFO fno;
	uint8_t tmpbuf[25];
	uint32_t temp, temp2;

	if (loginInfo.type == LIT_USER){
		if (!loginInfo.cnt) return;
		temp = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
		if (!temp) return;
		PInfoWnd2.id = temp;
	}
	if (!EEPROMDisk_Link()) {
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
		return;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
		return;
	}
	//ID2FileName(PInfoWnd2.id, tmpbuf);
	//EEPROMDisk_OpenWrite(tmpbuf);
	//check free space
	temp = EEPROMDisk_GetFree("0:");
	temp2 = EEPROMDisk_GetTotalFromMem();
	EEPROMDisk_Close();
    if (temp2 == 0) temp = 0;
	else temp = (temp*100)/temp2;
	//temp = 91;
	//is 99% free. full
	if (temp < 2) {
		FullInfo.status = FS_FULL;
		UI_PopupDataFull_Create();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		return;
	}
	else if (temp < 10) {
		FullInfo.status = FS_90PER;
		UI_PopupMemoryWarning_Create();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		return;
	}
	ID2FileName(PInfoWnd2.id, tmpbuf);
	if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//are you create id
		UI_PopupIDCreate_Create();
		return;
	}
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();
	//is right id
	UI_PopupIDRight_Create();
	
#else
	if(((EXERCISE_INFO_NUM*9)/10) <= Exercise.addr && Exercise.addr < EXERCISE_INFO_NUM) //memory 90% full
		UI_PopupMemoryWarning_Create();
	else if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	else{
		//if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
		//else loginInfo.type = LIT_USER;
		
		if (loginInfo.type == LIT_USER){
			UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
			if(PInfoWnd2.pi.pwd > 9999){
				loginInfo.type = LIT_SET_PWD;
				UI_Login_Create();
			}
			else UI_Login_Create();
		}
		else{
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
			UI_Angle_Create();
		}
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnRight(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	#ifdef PI_LOAD_V1
	UI_PiLoad2_Create();
	#else
	
	#endif
}

void UI_PiLoad_ProcessPINumberDisp(void)
{
	loading++;
	if (loading > 20) {
		App_SetUIProcess(UI_ProcessNull);
		UI_PiReset_Create();
	}
}

void UI_PiLoad_OnBnClickedBtnReset(void)
{
	uint32_t temp;
	
	loading = 0;
	temp = 0;
	//if (!loginInfo.cnt) return;
	temp = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
	//if (!temp) return;
	PInfoWnd2.id = temp;
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	UI_PINumber_Display();
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	App_SetUIProcess(UI_PiLoad_ProcessPINumberDisp);
}

void UI_PiLoad_OnBnClickedBtnPI1(void)
{	
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 1;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 0;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}


void UI_PiLoad_OnBnClickedBtnPI2(void)
{	
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 2;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 1;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnPI3(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 3;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 2;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}

#endif
}


void UI_PiLoad_OnBnClickedBtnPI4(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 4;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 3;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnPI5(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 5;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 4;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}


void UI_PiLoad_OnBnClickedBtnPI6(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 6;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 5;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnPI7(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 7;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 6;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}


void UI_PiLoad_OnBnClickedBtnPI8(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 8;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 7;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnPI9(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 9;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 8;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}


void UI_PiLoad_OnBnClickedBtnPI10(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 0;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 9;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

void UI_PiLoad_OnBnClickedBtnPI11(void)
{
#ifdef PI_LOAD_V2
	if (loginInfo.cnt > 0) loginInfo.cnt--;
	loginInfo.pwdBuf[loginInfo.cnt] = 0;
	UI_PiLoad_DisplayDel(loginInfo.cnt);
#else
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 12;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}


void UI_PiLoad_OnBnClickedBtnPI12(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 11;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad_OnBnClickedBtnPI13(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 12;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad_OnBnClickedBtnPI14(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 13;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad_OnBnClickedBtnPI15(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 14;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad_OnBnClickedBtnPI16(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 15;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad_OnBnClickedBtnPI17(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 16;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad_OnBnClickedBtnPI18(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 17;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad_OnBnClickedBtnPI19(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 18;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad_OnBnClickedBtnPI20(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 19;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);	
		}
}

void UI_PiLoad_OnBnClickedBtnPI21(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 20;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad_Init(void)
{
#ifdef PI_LOAD_V2
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	API_CreateWindow("", pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], ',', 2),
			71,29, hParent, RID_PLD_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PLD_BTN_LOAD+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PLD_BTN_CLOSE*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PLD_BTN_CLOSE*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PLD_BTN_CLOSE*2], ',', 2),
	//		32,31, hParent, RID_PLD_BTN_CLOSE, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LEFT*2], ',', 2),
			116,45, hParent, RID_PLD_BTN_LEFT, 0);
	//API_CreateWindow("", pBtnInfo[RID_PLD_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PLD_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PLD_BTN_RIGHT*2], ',', 2),
	//		116,45, hParent, RID_PLD_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	//numeric pad
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 2),
			93, 49, hParent, RID_PLD_BTN_NUM11, 0);	

	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pwdlcd.bmp,13,111\r");	
	
	//if (PiChange_Wnd.PrePressedNum != 255)
	//else PInfoWnd2.id = 0;
	//PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_LED_Control(LM_STAND_BY);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i b01.bmp,439,248\r");
	if (loginInfo.type == LIT_GUEST)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidel.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESET, BN_DISABLE);
		}	
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelk.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelc.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETC, BN_DISABLE);
		}
	}
	else{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,397,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETC, BN_PUSHED);
		}
	}
	//UI_PINumber_Display();
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);

	//UI_PINumber_Display();
	FullInfo.type = FT_PI;

	loginInfo.pwdLen = 8;
	loginInfo.cnt = 0;
	
#else //PI_LOAD_V1
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_PLD_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LEFT*2], ',', 2),
			116,45, hParent, RID_PLD_BTN_LEFT, 0);*/
	/*API_CreateWindow("", pBtnInfo[RID_PLD_BTN_RESET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_RESET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_RESET*2], ',', 2),
			71,29, hParent, RID_PLD_BTN_RESET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_RESET+Setup2.language)*2], ',', 2),
			71,29, hParent, RID_PLD_BTN_RESET+Setup2.language, 0);
	/*API_CreateWindow("", pBtnInfo[RID_PLD_BTN_LOAD*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LOAD*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_LOAD*2], ',', 2),
			116,45, hParent, RID_PLD_BTN_LOAD, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD_BTN_LOAD+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PLD_BTN_LOAD+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_CLOSE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_CLOSE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_CLOSE*2], ',', 2),
			32,31, hParent, RID_PLD_BTN_CLOSE, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_RIGHT*2], ',', 2),
			116,45, hParent, RID_PLD_BTN_RIGHT, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM11, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM12*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM12*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM12*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM12, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM13*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM13*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM13*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM13, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM14*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM14*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM14*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM14, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM15*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM15*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM15*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM15, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM16*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM16*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM16*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM16, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM17*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM17*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM17*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM17, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM18*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM18*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM18*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM18, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM19*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM19*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM19*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM19, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM20*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM20*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM20*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM20, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM21*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM21*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM21*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM21, 0);
	
	//if (PiChange_Wnd.PrePressedNum != 255)
	//else PInfoWnd2.id = 0;
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_LED_Control(LM_STAND_BY);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i b01.bmp,439,248\r");
	if (loginInfo.type == LIT_GUEST)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESET, BN_DISABLE);
		}	
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETC, BN_DISABLE);
		}
	}
	else{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD_BTN_RESETC, BN_PUSHED);
		}
	}

	UI_PINumber_Display();
	FullInfo.type = FT_PI;

	//UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 68,15,PInfoWnd2.id+1);
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
#endif
}

LRESULT UI_PiLoad_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiLoad_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// PI
		/*case RID_PLD_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnLeft();
				break;
			}
			return 0;*/
		case RID_PLD_BTN_RESET:
		case RID_PLD_BTN_RESETK:
		case RID_PLD_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnReset();
				break;
			}
			return 0;
		case RID_PLD_BTN_LOAD:
		case RID_PLD_BTN_LOADK:
		case RID_PLD_BTN_LOADC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnLoad();
				break;
			}
			return 0;
		case RID_PLD_BTN_CLOSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnClose();
				break;
			}
			return 0;
		case RID_PLD_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PLD_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI1();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI2();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI3();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI4();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI5();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI6();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI7();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI8();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI9();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM10: //num 0
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI10();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM11: //back space
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad_OnBnClickedBtnPI11();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PiLoad_Create(void)
{		
	App_SetWndProcFnPtr(UI_PiLoad_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i usldb.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_USER_SEL][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_PARENT;
}

void UI_PiReset_OnBnClickedOK(void)
{
	//PInfoWnd2.id = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
	//if (PInfoWnd2.id > 99999999) return;
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_InitVariable(); //pjg++180607
	PInfoWnd2.id = 0;
	//UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf); //pjg--180607
	loginInfo.type = LIT_USER;
	UI_PiLoad_Create();
}

void UI_PiReset_OnBnClickedCancel(void)
{
	//if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	//else loginInfo.type = LIT_USER;
	
	UI_PiLoad_Create();
}

void UI_PiReset_Init(void)
{
	uint8_t tmpbuf[30];
	FILINFO fno;
	uint8_t flag;

	flag = 0;
	/*API_CreateWindow("", pBtnInfo[RID_PILRS_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS_BTN_OK*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_PILRS_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_PILRS_BTN_CANCEL, 0);*/
	if (!EEPROMDisk_Link()) {
		//App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
		//return;
		flag = 0x01;
	}

	if (!flag) {
		if (!EEPROMDisk_Mount()) {
			EEPROMDisk_UnLink();
			//App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
			//return;
			flag |= 0x02;
		}
	}

	if (!flag) {
		PInfoWnd2.id = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
		if (PInfoWnd2.id <= 99999999 && PInfoWnd2.id > 0) {
			ID2FileName(PInfoWnd2.id, tmpbuf);
			if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
				flag |= 0x04;
			}
		}
		else flag |= 0x08;
	}
	
	if (flag > 0) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[(RID_RST_BTN_RESET_DISABLE+Setup2.language)*2]); 	
	}
	else {
		API_CreateWindow("", pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
				GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 1),
				GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 2),
				113, 46, hParent, RID_RST_BTN_RESET+Setup2.language, 0);
	}
	if (flag == 0 || flag >= 4) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
	}
	
	API_CreateWindow("", pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_RST_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PiReset_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiReset_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_RST_BTN_RESET:
		case RID_RST_BTN_RESETK:
		case RID_RST_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset_OnBnClickedOK();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_RST_BTN_CANCEL:
		case RID_RST_BTN_CANCELK:
		case RID_RST_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PiReset_Create(void)
{	
	App_SetWndProcFnPtr(UI_PiReset_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i pirspu.png,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_USER_DEL][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_RESET;
}

#ifdef PI_LOAD_V1
void  UI_PiLoad2_OnBnClickedBtnReset(void)
{
	UI_PiReset2_Create();
}

void UI_PiLoad2_OnBnClickedBtnClose(void)
{
	PInfoWnd2.id = PATIENT_NUM;
	UI_Home_Create();
}

void UI_PiLoad2_OnBnClickedBtnLeft(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad_Create();
}

void UI_PiLoad2_OnBnClickedBtnLoad(void)
{
	if((EXERCISE_INFO_NUM*9/10) <= Exercise.addr && Exercise.addr < EXERCISE_INFO_NUM)
		UI_PopupMemoryWarning_Create();
	else if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	else{
		if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
		else loginInfo.type = LIT_USER;
		
		if (loginInfo.type == LIT_USER){
			UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
			if(PInfoWnd2.pi.pwd > 9999){
				loginInfo.type = LIT_SET_PWD;
				UI_Login_Create();
			}
			else	 UI_Login_Create();
		}
		else{
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
			UI_Angle_Create();
		}
	}
}

void UI_PiLoad2_OnBnClickedBtnRight(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad3_Create();
}

void UI_PiLoad2_OnBnClickedBtnPI1(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 21;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI2(void)
{	
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 22;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI3(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 23;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI4(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 24;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI5(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 25;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad2_OnBnClickedBtnPI6(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 26;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI7(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 27;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad2_OnBnClickedBtnPI8(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 28;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI9(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 29;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI10(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum =30;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI11(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 31;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI12(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 32;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI13(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 33;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI14(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 34;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI15(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 35;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI16(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 36;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI17(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 37;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI18(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 38;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI19(void)
{
	
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 39;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad2_OnBnClickedBtnPI20(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 40;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);	
		}
}

void UI_PiLoad2_OnBnClickedBtnPI21(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 41;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

/*uint8_t lag_btn_num;
if (lagng == KOR) lag_btn_num = 0;
else (lagng == ENG) lag_btn_num = 1*2;
else (lagng == CHI) lag_btn_num = 2*2;*/

void UI_PiLoad2_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_RESET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_RESET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_RESET*2], ',', 2),
			71,29, hParent, RID_PLD2_BTN_RESET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD2_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD2_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD2_BTN_RESET+Setup2.language)*2], ',', 2),
			71,29, hParent, RID_PLD2_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_LEFT*2], ',', 2),
			116,45, hParent, RID_PLD2_BTN_LEFT, 0);
	/*API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_LOAD*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_LOAD*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_LOAD*2], ',', 2),
			116,45, hParent, RID_PLD2_BTN_LOAD, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD2_BTN_LOAD+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD2_BTN_LOAD+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD2_BTN_LOAD+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PLD2_BTN_LOAD+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_CLOSE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_CLOSE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_CLOSE*2], ',', 2),
			32,31, hParent, RID_PLD2_BTN_CLOSE, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_RIGHT*2], ',', 2),
			116,45, hParent, RID_PLD2_BTN_RIGHT, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM11*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM11, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM12*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM12*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM12*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM12, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM13*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM13*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM13*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM13, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM14*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM14*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM14*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM14, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM15*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM15*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM15*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM15, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM16*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM16*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM16*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM16, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM17*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM17*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM17*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM17, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM18*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM18*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM18*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM18, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM19*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM19*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM19*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM19, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM20*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM20*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM20*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM20, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD2_BTN_NUM21*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM21*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD2_BTN_NUM21*2], ',', 2),
			46, 53, hParent, RID_PLD2_BTN_NUM21, 0);
	
	//if (PiChange_Wnd.PrePressedNum != 255)
		PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	//else PInfoWnd2.id = 0;
	if (loginInfo.type == LIT_GUEST)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESET, BN_DISABLE);
		}	
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESETK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESETC, BN_DISABLE);
		}
	}
	else{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD2_BTN_RESETC, BN_PUSHED);
		}
	}

	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_LED_Control(LM_STAND_BY);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i b01.bmp,439,248\r");

	UI_PINumber_Display();
	FullInfo.type = FT_PI;

	//UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 68,15,PInfoWnd2.id+1);

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

LRESULT UI_PiLoad2_WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiLoad2_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// PI
		case RID_PLD2_BTN_RESET:
		case RID_PLD2_BTN_RESETK:
		case RID_PLD2_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnReset();
				break;
			}
			return 0;
		case RID_PLD2_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PLD2_BTN_LOAD:
		case RID_PLD2_BTN_LOADK:
		case RID_PLD2_BTN_LOADC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnLoad();
				break;
			}
			return 0;
		case RID_PLD2_BTN_CLOSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnClose();
				break;
			}
			return 0;
		case RID_PLD2_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI1();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI2();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI3();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI4();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI5();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI6();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI7();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI8();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI9();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM10:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI10();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM11:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI11();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM12:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI12();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM13:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI13();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM14:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI14();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM15:
			switch(lParam) {
			case BN_CLICKED: // push
				UI_PiLoad2_OnBnClickedBtnPI15();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM16:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI16();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM17:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI17();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM18:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI18();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM19:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI19();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM20:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI20();
				break;
			}
			return 0;
		case RID_PLD2_BTN_NUM21:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad2_OnBnClickedBtnPI21();
				break;
			}
			return 0;
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PiLoad2_Create(void)
{		
	App_SetWndProcFnPtr(UI_PiLoad2_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i usldb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[7][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
}

void UI_PiReset2_OnBnClickedOK(void)
{
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	loginInfo.type = LIT_USER;
	UI_PiLoad2_Create();
}

void UI_PiReset2_OnBnClickedCancel(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad2_Create();
}

void UI_PiReset2_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PILRS2_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS2_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS2_BTN_OK*2], ',', 2),
			113, 46, hParent, RID_PILRS2_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_PILRS2_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS2_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS2_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_PILRS2_BTN_CANCEL, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PILRS2_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS2_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS2_BTN_OK+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS2_BTN_OK+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PILRS2_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS2_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS2_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS2_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PiReset2_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiReset2_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_PILRS2_BTN_OK:
		case RID_PILRS2_BTN_OKK:
		case RID_PILRS2_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset2_OnBnClickedOK();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_PILRS2_BTN_CANCEL:
		case RID_PILRS2_BTN_CANCELK:
		case RID_PILRS2_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset2_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PiReset2_Create(void)
{	
	App_SetWndProcFnPtr(UI_PiReset2_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i pirspu.png,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[8][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.num = UI_WND_MODE_RESET;
}

void  UI_PiLoad3_OnBnClickedBtnReset(void)
{
	UI_PiReset3_Create();
}

void UI_PiLoad3_OnBnClickedBtnClose(void)
{
	PInfoWnd2.id = PATIENT_NUM;
	UI_Home_Create();
}

void UI_PiLoad3_OnBnClickedBtnLeft(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad2_Create();
}

void UI_PiLoad3_OnBnClickedBtnLoad(void)
{
	if((EXERCISE_INFO_NUM*9/10) <= Exercise.addr && Exercise.addr < EXERCISE_INFO_NUM)
		UI_PopupMemoryWarning_Create();
	else if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	else{
		if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
		else loginInfo.type = LIT_USER;
		
		if (loginInfo.type == LIT_USER){
			UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
			if(PInfoWnd2.pi.pwd > 9999){
				loginInfo.type = LIT_SET_PWD;
				UI_Login_Create();
			}
			else	 UI_Login_Create();
		}
		else{
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
			UI_Angle_Create();
		}
	}
}

void UI_PiLoad3_OnBnClickedBtnRight(void)
{	
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad4_Create();
}

void UI_PiLoad3_OnBnClickedBtnPI1(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 42;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI2(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 43;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI3(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 44;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI4(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 45;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI5(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 46;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI6(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 47;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();

	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI7(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 48;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI8(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 49;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI9(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 50;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI10(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 51;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI11(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 52;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI12(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 53;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI13(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 54;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI14(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 55;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI15(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 56;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI16(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 57;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI17(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 58;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI18(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum =59;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_OnBnClickedBtnPI19(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 60;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad3_OnBnClickedBtnPI20(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 61;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);	
		}
}

void UI_PiLoad3_OnBnClickedBtnPI21(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 62;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad3_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_RESET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_RESET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_RESET*2], ',', 2),
			71,29, hParent, RID_PLD3_BTN_RESET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD3_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD3_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD3_BTN_RESET+Setup2.language)*2], ',', 2),
			71,29, hParent, RID_PLD3_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_LEFT*2], ',', 2),
			116,45, hParent, RID_PLD3_BTN_LEFT, 0);
	/*API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_LOAD*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_LOAD*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_LOAD*2], ',', 2),
			116,45, hParent, RID_PLD3_BTN_LOAD, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD3_BTN_LOAD+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD3_BTN_LOAD+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD3_BTN_LOAD+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PLD3_BTN_LOAD+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_CLOSE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_CLOSE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_CLOSE*2], ',', 2),
			32,31, hParent, RID_PLD3_BTN_CLOSE, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_RIGHT*2], ',', 2),
			116,45, hParent, RID_PLD3_BTN_RIGHT, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM11*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM11, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM12*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM12*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM12*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM12, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM13*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM13*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM13*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM13, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM14*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM14*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM14*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM14, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM15*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM15*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM15*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM15, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM16*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM16*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM16*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM16, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM17*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM17*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM17*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM17, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM18*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM18*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM18*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM18, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM19*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM19*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM19*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM19, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM20*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM20*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM20*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM20, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD3_BTN_NUM21*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM21*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD3_BTN_NUM21*2], ',', 2),
			46, 53, hParent, RID_PLD3_BTN_NUM21, 0);
	
	//if (PiChange_Wnd.PrePressedNum != 255)
		PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	//else PInfoWnd2.id = 0;
	if (loginInfo.type == LIT_GUEST)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESET, BN_DISABLE);
		}	
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESETK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESETC, BN_DISABLE);
		}
	}
	else{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD3_BTN_RESETC, BN_PUSHED);
		}
	}
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_LED_Control(LM_STAND_BY);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i b01.bmp,439,248\r");

	UI_PINumber_Display();
	FullInfo.type = FT_PI;

	//UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 68,15,PInfoWnd2.id+1);
			if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
			}
			else {
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
			}
}

LRESULT UI_PiLoad3_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiLoad3_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// PI
		case RID_PLD3_BTN_RESET:
		case RID_PLD3_BTN_RESETK:
		case RID_PLD3_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnReset();
				break;
			}
			return 0;
		case RID_PLD3_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PLD3_BTN_LOAD:
		case RID_PLD3_BTN_LOADK:
		case RID_PLD3_BTN_LOADC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnLoad();
				break;
			}
			return 0;
		case RID_PLD3_BTN_CLOSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnClose();
				break;
			}
			return 0;
		case RID_PLD3_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI1();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI2();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI3();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI4();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI5();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI6();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI7();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI8();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI9();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM10:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI10();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM11:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI11();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM12:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI12();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM13:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI13();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM14:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI14();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM15:
			switch(lParam) {
			case BN_CLICKED: // push
				UI_PiLoad3_OnBnClickedBtnPI15();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM16:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI16();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM17:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI17();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM18:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI18();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM19:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI19();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM20:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI20();
				break;
			}
			return 0;
		case RID_PLD3_BTN_NUM21:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad3_OnBnClickedBtnPI21();
				break;
			}
			return 0;
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PiLoad3_Create(void)
{		
	App_SetWndProcFnPtr(UI_PiLoad3_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i usldb.bmp,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[7][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
}

void UI_PiReset3_OnBnClickedOK(void)
{
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	loginInfo.type = LIT_USER;
	UI_PiLoad3_Create();
}

void UI_PiReset3_OnBnClickedCancel(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad3_Create();
}

void UI_PiReset3_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PILRS3_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS3_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS3_BTN_OK*2], ',', 2),
			113, 46, hParent, RID_PILRS3_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_PILRS3_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS3_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS3_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_PILRS3_BTN_CANCEL, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PILRS3_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS3_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS3_BTN_OK+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS3_BTN_OK+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PILRS3_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS3_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS3_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS3_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PiReset3_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiReset3_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_PILRS3_BTN_OK:
		case RID_PILRS3_BTN_OKK:
		case RID_PILRS3_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset3_OnBnClickedOK();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_PILRS3_BTN_CANCEL:
		case RID_PILRS3_BTN_CANCELK:
		case RID_PILRS3_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset3_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PiReset3_Create(void)
{	
	App_SetWndProcFnPtr(UI_PiReset3_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i pirspu.png,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[8][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.num = UI_WND_MODE_RESET;
}

void  UI_PiLoad4_OnBnClickedBtnReset(void)
{
	UI_PiReset4_Create();
}

void UI_PiLoad4_OnBnClickedBtnClose(void)
{
	PInfoWnd2.id = PATIENT_NUM;
	UI_Home_Create();
}

void UI_PiLoad4_OnBnClickedBtnLeft(void)
{	
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad3_Create();
}

void UI_PiLoad4_OnBnClickedBtnLoad(void)
{
	if((EXERCISE_INFO_NUM*9/10) <= Exercise.addr && Exercise.addr < EXERCISE_INFO_NUM)
		UI_PopupMemoryWarning_Create();
	else if(Exercise.addr >= EXERCISE_INFO_NUM)	UI_PopupDataFull_Create();
	else{
		if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
		else loginInfo.type = LIT_USER;
		
		if (loginInfo.type == LIT_USER){
			UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
			if(PInfoWnd2.pi.pwd > 9999){
				loginInfo.type = LIT_SET_PWD;
				UI_Login_Create();
			}
			else	 UI_Login_Create();
		}
		else{
			UI_InitSetupVariable();
			UI_InitSystemVariable();
			UI_InitVariable();
			UI_Angle_Create();
		}
	}
}

void UI_PiLoad4_OnBnClickedBtnRight(void)
{

}

void UI_PiLoad4_OnBnClickedBtnPI1(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 63;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI2(void)
{	
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 64;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI3(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 65;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI4(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 66;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI5(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 67;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI6(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 68;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI7(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 69;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI8(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 70;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI9(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 71;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI10(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 72;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	
	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
    }
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI11(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 73;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI12(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 74;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI13(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 75;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI14(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 76;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI15(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 77;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI16(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 78;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI17(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 79;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI18(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 80;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_OnBnClickedBtnPI19(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 81;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();

	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}


void UI_PiLoad4_OnBnClickedBtnPI20(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 82;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);	
		}
}

void UI_PiLoad4_OnBnClickedBtnPI21(void)
{
	UI_PiReset_Enable();
	PiChange_Wnd.PrePressedNum = 83;
			
	UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 70,10,PiChange_Wnd.PrePressedNum+1);		
	PInfoWnd2.id = PiChange_Wnd.PrePressedNum;

	if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
		UI_PIVariable();
	}
	else {
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
		UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
	}
}

void UI_PiLoad4_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_RESET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_RESET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_RESET*2], ',', 2),
			71,29, hParent, RID_PLD4_BTN_RESET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD4_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD4_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD4_BTN_RESET+Setup2.language)*2], ',', 2),
			71,29, hParent, RID_PLD4_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_LEFT*2], ',', 2),
			116,45, hParent, RID_PLD4_BTN_LEFT, 0);
	/*API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_LOAD*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_LOAD*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_LOAD*2], ',', 2),
			116,45, hParent, RID_PLD4_BTN_LOAD, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PLD4_BTN_LOAD+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PLD4_BTN_LOAD+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PLD4_BTN_LOAD+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PLD4_BTN_LOAD+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_CLOSE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_CLOSE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_CLOSE*2], ',', 2),
			32,31, hParent, RID_PLD4_BTN_CLOSE, 0);
	/*API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_RIGHT*2], ',', 2),
			116,45, hParent, RID_PLD4_BTN_RIGHT, 0);*/
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM11*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM11, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM12*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM12*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM12*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM12, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM13*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM13*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM13*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM13, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM14*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM14*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM14*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM14, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM15*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM15*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM15*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM15, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM16*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM16*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM16*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM16, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM17*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM17*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM17*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM17, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM18*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM18*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM18*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM18, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM19*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM19*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM19*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM19, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM20*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM20*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM20*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM20, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD4_BTN_NUM21*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM21*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD4_BTN_NUM21*2], ',', 2),
			46, 53, hParent, RID_PLD4_BTN_NUM21, 0);
	
	//if (PiChange_Wnd.PrePressedNum != 255)
		PInfoWnd2.id = PiChange_Wnd.PrePressedNum;
	//else PInfoWnd2.id = 0;
	if (loginInfo.type == LIT_GUEST)
	{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESET, BN_DISABLE);
		}	
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESETK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i npidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESETC, BN_DISABLE);
		}
	}
	else{
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidel.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESET, BN_PUSHED);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelk.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESETK, BN_PUSHED);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pidelc.bmp,367,2\r");
			App_SetButtonOption(RID_PLD4_BTN_RESETC, BN_PUSHED);
		}
	}
	//UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	
	UI_LED_Control(LM_STAND_BY);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i b01.bmp,439,248\r");

	UI_PINumber_Display();
	FullInfo.type = FT_PI;

	//UI_DisplayDecimal_3UnitZero(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 68,15,PInfoWnd2.id+1);
			if (!UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf)) {
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, 0);
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160, 0);
			}
			else {
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 16,87, PInfoWnd2.pi.totalTime);
				UI_DisplayDecimal_5Unit(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 16,160,PInfoWnd2.pi.totalRepeat);
			}
}

LRESULT UI_PiLoad4_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiLoad4_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// PI
		case RID_PLD4_BTN_RESET:
		case RID_PLD4_BTN_RESETK:
		case RID_PLD4_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnReset();
				break;
			}
			return 0;
		case RID_PLD4_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PLD4_BTN_LOAD:
		case RID_PLD4_BTN_LOADK:
		case RID_PLD4_BTN_LOADC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnLoad();
				break;
			}
			return 0;
		case RID_PLD4_BTN_CLOSE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnClose();
				break;
			}
			return 0;
		/*case RID_PLD4_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnRight();
				break;
			}
			return 0;*/
		case RID_PLD4_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI1();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI2();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI3();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI4();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI5();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI6();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI7();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI8();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI9();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM10:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI10();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM11:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI11();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM12:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI12();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM13:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI13();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM14:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI14();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM15:
			switch(lParam) {
			case BN_CLICKED: // push
				UI_PiLoad4_OnBnClickedBtnPI15();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM16:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI16();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM17:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI17();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM18:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI18();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM19:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI19();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM20:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI20();
				break;
			}
			return 0;
		case RID_PLD4_BTN_NUM21:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiLoad4_OnBnClickedBtnPI21();
				break;
			}
			return 0;
				default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PiLoad4_Create(void)
{		
	App_SetWndProcFnPtr(UI_PiLoad4_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i usldb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[7][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
}

void UI_PiReset4_OnBnClickedOK(void)
{
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	loginInfo.type = LIT_USER;
	UI_PiLoad4_Create();
}

void UI_PiReset4_OnBnClickedCancel(void)
{
	if(PInfoWnd2.id == PATIENT_NUM) loginInfo.type = LIT_GUEST;
	else loginInfo.type = LIT_USER;
	
	UI_PiLoad4_Create();
}

void UI_PiReset4_Init(void)
{
	/*API_CreateWindow("", pBtnInfo[RID_PILRS4_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS4_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS4_BTN_OK*2], ',', 2),
			113, 46, hParent, RID_PILRS4_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_PILRS4_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PILRS4_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PILRS4_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_PILRS4_BTN_CANCEL, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PILRS4_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS4_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS4_BTN_OK+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS4_BTN_OK+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_PILRS4_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PILRS4_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PILRS4_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_PILRS4_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PiReset4_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiReset4_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_PILRS4_BTN_OK:
		case RID_PILRS4_BTN_OKK:
		case RID_PILRS4_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset4_OnBnClickedOK();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_PILRS4_BTN_CANCEL:
		case RID_PILRS4_BTN_CANCELK:
		case RID_PILRS4_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset4_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PiReset4_Create(void)
{	
	App_SetWndProcFnPtr(UI_PiReset4_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i pirspu.png,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[8][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.num = UI_WND_MODE_RESET;
}
#endif //PI_LOAD_V1

/*void UI_RunPIInit_OnBnClickedBtnOk(void)
{
	PInfoWnd2.pi.totalRepeat = 0;
	PInfoWnd2.pi.totalTime = 0;
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_Setup2_Create();
}

void UI_RunPIInit_OnBnClickedBtnNo(void)
{
	UI_Setup2_Create();
}

void UI_RunPIInit_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[RID_RI_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RI_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RI_BTN_OK*2], ',', 2),
			74, 31, hParent, RID_RI_BTN_OK, 0);
	API_CreateWindow("", pBtnInfo[RID_RI_BTN_NO*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RI_BTN_NO*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RI_BTN_NO*2], ',', 2),
			74, 31, hParent, RID_RI_BTN_NO, 0);

	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i iibk.bmp,91,81\r"); // pid txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i iiqt.bmp,99,100\r"); // msg txt
}

LRESULT UI_RunPIInit_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_RunPIInit_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam?? word low?? id?????
		//
		// Speed time setting
		case RID_RI_BTN_OK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_RunPIInit_OnBnClickedBtnOk();
				break;
			}
			return 0;
		case RID_RI_BTN_NO:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_RunPIInit_OnBnClickedBtnNo();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_RunPIInit_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_RunPIInit_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)"i iibk.bmp,91,81\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.num = UI_WND_MODE_RUN_PI_INIT;
	
}
//
// run complete(±∏µøøœ∑·)
void UI_PopupRunComplete_OnBnClickedBtnOk(void)
{
	UI_Play_Create();
	UI_LED_Control(LM_STAND_BY);
	Total_Counter = PInfoWnd2.pi.totalRepeat;
}
//±∏µøøœ∑· »≠∏È ±∏º∫
void UI_PopupRunComplete_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[RID_RC_BTN_OK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RC_BTN_OK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RC_BTN_OK*2], ',', 2),
			83, 35, hParent, RID_RC_BTN_OK, 0);

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rcmsg.bmp,83,55\r"); // msg txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnpid.bmp,67,142\r"); // pid txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,52,165\r"); //pid lcd
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnttt.bmp,196,142\r"); // total time txt
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnmtt.bmp,255,145\r"); // min of total time txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rclcd.bmp,178,165\r"); //total time lcd
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rntct.bmp,327,142\r"); // total rpt txt
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnrpt2.bmp,387,145\r"); // min of total rpt txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rclcd.bmp,310,165\r"); //total rpt lcd

	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 63, DISP_NUM_RC_Y_POS, PInfoWnd2.id);
	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 200, DISP_NUM_RC_Y_POS, PInfoWnd2.pi.totalTime);
	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 334, DISP_NUM_RC_Y_POS, PInfoWnd2.pi.totalRepeat);
	UI_LED_Control(LM_Complet);
          if (Setup2.sndGuid2 == BST_CHECKED)
              APP_SendMessage(hParent, WM_SOUND, 0, SNDID_RUN_COMPLETE);
}

LRESULT UI_PopupRunComplete_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupRunComplete_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		// Speed time setting
		case RID_RC_BTN_OK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupRunComplete_OnBnClickedBtnOk();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}
//±∏µøøœ∑· »≠∏È ª˝º∫
void UI_PopupRunComplete_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_PopupRunComplete_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pLdBkgImg, WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_Run_Process);
	UI_Wnd.num = UI_WND_MODE_RUN_COMPLETE;
	
}

//
// run(±∏µø)
void UI_Run_InitVar(void)
{
          RunWnd.repeat = 0;                    //±∏µø π›∫π»∏ºˆ √ ±‚»≠
//          Total_Counter = 0;
	RunWnd.play = UI_RUN_MODE_STOP;     //±∏µøªÛ≈¬
	RunWnd.dir = 0;                     //±∏µø πÊ«‚
//	RunWnd.angle = AngleWnd.exAngle;    //∏≈Õ¿« ±∏µø∞¢µµ ¿˙¿Â
          RunWnd.angle = Current_Angle - 5;
	RunWnd.tick = 0;                    //∏≈Õ ±∏µøΩ√∞£ ∆Ω(Ω√∞£)∞™
	RunWnd.time = SpdTmWnd.time;        //±∏µø«— Ω√∞£(∫– ¥‹¿ß)
	RunWnd2.motorOverloadCnt = 0;	//pjg++170626
}

void UI_Run_OnBnClickedBtnLeft(void)
{
        Run_mode = 0;
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		PInfoWnd2.pi.totalRepeat = Total_Counter;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
	RunWnd.play = 0;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); //pjg++170529

	UI_SpeedTime_Create();
}

void UI_Run_OnBnClickedBtnRight(void)
{
	//UI_Run_Init();
}
//√≥¿Ω ±∏µø(«√∑π¿Ã) πˆ∆∞ ¥≠∑Í ∞ÊøÏ
void UI_Run_OnBnClickedBtnPlayStart(void)
{
    Run_mode = 1;
	RunWnd.play = UI_RUN_MODE_PLAY;
	API_ChangeHMenu(hParent, RID_RN_BTN_PLAY, RID_RN_BTN_PLAYING);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAYING*2]);
	if (Setup2.sndGuid2== BST_CHECKED)
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_START);
	//App_SetTimer(1, 150000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed(150000)
	App_SetTimer(TIMER_ID_2, 200); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed
	App_SetUIProcess(UI_Run_Process); //pjg++170529
	UI_LED_Control(LM_RUN);
	Total_Counter = PInfoWnd2.pi.totalRepeat;
	Timer_sec = 0; //pjg++170608 buf fix: complete right now
}
//±∏µø ¡ﬂø° «√∑π¿Ã πˆ∆∞¿ª ¥ŸΩ√ ¥©∏• ∞ÊøÏ(¿œΩ√¡§¡ˆ)
void UI_Run_OnBnClickedBtnPause(void)
{

    Run_mode = 2;
	RunWnd.play = UI_RUN_MODE_PAUSE;
	API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PAUSE);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PAUSE*2]);
	if (Setup2.sndGuid2 == BST_CHECKED)
              APP_SendMessage(0, WM_SOUND, 0, SNDID_OPERATION_STOP);
	//App_SetTimer(1, 150000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed(150000)
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); //pjg++170529
	UI_LED_Control(LM_PAUSE);
	PInfoWnd2.pi.totalRepeat = Total_Counter;
	UI_Run_PopupMotorOverload(0);
}
//¿œΩ√¡§¡ˆø°º≠ «√∑π¿Ãπˆ∆∞¿ª ¥ŸΩ√ ¥©«¬ ∞ÊøÏ
void UI_Run_OnBnClickedBtnPlay(void)
{
    Run_mode = 1;
	RunWnd.play = UI_RUN_MODE_PLAY;
	API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAYING);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAYING*2]);
	if (Setup.sndGuid == BST_CHECKED)
		APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_START);
		App_SetTimer(TIMER_ID_2, 200); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed
		App_SetUIProcess(UI_Run_Process); //pjg++170529
		UI_LED_Control(LM_RUN);
		PInfoWnd2.pi.totalRepeat = Total_Counter;
		Timer_sec = 0; //pjg++170608 buf fix: complete right now
		
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
}
//∏ÿ√„πˆ∆∞(stop)¿ª ¥©∏•∞ÊøÏ
void UI_Run_OnBnClickedBtnStop(void)
{	
    Run_mode = 0;
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		//RunWnd.play = UI_RUN_MODE_STOP;
		API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_RN_BTN_PLAY);
		API_ChangeHMenu(hParent, RID_RN_BTN_PLAYING, RID_RN_BTN_PLAY);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
		if (Setup2.sndGuid2 == BST_CHECKED)
			APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_END);
		App_KillTimer(TIMER_ID_2);
//		UI_Run_PopupMotorOverload(0);
		UI_Run_InitVar();
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 67, DISP_NUM_RN1_Y_POS, RunWnd.time);
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 197, DISP_NUM_RN1_Y_POS, AngleWnd.exAngle);
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 327, DISP_NUM_RN1_Y_POS, RunWnd.repeat);
		App_SetUIProcess(UI_ProcessNull); //pjg++170529
		UI_LED_Control(LM_PAUSE);
		PInfoWnd2.pi.totalRepeat = Total_Counter;
		UI_Run_PopupMotorOverload(0);
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
}
//»Ø¿⁄¡§∫∏ √¢¿∏∑Œ ¿Ãµø
void UI_Run_OnBnClickedBtnGoFirst(void)
{
        Run_mode = 0;
	App_KillTimer(TIMER_ID_2);
	App_SetUIProcess(UI_ProcessNull); //pjg++170529
	UI_PatientInfo_Create();
	UI_LED_Control(LM_NONE);
	if (RunWnd.play >= UI_RUN_MODE_PLAY) {
		PInfoWnd2.pi.totalRepeat = Total_Counter;
		UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	}
}
void UI_Run_OnBnClickedBtnInit(void)
{
	
	UI_RunPIInit_Create();
}

void UI_Run_OnKeyUp(uint32_t nChar)
{
	switch (nChar) {
	case KEY_STOP:
		if (RunWnd.play == UI_RUN_MODE_STOP) {
			UI_Play_OnBnClickedBtnPlay();
		}
		else if (RunWnd.play == UI_RUN_MODE_PLAY) {
			UI_Play_OnBnClickedBtnPlaying();
		}
		else if (RunWnd.play == UI_RUN_MODE_PAUSE) {
			UI_Play_OnBnClickedBtnPlay();
		}
		break;
	default:
		break;
	}		
}

void UI_Run_OnKeyLong(uint32_t nChar)
{
	switch (nChar) {
	case KEY_STOP:
		Run_mode = 0;
		if (RunWnd.play >= UI_RUN_MODE_PLAY) {
			App_KillTimer(TIMER_ID_2);
		}
		UI_PatientInfo_Create();
	default:
		break;
	}
}

//pjg++170529
void UI_Run_ProgressiveMode(void)
{
	if (!Setup.angle || Setup.lsrpt_en == 0) return;
}

//pjg++170529
void UI_Run_WaitingMode(void)
{
	//if (!Setup.angle || Setup.lsrpt_en == 0) return;
	if (!Setup2.stand_by || Setup2.lsrpt_en2 == 0) return;

   if (MOTOR_STATE == MOTOR_CCW) 
   { // up
      if ((short)Current_Angle < AngleWnd.flAngle+5) 
      {

         if(Run_mode == 0){
            if (UI_Time.tmp_sec >= Setup2.stand_by) 
            {
               Run_mode = 1;
               //tmp_waitMode = 0;
               HAL_Delay(1000);
            }
         }
         else if (Run_mode == 1) 
         {
            Run_mode = 0;
            //tmp_waitMode = 1;
            //UI_Time.tmp_ms = 0;
            UI_Time.tmp_sec = 0;
            //Timer_sec = 0;
         }                              		  
      }
   }
   else 
   {
      if ((short)Current_Angle >= AngleWnd.exAngle+5) 
      {

         if(Run_mode == 0)
         {
            if (UI_Time.tmp_sec >= Setup2.stand_by) 
            {
               Run_mode = 1;
               //tmp_waitMode = 0;
               HAL_Delay(1000);
            }
         }
         else if (Run_mode == 1) 
         {
            Run_mode = 0;
           // tmp_waitMode = 1;
            //UI_Time.tmp_ms = 0;
            UI_Time.tmp_sec = 0;
            //UI_Time.tmp_ms = 0;
            //Timer_sec = 0;
         }                              
      }
   }
}
void UI_Run_ProcessNull(void)
{
}

void UI_Run_SetGotoHomeCmd(void)
{
	Home_mode = 1;
	Home_flag = 0;
}

int UI_Run_CheckHomePos(void)
{
	if (Home_sensor == 1) return 1;
	else return 0;
}
uint32_t UI_Run_GetNowMotorCurrent(void)
{
	return Motor_Current;
}

uint32_t UI_Run_GetMotorOverCurrent(void)
{
	return Over_Current;
}

void UI_Run_SetMotorOverCurrent(uint32_t current)
{
	Over_Current = current;
}

void UI_Run_PopupMotorOverload(uint32_t level)
{
	if (level > UI_Run_GetMotorOverCurrent()) {
		if (UI_Time.tmp_ms > 100) {
			UI_Time.tmp_ms = 0;
			RunWnd2.motorOverloadCnt++;
		}
		if (RunWnd2.motorOverloadCnt > 1) {
			if (UI_Wnd.child_num == UI_WND_MODE_CHILD_NONE) {
				if (Setup.sndGuid == BST_CHECKED)
					APP_SendMessage(hParent, WM_SOUND, 0, SNDID_MOTOR_OVERLOAD);
				APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnbk.bmp,46,2\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnwnrt.bmp,155,25\r");
				APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnmsgrt.bmp,85,66\r");
				UI_Wnd.child_num = UI_WND_MODE_CHILD_MSG;
				//RunWnd2.motorOverloadCnt = 0;
	 		}
		}
	}
	else {
		RunWnd2.motorOverloadCnt = 0;
		if (UI_Wnd.child_num == UI_WND_MODE_CHILD_MSG) {
			//APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnwnet.bmp,150,10\r");
			//APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnmsget.bmp,100,40\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (WPARAM)"i wnwh.bmp,46,2\r");

			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sttmt.bmp,70,15\r"); //time txt
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i stmtt.bmp,110,17\r"); // time min
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,58,38\r");	//time lcd
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnagt.bmp,200,15\r"); // angle txt
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i asdgt.bmp,240,15\r"); // deg txt
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,188,38\r"); //angle
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rncnt.bmp,330,15\r"); // repeat txt
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnrpt.bmp,370,16\r"); // rpt of rpt txt
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,318,38\r"); //rpt
			UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;

			UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 67, DISP_NUM_RN1_Y_POS, RunWnd.time);
			UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 197, DISP_NUM_RN1_Y_POS, RunWnd.angle);
			UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 327, DISP_NUM_RN1_Y_POS, RunWnd.repeat);
		}
	}
}

//±∏µø¡ﬂø° Ω««‡µ«¥¬ «‘ºˆ
void UI_Run_Process(void)
{
   //∏≈Õ ±∏µø¿Ã øœ∑· µ«æ˙¿ª ∞ÊøÏ
	if (RunWnd.time == 0) {
		if (RunWnd.play == UI_RUN_MODE_PLAY) {
			//App_SetUIProcess(UI_ProcessNull);
			API_ChangeHMenu(hParent, RID_RN_BTN_PAUSE, RID_PLAY_BTN_PLAY);
			API_ChangeHMenu(hParent, RID_PLAY_BTN_PLAYING, RID_PLAY_BTN_PLAY);
			//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAY*2]);
			//APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_END);
			App_KillTimer(TIMER_ID_2);
			RunWnd.play = UI_RUN_MODE_STOP;
			Run_mode = 0;
			if (UI_Run_GetMotorOverCurrent() < MOTOR_HOME_IN_CURRENT) {
				RunWnd2.OverCurrentBk = UI_Run_GetMotorOverCurrent();
				UI_Run_SetMotorOverCurrent(MOTOR_HOME_IN_CURRENT); //pjg++170623
			}
			else RunWnd2.OverCurrentBk = 0;
			UI_Run_SetGotoHomeCmd();
			PInfoWnd2.pi.totalRepeat = Total_Counter;
			UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);													   
		}

		if (UI_Run_CheckHomePos()) {
			if (RunWnd2.OverCurrentBk > 0) UI_Run_SetMotorOverCurrent(RunWnd2.OverCurrentBk);
			App_SetUIProcess(UI_ProcessNull); //pjg++170529
			UI_PopupRunComplete_Create();
		}
		
		if (Current_Error == 1) {
			App_SetUIProcess(UI_ProcessNull);			
			TLCD_DrawText("i wnbk.bmp,46,164\r");
			TLCD_DrawText("fc 255,0,0\r\n");
			TLCD_DrawText("f The motor is overloaded!,48,180\r\n");
			TLCD_DrawText("f Please check your load!,48,200\r\n");
			TLCD_DrawText("f And turn off the power!,48,220\r\n");
			while(1);
		}
		return;
	}

	//UI_Run_PopupMotorOverload(UI_Run_GetNowMotorCurrent());

	//UI_Run_ProgressiveMode();
	//UI_Run_WaitingMode();
}
//±∏µø¡ﬂø° Ω««‡µ«¥¬ «‘ºˆ

void UI_Run_DisplayValue(void)
{
	//±∏µø Ω√∞£ «•«ˆ
	UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_4PLACE, 410, 59, RunWnd.time);
    //∏≈Õ ∞¢µµ «•«ˆ
	UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_5PLACE, 410, 151, RunWnd.angle);
    //±∏µø π›∫π«— »Ωºˆ «•«ˆ
	UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_6PLACE, 410, 105, RunWnd.repeat);
       UI_DisplayDecimal5(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 58, 154, Total_Counter);
    //«ˆ¿Á±Ó¡ˆ ±∏µø«— √— Ω√∞£ «•«ˆ
//	PInfoWnd.totalTime++;
	UI_DisplayDecimal5(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 58,106, PInfoWnd2.pi.totalTime);
	//if (PInfoWnd.totalTime > 999) PInfoWnd.totalTime = 0;
}*/

/*void UI_Run_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_LEFT*2], ',', 2),
			39, 79, hParent, RID_RN_BTN_LEFT, 0);
	App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_RIGHT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_RIGHT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_RIGHT*2], ',', 2),
			39, 79, hParent, RID_RN_BTN_RIGHT, 0);
	//play/stop
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_PLAY*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_PLAY*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_PLAY*2], ',', 2),
			107, 101, hParent, RID_RN_BTN_PLAY, 0);
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_STOP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_STOP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_STOP*2], ',', 2),
			107, 101, hParent, RID_RN_BTN_STOP, 0);
	//go first
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_GO_FIRST*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_GO_FIRST*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_GO_FIRST*2], ',', 2),
			87, 45, hParent, RID_RN_BTN_GO_FIRST, 0);

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i sttmt.bmp,70,15\r"); //time txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i stmtt.bmp,110,17\r"); // time min
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,58,38\r");	//time lcd
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnagt.bmp,200,15\r"); // angle txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i asdgt.bmp,240,15\r"); // deg txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,188,38\r"); //angle
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rncnt.bmp,330,15\r"); // repeat txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnrpt.bmp,370,16\r"); // rpt of rpt txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i pilcd.bmp,318,38\r"); //rpt

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnpid.bmp,35,220\r"); // pid txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i srlcd.bmp,37,240\r"); //pid lcd
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnttt.bmp,117,220\r"); // total time txt
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnmtt.bmp,171,222\r"); // min of total time txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rulcd.bmp,130,240\r"); //total time lcd
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rntct.bmp,212,220\r"); // total rpt txt
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rnrpt2.bmp,266,222\r"); // min of total rpt txt
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rulcd.bmp,223,240\r"); //total rpt lcd

	//APP_SendMessage(hParent, WM_SOUND, 0, SNDID_SPEED_SETTING);
	UI_Run_InitVar();	
	
	//Total_Counter = PInfoWnd2.pi.totalRepeat;									  
	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 67, DISP_NUM_RN1_Y_POS, RunWnd.time);
	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 197, DISP_NUM_RN1_Y_POS, AngleWnd.exAngle);
	UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 327, DISP_NUM_RN1_Y_POS, RunWnd.repeat);
	UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_1PLACE, 46, DISP_NUM_RN2_Y_POS, PInfoWnd2.id);
	UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_2PLACE, 140, DISP_NUM_RN2_Y_POS, PInfoWnd2.pi.totalTime);
	//UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 233, DISP_NUM_RN2_Y_POS, Total_Counter);
          //UI_DisplayDecimal(UI_DISP_NUM_FNT5, UI_DISP_NUM_3PLACE, 233, DISP_NUM_RN2_Y_POS, Total_Counter);
	//uart_putstring("l +\r");
}*/

/*LRESULT UI_Run_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Run_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//run
		case RID_RN_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_RN_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_RN_BTN_PLAY:
			switch(lParam) {
			case BN_CLICKED: // push 
                                  Run_mode = 1;
				UI_Run_OnBnClickedBtnPlayStart();
				break;
			}
			return 0;
		case RID_RN_BTN_PLAYING:
			switch(lParam) {
			case BN_CLICKED: // push     
                                  Run_mode = 2;
				UI_Run_OnBnClickedBtnPause();
				break;
			}
			return 0;
		case RID_RN_BTN_PAUSE:
			switch(lParam) {
			case BN_CLICKED: // push 
                                  Run_mode = 1;                                
				UI_Run_OnBnClickedBtnPlay();
				break;
			}
			return 0;
		case RID_RN_BTN_STOP:
			switch(lParam) {
			case BN_CLICKED: // push 
                                  Run_mode = 0;                                
				UI_Run_OnBnClickedBtnStop();
				break;
			}
			return 0;
		case RID_RN_BTN_GO_FIRST:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Run_OnBnClickedBtnGoFirst();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		UI_Timer(wParam);
		break;
	case WM_KEYUP:
		UI_Run_OnKeyUp(wParam);
		break;
	case WM_KEYLONG:
		UI_Run_OnKeyLong(wParam);
		break;
	default:
		break;
	}

	return 0;
}*/
//±∏µø»≠∏È ª˝º∫
/*void UI_Run_Create(void)
{
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_Run_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pLdBkgImg, WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	App_SetUIProcess(UI_ProcessNull);
	UI_Wnd.num = UI_WND_MODE_RUN;
	UI_Wnd.child_num = UI_WND_MODE_CHILD_NONE;
}*/

//
// setup(ºº∫Œ º≥¡§1)
//

void UI_Progress_OnBnClickedBtnLeft(void)
{
	UI_AdSet_Create();
}

void UI_Progress_OnBnClickedBtnExit(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else 
	//	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813

	//UI_GoBackWnd();
	UI_Setup_Create(); //pjg++190910
} 

void UI_Progress_OnBnClickedBtnRight(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
	UI_Pause_Create();
}

void UI_Progress_OnBnClickedBtnDegUp(void)
{
	if(Setup3.PDeg < 10) Setup3.PDeg++;
	if(Setup3.PDeg == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
	}
	if(Setup3.PDeg > 5)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 167,75,Setup3.PDeg,2);
}

void UI_Progress_OnBnClickedBtnDegDown(void)
{
	if(Setup3.PDeg > 5) Setup3.PDeg--;
	if(Setup3.PDeg == 5)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);
	}
	if(Setup3.PDeg < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 167,75,Setup3.PDeg,2);
}

void UI_Progress_OnBnLongClickedBtnDegUp(void)
{
	if (Setup3.PDeg < 8) 
	{
		Setup3.PDeg += 2;
	}
	else if(Setup3.PDeg <= 8 || Setup3.PDeg < 10)
		Setup3.PDeg++;
	if(Setup3.PDeg == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
	}
	if(Setup3.PDeg > 5)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 167,75,Setup3.PDeg,2);
}

void UI_Progress_OnBnLongClickedBtnDegDown(void)
{
	if (Setup3.PDeg > 7) 
	{
		Setup3.PDeg -= 2;	
	}

	else if(Setup3.PDeg >= 7  || Setup3.PDeg > 1)
		Setup3.PDeg--;
	
	if(Setup3.PDeg == 5)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);
	}
	if(Setup3.PDeg < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 167,75,Setup3.PDeg,2);
}

void UI_Progress_OnBnClickedBtnTimeUp(void)
{
	if(Setup3.PTm < 20) Setup3.PTm++;
	{
		if(Setup3.PTm >= SpdTmWnd.time || Setup3.PTm == 20)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
		}
		if(Setup3.PTm > 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
			App_SetButtonOption(RID_PRO_BTN_TMM, BN_PUSHED);
		}
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
}

void UI_Progress_OnBnClickedBtnTimeDown(void)
{
	if(Setup3.PTm > 1) Setup3.PTm--;
	{
		if(Setup3.PTm < SpdTmWnd.time)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
		}
		if(Setup3.PTm == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
			App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);
		}
		if(Setup3.PTm < 20)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
		}	
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
}

void UI_Progress_OnBnLongClickedBtnTimeUp(void)
{
	if(Setup3.PTm < 20)
	{	if(Setup3.PTm < 16)
		{
			if(Setup3.PTm < SpdTmWnd.time-4)
				Setup3.PTm += 3;
			else
				Setup3.PTm++;
		}
		else
			Setup3.PTm++;
	}
	if(Setup3.PTm >= SpdTmWnd.time || Setup3.PTm == 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
	}
	if(Setup3.PTm > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
		App_SetButtonOption(RID_PRO_BTN_TMM, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
	/*if (Setup3.PTm < 17) 
	{
		Setup3.PTm += 3;
	}
	else if(Setup3.PTm<= 17 || Setup3.PTm < 20)
		Setup3.PTm++;*/
																				   
}

void UI_Progress_OnBnLongClickedBtnTimeDown(void)
{
	if(Setup3.PTm > 1)
	{
		if (Setup3.PTm > 3) Setup3.PTm -= 3;
		else Setup3.PTm--;
	}

	if(SpdTmWnd.time > Setup3.PTm){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
	}
	if(Setup3.PTm == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);
	}
	if(Setup3.PTm < 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
	}	
	/*if (Setup3.PTm > 3) 
	{
		Setup3.PTm -= 2;	
	}

	else if(Setup3.PTm>= 3  || Setup3.PTm> 1)
		Setup3.PTm --;*/
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
}

void UI_Progress_OnBnClickedBtnChk(void)
{	
	if (Setup3.ProChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);	
		App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
		App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);	
		Setup3.ProChk = BST_UNCHECKED;
	}
	else { 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		if	(UI_Wnd.prevNum == UI_WND_MODE_RUN) {
		}
		else {
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
			App_SetButtonOption(RID_PRO_BTN_DEGP, BN_PUSHED);
			App_SetButtonOption(RID_PRO_BTN_DEGM, BN_PUSHED);	
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
			App_SetButtonOption(RID_PRO_BTN_TMM, BN_PUSHED);
			App_SetButtonOption(RID_PRO_BTN_CHK, BN_PUSHED);
			if(Setup3.PTm >= SpdTmWnd.time){
				Setup3.PTm = SpdTmWnd.time;
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
			}
			else{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
			}
			if(Setup3.PDeg == 5)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
				App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);
			}
			if(Setup3.PDeg == 10)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
				App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
			}
			if(Setup3.PTm == 20)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
			}
			if(Setup3.PTm == 1)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
				App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);
			}
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
		}
		Setup3.ProChk = BST_CHECKED;
	}
}

void UI_Progress_Process(void)
{

}

//º≥¡§√¢ ±∏º∫
void UI_Progress_Init(void)
{
   // limit section repeat button
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
   	/*API_CreateWindow("", pBtnInfo[RID_PRO_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PRO_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PRO_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_PRO_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_PRO_BTN_DEGP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PRO_BTN_DEGP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PRO_BTN_DEGP*2], ',', 2),
			58, 46, hParent, RID_PRO_BTN_DEGP, 0);
	API_CreateWindow("", pBtnInfo[RID_PRO_BTN_DEGM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PRO_BTN_DEGM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PRO_BTN_DEGM*2], ',', 2),
			58, 46, hParent, RID_PRO_BTN_DEGM, 0);
	API_CreateWindow("", pBtnInfo[RID_PRO_BTN_TMP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PRO_BTN_TMP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PRO_BTN_TMP*2], ',', 2),
			58, 46, hParent, RID_PRO_BTN_TMP, 0);
	API_CreateWindow("", pBtnInfo[RID_PRO_BTN_TMM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PRO_BTN_TMM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PRO_BTN_TMM*2], ',', 2),
			58, 46, hParent, RID_PRO_BTN_TMM, 0);
	API_CreateWindow("", pBtnInfo[RID_PRO_BTN_CHK*2], BS_CHECKBOX, 
			420,4,
			46, 26, hParent, RID_PRO_BTN_CHK, 0);

	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	if (Setup3.ProChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		if (UI_Wnd.prevNum == UI_WND_MODE_RUN) {
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
			App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
			App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);	
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
			App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);	
		}
		else {
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
			App_SetButtonOption(RID_PRO_BTN_DEGP, BN_PUSHED);
			App_SetButtonOption(RID_PRO_BTN_DEGM, BN_PUSHED);	
			App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
			App_SetButtonOption(RID_PRO_BTN_TMM, BN_PUSHED);
			if(Setup3.PTm >= SpdTmWnd.time){
				Setup3.PTm = SpdTmWnd.time;
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
			}
			else{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_PUSHED);
			}
			if(Setup3.PDeg == 5)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
				App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);
			}
			if(Setup3.PDeg == 10)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,66\r");
				App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
			}
			if(Setup3.PTm == 20)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
				App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
			}
			if(Setup3.PTm == 1)
			{
				APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
				App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);
			}
		}
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_PRO_BTN_DEGP, BN_DISABLE);
		App_SetButtonOption(RID_PRO_BTN_DEGM, BN_DISABLE);	
		App_SetButtonOption(RID_PRO_BTN_TMP, BN_DISABLE);
		App_SetButtonOption(RID_PRO_BTN_TMM, BN_DISABLE);	
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 167,75,Setup3.PDeg,2);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 160,152,Setup3.PTm,2);	
	if(Setup3.VibChk == BST_CHECKED)
		App_SetButtonOption(RID_PRO_BTN_CHK, BN_DISABLE);
	else
		App_SetButtonOption(RID_PRO_BTN_CHK, BN_PUSHED);
}							   

LRESULT UI_Progress_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Progress_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//setup
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_PRO_BTN_DEGP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnDegUp();
				break;
			case BN_LONGPUSHED: //long push 
                            UI_Progress_OnBnLongClickedBtnDegUp();
                            break;      
			}
			return 0;
		case RID_PRO_BTN_DEGM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnDegDown();
				break;
			case BN_LONGPUSHED: //long push 
               UI_Progress_OnBnLongClickedBtnDegDown();
               break; 
			}
			return 0;
		case RID_PRO_BTN_TMP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnTimeUp();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Progress_OnBnLongClickedBtnTimeUp();
                break;      
			}
			return 0;
		case RID_PRO_BTN_TMM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Progress_OnBnClickedBtnTimeDown();
				break;
			case BN_LONGPUSHED: //long push 
               UI_Progress_OnBnLongClickedBtnTimeDown();
               break; 
			}
			return 0;
		case RID_PRO_BTN_CHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Progress_OnBnClickedBtnChk();
                break;
			}
			return 0; 
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam); 
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

//º≥¡§√¢ ª˝º∫
void UI_Progress_Create(void)
{
	App_SetWndProcFnPtr(UI_Progress_WndProc);

	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_PRO][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_PROGRESS;
}
//ydy>++170330 all

// setup(ºº∫Œ º≥¡§2)
void UI_Pause_OnBnClickedBtnLeft(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	UI_AdSet_Create();
}

void UI_Pause_OnBnClickedBtnExit(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else 
	//	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
}


void UI_Pause_OnBnClickedBtnRight(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
	UI_Vibration_Create();
}
#define LIMITED_PAUSE_MAX_VALUE				99
void UI_Pause_OnBnClickedBtnUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.LP < LIMITED_PAUSE_MAX_VALUE) Setup3.LP ++;
	if(Setup3.LP == LIMITED_PAUSE_MAX_VALUE)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.LP > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.LP,2);
}

void UI_Pause_OnBnClickedBtnDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.LP > 1)Setup3.LP --;

	if(Setup3.LP == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.LP < LIMITED_PAUSE_MAX_VALUE)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.LP,2);
}

void UI_Pause_OnBnLongClickedBtnUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	/*
	if (Setup3.LP < 8) 
	{
		Setup3.LP += 2;
	}
	//else if(Setup3.LP<= 8 || Setup3.LP < 98)
	else if(Setup3.LP<= 8 || Setup3.LP < 98)*/
	if (Setup3.LP < LIMITED_PAUSE_MAX_VALUE-5)
		Setup3.LP += 5;
	else  Setup3.LP++;

	if(Setup3.LP == LIMITED_PAUSE_MAX_VALUE)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.LP > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.LP,2);
}

void UI_Pause_OnBnLongClickedBtnDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	/*if (Setup3.LP > 3) 
	{
		Setup3.LP -= 2;	
	}

	else if(Setup3.LP>= 3  || Setup3.LP> 1)*/
	if (Setup3.LP > 5)
		Setup3.LP -= 5;
	else {
		if (Setup3.LP > 1) Setup3.LP--;
	}
	if(Setup3.LP == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.LP < LIMITED_PAUSE_MAX_VALUE)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.LP,2);
}

void UI_Pause_OnBnClickedBtnChk(void)
{
	if (Setup3.LPChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);	
		Setup3.LPChk = BST_UNCHECKED;
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);	
		Setup3.LPChk = BST_CHECKED;
		if(Setup3.LP == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
			App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
		}
		if(Setup3.LP == LIMITED_PAUSE_MAX_VALUE)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
			App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
		}
	}
}

void UI_Pause_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_CHK*2], BS_CHECKBOX, 
			420,4,
			46, 26, hParent, RID_LP_BTN_CHK, 0);

	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.LP,2);

	if (Setup3.LPChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);	
		if(Setup3.LP == 1)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
			App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
		}
		if(Setup3.LP == LIMITED_PAUSE_MAX_VALUE)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
			App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
		}

	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);	
	}
	
}

LRESULT UI_Pause_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Pause_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//setup
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pause_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pause_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_PRO_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pause_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pause_OnBnClickedBtnUp();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Pause_OnBnLongClickedBtnUp();
                break;  
			}
			return 0;
		case RID_LP_BTN_PAUSEM :
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Pause_OnBnClickedBtnDown();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Pause_OnBnLongClickedBtnDown();
                break; 
			}
			return 0;
		case RID_LP_BTN_CHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Pause_OnBnClickedBtnChk();
                break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam); 
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
	
}

void UI_Pause_Create(void)
{	
	App_SetWndProcFnPtr(UI_Pause_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i limb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_LIM][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_PAUSE;
}

//ydy<++170330 all

// setup(ºº∫Œ º≥¡§3)
void UI_Vibration_OnBnClickedBtnLeft(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	UI_AdSet_Create();
	
}

void UI_Vibration_OnBnClickedBtnExit(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else 
	//	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();

}

void UI_Vibration_OnBnClickedBtnRight(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
	UI_Sens_Create();
}

void UI_Vibration_OnBnClickedBtnDegUp(void)
{
	if(Setup3.VDeg< 10) Setup3.VDeg++;

	if(Setup3.VDeg == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
	}
	if(Setup3.VDeg > 3)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 165,77,Setup3.VDeg,2);
}

void UI_Vibration_OnBnClickedBtnDegDown(void)
{
	if(Setup3.VDeg > 3) Setup3.VDeg --;

	if(Setup3.VDeg == 3)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);
	}
	if(Setup3.VDeg < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 165,77,Setup3.VDeg,2);
}

void UI_Vibration_OnBnLongClickedBtnDegUp(void)
{
	if (Setup3.VDeg < 8) 
	{
		Setup3.VDeg += 2;
	}
	else if(Setup3.VDeg<= 8 || Setup3.VDeg < 10)
		Setup3.VDeg++;

	if(Setup3.VDeg == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
	}
	if(Setup3.VDeg > 3)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 165,77,Setup3.VDeg,2);
}

void UI_Vibration_OnBnLongClickedBtnDegDown(void)
{
	if (Setup3.VDeg > 5) 
	{
		Setup3.VDeg -= 2;	
	}

	else if(Setup3.VDeg>= 5  || Setup3.VDeg> 3)
		Setup3.VDeg --;

	if(Setup3.VDeg == 3)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);
	}
	if(Setup3.VDeg < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 165,77,Setup3.VDeg,2);
}

void UI_Vibration_OnBnClickedBtnCntUp(void)
{
	if(Setup3.VCnt < 20) Setup3.VCnt ++;

	if(Setup3.VCnt == 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
	}
	if(Setup3.VCnt > 2)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 165,148,Setup3.VCnt,2);
}

void UI_Vibration_OnBnClickedBtnCntDown(void)
{
	if(Setup3.VCnt > 2) Setup3.VCnt --;
	if(Setup3.VCnt == 2)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
	}
	if(Setup3.VCnt < 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 165,148,Setup3.VCnt,2);
}

void UI_Vibration_OnBnLongClickedBtnCntUp(void)
{
	if (Setup3.VCnt < 17) 
	{
		Setup3.VCnt += 3;
	}
	else if(Setup3.VCnt<= 17 || Setup3.VCnt < 20)
		Setup3.VCnt++;

	if(Setup3.VCnt == 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
	}
	if(Setup3.VCnt > 2)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 165,148,Setup3.VCnt,2);
}

void UI_Vibration_OnBnLongClickedBtnCntDown(void)
{
	if (Setup3.VCnt > 5) 
	{
		Setup3.VCnt -= 3;	
	}

	else if(Setup3.VCnt>= 5  || Setup3.VCnt> 2)
		Setup3.VCnt --;
	if(Setup3.VCnt == 2)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
	}
	if(Setup3.VCnt < 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 165,148,Setup3.VCnt,2);
}

void UI_Vibration_OnBnClickedBtnChk(void)
{
	if (Setup3.VibChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);	
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
		Setup3.VibChk = BST_UNCHECKED;
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_PUSHED);
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_PUSHED);	
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_PUSHED);
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_PUSHED);
		App_SetButtonOption(RID_VIB_BTN_CHK, BN_PUSHED);
		Setup3.VibChk = BST_CHECKED;
		if(Setup3.VDeg == 10)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
			App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
		}
		if(Setup3.VDeg == 3)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
			App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);
		}
		if(Setup3.VCnt == 20)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
			App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
		}
		if(Setup3.VCnt == 2)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,140\r");
			App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
		}
	}
}

void UI_Vibration_Process(void)
{

}

void UI_Vibration_Init(void)
{
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_VIB_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VIB_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VIB_BTN_EXIT*2], ',', 2),
			138, 60, hParent, RID_VIB_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_VIB_BTN_DEGP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VIB_BTN_DEGP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VIB_BTN_DEGP*2], ',', 2),
			58, 46, hParent, RID_VIB_BTN_DEGP, 0);
	API_CreateWindow("", pBtnInfo[RID_VIB_BTN_DEGM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VIB_BTN_DEGM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VIB_BTN_DEGM*2], ',', 2),
			58, 46, hParent, RID_VIB_BTN_DEGM, 0);
	API_CreateWindow("", pBtnInfo[RID_VIB_BTN_CNTP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VIB_BTN_CNTP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VIB_BTN_CNTP*2], ',', 2),
			58, 46, hParent, RID_VIB_BTN_CNTP, 0);
	API_CreateWindow("", pBtnInfo[RID_VIB_BTN_CNTM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VIB_BTN_CNTM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VIB_BTN_CNTM*2], ',', 2),
			58, 46, hParent, RID_VIB_BTN_CNTM, 0);
	API_CreateWindow("", pBtnInfo[RID_VIB_BTN_CHK*2], BS_CHECKBOX, 
			420,4,
			46, 26, hParent, RID_VIB_BTN_CHK, 0);
	
	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 165,77,Setup3.VDeg,2);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 165,148,Setup3.VCnt,2);

	if (Setup3.VibChk == BST_CHECKED) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,140\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,140\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_PUSHED);
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_PUSHED);	
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_PUSHED);
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_PUSHED);
		if(Setup3.VDeg == 10)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
			App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
		}
		if(Setup3.VDeg == 3)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
			App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);
		}
		if(Setup3.VCnt == 20)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
			App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
		}
		if(Setup3.VCnt == 2)
		{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
			App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
		}
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ndchk.jpg,420,4\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,66\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,141\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,346,140\r");
		App_SetButtonOption(RID_VIB_BTN_DEGP, BN_DISABLE);
		App_SetButtonOption(RID_VIB_BTN_DEGM, BN_DISABLE);	
		App_SetButtonOption(RID_VIB_BTN_CNTP, BN_DISABLE);
		App_SetButtonOption(RID_VIB_BTN_CNTM, BN_DISABLE);
		
	}
	if(Setup3.ProChk == BST_CHECKED)
		App_SetButtonOption(RID_VIB_BTN_CHK, BN_DISABLE);
	else
		App_SetButtonOption(RID_VIB_BTN_CHK, BN_PUSHED);
}

LRESULT UI_Vibration_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Vibration_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//setup
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_PRO_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_VIB_BTN_DEGP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnDegUp();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Vibration_OnBnLongClickedBtnDegUp();
                break;  
			}
			return 0;
		case RID_VIB_BTN_DEGM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnDegDown();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Vibration_OnBnLongClickedBtnDegDown();
                break;  
			}
			return 0;
		case RID_VIB_BTN_CNTP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnCntUp();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Vibration_OnBnLongClickedBtnCntUp();
                break;  
			}
			return 0;
		case RID_VIB_BTN_CNTM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Vibration_OnBnClickedBtnCntDown();
				break;
			case BN_LONGPUSHED: //long push 
                UI_Vibration_OnBnLongClickedBtnCntDown();
				break;
			}
			return 0;
		case RID_VIB_BTN_CHK:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_Vibration_OnBnClickedBtnChk();
                break;
			}
			return 0; 
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam); 
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_Vibration_Create(void)
{	
	App_SetWndProcFnPtr(UI_Vibration_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i vibb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_VIB][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_VIBRATION;
}

///////////////////////////////////////////////////////////////////////////////
//setup4
void UI_HomePos_OnBnClickedBtnLeft(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	UI_AdSet_Create();
}

void UI_HomePos_OnBnClickedBtnExit(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else 
	//	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
		
}

void UI_HomePos_OnBnClickedBtnRight(void)
{
	UI_AngleMeaSens_Create();
}

void UI_HomePos_OnBnClickedBtnHPosUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.IPos < 90) Setup3.IPos++;
	if(Setup3.IPos == 90)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.IPos  > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.IPos,2);
}

void UI_HomePos_OnBnClickedBtnHPosDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.IPos > 0) Setup3.IPos --;
	if(Setup3.IPos == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.IPos < 90)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.IPos,2);
}

void UI_HomePos_OnBnLongClickedBtnHPosUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if (Setup3.IPos < 86) 
	{
		Setup3.IPos += 5;
	}
	else if(Setup3.IPos<= 86 || Setup3.IPos < 90)
		Setup3.IPos++;

	if(Setup3.IPos  == 90)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.IPos  > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,pos,113,Setup3.IPos,2);
}

void UI_HomePos_OnBnLongClickedBtnHPosDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if (Setup3.IPos > 5) 
	{
		Setup3.IPos -= 5;	
	}

	else if(Setup3.IPos >= 5  || Setup3.IPos > 0)
		Setup3.IPos --;

	if(Setup3.IPos == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.IPos < 90)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.IPos,2);
}	

void UI_HomePos_Init(void)
{	
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);


	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.IPos,2);
	
	if(Setup3.IPos == 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.IPos == 90)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
}

LRESULT UI_HomePos_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_HomePos_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//setup
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_HomePos_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_HomePos_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_PRO_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_HomePos_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_HomePos_OnBnClickedBtnHPosUp();
				break;
			case BN_LONGPUSHED: //long push 
                UI_HomePos_OnBnLongClickedBtnHPosUp();
                break;  
			}
			return 0;
		case RID_LP_BTN_PAUSEM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_HomePos_OnBnClickedBtnHPosDown();
				break;
			case BN_LONGPUSHED: //long push 
                UI_HomePos_OnBnLongClickedBtnHPosDown();
                break;  
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam); 
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_HomePos_Create(void)
{	
	App_SetWndProcFnPtr(UI_HomePos_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i senb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_HOME][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_HOME_POS;
}

///////////////////////////////////////////////////////////////////////////////
//setup4
void UI_Sens_OnBnClickedBtnLeft(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	
	UI_AdSet_Create();
}

void UI_Sens_OnBnClickedBtnExit(void)
{
	//if (loginInfo.type == LIT_USER) UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
	//else 
	//	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
}

void UI_Sens_OnBnClickedBtnRight(void)
{
	UI_HomePos_Create();
}

void UI_Sens_OnBnClickedBtnSSVUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.sensitivity < 4) Setup3.sensitivity++;
	//Over_Current = Motor_OverCurTbl[Setup3.sensitivity-1];
	MotorDrv_SetSensCurrent(Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);

	if(Setup3.sensitivity == 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.sensitivity > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.sensitivity,1);
}

void UI_Sens_OnBnClickedBtnSSVDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.sensitivity > 1) Setup3.sensitivity--;
	//Over_Current = Motor_OverCurTbl[Setup3.sensitivity-1];
	MotorDrv_SetSensCurrent(Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);

	if(Setup3.sensitivity == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.sensitivity < 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.sensitivity,1);
}

void UI_Sens_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);


	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.sensitivity,1);
	
	if(Setup3.sensitivity == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.sensitivity == 4)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,285,103\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
}

LRESULT UI_Sens_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Sens_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		//
		//setup
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Sens_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Sens_OnBnClickedBtnExit();
				break;
			}
			return 0;
		case RID_PRO_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Sens_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Sens_OnBnClickedBtnSSVUp();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_Sens_OnBnClickedBtnSSVDown();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam); 
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_Sens_Create(void)
{	
	App_SetWndProcFnPtr(UI_Sens_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i senb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_SENS][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_SENSE;
}

///////////////////////////////////////////////////////////////////////////////
void UI_PopupUSBCheck_AniProgress(uint32_t pos)
{
	switch (pos%10) {
	case 1:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lbrb.bmp,102,179\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,107,179\r");
		break;	
	case 2:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,136,179\r");
		break;
	case 3:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,165,179\r");
		break;
	case 4:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,195,179\r");
		break;
	case 5:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,225,179\r");
		break;
	case 6:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,255,179\r");
		break;
	case 7:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,285,179\r");
		break;
	case 8:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,315,179\r");
		break;
	case 9:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,343,179\r");
		break;
	case 0:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i fbar.bmp,99,176\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,107,179\r");
		break;
	}
}

void UI_PopupUSBCheck_Process(void)
{
	USBDisk_CheckDetect();
}

void UI_PopupUSBCheck_Timer(uint16_t nIDEvent)
{
	loading++;
	if (loading > 30 || USBDisk_GetReadyFlag()) {
		App_SetUIProcess(UI_ProcessNull);
		App_KillTimer(1);
		USBDisk_UnMount();
		USBDisk_UnLink();
		UI_PiUSB_Create();
		return;
	}

	if (loading)
		UI_PopupUSBCheck_AniProgress(loading);
}

void UI_PopupUSBCheck_Init(void)
{
	loading = 0;
	USBDisk_SetReadyFlag(0);
	if (!USBDisk_Link()) {
		UI_PiUSB_Create();
		return;
	}
	if (!USBDisk_Mount()) {
		USBDisk_UnLink();
		UI_PiUSB_Create();
		return;
	}
	UI_PopupUSBCheck_AniProgress(loading);
	App_SetTimer(1, 50);
	App_SetUIProcess(UI_PopupUSBCheck_Process);
}

LRESULT UI_PopupUSBCheck_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupUSBCheck_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;         
	case WM_TIMER:
		UI_PopupUSBCheck_Timer(wParam);
		break;
	default:
		break;
	}

	return 0;
}
//USB√¢ ª˝º∫
void UI_PopupUSBCheck_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupUSBCheck_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i susbb.png,0,0\r", WS_OVERLAPPEDWINDOW, 
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_USB_CHECK][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_USB_CHK;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PiUSB_OnBnClickedBtnPI1(void)
{	
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 1;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}


void UI_PiUSB_OnBnClickedBtnPI2(void)
{	
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 2;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}

void UI_PiUSB_OnBnClickedBtnPI3(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 3;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}


void UI_PiUSB_OnBnClickedBtnPI4(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 4;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}

void UI_PiUSB_OnBnClickedBtnPI5(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 5;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}


void UI_PiUSB_OnBnClickedBtnPI6(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 6;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}

void UI_PiUSB_OnBnClickedBtnPI7(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 7;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}


void UI_PiUSB_OnBnClickedBtnPI8(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 8;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}

void UI_PiUSB_OnBnClickedBtnPI9(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 9;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}


void UI_PiUSB_OnBnClickedBtnPI10(void)
{
	if (loginInfo.cnt < loginInfo.pwdLen) {
		loginInfo.pwdBuf[loginInfo.cnt] = 0;
		UI_PiLoad_DisplayID();
		loginInfo.cnt++;
	}
}

void UI_PiUSB_OnBnClickedBtnPI11(void)
{
	if (loginInfo.cnt > 0) loginInfo.cnt--;
	loginInfo.pwdBuf[loginInfo.cnt] = 0;
	UI_PiLoad_DisplayDel(loginInfo.cnt);
}

void UI_PiUSB_OnBnClickedSave(void)
{
	FILINFO fno;
	uint8_t tmpbuf[30];
	uint32_t temp;//, temp2;

	//if (loginInfo.type == LIT_USER){
		if (!loginInfo.cnt) return;
		temp = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
		if (!temp || temp > 99999999) return;
		//PInfoWnd2.id = temp;
	//}
	if (!EEPROMDisk_Link()) {
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
		return;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
		return;
	}

	ID2FileName(temp, tmpbuf);
	if (EEPROMDisk_stat((char *)tmpbuf, &fno) != 1) {
		temp = 0;
	}
	EEPROMDisk_UnMount();
	EEPROMDisk_UnLink();

	if (temp) UI_PopupSaving_Create();
}

void UI_PiUSB_OnBnClickedCancel(void)
{
	App_SetButtonOption(RID_USB_BTN_SAVE, BN_PUSHED);
	App_SetButtonOption(RID_USB_BTN_SAVEK, BN_PUSHED);
	App_SetButtonOption(RID_USB_BTN_SAVEC, BN_PUSHED);
	UI_Setup_Create();
}

void UI_PiUSB_Init(void)
{
	//clear user mark
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk2.bmp,6,3\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	// button
	//numeric pad
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM2*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM2*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM2, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM3*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM3*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM3, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM4*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM4*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM4, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM5*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM5*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM5, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM6*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM6*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM6, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM7*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM7*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM7, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM8*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM8*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM8, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM9*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM9*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM9, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM10*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM10*2], ',', 2),
			46, 53, hParent, RID_PLD_BTN_NUM10, 0);
	API_CreateWindow("", pBtnInfo[RID_PLD_BTN_NUM11*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_PLD_BTN_NUM11*2], ',', 2),
			93, 49, hParent, RID_PLD_BTN_NUM11, 0);	

	API_CreateWindow("", pBtnInfo[(RID_USB_BTN_SAVE+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_USB_BTN_SAVE+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_USB_BTN_SAVE+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_USB_BTN_SAVE+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_USB_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_USB_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_USB_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_USB_BTN_CANCEL+Setup2.language, 0);
	

	//UI_PINumber_Display();
	loginInfo.cnt = 0;
	loginInfo.pwdLen = 8;
	
	//if (!USBDisk_IsReady(3000)) { //ran in ui_usbcheck
	if (!USBDisk_GetReadyFlag()) {	
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsa.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVE, BN_DISABLE);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsak.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVEK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsac.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVEC, BN_DISABLE);
		}
		//return;
	}

	//if (!USBDisk_OpenWrite("hexar\\hx_data.txt")) {
	/*if (0) {//!USBDisk_OpenWrite("KR20P.HXR")) {
		if(Setup2.language == LT_ENG){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsa.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVE, BN_DISABLE);
		}
		else if(Setup2.language == LT_KOR){
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsak.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVEK, BN_DISABLE);
		}
		else{
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i usnsac.bmp,122,222\r");
			App_SetButtonOption(RID_USB_BTN_SAVEC, BN_DISABLE);
		}
		return;
	}*/

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
}

LRESULT UI_PiUSB_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiUSB_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_USB_BTN_SAVE:
		case RID_USB_BTN_SAVEK:
		case RID_USB_BTN_SAVEC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedSave();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_USB_BTN_CANCEL:
		case RID_USB_BTN_CANCELK:
		case RID_USB_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI1();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM2:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI2();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM3:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI3();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM4:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI4();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM5:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI5();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM6:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI6();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM7:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI7();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM8:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI8();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM9:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI9();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM10:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI10();
				break;
			}
			return 0;
		case RID_PLD_BTN_NUM11:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiUSB_OnBnClickedBtnPI11();
				break;
			}
			return 0;	
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB√¢ ª˝º∫
void UI_PiUSB_Create(void)
{
	App_SetWndProcFnPtr(UI_PiUSB_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i susbb.png,0,0\r", WS_OVERLAPPEDWINDOW, 
	//hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_USB_SAVE_REQ][Setup2.language], WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_USER_SEL][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_PI_USB;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupSaving_Timer(uint16_t nIDEvent)
{
	loading++;
	if (loading > 10) {
		App_KillTimer(TIMER_ID_3);
		UI_Setup_Create();
		return;
	}
	UI_AniProgress2(loading);
}

//USB ¿˙¿Â »≠∏È ±∏º∫
//uint32_t test_delay = 0;
void UI_PopupSaving_Process(void)
{
#ifdef USE_FUN_EEPROM_DISK
	//int _1st, _2nd, _3rd;
	uint8_t key;
	FILINFO fno;
	uint32_t temp, temp2, temp3;
    int i, j;
	float ftemp;
	uint8_t seed;
	uint8_t buf[30];
	USB_DATA_HEADER *udh;
	USB_DATA_FMT *udf;
	int per;
	uint32_t size;

#if 1

	if (!EEPROMDisk_Link()) {
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
		UI_Setup_Create();
		return;
	}
	if (!EEPROMDisk_Mount()) {
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
		//HAL_Delay(1000);
		UI_Setup_Create();
		return;
	}
	temp = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
	ID2FileName(temp, (uint8_t *)buf);
	if (EEPROMDisk_stat((char *)buf, &fno) != 1) {
		//not init
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		App_SetUIProcess(UI_ProcessNull);
		API_SetErrorCode(EC_EEP_NOFILE, EDT_DISP);
		UI_Setup_Create();
		return;
	}

	if (loading == 0) {
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		Option.copypos = 0;
		Option.usbcopypos = 0;
	}
	else {
		if (fno.fsize > Option.copypos) {
			size = fno.fsize - Option.copypos;
			if (size >= FLASH_PAGE_SIZE16) size = FLASH_PAGE_SIZE16;
		}
		else {
			//end
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			//HAL_Delay(1000);
			UI_Setup_Create();
			return;
		}
			
		if (!EEPROMDisk_OpenRead((char *)buf)) {
			//no file
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_OPEN, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		
		if (!EEPROMDisk_lseek(Option.copypos)) {
			EEPROMDisk_Close();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_SEEK, EDT_DISP);
			UI_Setup_Create();
			return;
		}

		if (!EEPROMDisk_Read((uint8_t *)CommonBuf, size)) {
			EEPROMDisk_Close();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_READ, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		EEPROMDisk_Close();
		EEPROMDisk_UnMount();
		EEPROMDisk_UnLink();
		//
		//copy to usb
		if (!USBDisk_Link()) {
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_LINK, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		if (!USBDisk_Mount()) {
			USBDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		//temp = PInfoWnd2.id;
		temp = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);
		j = 0;
		for (i = 0; i < 8; i++) {
			temp2 = Multiplier(10, 7-i);
			if (temp2 == 0) buf[i] = 0;
            else buf[i] = temp/temp2 + '0';
			temp %= temp2;
			j++;
		}
		buf[j++] = '.';
		buf[j++] = 'H'; //he
		buf[j++] = 'X'; //xa
		buf[j++] = 'R'; //r
		buf[j++] = 0;
		if (!USBDisk_OpenWrite((char *)buf)) {
			USBDisk_UnMount();
			USBDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_OPEN, EDT_DISP);
			UI_Setup_Create();
			return;
		}	
		if (!USBDisk_lseek(Option.copypos+Option.usbcopypos)) {
			USBDisk_Close();
			USBDisk_UnMount();
			USBDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_SEEK, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		//header
		if (loading == 1) {
			seed = 43;//(uint8_t)UI_Time.time.ms;
			ftemp = arm_sin_f32((float)seed) + arm_sin_f32((float)seed/2) + arm_sin_f32((float)seed/3);
			temp = (uint32_t)((3.0-ftemp) * 100);
			key = (BYTE)(temp%256);
			if (key == 0) key = seed/2;
			j = 0;
			//header
			udh = (USB_DATA_HEADER *)buf;
			udh->h = 'H';
			udh->x = 'X';
			udh->ver = 0x01;
			udh->key = seed;
			j = sizeof(USB_DATA_HEADER);
			//
			//exe data
			udf = (USB_DATA_FMT *)&buf[4];
			udf->k = 'K';
			udf->r = 'R';
			udf->size = (uint16_t)((fno.fsize) / sizeof(SAVE_EXERCISE_INFO_V2));
			udf->id = ConvertString2U32(loginInfo.pwdBuf, loginInfo.cnt);//0xab907856;//
			//data encrption
			for (i = 0; i < sizeof(USB_DATA_FMT)-1; i++) {
				//temp = buf[4+i]^key;
				//buf[4+i] = temp;
			}
			j += sizeof(USB_DATA_FMT);
			buf[j] = 0;
			buf[j+1] = 0;
			if (!USBDisk_Write((uint8_t *)buf, (uint32_t)j)) {
				USBDisk_Close();
				USBDisk_UnMount();
				USBDisk_UnLink();
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_USB_WRITE, EDT_DISP);
				UI_Setup_Create();
				return;
			}
			Option.usbcopypos = j;
		}

		//05,140deg disp error bug fix
		SAVE_EXERCISE_INFO_V2 *ptr;
		ptr = (SAVE_EXERCISE_INFO_V2 *)CommonBuf;
		#if 1
		#else
		for (i = 0; i < size/sizeof(SAVE_EXERCISE_INFO_V2); i++) {
			if (ptr[i].flg == 0) {
				if (ptr[i].spm.exangle >= 0 && ptr[i].spm.exangle < 5) ptr[i].spm.exangle += 30; 
				if (ptr[i].spm.flangle >= 140 && ptr[i].spm.flangle <= 145) ptr[i].spm.flangle += 70; 
			}
			else {
				for (j = 0; j < 3; j++) {
					if (ptr[i].smm.mi[j].exangle >= 0 && ptr[i].smm.mi[j].exangle < 5) ptr[i].smm.mi[j].exangle += 30; 
					if (ptr[i].smm.mi[j].flangle >= 140 && ptr[i].smm.mi[j].flangle <= 145) ptr[i].smm.mi[j].flangle += 70; 
				}
			}
		}
		#endif
		//data encrption
		//for (i = 0; i < size; i++) CommonBuf[i] ^= key;
		if (!USBDisk_Write((uint8_t *)CommonBuf, size)) {
			USBDisk_Close();
			USBDisk_UnMount();
			USBDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_WRITE, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		USBDisk_Close();
		USBDisk_UnMount();
		USBDisk_UnLink();
		Option.copypos += size;
	}
	loading++;

	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'b';
	buf[6] = 'm';
	buf[7] = 'p';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '2';
	buf[11] = '2';
	buf[12] = ',';
	buf[13] = '1';
	buf[14] = '4';
	buf[15] = '4';
	buf[16] = '\r';

	per = (Option.copypos*100)/fno.fsize;
	temp3 = per/100;
	temp = per%100;
	temp2 = temp/10;
	temp = temp%10;
	buf[3] = '0'+temp;
	TLCD_DrawText((char *)buf);
	buf[3] = '0'+temp2;
	buf[10] = '0';
	buf[11] = '8';
	if(temp2 > 0 || temp3 > 0)
		TLCD_DrawText((char *)buf);
	buf[3] = '0'+temp3;
	buf[9] = '1';
	buf[10] = '9';
	buf[11] = '4';
	if(temp3 > 0)
		TLCD_DrawText((char *)buf);
	UI_AniProgress2(per/10);
#endif	
#if 0
	if (loading == 0) {
		if (!EEPROMDisk_Link()) {
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_LINK, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		if (!EEPROMDisk_Mount()) {
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP);
			UI_Setup_Create();
			return;
		}
		if (!EEPROMDisk_FindFirst("0:/KR20P", &buf[9])) {
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			EEPROMDisk_CloseDir();
			App_SetUIProcess(UI_ProcessNull);
			//API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
			UI_Setup_Create();
		}
		else {
			j = 0;
			buf[j++] = EEPROMDISK_DRIVE;
			buf[j++] = ':';
			buf[j++] = '/';
			buf[j++] = 'K';
			buf[j++] = 'R';
			buf[j++] = '2';
			buf[j++] = '0';
			buf[j++] = 'P';
			buf[j++] = '/';
			if (!USBDisk_Link()) {
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				EEPROMDisk_CloseDir();
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_USB_LINK, EDT_DISP);
				UI_Setup_Create();
				return;
			}
			if (!USBDisk_Mount()) {
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				EEPROMDisk_CloseDir();
				USBDisk_UnLink();
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
				UI_Setup_Create();
				return;
			}
			if (!USBDisk_OpenWrite(buf)) {
				EEPROMDisk_UnMount();
				EEPROMDisk_UnLink();
				EEPROMDisk_CloseDir();
				USBDisk_UnMount();
				USBDisk_UnLink();
				App_SetUIProcess(UI_ProcessNull);
				API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
				UI_Setup_Create();
				return;
			}

			if (!EEPROMDisk_OpenRead(buf)) {
			}
			else {
				temp = EEPROMDisk_GetSize();
				if (temp < 255*1024) {
					while (temp > 0) {
						if (temp > 32*1024) {
							if (!EEPROMDisk_Read((uint8_t *)CommonBuf, 32*1024)) {
							}
							else {
								if (!USBDisk_Write((uint8_t *)CommonBuf, 32*1024)) {
									EEPROMDisk_UnMount();
									EEPROMDisk_UnLink();
									EEPROMDisk_CloseDir();
									USBDisk_UnMount();
									USBDisk_UnLink();
									USBDisk_Close();
									App_SetUIProcess(UI_ProcessNull);
									API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
									UI_Setup_Create();
								}
							}
							temp -= 32*1024;
						}
						else {
							if (!EEPROMDisk_Read((uint8_t *)CommonBuf, temp)) {
							}
							else {
								if (!USBDisk_Write((uint8_t *)CommonBuf, temp)) {
									EEPROMDisk_UnMount();
									EEPROMDisk_UnLink();
									EEPROMDisk_CloseDir();
									USBDisk_UnMount();
									USBDisk_UnLink();
									USBDisk_Close();
									App_SetUIProcess(UI_ProcessNull);
									API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
									UI_Setup_Create();
								}
							}
							temp = 0;
						}
					}
				}
			}
			EEPROMDisk_Close();
		}
		loading++;
	}
	else {
		if (!EEPROMDisk_FindNext(&buf[9])) {
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			EEPROMDisk_CloseDir();
			USBDisk_UnMount();
			USBDisk_UnLink();
			USBDisk_Close();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
			UI_Setup_Create();
		}
		else {
			j = 0;
			buf[j++] = EEPROMDISK_DRIVE;
			buf[j++] = ':';
			buf[j++] = '/';
			buf[j++] = 'K';
			buf[j++] = 'R';
			buf[j++] = '2';
			buf[j++] = '0';
			buf[j++] = 'P';
			buf[j++] = '/';
			if (!EEPROMDisk_OpenRead(buf)) {
			}
			else {
				temp = EEPROMDisk_GetSize();
				if (temp < 255*1024) {
					while (temp > 0) {
						if (temp > 32*1024) {
							if (!EEPROMDisk_Read((uint8_t *)CommonBuf, 32*1024)) {
							}
							else {
								if (!USBDisk_Write((uint8_t *)CommonBuf, 32*1024)) {
									EEPROMDisk_UnMount();
									EEPROMDisk_UnLink();
									EEPROMDisk_CloseDir();
									USBDisk_UnMount();
									USBDisk_UnLink();
									USBDisk_Close();
									App_SetUIProcess(UI_ProcessNull);
									API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
									UI_Setup_Create();
								}
							}
							temp -= 32*1024;
						}
						else {
							if (!EEPROMDisk_Read((uint8_t *)CommonBuf, temp)) {
							}
							else {
								if (!USBDisk_Write((uint8_t *)CommonBuf, temp)) {
									EEPROMDisk_UnMount();
									EEPROMDisk_UnLink();
									EEPROMDisk_CloseDir();
									USBDisk_UnMount();
									USBDisk_UnLink();
									USBDisk_Close();
									App_SetUIProcess(UI_ProcessNull);
									API_SetErrorCode(EC_USB_MOUNT, EDT_DISP);
									UI_Setup_Create();
								}
							}
							temp = 0;
						}
					}
				}
			}
			EEPROMDisk_Close();
		}
		loading++;
	}

	per = (loading);///(PATIENT_NUM);
	//per += 1;
	_3rd= per/100;
	temp = per%100;
	_2nd = temp/10;
	_1st = temp%10;
	buf[3] = '0'+_1st;
	TLCD_DrawText(buf);
	buf[3] = '0'+_2nd;
	buf[10] = '1';
	buf[11] = '2';
	if(_2nd > 0 || _3rd > 0)
		TLCD_DrawText(buf);
	buf[3] = '0'+_3rd;
	buf[9] = '1';
	buf[10] = '9';
	buf[11] = '8';
	if(_3rd > 0)
		TLCD_DrawText(buf);
	UI_AniProgress2(per/10);
#endif

#else
	int _1st, _2nd, _3rd;
	int per;
	int temp;
	char buf[30];
	uint8_t i, j;

	if (UI_Time.tmp_ms == 0) {
		UI_Time.tmp_ms = 30;
	}
	else {
		return;
	}
	
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'b';
	buf[6] = 'm';
	buf[7] = 'p';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '2';
	buf[11] = '6';
	buf[12] = ',';
	buf[13] = '1';
	buf[14] = '4';
	buf[15] = '2';
	buf[16] = '\r';
	
	if (loading >= PATIENT_NUM+0) {
		if (loading == PATIENT_NUM+1) {
			buf[3] = '0';
			TLCD_DrawText(buf);
			buf[3] = '0';
			buf[10] = '1';
			buf[11] = '2';
			TLCD_DrawText(buf);
			buf[3] = '1';
			buf[9] = '1';
			buf[10] = '9';
			buf[11] = '8';
			TLCD_DrawText(buf);
			UI_Time.tmp_ms = 1000;//3000;
			USBDisk_Close();
			USBDisk_UnMount();
			USBDisk_UnLink();
		}
		else if (loading > PATIENT_NUM+2) {
			App_SetUIProcess(UI_ProcessNull); 
			UI_Setup_Create();
		}
		loading++;
		return;
	}
	
	//if (!UI_LoadPIFromEEPROM(loading, (uint8_t *)CommonBuf)) {
	//	API_SetErrorCode(EC_EEP_READ, EDT_DISP);
	//	App_SetUIProcess(UI_ProcessNull); 
	//	USBDisk_Close();
	//	return;
	//}
	//if (!UI_Exercise_LoadAllData(loading, (uint8_t *)&CommonBuf[sizeof(PI_PARAM)])) {
	if (!UI_Exercise_LoadAllData(loading, (uint8_t *)CommonBuf)) {
		API_SetErrorCode(EC_EEP_READ, EDT_DISP);
		App_SetUIProcess(UI_ProcessNull); 
		USBDisk_Close();
		USBDisk_UnMount();
		USBDisk_UnLink();
		return;
	}
	//CommonBuf[sizeof(PI_PARAM)-1] = '0'+ loading%10;
	if (!USBDisk_Write((uint8_t *)CommonBuf, sizeof(PI_PARAM)+sizeof(EXERCISE_INFO)*EXERCISE_INFO_NUM)) {
		API_SetErrorCode(EC_USB_WRITE, EDT_DISP);
		App_SetUIProcess(UI_ProcessNull); 
		USBDisk_Close();
		USBDisk_UnMount();
		USBDisk_UnLink();
		return;
	}

	loading++;			  
	per = (loading*100)/(PATIENT_NUM);
	//per += 1;
	_3rd= per/100;
	temp = per%100;
	_2nd = temp/10;
	_1st = temp%10;
	buf[3] = '0'+_1st;
	TLCD_DrawText(buf);
	buf[3] = '0'+_2nd;
	buf[10] = '1';
	buf[11] = '2';
	if(_2nd > 0 || _3rd > 0)
		TLCD_DrawText(buf);
	buf[3] = '0'+_3rd;
	buf[9] = '1';
	buf[10] = '9';
	buf[11] = '8';
	if(_3rd > 0)
		TLCD_DrawText(buf);
	UI_AniProgress2(per/10);
#endif
}

void UI_PopupSaving_Init(void)
{
	//int i;
	//unsigned long j, k;
	/*APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,224,157\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,238,157\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,254,157\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i L0.png,226,158\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i Lpc.png,244,158\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ldb.bmp,99,190\r"); */
	loading = 0;
	//App_SetTimer(TIMER_ID_3, 400000);
	/*for(i=0;i<10;i++){
		UI_AniProgress2(i);
		//OS_Process();
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,166,159,(i+1) *10);
		App_Process();
		for(j=0;j<0xfffffffe;j++) k = 0;*/
		//OS_Process();
	App_SetUIProcess(UI_PopupSaving_Process); 

}

LRESULT UI_PopupSaving_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupSaving_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		UI_PopupSaving_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PopupSaving_Create(void)
{	

	App_SetWndProcFnPtr(UI_PopupSaving_WndProc);
	//hParent = API_CreateWindow("main", (LPCSTR)"i saving.png,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_USB_SAVING][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_SAVING;
}


void UI_VersionInfo_OnBnClickedVsUpdate(void)
{
	
}

void UI_VersionInfo_OnBnClickedVsDel(void)//Reset(void)
{
	UI_PopupDelReq_Create();
}

//setup parameter default
void UI_VersionInfo_OnBnClickedVsDef(void)
{
	UI_PopupResetReq_Create();
}

void UI_VersionInfo_OnBnClickedVsBack(void)
{
	UI_Setup_Create();
}

void UI_VersionInfo_SWVersion(uint32_t FW)
{
	//char buf[18];
	char *buf;
	char i;//, j;

	buf = CommonBuf;
	i = 0;
	
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '0';
	buf[5] = '.';
	buf[6] = 'p';
	buf[7] = 'n';
	buf[8] = 'g';
	buf[9] = ',';
	buf[10] = '1';
	buf[11] = '9';
	buf[12] = '5';
	buf[13] = ',';
	buf[14] = '1';
	buf[15] = '0';
	buf[16] = '4';
	buf[17] = '\r';
	buf[18] = 0;
	//i = 18;
	FW = FW & 0x00ff;
	buf[4] = FW_VER[0];
	//1st 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[0]); 
	
	for (i = 0; i < 19; i++) CommonBuf[20+i] = buf[i];
	buf = &CommonBuf[20];
	buf[4] = FW_VER[1];
	buf[10] = '2';	//x coord
	buf[11] = '0';
	buf[12] = '9';
	//i = 20;
	//2st 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[20]); 
	
	for (i = 0; i < 19; i++) CommonBuf[40+i] = buf[i];
	buf = &CommonBuf[40];
	buf[4] = FW_VER[2];
	buf[10] = '2';	//x coord
	buf[11] = '2';
	buf[12] = '3';
	//3st 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[40]); 
	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,202,104\r"); //dot
	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,216,104\r"); 
}

void UI_VersionInfo_HWVersion(uint16_t HW)
{
	//char buf[18];
	char *buf;
	char i;//, j;

	buf = &CommonBuf[60];
	i = 0;
	
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '0';
	buf[5] = '.';
	buf[6] = 'p';
	buf[7] = 'n';
	buf[8] = 'g';
	buf[9] = ',';
	buf[10] = '3';
	buf[11] = '2';
	buf[12] = '0';
	buf[13] = ',';
	buf[14] = '1';
	buf[15] = '0';
	buf[16] = '4';
	buf[17] = '\r';
	buf[18] = 0;

	HW = HW & 0x00ff;
	buf[4] = HW_VER[0];	
	for (i = 0; i < 19; i++) CommonBuf[60+i] = buf[i];
	//1st 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[60]); 

	for (i = 0; i < 19; i++) CommonBuf[80+i] = buf[i];
	buf = &CommonBuf[80];
	buf[4] = HW_VER[1];
	buf[10] = '3';	//x coord
	buf[11] = '3';
	buf[12] = '4';
	//2st 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[80]); 

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,327,104\r"); 
}

void UI_VersionInfo_SerialNumber(void)
{
	uint32_t temp, remain;
	uint8_t num_buf[10];
	int i, j, k;
	//char buf[18];
	char *buf;
	uint16_t pos, pos_rem;
	
	buf = &CommonBuf[100];
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '0';
	buf[5] = '.';
	buf[6] = 'p';
	buf[7] = 'n';
	buf[8] = 'g';
	buf[9] = ',';
	buf[10] = '1';
	buf[11] = '9';
	buf[12] = '5';
	buf[13] = ',';
	buf[14] = '1';
	buf[15] = '2';
	buf[16] = '5';
	buf[17] = '\r';
	buf[18] = 0;

	pos = 195;
	temp = SysInfo.sn;
	k = 0;
	for (i = 0; i < 10; i++) {
		remain = Multiplier(10, 9-i);
		if (remain == 0) num_buf[i] = 0;
        else num_buf[i] = temp/remain;
		buf[4] = num_buf[i] + '0';
		buf[10] = pos/100 + '0';
		pos_rem = pos%100;
		buf[11] = pos_rem/10 + '0';
		pos_rem = pos_rem%10;
		buf[12] = pos_rem + '0';
		//1st 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[100+k*20]); 
		pos += 10;

		//display
		if (remain == 0) temp = 0;
        else temp = temp%remain;
		k++;
		for (j = 0; j < 19; j++) CommonBuf[100+k*20+j] = buf[j];
		buf = &CommonBuf[100+k*20];
	}
}

void UI_VersionInfo_CompileDateTime(void)
{
	uint8_t num_buf[14];
	int i, j, k;
	char *buf;
	uint16_t pos, pos_rem;
	
	buf = &CommonBuf[200];
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	buf[3] = '0';
	buf[4] = '0';
	buf[5] = '.';
	buf[6] = 'p';
	buf[7] = 'n';
	buf[8] = 'g';
	buf[9] = ',';
	buf[10] = '2';
	buf[11] = '3';
	buf[12] = '5';
	buf[13] = ',';
	buf[14] = '1';
	buf[15] = '0';
	buf[16] = '4';
	buf[17] = '\r';
	buf[18] = 0;

	k = 0;
	num_buf[k++] = '0';
	switch (CompileDate[0]) { //month 2unit
		case 'J' : //1,6,7
			if (CompileDate[1] == 'a') num_buf[k++] = '1';
			else if (CompileDate[1] == 'u') {
				if (CompileDate[2] == 'n') num_buf[k++] = '6';
				else num_buf[k++] = '7';
			}
			else num_buf[k++] = '0';
			break;
		case 'F' : //2
			num_buf[k++] = '2';
			break;
		case 'M' : //3,5
			if (CompileDate[2] == 'r') num_buf[k++] = '3';
			else num_buf[k++] = '5';
			break;
		case 'A' : //4,8
			if (CompileDate[1] == 'p') num_buf[k++] = '4';
			else num_buf[k++] = '8';
			break;
		case 'S' : //9
			num_buf[k++] = '9';
			break;
		case 'O' : //10
			num_buf[0] = '1';
			num_buf[k++] = '0';
			break;
		case 'N' : //11
			num_buf[0] = '1';
			num_buf[k++] = '1';
			break;
		case 'D' : //12
			num_buf[0] = '1';
			num_buf[k++] = '2';
			break;
	}
	num_buf[k++] = '.';
	if (CompileDate[4] == ' ') { //day
		num_buf[k++] = CompileDate[5];
	}
	else {
		num_buf[k++] = CompileDate[4];
		num_buf[k++] = CompileDate[5];
	}
	num_buf[k++] = '.';
	num_buf[k++] = CompileDate[7];
	num_buf[k++] = CompileDate[8];
	num_buf[k++] = CompileDate[9];
	num_buf[k++] = CompileDate[10];
	num_buf[k++] = 0;

	pos = 253;
	k = 0;
	for (i = 0; i < 11; i++) { //year
		if (num_buf[i] == 0) break;
		if (num_buf[i] == '.') {
			if (i == 2) APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,273,104\r"); 
			else if (i == 4) APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,293,104\r"); 
			else APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,303,104\r"); 
		}
		else {	
			buf[4] = num_buf[i];
			buf[10] = pos/100 + '0';
			pos_rem = pos%100;
			buf[11] = pos_rem/10 + '0';
			pos_rem = pos_rem%10;
			buf[12] = pos_rem + '0';
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)buf); 
		}
		pos += 10;

		//display
		k++;
		for (j = 0; j < 19; j++) CommonBuf[200+k*20+j] = buf[j];
		buf = &CommonBuf[200+k*20];
	}
}

void UI_VersionInfo_Init(void)
{
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	// button
	/*API_CreateWindow("", pBtnInfo[RID_VI_BTN_UPDATE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VI_BTN_UPDATE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VI_BTN_UPDATE*2], ',', 2),
			113, 46, hParent, RID_VI_BTN_UPDATE, 0);*/
	API_CreateWindow("", pBtnInfo[RID_VI_BTN_BACK*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VI_BTN_BACK*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VI_BTN_BACK*2], ',', 2),
			46,44, hParent, RID_VI_BTN_BACK, 0);
	/*API_CreateWindow("", pBtnInfo[RID_VI_BTN_RESET*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_VI_BTN_RESET*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_VI_BTN_RESET*2], ',', 2),
			113, 46, hParent, RID_VI_BTN_RESET, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_VI_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_VI_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_VI_BTN_RESET+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_VI_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_VI_BTN_DEF+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_VI_BTN_DEF+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_VI_BTN_DEF+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_VI_BTN_DEF+Setup2.language, 0);
	UI_VersionInfo_SWVersion(0);
	//UI_VersionInfo_HWVersion(0);
   // UI_VersionInfo_SerialNumber();
	UI_VersionInfo_CompileDateTime();
	/*	
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i 001.png,230,104\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,236,104\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i 000.png,242,104\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i 001.png,320,104\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i dt.png,326,104\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i 001.png,332,104\r");
	*/
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	
}

LRESULT UI_VersionInfo_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_VersionInfo_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		/* case RID_VI_BTN_UPDATE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_VersionInfo_OnBnClickedVsUpdate();
				break;
			}
			break;*/
		case RID_VI_BTN_BACK:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_VersionInfo_OnBnClickedVsBack();
				break;
			}
			break;
		case RID_VI_BTN_RESET: //delete
		case RID_VI_BTN_RESETK:
		case RID_VI_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_VersionInfo_OnBnClickedVsDel();//Reset();
				break;
			}
			break;
		case RID_VI_BTN_DEF: //default
		case RID_VI_BTN_DEFK:
		case RID_VI_BTN_DEFC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_VersionInfo_OnBnClickedVsDef();
				break;
			}
			break;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}

//»Ø¿⁄πˆ¿¸¡§∫∏√¢ ª˝º∫
void UI_VersionInfo_Create(void)
{
	App_SetWndProcFnPtr(UI_VersionInfo_WndProc);

	//hParent = API_CreateWindow("main", (LPCSTR)"i syinb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_SETUP_SYSINFO][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_VERSION;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupResetReq_OnBnClickedOk(void)
{
	uint8_t temp;
	
	//UI_PopupDelData_Create();
	temp = Setup2.language; //pjg++191223
	UI_InitSetupVariable();
	Setup2.language = temp;
	UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_VersionInfo_Create();
}

void UI_PopupResetReq_OnBnClickedCancel(void)
{
	//UI_Setup_Create();
	UI_VersionInfo_Create();
}

void UI_PopupResetReq_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_RST_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_RST_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PopupResetReq_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupResetReq_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_RST_BTN_RESET:
		case RID_RST_BTN_RESETK:
		case RID_RST_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupResetReq_OnBnClickedOk();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_RST_BTN_CANCEL:
		case RID_RST_BTN_CANCELK:
		case RID_RST_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupResetReq_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}

void UI_PopupResetReq_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupResetReq_WndProc);

	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_RESET_REQ][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_RESET;
}

///////////////////////////////////////////////////////////////////////////////
void UI_PopupDeleting_Process(void)
{
	uint8_t *buf;//[30], *pbuf;
	uint8_t i, j;
	int per;
	uint32_t temp, temp2, temp3;

	if (UI_Time.tmp_ms == 0) {
		UI_Time.tmp_ms = 30;
	}
	else {
		return;
	}
	
	buf = (uint8_t *)CommonBuf;
	
	if (loading == 0) {
		if (!EEPROMDisk_Link()) {
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_LINK, EDT_DISP_HALT);
			UI_VersionInfo_Create();
			return;
		}
		if (!EEPROMDisk_Mount()) {
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			API_SetErrorCode(EC_EEP_MOUNT, EDT_DISP_HALT);
			UI_VersionInfo_Create();
			return;
		}
		if (!EEPROMDisk_FindFirst("0:/KR20P", (char *)"*.HKE", (char *)&buf[9])) {
			EEPROMDisk_CloseDir();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			UI_VersionInfo_Create();
			return;
		}
	}
	else {
		if (!EEPROMDisk_FindNext((char *)"*.HKE", (char *)&buf[9])) {
			EEPROMDisk_CloseDir();
			EEPROMDisk_UnMount();
			EEPROMDisk_UnLink();
			App_SetUIProcess(UI_ProcessNull);
			UI_VersionInfo_Create();
			return;
		}
	}

	j = 0;
	buf[j++] = EEPROMDISK_DRIVE;
	buf[j++] = ':';
	buf[j++] = '/';
	buf[j++] = 'K';
	buf[j++] = 'R';
	buf[j++] = '2';
	buf[j++] = '0';
	buf[j++] = 'P';
	buf[j++] = '/';
	if (!EEPROMDisk_Delete((char *)buf)) {
		API_SetErrorCode(EC_EEP_DEL, EDT_DISP);
	}
	loading++;

	//
	//display progress
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'b';
	buf[6] = 'm';
	buf[7] = 'p';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '2';
	buf[11] = '2';
	buf[12] = ',';
	buf[13] = '1';
	buf[14] = '4';
	buf[15] = '4';
	buf[16] = '\r';
	buf[17] = 0;

	per = loading%100;
	temp3 = per/100;
	temp = per%100;
	temp2 = temp/10;
	temp = temp%10;
	buf[3] = '0'+temp;
	//TLCD_DrawText((char *)buf);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[0]); 

	for (i = 0; i < 18; i++) CommonBuf[20+i] = buf[i];
	buf = (uint8_t *)&CommonBuf[20];
	buf[3] = '0'+temp2;
	buf[10] = '0';
	buf[11] = '8';
	if(temp2 > 0 || temp3 > 0) {
		//TLCD_DrawText((char *)buf);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[20]); 
	}

	for (i = 0; i < 18; i++) CommonBuf[40+i] = buf[i];
	buf = (uint8_t *)&CommonBuf[40];
	buf[3] = '0'+temp3;
	buf[9] = '1';
	buf[10] = '9';
	buf[11] = '4';
	if(temp3 > 0) {
		//TLCD_DrawText((char *)buf);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)&CommonBuf[40]); 
	}
	UI_AniProgress2(per%10);
}

void UI_PopupDeleting_Init(void)
{
	loading = 0;
	App_SetUIProcess(UI_PopupDeleting_Process); 
}

LRESULT UI_PopupDeleting_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupDeleting_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//User data delete
void UI_PopupDeleting_Create(void)
{	
	App_SetWndProcFnPtr(UI_PopupDeleting_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_RESETING][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DELETING;
}

//////////////////////////////////////////////////////////////////////////////
void UI_PopupDelReq_OnBnClickedOk(void)
{
	UI_PopupDeleting_Create();
}

void UI_PopupDelReq_OnBnClickedCancel(void)
{
	//UI_Setup_Create();
	UI_VersionInfo_Create();
}

void UI_PopupDelReq_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RST_BTN_RESET+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_RST_BTN_RESET+Setup2.language, 0);
	API_CreateWindow("", pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RST_BTN_CANCEL+Setup2.language)*2], ',', 2),
			113, 46, hParent, RID_RST_BTN_CANCEL+Setup2.language, 0);
}

LRESULT UI_PopupDelReq_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupDelReq_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_RST_BTN_RESET:
		case RID_RST_BTN_RESETK:
		case RID_RST_BTN_RESETC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDelReq_OnBnClickedOk();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_RST_BTN_CANCEL:
		case RID_RST_BTN_CANCELK:
		case RID_RST_BTN_CANCELC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupDelReq_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}

void UI_PopupDelReq_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupDelReq_WndProc);

	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_DEL_DATA][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DEL;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
void UI_PopupDelData_Process(void)
{
#ifdef PI_LOAD_V1
	int _1st, _2nd, _3rd;
	int per;
	int temp;
	char buf[18];

	if (UI_Time.tmp_ms == 0) {
		UI_Time.tmp_ms = 30;
	}
	else {
		return;
	}
	
	buf[0] = 'i';
	buf[1] = ' ';
	buf[2] = '0';
	//buf[3] = '0';
	buf[4] = '.';
	buf[5] = 'b';
	buf[6] = 'm';
	buf[7] = 'p';
	buf[8] = ',';
	buf[9] = '2';
	buf[10] = '2';
	buf[11] = '6';
	buf[12] = ',';
	buf[13] = '1';
	buf[14] = '4';
	buf[15] = '2';
	buf[16] = '\r';
	
	if (loading > PATIENT_NUM+0) {
		if (loading == PATIENT_NUM+1) {
			buf[3] = '0';
			TLCD_DrawText(buf);
			buf[3] = '0';
			buf[10] = '1';
			buf[11] = '2';
			TLCD_DrawText(buf);
			buf[3] = '1';
			buf[9] = '1';
			buf[10] = '9';
			buf[11] = '8';
			TLCD_DrawText(buf);
			UI_Time.tmp_ms = 1000;//3000;
		}
		else if (loading > PATIENT_NUM+2) {
			App_SetUIProcess(UI_ProcessNull); 
			UI_Setup_Create();
		}
		loading++;
		return;
	}

	if (!UI_DeletePIFromEEPROM(loading)) {
		UI_SavePIToEEPROM(loading, (uint8_t *)CommonBuf);
		return;
	}
	
	per = (loading*100)/(PATIENT_NUM);
	//per += 1;
	_3rd= per/100;
	temp = per%100;
	_2nd = temp/10;
	_1st = temp%10;
	buf[3] = '0'+_1st;
	TLCD_DrawText(buf);
	buf[3] = '0'+_2nd;
	buf[10] = '1';
	buf[11] = '2';
	if(_2nd > 0 || _3rd > 0)
		TLCD_DrawText(buf);
	buf[3] = '0'+_3rd;
	buf[9] = '1';
	buf[10] = '9';
	buf[11] = '8';
	if(_3rd > 0)
		TLCD_DrawText(buf);
	UI_AniProgress2(per/10);
	loading++;

#else
	UI_InitSetupVariable();
	UI_VersionInfo_Create();
#endif
}

void UI_PopupDelData_Init(void)
{
	loading = 0;
	App_SetUIProcess(UI_PopupDelData_Process); 
}

LRESULT UI_PopupDelData_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupDelData_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		//UI_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_PopupDelData_Create(void)
{	
	App_SetWndProcFnPtr(UI_PopupDelData_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_DEL_DATA][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_DEL_DATA;
}
#endif

//////////////////////////////////////////////////////////////////////////////
/*void UI_PiReset_OnBnClickedAll(void)
{
	int i;
	i = PInfoWnd2.id;

	for(i=0;i<120;i++)
		{
			UI_DeletePIFromEEPROM(i);
			UI_SavePIToEEPROM(i, (uint8_t *)CommonBuf);
		}
	UI_Deleting_Create();
}

void UI_PiReset_OnBnClickedDelete(void)
{
	UI_DeletePIFromEEPROM(PInfoWnd2.id);
	UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	UI_Deleting_Create();
}

void UI_PiReset_OnBnClickedCancel(void)
{
	UI_Setup_Create();
}

void UI_PiReset_Init(void)
{
	// button
	API_CreateWindow("", pBtnInfo[RID_RESET_BTN_ALL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RESET_BTN_ALL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RESET_BTN_ALL*2], ',', 2),
			113, 46, hParent, RID_RESET_BTN_ALL, 0);
	API_CreateWindow("", pBtnInfo[RID_RESET_BTN_DELETE*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RESET_BTN_DELETE*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RESET_BTN_DELETE*2], ',', 2),
			113, 46, hParent, RID_RESET_BTN_DELETE, 0);
	API_CreateWindow("", pBtnInfo[RID_RESET_BTN_CANCEL*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RESET_BTN_CANCEL*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RESET_BTN_CANCEL*2], ',', 2),
			113, 46, hParent, RID_RESET_BTN_CANCEL, 0);
	
	
}

LRESULT UI_PiReset_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PiReset_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_RESET_BTN_ALL:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset_OnBnClickedAll();
				break;                                       
			default:
				break;
			}
			return 0;	
		case RID_RESET_BTN_DELETE:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset_OnBnClickedDelete();
				break;                                       
			default:
				break;
			}
			return 0;
		case RID_RESET_BTN_CANCEL:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PiReset_OnBnClickedCancel();
				break;                                          
			default:
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}
//USB√¢ ª˝º∫
void UI_PiReset_Create(void)
{
	App_SetWndProcFnPtr(UI_PiReset_WndProc);

	hParent = API_CreateWindow("main", (LPCSTR)"i srset.jpg,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
}

//USB ¿˙¿Â »≠∏È ±∏º∫
void UI_Deleting_Init(void)
{
	//int i;
	//unsigned long j, k;
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,224,157\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,238,157\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i bnk.bmp,254,157\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i L0.png,226,158\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i Lpc.png,244,158\r"); 
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i ldb.bmp,99,190\r"); 
	
	loading = 0;
	App_SetTimer(TIMER_ID_4, 400000);
	for(i=0;i<10;i++){
		UI_AniProgress2(i);
		//OS_Process();
		UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,166,159,(i+1) *10);
		App_Process();
		for(j=0;j<0xfffffffe;j++) k = 0;
		//OS_Process();
}

LRESULT UI_Deleting_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_Deleting_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_TIMER:
		UI_Timer(wParam);
		break;
	case WM_SYSCOMMAND:
		if (wParam) {
		}
		break;					
	default:
		break;
	}

	return 0;
}
//USB ¿˙¿Â »≠∏È ª˝º∫
void UI_Deleting_Create(void)
{	
	App_SetWndProcFnPtr(UI_Deleting_WndProc);
	
	hParent = API_CreateWindow("main", (LPCSTR)"i dting.jpg,0,0\r", WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.num = UI_WND_MODE_SAVING;
}
*/

///////////////////////////////////////////////////////////////////////////////
void UI_PopupAngleMeaReport_OnBnClickedOk(void)
{
	//save mea data
	if (loginInfo.type == LIT_USER) 
		UI_SaveExerciseData(1);
	UI_Home_Create();
}

void UI_PopupAngleMeaReport_Init(void)
{
	short ex, fl;//, temp;
	int i;
	
	API_CreateWindow("", pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_RC_BTN_OK+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_RC_BTN_OK+Setup2.language, 0);
	
	//ex
	ex = 0;
	fl = 0;
	for (i = 0; i < ANG_MEA_TOTAL_COUNT; i++) {
		ex += (SaveExeInfoV2.smm.mi[i].exangle-SAVE_OFFSET_VALUE);
		fl += (SaveExeInfoV2.smm.mi[i].flangle-SAVE_OFFSET_VALUE);
	}
	ex /= ANG_MEA_TOTAL_COUNT;
	fl /= ANG_MEA_TOTAL_COUNT;
	//ex = 20;
	//fl = 30;
	if (ex > 99) UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 123, 135, ex);
	else if (ex > 9) UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 123+DISP_NUM_9P_10_POS, 135, ex);
	else UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 123+DISP_NUM_9P_1_POS, 135, ex);
	
	if (fl > 99) UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 273, 135, fl);
	else if (fl > 9) UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 273+DISP_NUM_9P_10_POS, 135, fl);
	else UI_DisplayDecimal(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 273+DISP_NUM_9P_1_POS, 135, fl);
}

LRESULT UI_PopupAngleMeaReport_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupAngleMeaReport_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_RC_BTN_OK:
		case RID_RC_BTN_OKK:
		case RID_RC_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupAngleMeaReport_OnBnClickedOk();
				break;                                          
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	default:
		break;
	}

	return 0;
}

void UI_PopupAngleMeaReport_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupAngleMeaReport_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i angb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_MEA_REPORT][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_User_Process);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_ANGLE_MEA_REPORT;
}

///////////////////////////////////////////////////////////////////////////////
void UI_PopupAngleMeaComplete_OnBnClickedOk(void)
{
	AngleWnd.exAngle = ANG_CHK_ANGLE_MIN;//AngChk.ext;
	AngleWnd.flAngle = ANG_CHK_ANGLE_MAX;//AngChk.flex;
	AngChk.runOne = 0;
	//RunWnd.complete = 0;
	if (SaveExeInfoV2.flg == MST_MEASURE) { //measure
		if (SysInfo.meaCnt >= ANG_MEA_TOTAL_COUNT) {
			UI_PopupAngleMeaReport_Create();
		}
		else {
			UI_AngleMeasure_Create();
		}
	}
	else {
		UI_AngleMeasure_Create();//UI_SpeedTime_Create();
	}
}

void UI_PopupAngleMeaComplete_OnBnClickedNo(void)
{
	if (SaveExeInfoV2.flg == MST_MEASURE) {
		UI_PopupAngleMeaReport_Create();	
	}
	else {
		UI_AngleMeasure_Create();
	}
}

void UI_PopupAngleMeaComplete_OnBnClickedRetry(void)
{
	//RunWnd.complete = 0;
	if (SysInfo.meaCnt) SysInfo.meaCnt--;
	AngleWnd.exAngle = ANG_CHK_ANGLE_MIN;//AngChk.ext;
	AngleWnd.flAngle = ANG_CHK_ANGLE_MAX;//AngChk.flex;
	AngChk.runOne = 0;
	UI_AngleMeasure_Create();
}

void UI_PopupAngleMeaComplete_Init(void)
{
	API_CreateWindow("", pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], ',', 2),
			112, 45, hParent, RID_MC_BTN_OK+Setup2.language, 0);
	//if (SaveExeInfoV2.flg == MST_MEASURE) {
		//API_CreateWindow("", pBtnInfo[RID_RC_BTN_NO*2], BS_PUSHBUTTON, 
		//		GetNumFromString(pBtnInfo[RID_RC_BTN_NO*2], ',', 1),
		//		GetNumFromString(pBtnInfo[RID_RC_BTN_NO*2], ',', 2),
		//		124, 51, hParent, RID_RC_BTN_NO, 0);
	//}
	API_CreateWindow("", pBtnInfo[(RID_MC_BTN_RETRY+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_MC_BTN_RETRY+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_MC_BTN_RETRY+Setup2.language)*2], ',', 2),
			112, 45, hParent, RID_MC_BTN_RETRY+Setup2.language, 0);
}

LRESULT UI_PopupAngleMeaComplete_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupAngleMeaComplete_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_MC_BTN_OK:
		case RID_MC_BTN_OKK:
		case RID_MC_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupAngleMeaComplete_OnBnClickedOk();
				break;                                       
			}
			return 0;
		case RID_MC_BTN_RETRY:
		case RID_MC_BTN_RETRYK:
		case RID_MC_BTN_RETRYC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupAngleMeaComplete_OnBnClickedRetry();
				break;                                          
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	default:
		break;
	}

	return 0;
}

void UI_PopupAngleMeaComplete_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupAngleMeaComplete_WndProc);
	if (SaveExeInfoV2.flg == MST_REHAB) { //rehab
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_MEA_COMPLETE][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	else {
		hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_POP_MEA_COMPLETE1+SysInfo.meaCnt-1][Setup2.language], WS_OVERLAPPEDWINDOW,
				0, 0, 480, 272, hDeskTop, 0, 0);
	}
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_ANGLE_MEA_COMPLETE;
}

#if 0
//////////////////////////////////////////////////////////////////////////////
void UI_PopupAngleMeaLowPos_Timer(uint16_t nIDEvent)
{
	App_KillTimer(0);
	UI_AngleMeasure_Create();
}

void UI_PopupAngleMeaLowPos_OnBnClickedOk(void)
{
	UI_AngleMeasure_Create();
}


void UI_PopupAngleMeaLowPos_Init(void)
{
	API_CreateWindow("", pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_MC_BTN_OK+Setup2.language)*2], ',', 2),
			124, 51, hParent, RID_MC_BTN_OK+Setup2.language, 0);
	App_SetTimer(0, 1000);
}

LRESULT UI_PopupAngleMeaLowPos_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_PopupAngleMeaLowPos_Init();
		break;
	case WM_COMMAND:
		switch(wParam) {
		case RID_MC_BTN_OK:
		case RID_MC_BTN_OKK:
		case RID_MC_BTN_OKC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_PopupAngleMeaLowPos_OnBnClickedOk();
				break;                                       
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;
	case WM_BACKLIGHT:
		TLCD_CtrlBackLight((char *)lParam);
		break;                    
	case WM_TIMER:
		UI_PopupAngleMeaLowPos_Timer(wParam);
		break;                    
	default:
		break;
	}

	return 0;
}

void UI_PopupAngleMeaLowPos_Create(void)
{
	App_SetWndProcFnPtr(UI_PopupAngleMeaLowPos_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[19][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	UI_Wnd.child_prevNum = UI_Wnd.child_num;
	UI_Wnd.child_num = UI_WND_MODE_ANGLE_MEA_LOW_POS;
}
#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef PI_LOAD_V2
void UI_AngleMeasure_SetAngle_Pic(short angle,short angle2)
{      
	//int temp, temp2;
	
	if (angle == Option.temp16 && angle2 == Option.temp162) return;
	Option.temp16 = angle;
	Option.temp162 = angle2;

	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i agmep.bmp,167,49\r");
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i arange.bmp,167,49\r");
#if 1
	angle = (angle/5)*5;
	if (angle < 0) {
		AngPicBlueBuf[4] = '-';
		AngPicBlueBuf[5] = '0';
		AngPicBlueBuf[6] = ((angle%10)*-1) + '0';
	}
	else if (angle < 10) {
		AngPicBlueBuf[4] = '0';
		AngPicBlueBuf[5] = '0';
		AngPicBlueBuf[6] = (angle%10) + '0';
	}
	else if (angle < 100) {
		AngPicBlueBuf[4] = '0';
		AngPicBlueBuf[5] = (angle/10) + '0';
		AngPicBlueBuf[6] = (angle%10) + '0';
	}
	else {
	  	AngPicBlueBuf[4] = '1';
	  	AngPicBlueBuf[5] = (angle%100)/10 + '0';
	  	AngPicBlueBuf[6] = (angle%10) + '0';
	}
	angle2 = (angle2/5)*5;
	if (angle2 < 0) {
		AngPicIndigoBuf[4] = '-';
		AngPicIndigoBuf[5] = '0';
		angle2 *= -1;
		AngPicIndigoBuf[6] = ((angle2%10)*-1) + '0';
	}
	else if (angle2 < 10) {
		AngPicIndigoBuf[4] = '0';
		AngPicIndigoBuf[5] = '0';
		AngPicIndigoBuf[6] = (angle2%10) + '0';
	}
	else if (angle2 < 100) {
		AngPicIndigoBuf[4] = '0';
		AngPicIndigoBuf[5] = (angle2/10) + '0';
		AngPicIndigoBuf[6] = (angle2%10) + '0';
	}
	else {
	  	AngPicIndigoBuf[4] = '1';
	  	AngPicIndigoBuf[5] = (angle2%100)/10 + '0';
	  	AngPicIndigoBuf[6] = (angle2%10) + '0';
	}

#else
	if (!(angle%5)) {
		if (angle < 0) {
			AngPicBlueBuf[4] = '0';
			AngPicBlueBuf[5] = '0';
			AngPicBlueBuf[6] = (angle%10) + '0';
		}
		else if (angle < 10) {
			AngPicBlueBuf[4] = '0';
			AngPicBlueBuf[5] = '0';
			AngPicBlueBuf[6] = (angle%10) + '0';
		}
		else if (angle < 100) {
			AngPicBlueBuf[4] = '0';
			AngPicBlueBuf[5] = (angle/10) + '0';
			AngPicBlueBuf[6] = (angle%10) + '0';
		}
		else {
		  	AngPicBlueBuf[4] = '1';
		  	AngPicBlueBuf[5] = (angle%100)/10 + '0';
		  	AngPicBlueBuf[6] = (angle%10) + '0';
		}
	}

	if (!(angle2%5)) {
		if (angle2 < 0) {
			AngPicIndigoBuf[4] = '-';
			AngPicIndigoBuf[5] = '0';
			AngPicIndigoBuf[6] = ((angle2%10)*-1) + '0';
		}
		else if (angle2 < 10) {
			AngPicIndigoBuf[4] = '0';
			AngPicIndigoBuf[5] = '0';
			AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
		else if (angle2 < 100) {
			AngPicIndigoBuf[4] = '0';
			AngPicIndigoBuf[5] = (angle2/10) + '0';
			AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
		else {
		  	AngPicIndigoBuf[4] = '1';
		  	AngPicIndigoBuf[5] = (angle2%100)/10 + '0';
		  	AngPicIndigoBuf[6] = (angle2%10) + '0';
		}
	}
#endif
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)AngPicIndigoBuf);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)AngPicBlueBuf);
	if (SaveExeInfoV2.flg == MST_REHAB) {
		//if ((SysInfo.meaCnt >= 1) || (Exercise.dataCnt > 0)) {
		if (SysInfo.meaCnt >= 1) {
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pImgNameInfo[IMG_REHB_DIS][Setup2.language]);
		}
		else APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pImgNameInfo[IMG_REHB_EN][Setup2.language]);
	}
	else APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pImgNameInfo[IMG_MEA_EN][Setup2.language]);
    APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd); 
}

void UI_AngleMeasure_Timer(uint16_t nIDEvent)
{
	switch (nIDEvent) {
		case 2: //homein
			if (SaveExeInfoV2.flg == MST_REHAB) {
				if (SysInfo.meaCnt == 0) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);
				}
				else {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,RunWnd.angle,3);
				}
			}
			else {
				if (SysInfo.meaCnt == 0) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 1) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 2) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,RunWnd.angle,3);
				}
			}
			break;
		case 5: //end measure
			App_KillTimer(TIMER_ID_5);
			UI_PopupAngleMeaComplete_Create();
			//UI_PopupAngleMeaReport_Create();
			break;
		case 6: //on play
			if (MotorDrv_GetDirection() == MDD_CCW) {
				UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle, RunWnd.angle);
				if (SysInfo.meaCnt == 0) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 1) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 2) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,RunWnd.angle,3);
				}
			}
			else {
				UI_AngleMeasure_SetAngle_Pic(RunWnd.angle, AngleWnd.flAngle);
				if (SysInfo.meaCnt == 0) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 1) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,RunWnd.angle,3);
				}
				else if (SysInfo.meaCnt == 2) {
					UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,RunWnd.angle,3);
				}
			}
			break;
	}
}

//pjg++190805
void UI_AngleMeasure_CheckCurrentSensitivity(void)
{
	uint32_t cur;//, result;
	float ftemp, ftemp2;
	uint8_t buf[30];
	//uint8_t temp;
	
	if (UI_Time.tmp2_ms == 0) { //run every 10ms
		UI_Time.tmp2_ms = 10;
	}
	else return;

	cur = MotorDrv_GetCurrent();
	AngChk.cur = cur;
	if (AngChk.runOne == 0 || UI_Time.tmp4_ms != 0) {
		AngChk.prevCur = cur;
		AngChk.runOne++;// = 1;
		AngChk.prev = (float)cur;
		AngChk.angle = RunWnd.angle;
		AngChk.sameCnt = 0;
		//if (MotorDrv_GetDirection() == MDD_CCW) {
			//AngChk.y = (short)(0.00002 * 134 * 134 * 134 +
			//		-0.002 * 134 * 134 +
			//		0.1259 * 134);
		//	AngChk.y = 29;
		//}
		//else {
			//AngChk.y = (short)(0.00009 * 115 * 115 * 115 +
			//		-0.0188 * 115 * 115 +
			//		0.7484 * 115);
		//	AngChk.y = -25;
		//}
		//test.cnt = 0;
		//test.ang = (uint8_t *)CommonBuf;
		//result = cur;
		return;
	}
	else {
		ftemp = AngChk.alpha*(float)cur;
		ftemp2 = ((float)1.0-AngChk.alpha)*AngChk.prev;
		AngChk.prev = ftemp+ftemp2;
		AngChk.prevCur = (uint32_t)(AngChk.prev);
		/*
		if (test.cnt < TEST_BUF_SIZE/2) {
			test.buf[test.cnt] = ftemp;
			test.buf2[test.cnt] = ftemp2;
			test.ang[test.cnt] = RunWnd.angle;
			test.cnt++;
		}
		else {
			//AngChk.diff = cur-AngChk.prevCur;
		}
		*/
	}
	
	//if (RunWnd.angle > AngleWnd.flAngle-2 || RunWnd.angle < AngleWnd.exAngle+4) {
		//AngChk.prev = (float)cur;
	//	return;
	//}

	if (AngChk.angle > RunWnd.angle) {
		if ((AngChk.angle - RunWnd.angle) < ANGLE_MIN_DEG) return;
	}
	else {
		if ((RunWnd.angle- AngChk.angle) < ANGLE_MIN_DEG) return;
	}

	AngChk.diff = cur-AngChk.prevCur;
#if 1 // current monitor
		//temp = MotorDrv_GetCurrent();
		ConvertULong2String((int16_t)cur, (uint8_t *)buf, 11);
		uart_putstring3((char *)buf);
		uart_putchar3(',');
		ConvertULong2String((int16_t)AngChk.prevCur, (uint8_t *)buf, 11);
		uart_putstring3((char *)buf);
		uart_putchar3(',');
		ConvertULong2String((int8_t)AngChk.diff, (uint8_t *)buf, 11);
		uart_putstring3((char *)buf);
		uart_putchar3(',');
		ConvertULong2String((int16_t)RunWnd.angle, buf, 11);
		uart_putstring3((char *)buf);
		uart_putchar3('\r');
		uart_putchar3('\n');
#endif

#ifdef USE_ANG_MEA_METHOD_TYPE1
#elif USE_ANG_MEA_METHOD_TYPE2
	if (MotorDrv_GetDirection() == MDD_CCW) {
		AngChk.temp = (short)(0.0016 * (float)RunWnd.angle * (float)RunWnd.angle +
				-0.0851 * (float)RunWnd.angle + 1.8172);
		AngChk.diff += AngChk.temp;
	}
	else {
		AngChk.temp = (short)(-0.0023 * (float)RunWnd.angle * (float)RunWnd.angle +
				-0.0785 * (float)RunWnd.angle + 44.175);
		AngChk.diff += (AngChk.temp-5);
	}
#else
	if (MotorDrv_GetDirection() == MDD_CCW) {
		//AngChk.temp = (short)(0.00002 * (float)RunWnd.angle * (float)RunWnd.angle * (float)RunWnd.angle +
		//		-0.002 * (float)RunWnd.angle * (float)RunWnd.angle +
		//		0.1259 * (float)RunWnd.angle);
		//AngChk.gap = AngChk.gapBk + (AngChk.upOffset * AngChk.temp)/AngChk.y;
		AngChk.temp = (short)(0.0016 * (float)RunWnd.angle * (float)RunWnd.angle +
				-0.0851 * (float)RunWnd.angle + 1.8172);
		AngChk.temp *= AngChk.upOffset;
		AngChk.gap = AngChk.gapBk + AngChk.temp;
	}
	else {
		//AngChk.temp = (short)(0.00009 * (float)RunWnd.angle * (float)RunWnd.angle * (float)RunWnd.angle +
		//		-0.0188 * (float)RunWnd.angle * (float)RunWnd.angle +
		//		0.7484 * (float)RunWnd.angle);
		//AngChk.gap = AngChk.gapBk + (AngChk.upOffset * AngChk.temp)/AngChk.y;
		AngChk.temp = (short)(-0.0023 * (float)RunWnd.angle * (float)RunWnd.angle +
				-0.0785 * (float)RunWnd.angle + 44.175);
		AngChk.temp *= AngChk.upOffset;
		AngChk.gap = AngChk.gapBk + AngChk.temp;
	}
#endif

	if (AngChk.diff > AngChk.gap) {
		AngChk.sameCnt++;
		if (AngChk.sameCnt > 3 && UI_Time.tmp3_ms == 0) {
			MotorDrv_SetFlagSensCurrent(0);
			AngChk.runOne = 0;
			UI_Time.tmp3_ms = 2000;
			if (MotorDrv_GetDirection() == MDD_CCW) {
				AngleWnd.flAngle = RunWnd.angle;
			}
			else {
				if ((AngleWnd.flAngle - RunWnd.angle) > ANGLE_MIN_GAP) //pjg++191028 : prevent vibration action
					AngleWnd.exAngle = RunWnd.angle;
			}
		}
	}
	else {
		if (AngChk.sameCnt) AngChk.sameCnt--;
	}
}

void UI_AngleMeasure_DispMeaValue(void)
{
	if (MotorDrv_GetDirection() == MDD_CCW) { //down
		if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-3) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,AngleWnd.flAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].flangle = AngleWnd.flAngle+SAVE_OFFSET_VALUE;
			}
			else {
				SaveExeInfoV2.spm.mi.flangle = AngleWnd.flAngle+SAVE_OFFSET_VALUE;
			}
		}
		else if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-2) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,AngleWnd.flAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].flangle = AngleWnd.flAngle+SAVE_OFFSET_VALUE;
			}
		}
		else if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-1) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,AngleWnd.flAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].flangle = AngleWnd.flAngle+SAVE_OFFSET_VALUE;
			}
		}
	}
	else {
		if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-3) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,AngleWnd.exAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].exangle = AngleWnd.exAngle+SAVE_OFFSET_VALUE;
			}
			else {
				SaveExeInfoV2.spm.mi.exangle = AngleWnd.exAngle+SAVE_OFFSET_VALUE;
			}
		}
		else if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-2) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,AngleWnd.exAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].exangle = AngleWnd.exAngle+SAVE_OFFSET_VALUE;
			}
		}
		else if (SysInfo.meaCnt == ANG_MEA_TOTAL_COUNT-1) {
			UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,AngleWnd.exAngle,3);
			if (SaveExeInfoV2.flg == MST_MEASURE) {
				SaveExeInfoV2.smm.mi[SysInfo.meaCnt].exangle = AngleWnd.exAngle+SAVE_OFFSET_VALUE;
			}
		}
	}
}

void UI_AngleMeasure_OnBnClickedBtnSetAngle()
{
	//UI_Time.tmp2_ms = 1000;
	UI_Time.tmp3_ms = 2500;
	if (MotorDrv_GetDirection() == MDD_CCW) { //down
		if (RunWnd.angle < ANGLE_MIN_DEG) return;
		AngleWnd.flAngle = RunWnd.angle;
		UI_AngleMeasure_SetAngle_Pic(RunWnd.angle, AngleWnd.flAngle);
	}
	else {
		if ((AngleWnd.flAngle - RunWnd.angle) > ANGLE_MIN_GAP)
			AngleWnd.exAngle = RunWnd.angle;
	}
}

void UI_AngleMeasure_Process(void)
{
	if (MotorDrv_GetFlagLimitSW()) {
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_OVER_RANGE, EDT_DISP_HALT);
		return;
	}
	
	if (AngChk.mode == 0) {
		UI_AngleMeasure_CheckCurrentSensitivity();
	}
	
	if (MotorDrv_GetDirection() == MDD_CCW) {
		if (MotorDrv_GetPosition() >= AngleWnd.flAngle) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				UI_AngleMeasure_DispMeaValue();
				UI_AngleMeasure_SetAngle_Pic(RunWnd.angle, AngleWnd.flAngle);
				UI_Time.tmp3_ms = 1000;
				UI_Time.tmp4_ms = 2000;

				MotorDrv_SetDirection(MDD_CW);
				MotorDrv_SetFlagRunOne(0);
				MotorDrv_MoveUp();
				AngChk.runOne = 0;
				AngChk.mode = 0;
				AngChk.prevCur = 0;
				#ifdef USE_ANG_MEA_METHOD_TYPE1
				AngChk.gapBk = amSensitivity[AMD_EXTENTION][Setup3.amSens-1][AMVT_GAP];
				AngChk.upOffset = amSensitivity[AMD_EXTENTION][Setup3.amSens-1][AMVT_NOGRAVITY2MAX];
				AngChk.gap = AngChk.gapBk + AngChk.upOffset;
				#elif USE_ANG_MEA_METHOD_TYPE2
				AngChk.gap = amSensitivity[AMD_EXTENTION][Setup3.amSens-1][AMVT_GAP];
				#endif
			}
		}
		else {
			MotorDrv_MoveDown();
			#ifdef USE_ANG_MEA_METHOD_TYPE1
			if (RunWnd.angle < ANG_MEA_NO_GRAVITY_ANGLE) {
				AngChk.gapBk = amSensitivity[AMD_FLEXTION][Setup3.amSens-1][AMVT_GAP];
				AngChk.gap = AngChk.gapBk;
			}
			else {
				AngChk.upOffset = amSensitivity[AMD_FLEXTION][Setup3.amSens-1][AMVT_NOGRAVITY2MAX];
				AngChk.gap = AngChk.gapBk + AngChk.upOffset;
			}
			#elif USE_ANG_MEA_METHOD_TYPE2
			AngChk.gap = amSensitivity[AMD_EXTENTION][Setup3.amSens-1][AMVT_GAP];
			#endif
		}
	}
	else {
		if (MotorDrv_GetPosition() <= AngleWnd.exAngle) {
			if (!MotorDrv_GetFlagRunOne()) {
				MotorDrv_SetFlagRunOne(1);
				MotorDrv_SetTargetPosition(AngleWnd.exAngle);
				//MotorDrv_Disable();
				//MotorDrv_Release();
				UI_AngleMeasure_DispMeaValue();
				if (SysInfo.meaCnt < ANG_MEA_TOTAL_COUNT) SysInfo.meaCnt++;
				if (SysInfo.meaCnt >= ANG_MEA_TOTAL_COUNT) {
					//gray background
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,163\r");
					APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,163\r");
				}
				UI_AngleMeasure_OnBnClickedBtnPause();
				if (RunWnd.angle < ANG_MEA_NO_GRAVITY_ANGLE) {
					AngChk.gapBk = amSensitivity[AMD_FLEXTION][Setup3.amSens-1][AMVT_GAP];
					AngChk.gap = AngChk.gapBk;
				}
				else {
					AngChk.upOffset = amSensitivity[AMD_FLEXTION][Setup3.amSens-1][AMVT_NOGRAVITY2MAX];
					AngChk.gap = AngChk.gapBk + AngChk.upOffset;
				}
				//popup
				App_SetTimer(TIMER_ID_5, 10);
			}
		}
		else {
			MotorDrv_MoveUp();
		}
	}
}


void UI_AngleMeasure_OnBnClickedBtnLeft(void)
{
   	UI_User_Create();
}

void UI_AngleMeasure_OnBnClickedBtnHome(void)
{	
	if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);
	else{
		//UI_InitSetupVariable();
		//UI_InitSystemVariable();
		UI_InitVariable();
	}
	
   	UI_Home_Create();
}

void UI_AngleMeasure_OnBnClickedBtnRight(void)
{
	//if (loginInfo.type == LIT_USER) ;//pjg--190725 UI_SavePIToEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);	
//	if (SysInfo.meaCnt < 1) return;
	UI_SpeedTime_Create();//UI_Angle_Create();
}

void UI_AngleMeasure_OnBnClickedBtnAutoCheckAngle(void)		 
{
	if (RunWnd.play == UI_RUN_MODE_HOME) return;
	
	if (SaveExeInfoV2.flg == MST_REHAB) {
		if (SysInfo.meaCnt > 0) return;
	}
	//if (MotorDrv_GetPosition() >= 30) {
		//please move to home position
		//UI_PopupAngleMeaLowPos_Create();
		//return;
	//}
	MotorDrv_Run();
	UI_SetFunRunMode(UI_FRM_NORMAL);
	//RunWnd.time = SpdTmWnd.time;
	RunWnd.play = UI_RUN_MODE_PLAY;
	//API_ChangeHMenu(hParent, RID_RN_BTN_PLAY, RID_RN_BTN_PLAYING);
//	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pBtnInfo[RID_RN_BTN_PLAYING*2]);
	//if (Setup2.sndGuid2 == BST_CHECKED)
	APP_SendMessage(hParent, WM_SOUND, 0, SNDID_OPERATION_START);
	//App_SetTimer(1, 150000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed(150000)
	App_SetTimer(TIMER_ID_6, 50);//00000); // ≥ π´ ∫¸∏£∏È ±˙¡¸ : max speed
	App_SetUIProcess(UI_AngleMeasure_Process); //pjg++170529
	UI_LED_Control(LM_RUN);
	//Total_Counter = PInfoWnd2.pi.totalRepeat;
	Timer_sec = 0; //pjg++170608 buf fix: complete right now
	App_SetButtonOption(RID_ANG_BTN_LEFT, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_RIGHT, BN_DISABLE);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_HOME+Setup2.language, BN_DISABLE);
	App_SetButtonOption(RID_RN_BTN_SETUP, BN_DISABLE);
	//App_SetButtonOption(RID_ANG_BTN_MEA, BN_DISABLE);
	//App_SetButtonOption(RID_ANG_BTN_STOP, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	//KeyDrv_Disable(1);	
	//AngChk.ext = AngleWnd.exAngle;
	//AngChk.flex = AngleWnd.flAngle;
	AngleWnd.exAngle = ANG_CHK_ANGLE_MIN;
	AngleWnd.flAngle = ANG_CHK_ANGLE_MAX;
	AngChk.runOne = 0;
	AngChk.mode = 0;
	AngChk.prevCur = 0;
	UI_Time.tmp3_ms = 1000;
	UI_Time.tmp4_ms = 2000;
	MotorDrv_SetDirection(MDD_CCW);
	AngChk.gap = amSensitivity[AMD_FLEXTION][Setup3.amSens-1][AMVT_GAP];
	MotorDrv_SetFlagRunOne(0);
	
	if (SysInfo.meaCnt == 0) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);
	}
	else if (SysInfo.meaCnt == 1) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 38,125,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_8PLACE, 363,125,RunWnd.angle,3);
	}
	else if (SysInfo.meaCnt == 2) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_7PLACE, 38,172,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_8PLACE, 363,172,RunWnd.angle,3);
	}
	Option.temp16 = RunWnd.angle - 5;
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,RunWnd.angle);//AngleWnd.flAngle);
	MotorDrv_SetOverCurrent(RUN_CHK_OVERCURRENT);
}

void UI_AngleMeasure_OnBnClickedBtnPause(void)
{
	if (UI_Time.tmp_ms) return;

	MotorDrv_Pause();
	if (RunWnd.play == UI_RUN_MODE_HOME) RunWnd.play = UI_RUN_MODE_HOME_PAUSE;
	else RunWnd.play = UI_RUN_MODE_PAUSE;
    APP_SendMessage(0, WM_SOUND, 0, SNDID_OPERATION_STOP);
	App_KillTimer(TIMER_ID_6);
	App_SetUIProcess(UI_ProcessNull); //pjg++170529
	UI_LED_Control(LM_PAUSE);

	App_SetButtonOption(RID_ANG_BTN_HOME+Setup2.language, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_PUSHED);
	if (SaveExeInfoV2.flg == MST_REHAB) {
		if (SysInfo.meaCnt >= 1) App_SetButtonOption(RID_ANG_BTN_RIGHT, BN_PUSHED);
	}
	else {
	}
	App_SetButtonOption(RID_RN_BTN_SETUP, BN_PUSHED);
	//App_SetButtonOption(RID_ANG_BTN_MEA, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_STOP, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
	//App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
	if (SaveExeInfoV2.flg == MST_REHAB) {
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}
	//KeyDrv_Disable(0);
	if (MotorDrv_GetDirection() == MDD_CCW)
		UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle, RunWnd.angle);
	else UI_AngleMeasure_SetAngle_Pic(RunWnd.angle,AngleWnd.flAngle);
	MotorDrv_SetTimerValue(0, 4000);
	MotorDrv_SetSensFlagRunOne(0);
	MotorDrv_SetFlagSensCurrent(0);
}

void UI_AngleMeasure_OnBnClickedBtnExUp(void)
{
	loading = 0;
	if(AngleWnd.exAngle < RUN_ANGLE_MAX) AngleWnd.exAngle += Setup3.AngBtnStep;
	if (AngleWnd.exAngle > RUN_ANGLE_MAX) AngleWnd.exAngle = RUN_ANGLE_MAX;
	if(AngleWnd.exAngle > RUN_ANGLE_MIN) {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
	}
	//if(AngleWnd.flAngle <= (AngleWnd.exAngle+25)) AngleWnd.exAngle = AngleWnd.flAngle + 25;
	if(AngleWnd.flAngle <= (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}

	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,125,AngleWnd.exAngle,3);
}

void UI_AngleMeasure_OnBnClickedBtnExDown(void)
{
    if(AngleWnd.exAngle > RUN_ANGLE_MIN) AngleWnd.exAngle -= Setup3.AngBtnStep;
	if (AngleWnd.exAngle < RUN_ANGLE_MIN) AngleWnd.exAngle = RUN_ANGLE_MIN;
	if(AngleWnd.exAngle == RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	}
	//if(AngleWnd.flAngle > (AngleWnd.exAngle+25)) AngleWnd.exAngle = AngleWnd.flAngle + 25;
	if(AngleWnd.flAngle > (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}

	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE,38,125,AngleWnd.exAngle,3);
	
}

//unsigned char 
void UI_AngleMeasure_OnBnLongClickedBtnExUp(void)
{
	if(AngleWnd.flAngle <= (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}	
	//else if(AngleWnd.flAngle <= (AngleWnd.exAngle+30) && AngleWnd.flAngle > (AngleWnd.exAngle+ANGLE_MIN_GAP))
	else if(AngleWnd.flAngle <= (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE+ANGLE_MOVE_GAP) && 
		AngleWnd.flAngle > (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE)) {
		AngleWnd.exAngle++;
	}
	else {
		AngleWnd.exAngle += ANGLE_MOVE_GAP;
		if (AngleWnd.exAngle%ANGLE_MOVE_GAP) { //pjg++190814
			AngleWnd.exAngle -= AngleWnd.exAngle%ANGLE_MOVE_GAP;
		}
	}
	
	if(AngleWnd.exAngle > RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
	}
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5, (AngleWnd.flAngle/5)*5);
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,125,AngleWnd.exAngle,3);
}

void UI_AngleMeasure_OnBnLongClickedBtnExDown(void)
{
	if(AngleWnd.flAngle > (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}
	if (AngleWnd.exAngle > -1) 
	{
		AngleWnd.exAngle -= ANGLE_MOVE_GAP;	
		if (AngleWnd.exAngle%ANGLE_MOVE_GAP) { //pjg++190814
			AngleWnd.exAngle -= AngleWnd.exAngle%ANGLE_MOVE_GAP;
		}
	}
	else if(AngleWnd.exAngle >= -1 || AngleWnd.exAngle > RUN_ANGLE_MIN) {
		AngleWnd.exAngle --;
	}
	//else return;
	//UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.exAngle <= RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	}
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,125,AngleWnd.exAngle,3);
}

void UI_AngleMeasure_OnBnClickedBtnFlUp(void)
{
	if(AngleWnd.flAngle < RUN_ANGLE_MAX) AngleWnd.flAngle += Setup3.AngBtnStep;
	if (AngleWnd.flAngle > RUN_ANGLE_MAX) AngleWnd.flAngle = RUN_ANGLE_MAX;
	if(AngleWnd.flAngle == RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	}
	if(AngleWnd.exAngle < (AngleWnd.flAngle-RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
	}

	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,125,AngleWnd.flAngle,3);
}

void UI_AngleMeasure_OnBnClickedBtnFlDown(void){
	if(AngleWnd.flAngle > Setup3.AngBtnStep) AngleWnd.flAngle -= Setup3.AngBtnStep;
	else AngleWnd.flAngle--;
	if (AngleWnd.flAngle < RUN_ANGLE_MIN) AngleWnd.flAngle = RUN_ANGLE_MIN;
	if(AngleWnd.flAngle < RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
	}
	//if (AngleWnd.exAngle >= (AngleWnd.flAngle-25)) AngleWnd.flAngle = AngleWnd.exAngle-25;
	if(AngleWnd.exAngle >= (AngleWnd.flAngle-RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
	}
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,125,AngleWnd.flAngle,3);
	
}

void UI_AngleMeasure_OnBnLongClickedBtnFlUp(void)
{	
	if(AngleWnd.exAngle < (AngleWnd.flAngle-RUN_MIN_EXE_ANGLE))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
	}
	
	if (AngleWnd.flAngle <= (RUN_ANGLE_MAX-ANGLE_MOVE_GAP)) { //AngleWnd.exAngle = 0;
		AngleWnd.flAngle += ANGLE_MOVE_GAP;
		if (AngleWnd.flAngle%ANGLE_MOVE_GAP) { //pjg++190814
			AngleWnd.flAngle -= AngleWnd.flAngle%ANGLE_MOVE_GAP;
		}
	}
	else if(AngleWnd.flAngle < (RUN_ANGLE_MAX-ANGLE_MOVE_GAP+1)) {// || AngleWnd.flAngle < RUN_ANGLE_MAX) {
		AngleWnd.flAngle++;
	}
	//else return;

	//if(AngleWnd.exAngle%5 <= 0)
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.flAngle >= RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	}
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE,363,125,AngleWnd.flAngle,3);
}

void UI_AngleMeasure_OnBnLongClickedBtnFlDown(void)
{
	if(AngleWnd.exAngle >= (AngleWnd.flAngle-RUN_MIN_EXE_ANGLE) || AngleWnd.flAngle == 20)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}	
	else if(AngleWnd.exAngle >= (AngleWnd.flAngle-(ANGLE_MIN_GAP+5)) && 
		AngleWnd.exAngle < (AngleWnd.flAngle-ANGLE_MIN_GAP)) {							
		AngleWnd.flAngle--;
	}
	else {
		AngleWnd.flAngle -= ANGLE_MOVE_GAP;
		if (AngleWnd.flAngle%ANGLE_MOVE_GAP) { //pjg++190814
			AngleWnd.flAngle -= AngleWnd.flAngle%5;
		}
	}
	
	//else return;
//	UI_Angle_SetAngle_Pic((AngleWnd.exAngle/5)*5,(AngleWnd.flAngle/5)*5);
	if(AngleWnd.flAngle < RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
	}
	UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,125,AngleWnd.flAngle,3);
	
}

void UI_AngleMeasure_OnBnClickedBtnSetup(void)
{
	UI_Setup_Create();
}

void UI_AngleMeasure_Process_HomeIn(void)
{
	if (UI_GotoHomePosition()) {
		RunWnd.play = UI_RUN_MODE_PAUSE;
		App_SetUIProcess(UI_ProcessNull);
		UI_AngleMeasure_Timer(TIMER_ID_2);
		API_ChangeHMenu(hParent, RID_RN_BTN_STOP, RID_RN_BTN_PLAY);
		Option.temp16 = 200;
		UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,RunWnd.angle);
		App_SetButtonOption(RID_ANG_BTN_LEFT, BN_PUSHED);
		App_SetButtonOption(RID_ANG_BTN_RIGHT, BN_PUSHED);
		App_SetButtonOption(RID_ANG_BTN_HOME+Setup2.language, BN_PUSHED);
		//App_SetButtonOption(RID_ANG_BTN_SETUP, BN_PUSHED);
		//App_SetButtonOption(RID_ANG_BTN_STOP, BN_PUSHED);
		//App_SetButtonOption(RID_RN_BTN_PROCHK, BN_PUSHED);
		//App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_PUSHED);
		//App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_PUSHED);
		if (SaveExeInfoV2.flg == MST_REHAB) {
			//App_SetButtonOption(RID_ANG_BTN_REH, BN_PUSHED);
			App_SetButtonOption(RID_ANG_BTN_EXP, BN_PUSHED);
			App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
			App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
			App_SetButtonOption(RID_ANG_BTN_FLM, BN_PUSHED);
		}
		else {
			//App_SetButtonOption(RID_ANG_BTN_MEA, BN_PUSHED);
		}
		KeyDrv_Disable(0);
		//UI_Run_InitVar();
		App_KillTimer(TIMER_ID_2);
		//MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent thtat motor is break when fast press machine
		//MotorDrv_SetSensCurrent(STANDBY_SENS_CURRENT);
	}
}

void UI_AngleMeasure_OnBnLongClickedBtnHome(void)
{
	if (RunWnd.play == UI_RUN_MODE_HOME) return;
	if (RunWnd.play == UI_RUN_MODE_PLAY) return;
	if (SaveExeInfoV2.flg == MST_REHAB) {
		//if (SysInfo.meaCnt < 1) return;
	}
	else {
		if (SysInfo.meaCnt < ANG_MEA_TOTAL_COUNT && SysInfo.meaCnt > 0) return;
	}

	UI_Time.tmp_ms = 2500;
	UI_Run_SetGotoHomeCmd();
  	UI_SetFunRunMode(UI_FRM_GOTO_HOME_POS);
	 //Run_mode = 0;
	RunWnd.play = UI_RUN_MODE_HOME;
	App_SetTimer(TIMER_ID_2, 30);//00000);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i agmestu.png,186,49\r");
	APP_SendMessage(hParent, WM_SOUND, 0, SNDID_GO_HOME);
	
	App_SetButtonOption(RID_ANG_BTN_LEFT, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_RIGHT, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_RIGHT, BN_DISABLE);
	App_SetButtonOption(RID_ANG_BTN_HOME+Setup2.language, BN_DISABLE);
	//App_SetButtonOption(RID_ANG_BTN_SETUP, BN_DISABLE);
	//App_SetButtonOption(RID_ANG_BTN_MEA, BN_DISABLE);
	//App_SetButtonOption(RID_ANG_BTN_STOP, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_PROCHK, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_LIMCHK, BN_DISABLE);
	//App_SetButtonOption(RID_RN_BTN_VIBCHK, BN_DISABLE);
	if (SaveExeInfoV2.flg == MST_REHAB) {
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}
	//KeyDrv_Disable(1);	

	App_SetUIProcess(UI_AngleMeasure_Process_HomeIn); //pjg++171018
	MotorDrv_SetOverCurrent(STANDBY_CHK_CURRENT); //pjg++190904 : to prevent that motor is broken when fast press machine
}

void UI_AngleMeasure_SetRehabMode(void)
{
	API_CreateWindow("", pBtnInfo[(RID_ANG_BTN_REH+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_REH+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_REH+Setup2.language)*2], ',', 2),
			124, 124, hParent, RID_ANG_BTN_REH+Setup2.language, 0);
	
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_EXP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXP*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_EXP, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_EXM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_EXM*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_EXM, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_FLP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLP*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_FLP, 0);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_FLM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_FLM*2], ',', 2),
			58, 46, hParent, RID_ANG_BTN_FLM, 0);

	if ((SysInfo.meaCnt >= 1) || (Exercise.dataCnt > 0)) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77, SaveExeInfoV2.spm.mi.exangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77, SaveExeInfoV2.spm.mi.flangle-SAVE_OFFSET_VALUE,3);
		if (SysInfo.meaCnt >= 1) {
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,69\r");
			APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,69\r");
		}
		if ((SaveExeInfoV2.spm.mi.exangle-SAVE_OFFSET_VALUE) >= Setup3.AutoAngValue) { //-5 is sub value because add 5 when save exangle value
			AngleWnd.exAngle = SaveExeInfoV2.spm.mi.exangle - SAVE_OFFSET_VALUE - Setup3.AutoAngValue;
			if (AngleWnd.exAngle < RUN_ANGLE_MIN) AngleWnd.exAngle = RUN_ANGLE_MIN;
		}
		else {
            AngleWnd.exAngle = SaveExeInfoV2.spm.mi.exangle - SAVE_OFFSET_VALUE - Setup3.AutoAngValue;
			if (AngleWnd.exAngle < RUN_ANGLE_MIN) AngleWnd.exAngle = RUN_ANGLE_MIN;
		}
		AngleWnd.flAngle = SaveExeInfoV2.spm.mi.flangle - SAVE_OFFSET_VALUE + Setup3.AutoAngValue;
		if (AngleWnd.flAngle > RUN_ANGLE_MAX) AngleWnd.flAngle = RUN_ANGLE_MAX;
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,AngleWnd.exAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,AngleWnd.flAngle,3);
		if (SysInfo.meaCnt >= 1) App_SetButtonOption(RID_ANG_BTN_REH+Setup2.language, BN_DISABLE);
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)pImgNameInfo[IMG_REHB_DIS][Setup2.language]);
	}
	else {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,0,3);//AngleWnd.exAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);//AngleWnd.flAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,AngleWnd.exAngle,3); //pjg<>200110
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,AngleWnd.flAngle,3); //pjg<>200110
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
		//App_SetButtonOption(RID_ANG_BTN_RIGHT, BN_DISABLE);
	}

	if(AngleWnd.exAngle > RUN_ANGLE_MIN){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_PUSHED);
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,77,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXM, BN_DISABLE);
	}
	
	if(AngleWnd.flAngle < RUN_ANGLE_MAX){
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_PUSHED);
	}
	else{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,345,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLP, BN_DISABLE);
	}
	
	if(AngleWnd.flAngle <= (AngleWnd.exAngle+RUN_MIN_EXE_ANGLE+Setup3.AutoAngValue))
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,16,166\r");
		App_SetButtonOption(RID_ANG_BTN_EXP, BN_DISABLE);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,405,166\r");
		App_SetButtonOption(RID_ANG_BTN_FLM, BN_DISABLE);
	}
}

void UI_AngleMeasure_SetMeasureMode(void)
{
	API_CreateWindow("", pBtnInfo[(RID_ANG_BTN_MEA+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_MEA+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_MEA+Setup2.language)*2], ',', 2),
			124, 124, hParent, RID_ANG_BTN_MEA+Setup2.language, 0);
	
	if (SysInfo.meaCnt >= ANG_MEA_TOTAL_COUNT) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,SaveExeInfoV2.smm.mi[0].exangle-SAVE_OFFSET_VALUE,3);//AngleWnd.exAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,SaveExeInfoV2.smm.mi[0].flangle-SAVE_OFFSET_VALUE,3);//AngleWnd.flAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,SaveExeInfoV2.smm.mi[1].exangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,SaveExeInfoV2.smm.mi[1].flangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,SaveExeInfoV2.smm.mi[2].exangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,SaveExeInfoV2.smm.mi[2].flangle-SAVE_OFFSET_VALUE,3);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,69\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,117\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,163\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,69\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,117\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,163\r");
	}
	else if (SysInfo.meaCnt >= ANG_MEA_TOTAL_COUNT-1) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,SaveExeInfoV2.smm.mi[0].exangle-SAVE_OFFSET_VALUE,3);//AngleWnd.exAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,SaveExeInfoV2.smm.mi[0].flangle-SAVE_OFFSET_VALUE,3);//AngleWnd.flAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,SaveExeInfoV2.smm.mi[1].exangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,SaveExeInfoV2.smm.mi[1].flangle-SAVE_OFFSET_VALUE,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,RunWnd.angle,3);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,69\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,117\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,69\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,117\r");
	}
	else if (SysInfo.meaCnt >= ANG_MEA_TOTAL_COUNT-2) {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,SaveExeInfoV2.smm.mi[0].exangle-SAVE_OFFSET_VALUE,3);//AngleWnd.exAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,SaveExeInfoV2.smm.mi[0].flangle-SAVE_OFFSET_VALUE,3);//AngleWnd.flAngle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,RunWnd.angle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,0,3);
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,18,69\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i meacom.png,346,69\r");
	}
	else {
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, 38,77,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, 363,77,RunWnd.angle,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_3PLACE, 38,125,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_4PLACE, 363,125,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_5PLACE, 38,172,0,3);
		UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_6PLACE, 363,172,0,3);
	}
}

void UI_AngleMeasure_Init(void)
{		
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufStartCmd);
	API_CreateWindow("", pBtnInfo[RID_ANG_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_ANG_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_ANG_BTN_LEFT*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	API_CreateWindow("", pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_ANG_BTN_HOME+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_ANG_BTN_HOME+Setup2.language, 0);
	if (SaveExeInfoV2.flg == MST_REHAB) {
		API_CreateWindow("", pBtnInfo[RID_ANG_BTN_RIGHT*2], BS_PUSHBUTTON, 
				GetNumFromString(pBtnInfo[RID_ANG_BTN_RIGHT*2], ',', 1),
				GetNumFromString(pBtnInfo[RID_ANG_BTN_RIGHT*2], ',', 2),
				116, 45, hParent, RID_ANG_BTN_RIGHT, 0);
	}
	else {
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	}
	API_CreateWindow("", pBtnInfo[RID_RN_BTN_SETUP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_RN_BTN_SETUP*2], ',', 2),
			30, 30, hParent, RID_RN_BTN_SETUP, 0);

	if (SaveExeInfoV2.flg == MST_REHAB) { //rehab
		UI_AngleMeasure_SetRehabMode();
	}
	else {
		UI_AngleMeasure_SetMeasureMode();
	}

	UI_LED_Control(LM_STAND_BY);

	UI_PINumber_Display();
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)DoubleBufEndCmd);
	Option.temp16 = 200;

	if ((SysInfo.meaCnt < 1 || SaveExeInfoV2.flg == MST_MEASURE) && Exercise.dataCnt == 0) {
		//UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,RunWnd.angle);
		UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
	}
	else {
		if ((SysInfo.meaCnt >= 1 || Exercise.dataCnt > 0) && SaveExeInfoV2.flg == MST_REHAB) {
			UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle,AngleWnd.flAngle);
			//if (SysInfo.meaCnt >= 1 || Exercise.dataCnt > 0) App_SetButtonOption(RID_ANG_BTN_REH+Setup2.language, BN_DISABLE);
		}
	}
	
	if (SaveExeInfoV2.flg == MST_MEASURE) {
		if (SysInfo.meaCnt < ANG_MEA_TOTAL_COUNT && RunWnd.play != UI_RUN_MODE_STOP) {
			UI_AngleMeasure_OnBnClickedBtnAutoCheckAngle();
		}
	}
	//MotorDrv_SetSensCurrent((uint32_t)Motor_OverCurTbl[Setup3.sensitivity-1][SpdTmWnd.speed-1]);
}

LRESULT UI_AngleMeasure_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_AngleMeasure_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_ANG_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				//MessageBox(hWnd,TEXT("Hello"),TEXT("Button"),MB_OK);
				UI_AngleMeasure_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_ANG_BTN_HOME:
		case RID_ANG_BTN_HOMEK:
		case RID_ANG_BTN_HOMEC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeasure_OnBnClickedBtnHome();
				break;
			}
			return 0;
		case RID_ANG_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeasure_OnBnClickedBtnRight();
				break;
			}
			return 0;
		case RID_ANG_BTN_EXP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_AngleMeasure_OnBnClickedBtnExUp();
                break;
            case BN_LONGPUSHED: //long push 
                UI_AngleMeasure_OnBnLongClickedBtnExUp();
                break;                                       
			}
			return 0;
		case RID_ANG_BTN_EXM:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_AngleMeasure_OnBnClickedBtnExDown();
                break;
            case BN_LONGPUSHED: //long push 
                UI_AngleMeasure_OnBnLongClickedBtnExDown();
                break;                                        
			}
			return 0;
		case RID_ANG_BTN_FLP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_AngleMeasure_OnBnClickedBtnFlUp();
                break;
            case BN_LONGPUSHED: //long push 
                UI_AngleMeasure_OnBnLongClickedBtnFlUp();
                break;                                       
			}
			return 0;
		case RID_ANG_BTN_FLM:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_AngleMeasure_OnBnClickedBtnFlDown();
                break;
            case BN_LONGPUSHED: //long push 
                UI_AngleMeasure_OnBnLongClickedBtnFlDown();
                break;                                        
			}
			return 0;
		case RID_ANG_BTN_MEA: //pjg++190814
		case RID_ANG_BTN_MEAK:
		case RID_ANG_BTN_MEAC:
		case RID_ANG_BTN_REH: //pjg++190814
		case RID_ANG_BTN_REHK:
		case RID_ANG_BTN_REHC:
			switch(lParam) {
			case BN_CLICKED: // push 
				if (RunWnd.play == UI_RUN_MODE_HOME)
					UI_AngleMeasure_OnBnClickedBtnPause();
				else if (RunWnd.play == UI_RUN_MODE_HOME_PAUSE) //at not measure
					UI_AngleMeasure_OnBnLongClickedBtnHome();
				else if (RunWnd.play != UI_RUN_MODE_PLAY && RunWnd.play != UI_RUN_MODE_HOME_PAUSE)
                	UI_AngleMeasure_OnBnClickedBtnAutoCheckAngle();
				else UI_AngleMeasure_OnBnClickedBtnSetAngle();
                break;
            case BN_LONGPUSHED: //long push 
				if (App_GetLongTouchCnt() > 1) break;
				if (RunWnd.play == UI_RUN_MODE_PLAY || RunWnd.play == UI_RUN_MODE_HOME) {
               		UI_AngleMeasure_OnBnClickedBtnPause();
				}
				else {
					UI_AngleMeasure_OnBnLongClickedBtnHome();
				}
                break;   
			case BN_UNHILITE: //lose focus
				if (MotorDrv_GetDirection() == MDD_CCW)
					UI_AngleMeasure_SetAngle_Pic(AngleWnd.exAngle, RunWnd.angle);
				else UI_AngleMeasure_SetAngle_Pic(RunWnd.angle, AngleWnd.flAngle);
				break;	 
			}
			return 0;
		case RID_RN_BTN_SETUP:
			switch(lParam) {
			case BN_CLICKED: // push 
                UI_AngleMeasure_OnBnClickedBtnSetup();
                break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	case WM_TIMER:  //pjg++170928
		//UI_Timer(wParam);
		UI_AngleMeasure_Timer(wParam);
		break;
	case WM_KEYUP:  //pjg++170928
		if (RunWnd.play == UI_RUN_MODE_HOME)
			UI_AngleMeasure_OnBnClickedBtnPause();
		else if (RunWnd.play != UI_RUN_MODE_PLAY)
			UI_AngleMeasure_OnBnClickedBtnAutoCheckAngle();
		else UI_AngleMeasure_OnBnClickedBtnSetAngle();
		break;
	case WM_KEYLONG:  //pjg++170928
		if (App_GetLongButtonCnt() > 1) break;
		if (RunWnd.play == UI_RUN_MODE_PLAY || RunWnd.play == UI_RUN_MODE_HOME) {
			UI_AngleMeasure_OnBnClickedBtnPause();
		}
		else {
			UI_AngleMeasure_OnBnLongClickedBtnHome();
		}
		break;
	default:
		break;
	}

	return 0;
}

void UI_AngleMeasure_Create(void)
{		
	//API_SetWndNum(UI_WND_MODE_PARENT);
	App_SetWndProcFnPtr(UI_AngleMeasure_WndProc);
	
	//hParent = API_CreateWindow("main", (LPCSTR)"i angb.bmp,0,0\r", WS_OVERLAPPEDWINDOW,
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_ANG_MEA][Setup2.language], WS_OVERLAPPEDWINDOW, 
			0, 0, 480, 272, hDeskTop, 0, 0);
	//App_SetUIProcess(UI_User_Process);
	UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.num = UI_WND_MODE_ANGLE_MEA;
}
#endif

///////////////////////////////////////////////////////////////////////////////
void UI_AutoAngleValue_OnBnClickedBtnLeft(void)
{
	//UI_AngleBtnStep_Create();
	UI_AdSet_Create();
}

void UI_AutoAngleValue_OnBnClickedBtnRight(void)
{
	//UI_AutoAngValue_Create();
}

void UI_AutoAngleValue_OnBnClickedBtnExit(void)
{
	//UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
}

void UI_AutoAngleValue_OnBnClickedBtnAavUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.AutoAngValue < 10) Setup3.AutoAngValue++;
	if(Setup3.AutoAngValue == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.AutoAngValue  > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AutoAngValue,1);
}

void UI_AutoAngleValue_OnBnClickedBtnAavDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.AutoAngValue > 1) Setup3.AutoAngValue--;
	if(Setup3.AutoAngValue == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.AutoAngValue < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AutoAngValue,2);
}

void UI_AutoAngleValue_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);

	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AutoAngValue,1);
	
}

LRESULT UI_AutoAngleValue_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_AutoAngleValue_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AutoAngleValue_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AutoAngleValue_OnBnClickedBtnExit();
				break;
			}
			return 0;
		/*case RID_SH_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SensHp_OnBnClickedBtnRight();
				break;
			}
			return 0;*/
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AutoAngleValue_OnBnClickedBtnAavUp();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AutoAngleValue_OnBnClickedBtnAavDown();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_AutoAngleValue_Create(void)
{
	App_SetWndProcFnPtr(UI_AutoAngleValue_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_AUTOAVS][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_AUTO_AGN_VALUE;
}

///////////////////////////////////////////////////////////////////////////////
void UI_AngleBtnStep_OnBnClickedBtnLeft(void)
{
	//UI_AngleMeaSens_Create();
	UI_AdSet_Create();
}

void UI_AngleBtnStep_OnBnClickedBtnRight(void)
{
	UI_AutoAngleValue_Create();
}

void UI_AngleBtnStep_OnBnClickedBtnExit(void)
{
	//UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
}

void UI_AngleBtnStep_OnBnClickedBtnBsUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.AngBtnStep < 10) Setup3.AngBtnStep++;
	if(Setup3.AngBtnStep == 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.AngBtnStep  > 0)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AngBtnStep,1);
}

void UI_AngleBtnStep_OnBnClickedBtnBsDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	if(Setup3.AngBtnStep > 1) Setup3.AngBtnStep--;
	if(Setup3.AngBtnStep == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.AngBtnStep < 10)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AngBtnStep,2);
}

void UI_AngleBtnStep_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);

	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_2PLACE, pos,113,Setup3.AngBtnStep,1);
	
}

LRESULT UI_AngleBtnStep_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_AngleBtnStep_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleBtnStep_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleBtnStep_OnBnClickedBtnExit();
				break;
			}
			return 0;
		/*case RID_SH_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SensHp_OnBnClickedBtnRight();
				break;
			}
			return 0;*/
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleBtnStep_OnBnClickedBtnBsUp();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleBtnStep_OnBnClickedBtnBsDown();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_AngleBtnStep_Create(void)
{
	App_SetWndProcFnPtr(UI_AngleBtnStep_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_ANGMS][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_AGN_BTN_STEP;
}

///////////////////////////////////////////////////////////////////////////////
void UI_AngleMeaSens_OnBnClickedBtnLeft(void)
{
	//UI_HomePos_Create();
	UI_AdSet_Create();
}

void UI_AngleMeaSens_OnBnClickedBtnRight(void)
{
	UI_AngleBtnStep_Create();
}

void UI_AngleMeaSens_OnBnClickedBtnExit(void)
{
	//UI_SaveParamToEEPROM(CommonBuf); //pjg++190813
	UI_Setup_Create();
}

void UI_AngleMeaSens_OnBnClickedBtnAgsUp(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.amSens < ANG_MEA_MAX_SENS_STEP) {
		Setup3.amSens++;
		//Setup3.amp.gapBk++;
		//AngChk.gapBk = Setup3.amp.gapBk;//*2;
		//AngChk.gap = AngChk.gapBk;
	}

	if(Setup3.amSens == ANG_MEA_MAX_SENS_STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nplus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_DISABLE);
	}
	if(Setup3.amSens > 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i minus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_PUSHED);
	}
	
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.amSens,1);
}

void UI_AngleMeaSens_OnBnClickedBtnAgsDown(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;
	
	if(Setup3.amSens > 1) Setup3.amSens--;
	//AngChk.gapBk = Setup3.amp.gapBk;//*2;
	//AngChk.gap = AngChk.gapBk;

	if(Setup3.amSens == 1)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nminus.bmp,345,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEM, BN_DISABLE);
	}
	if(Setup3.amSens < ANG_MEA_MAX_SENS_STEP)
	{
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i plus.bmp,286,102\r");
		App_SetButtonOption(RID_LP_BTN_PAUSEP, BN_PUSHED);
	}
	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.amSens,1);
}

void UI_AngleMeaSens_Init(void)
{
	int pos;
	
	if (Setup2.language == LT_CHINA) pos = 148;
	else pos = 158;

	API_CreateWindow("", pBtnInfo[RID_LP_BTN_LEFT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_LEFT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_LEFT, 0);
	//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nleft.bmp,44,222\r");
	/*API_CreateWindow("", pBtnInfo[RID_LP_BTN_EXIT*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_EXIT*2], ',', 2),
			116, 44, hParent, RID_LP_BTN_EXIT, 0);*/
	API_CreateWindow("", pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 1),
			GetNumFromString(pBtnInfo[(RID_PRO_BTN_EXIT+Setup2.language)*2], ',', 2),
			116, 45, hParent, RID_PRO_BTN_EXIT+Setup2.language, 0);
	//API_CreateWindow("", pBtnInfo[RID_PRO_BTN_RIGHT*2], BS_PUSHBUTTON, 
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 1),
	//		GetNumFromString(pBtnInfo[RID_PRO_BTN_RIGHT*2], ',', 2),
	//		116, 44, hParent, RID_PRO_BTN_RIGHT, 0);
	APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i nright.bmp,324,222\r");
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEP*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEP*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEP, 0);
	API_CreateWindow("", pBtnInfo[RID_LP_BTN_PAUSEM*2], BS_PUSHBUTTON, 
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 1),
			GetNumFromString(pBtnInfo[RID_LP_BTN_PAUSEM*2], ',', 2),
			58, 46, hParent, RID_LP_BTN_PAUSEM, 0);


	UI_LED_Control(LM_STAND_BY);

	//if (loginInfo.type == LIT_USER) UI_LoadPIFromEEPROM(PInfoWnd2.id, (uint8_t *)CommonBuf);

	UI_DisplayDecimalSel(UI_DISP_NUM_FNT9, UI_DISP_NUM_1PLACE, pos,113,Setup3.amSens,1);
	
}

LRESULT UI_AngleMeaSens_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		UI_AngleMeaSens_Init();
		break;
	case WM_COMMAND:
		switch(wParam) { // wParam¿« word lowø° id≥Ø∂Ûø»
		case RID_LP_BTN_LEFT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeaSens_OnBnClickedBtnLeft();
				break;
			}
			return 0;
		case RID_PRO_BTN_EXIT:
		case RID_PRO_BTN_EXITK:
		case RID_PRO_BTN_EXITC:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeaSens_OnBnClickedBtnExit();
				break;
			}
			return 0;
		/*case RID_SH_BTN_RIGHT:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_SensHp_OnBnClickedBtnRight();
				break;
			}
			return 0;*/
		case RID_LP_BTN_PAUSEP:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeaSens_OnBnClickedBtnAgsUp();
				break;
			}
			return 0;
		case RID_LP_BTN_PAUSEM:
			switch(lParam) {
			case BN_CLICKED: // push 
				UI_AngleMeaSens_OnBnClickedBtnAgsDown();
				break;
			}
			return 0;
		default:
			return 0;
		}
		break;
	case WM_PAINT:
		TLCD_DrawPicture((char *)lParam);
		break;
	case WM_SOUND:
		if (wParam == 0) 	TLCD_SoundOut(pSndInfo[lParam][Setup2.language]);
		else if (wParam == 1) TLCD_CtrlSound((char *)lParam);
		break;				
	default:
		break;
	}

	return 0;
}

void UI_AngleMeaSens_Create(void)
{
	App_SetWndProcFnPtr(UI_AngleMeaSens_WndProc);
	hParent = API_CreateWindow("main", (LPCSTR)pWndBgNameInfo[WND_SETUP_AD_MEA_SENS][Setup2.language], WS_OVERLAPPEDWINDOW,
			0, 0, 480, 272, hDeskTop, 0, 0);
	//UI_Wnd.prevNum = UI_Wnd.num;
	UI_Wnd.child_num = UI_WND_MODE_AGN_SENSE;
}

///////////////////////////////////////////////////////////////////////////////
void UI_SetWindowMode(uint8_t mode)
{
	//UI_WndNum =  mode;

	switch (mode) {
	case UI_WND_MODE_DESKTOP:
		break;
	//case UI_WND_MODE_PATIENT:
		//UI_PatientInfo_Create();
		//break;
	case UI_WND_MODE_ANGLE_SET_EX:
		//UI_Angle_Create();
		break;
	default:
		break;
	}

}

//pjg++190830
int UI_CheckMotorStatus(void)
{
	//int i;
	
	if (MotorDrv_IsFaultAction()) {
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		UI_SetMotorErrorCode();
		return 0;
	}
	if (MotorDrv_IsOverCurrent()) {
		//over cur error
		MotorDrv_Stop();
		UI_SetFunRunMode(UI_FRM_NONE);
		API_SetErrorCode(EC_MOTOR_OVERLOAD, EDT_DISP_HALT);
		return 0;
	}
	//when pause mode if it get extern power motor must move to down for motor gear is not broken
	if (RunWnd.play == UI_RUN_MODE_PAUSE || RunWnd.play == UI_RUN_MODE_HOME_PAUSE) {
		if (MotorDrv_IsStandbyCurrent() && UI_Time.tmp3_ms == 0) {
			//for (i = 0; i < MotorDrv_GetGapValueOnStandby(); i++)
//			MotorDrv_MoveDownWithValue((MotorDrv_GetGapValueOnStandby()*MotorDrv_GetGapValueOnStandby())/2);
			MotorDrv_SetFlagStandbyCurrent(0);
		}
	}
	return 1;
}

//Ω√Ω∫≈€ √ ±‚»≠≥™ »®¿∏∑Œ ¿Ãµø«œ¥¬ «¡∑Œ±◊∑•Ω∫πŸ ±∏«ˆ
void UI_AniProgress(uint32_t pos)
{
	switch (pos%10) {
	case 1:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lbrb.bmp,102,193\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,107,193\r");
		break;	
	case 2:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,136,193\r");
		break;
	case 3:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,165,193\r");
		break;
	case 4:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,195,193\r");
		break;
	case 5:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,225,193\r");
		break;
	case 6:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,255,193\r");
		break;
	case 7:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,285,193\r");
		break;
	case 8:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,315,193\r");
		break;
	case 9:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,343,193\r");
		break;
	case 0:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i fbar.bmp,99,190\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rbrb.bmp,107,193\r");
		/*//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,102,193\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lbrb.bmp,102,193\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lbrb.bmp,102,193\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,136,193\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,170,193\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,204,193\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,238,193\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,272,193\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,306,193\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,340,193\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i barc.bmp,344,193\r"); 
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lbrb.bmp,102,193\r");*/
		break;
	}
}

void UI_AniProgress2(uint32_t pos)
{
	switch (pos) {
	case 0:
                break;
	case 1:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i lsndb.bmp,102,176\r");
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,107,176\r");
		//APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i fbar.bmp,99,173\r"); 
		break;	
	case 2:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,132,176\r");
		break;
	case 3:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,157,176\r");
		break;
	case 4:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,182,176\r");
		break;
	case 5:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,207,176\r");
		break;	
	case 6:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,232,176\r");
		break;
	case 7:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,260,176\r");
		break;
	case 8:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,288,176\r");
		break;
	case 9:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,316,176\r");
		break;	
	case 10:
		APP_SendMessage(hParent, WM_PAINT, 0, (LPARAM)"i rsndb.bmp,344,176\r");
		break;
	}
}
			
#if 0
//int home_switch = 0;
void UI_Timer(uint16_t nIDEvent)
{
	
	switch (nIDEvent) {
	case TIMER_ID_0:    //init Ω√Ω∫≈€ √ ±‚»≠ «¡∑Œ±◊∑°Ω∫πŸ «•«ˆ
		//UI_SystemInit_Timer();
		break;
	case TIMER_ID_1:    //loading »®¿Œ¿∏∑Œ ¿Ãµø«“∂ß «•«ˆ
		UI_Loading_Timer(0);
		break;
	case TIMER_ID_2:    //playing ±∏µø«“∂ß Ω««‡
		if (RunWnd.time > 0){
			if(RunWnd.play != UI_RUN_MODE_HOME){
				if(Timer_sec >= 6){	//1min
					RunWnd.time--;
					PInfoWnd2.pi.totalTime++;
					Timer_sec = 0;
				}
				//PInfoWnd2.pi.totalRepeat += RunWnd.repeat;//Total_Counter;
			}
		}
		//UI_Run_Process();
		UI_Run_DisplayValue();
		break;
	case TIMER_ID_3:    //USB Saving
		loading++;
		if (loading > 10) {
			App_KillTimer(TIMER_ID_3);
			UI_Setup_Create();
			return;
		}
		UI_AniProgress2(loading);
        // Timer_sec = 0;
		break;
	/*case TIMER_ID_4:    
		loading++;
		
		if (loading > 10) {
			App_KillTimer(TIMER_ID_4);
			UI_Setup_Create();
			return;
		}
		UI_AniProgress2(loading);
                   // Timer_sec = 0;
		break;*/
	//default:
	//	break;
	case TIMER_ID_5:    //Product	
		//UI_Calibration_DisplayValue();
		break;
	case TIMER_ID_6:    //angle measure	
		//UI_AngleMeasure_Timer();
		break;
	}
}
#endif

