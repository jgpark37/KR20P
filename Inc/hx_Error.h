/*
*******************************************************************************
*
*                              Hexar Systems, Inc.
*                      104, Factory Experiment Bldg, No41.55
*              Hanyangdaehak-ro, Sangnok-gu, Ansan, Gyeonggi-do, Korea
*
*                (c) Copyright 2017, Hexar Systems, Inc., Sangnok-gu, Ansan
*
* All rights reserved. Hexar Systemsâ€™s source code is an unpublished work and the
* use of a copyright notice does not imply otherwise. This source code contains
* confidential, trade secret material of Hexar Systems, Inc. Any attempt or participation
* in deciphering, decoding, reverse engineering or in any way altering the 
source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_Error.h
* Programmer(s)	: PJG
*                 Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2019/09/6
* Description		: error message
*******************************************************************************
*
*/

#ifndef _ERROR_H_
#define _ERROR_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define OS_ERROR_BUF_SIZE		5

/* Private typedef -----------------------------------------------------------*/
enum _OS_ERROR_CODE {
	//µå¶óÀÌ¹ö IC
	EC_MDIC_UNDER_VOL,			//ÀúÀü¾Ð
	EC_MDIC_HIGH_TEMP,			//°í¿Â
	EC_MDIC_HALL,					//È¦ ¼¾½Ì ¿¡·¯
	EC_MDIC_SHORT_GND,			//±×¶ó¿îµå·Î ´Ü¶ô
	EC_MDIC_SHORT_VCC,			//Àü¿øÀ¸·Î ´Ü¶ô
	EC_MDIC_MOTOR_SHORT,			//¸ðÅÍ ´Ü¶ô
	EC_MDIC_LOW_CUR,				//ÀúÀü·ù
	//¸ðÅÍ°ü·Ã
	EC_MOTOR_OVERLOAD	= 20,	//¸ðÅÍ °úºÎÇÏ
	//LCD		
	EC_LCD_COMM			= 40,	//LCD¿Í Åë½Å ¿¡·¯
	//HOME-IN		
	EC_HOMEIN				= 60,	//È¨ÀÎ ¿¡·¯
	EC_OVER_RANGE,				//±¸µ¿ Áß Á¦ÇÑ ¹üÀ§ ¹þ¾î³­ °æ¿ì 
	//USB		
	EC_USB_DISK_FAIL		= 80,	//USB ÀÎ½Ä ¾ÈµÊ
	EC_USB_OPEN,					//ÆÄÀÏ ¿­±â ¾ÈµÊ
	EC_USB_WRITE,					//ÆÄÀÏ ¾²±â ¾ÈµÊ
	EC_USB_READ,					//ÆÄÀÏ ÀÐ±â ¾ÈµÊ
	EC_USB_MOUNT,					//pjg++190830
	EC_USB_LINK,					//pjg++190830
	EC_USB_SEEK,
	EC_USB_MEM_FULL,				//pjg++190830
	//EEPROM		
	EC_EEP_LINK				= 90,
	EC_EEP_MOUNT,				//pjg++190830
	EC_EEP_FORMAT,				//pjg++190830
	EC_EEP_OPEN,				//pjg++190830
	EC_EEP_READ,				//ÀÐ±â ¾ÈµÊ
	EC_EEP_WRITE,					//95 ¾²±â ¾ÈµÊ
	EC_EEP_SEEK,
	EC_EEP_MKDIR,
	EC_EEP_NOFILE,
	EC_EEP_MEM_FULL,				//pjg++190830
	//EC_EEP_DISK_FAIL,			//pjg++190830
	EC_EEP_FILE_SIZE,			//100
	EC_EEP_DEL,
	EC_EEP_DATA,				//pjg++191106
	EC_EEP_DATA_FMT,			//pjg++191201 : data format wrong
	//Login
	EC_LOGIN_PWD_ERR		= 110,				//¾ÏÈ£5È¸ Æ²¸²
	//etc
	EC_EMPTY_SYSINFO,				//½Ã½ºÅÛ ÆÄ¶ó¹ÌÅÍ ¾øÀ½. (Calibration ½ÇÇà )
	EC_NO_FIRMWARE,				//Æß¿þ¾î ¾øÀ½(DFU)
	Error_MAX
};

enum _API_ERROR_DISP_TYPE {   //pjg++171220
	EDT_NONE,
	EDT_DISP,
	EDT_DISP_HALT,
	EDT_MAX
};

//pjg++171220
typedef struct _tagOS_ERROR_INFO {
	uint8_t head;
	uint8_t tail;
	uint16_t buf[OS_ERROR_BUF_SIZE];	//msb 2 bit : type info, 16384 error save
}ERROR_INFO;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern ERROR_INFO ErrorInfo;   //pjg++171220

/* Private function prototypes -----------------------------------------------*/

#endif //_ERROR_H_

////////////////////////////Function////////////////////////////////////////

