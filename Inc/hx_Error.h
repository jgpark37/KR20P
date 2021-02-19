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
	//����̹� IC
	EC_MDIC_UNDER_VOL,			//������
	EC_MDIC_HIGH_TEMP,			//���
	EC_MDIC_HALL,					//Ȧ ���� ����
	EC_MDIC_SHORT_GND,			//�׶���� �ܶ�
	EC_MDIC_SHORT_VCC,			//�������� �ܶ�
	EC_MDIC_MOTOR_SHORT,			//���� �ܶ�
	EC_MDIC_LOW_CUR,				//������
	//���Ͱ���
	EC_MOTOR_OVERLOAD	= 20,	//���� ������
	//LCD		
	EC_LCD_COMM			= 40,	//LCD�� ��� ����
	//HOME-IN		
	EC_HOMEIN				= 60,	//Ȩ�� ����
	EC_OVER_RANGE,				//���� �� ���� ���� ��� ��� 
	//USB		
	EC_USB_DISK_FAIL		= 80,	//USB �ν� �ȵ�
	EC_USB_OPEN,					//���� ���� �ȵ�
	EC_USB_WRITE,					//���� ���� �ȵ�
	EC_USB_READ,					//���� �б� �ȵ�
	EC_USB_MOUNT,					//pjg++190830
	EC_USB_LINK,					//pjg++190830
	EC_USB_SEEK,
	EC_USB_MEM_FULL,				//pjg++190830
	//EEPROM		
	EC_EEP_LINK				= 90,
	EC_EEP_MOUNT,				//pjg++190830
	EC_EEP_FORMAT,				//pjg++190830
	EC_EEP_OPEN,				//pjg++190830
	EC_EEP_READ,				//�б� �ȵ�
	EC_EEP_WRITE,					//95 ���� �ȵ�
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
	EC_LOGIN_PWD_ERR		= 110,				//��ȣ5ȸ Ʋ��
	//etc
	EC_EMPTY_SYSINFO,				//�ý��� �Ķ���� ����. (Calibration ���� )
	EC_NO_FIRMWARE,				//�߿��� ����(DFU)
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

