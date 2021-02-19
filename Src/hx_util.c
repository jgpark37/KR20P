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
* Filename		: hx_util.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2017/02/21
* Description		: utility
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hx_util.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

uint8_t ConvertDec2Hex(uint8_t data)
{
	if (data < 0x0a) return (data+'0');
	return (data + '7');
}

uint16_t ConvertString2U16(char *buf, char _char, char findpos)
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

float ConvertString2Float(uint8_t *buf, uint8_t cnt)
{
	int i;
	uint8_t minus;
	uint16_t data;
	float fdata;
	uint8_t point_num;
	uint8_t num_cnt;

	i = 0;
	minus = 0;
	point_num = 0;
	data = 0;
	num_cnt = 0;

#if 1	
	for (i = 0; i < cnt; i++) {
		switch (buf[i]) {
		case '-':
			minus = 1;
			break;
		case '+':
			minus = 0;
			break;
		case '.':
			point_num = 1;
			break;
		default:
			if (num_cnt > 0) data *= 10;
			data += (buf[i]-'0');
			num_cnt++;
			if (point_num > 0) point_num++;
			break;
		}
	}
	fdata = data;
	if (point_num) {
		for (i = 0; i < point_num-1; i++) fdata /= 10;
	}
		
	if (minus) fdata *= -1;
#else
	if(buf[0] == '-')
	{
		if(buf[2] == '.')      //-x.xx
			fdata = -1 * ( buf[1]-48 + (buf[3]-48)*0.1 + (buf[4]-48)*0.01 );            
		else if(buf[3] == '.')   //-xx.xx
			fdata = -1 * ( (buf[1]-48)*10 + (buf[2]-48) + (buf[4]-48)*0.1 + (buf[5]-48)*0.01 );
		else if(buf[4] == '.')   //-xxx.xx
			fdata = -1 * ( (buf[1]-48)*100 + (buf[2]-48)*10 + (buf[3]-48) + (buf[5]-48)*0.1 + (buf[6]-48)*0.01 );
	}
	else //if(buf[0] != '-')
	{
		if(buf[1] == '.')      //x.xx
			fdata = ( (buf[0]-48) + (buf[2]-48)*0.1 + (buf[3]-48)*0.01 );            
		else if(buf[2] == '.')   //xx.xx
			fdata = ( (buf[0]-48)*10 + (buf[1]-48) + (buf[3]-48)*0.1 + (buf[4]-48)*0.01 );
		else if(buf[3] == '.')   //xxx.xx
			fdata = ( (buf[0]-48)*100 + (buf[1]-48)*10 + (buf[2]-48) + (buf[4]-48)*0.1 + (buf[5]-48)*0.01 );
	}

	//ÃâÃ³: http://sparklingstar.tistory.com/182 [^.^]
#endif

	return fdata;
}

uint32_t Multiplier(int x, int n)
{
	int i;
	uint32_t temp;

	temp = 1;
	for (i = 0; i < n; i++) temp *= x;
    return temp;
}

void ConvertLong2String(int32_t data, char *buf, uint8_t maxbuf)
{
	uint8_t temp;
	int i;

    i = 0;
	if (data < 0) maxbuf--;
	buf[0] = data%10 + '0';
	if (maxbuf > 1) {
		for (i = 1; i < 10; i++) {
			if (i >= maxbuf) break;
			temp = data/Multiplier(10, i);
			if (temp == 0) break;
			buf[i] = temp + '0';
		}
	}
	if (data < 0) buf[i] = '-';
}

void ConvertFloat2String(float data, char *buf, uint8_t maxbuf)
{
	float ftemp;
	uint8_t temp;
	uint16_t temp16;
	int i;

	if (maxbuf < 6) return;
	i = 0;
	ftemp = data;
	temp = (uint8_t)ftemp;
	buf[i++] = temp+'0';
	buf[i++] = '.';
	ftemp *= 10;
	temp = (uint8_t)ftemp;
	buf[i++] = temp+'0';
	ftemp *= 10;
	temp = (uint8_t)ftemp;
	buf[i++] = temp%10+'0';
	ftemp *= 10;
	temp16 = (uint16_t)ftemp;
	temp = temp16%100;
	buf[i++] = temp%10+'0';
	buf[i] = 0;
}

uint32_t ConvertString2U32(uint8_t *buf, uint8_t len)
{
	int i;
	uint32_t data;

	data = 0;
	for (i = len-1; i >= 0; i--) {
		data += buf[i]*Multiplier(10, len-i-1);
	}
	return data;
}

void ConvertULong2String(int32_t data, uint8_t *buf, uint8_t maxbuf)
{
#if 1
	int32_t temp;
	int32_t udata;
	int i;

	if (maxbuf < 8) return;
	i = 0;
	
	if (data < 0) {
		buf[i++] = '-';
		udata = data;
		udata *= -1;
	}
	else udata = data;
	
	if (udata > 999999) {
		buf[i++] = udata/1000000 + '0';
		temp = udata%1000000;
		buf[i++] = temp/100000 + '0';
		temp = temp%100000;
		buf[i++] = temp/10000 + '0';
		temp = temp%10000;
		buf[i++] = temp/1000 + '0';
		temp = temp%1000;
		buf[i++] = temp/100 + '0';
		temp = temp%100;
		buf[i++] = temp/10 + '0';
		temp = temp%10;
		buf[i++] = temp + '0';
	}
	else if (udata > 99999) {
		buf[i++] = udata/100000 + '0';
		temp = udata%100000;
		buf[i++] = temp/10000 + '0';
		temp = temp%10000;
		buf[i++] = temp/1000 + '0';
		temp = temp%1000;
		buf[i++] = temp/100 + '0';
		temp = temp%100;
		buf[i++] = temp/10 + '0';
		temp = temp%10;
		buf[i++] = temp + '0';
	}
	else if (udata > 9999) {
		buf[i++] = udata/10000 + '0';
		temp = udata%10000;
		buf[i++] = temp/1000 + '0';
		temp = temp%1000;
		buf[i++] = temp/100 + '0';
		temp = temp%100;
		buf[i++] = temp/10 + '0';
		temp = temp%10;
		buf[i++] = temp + '0';
	}
	else if (udata > 999) {
		buf[i++] = udata/1000 + '0';
		temp = udata%1000;
		buf[i++] = temp/100 + '0';
		temp = temp%100;
		buf[i++] = temp/10 + '0';
		temp = temp%10;
		buf[i++] = temp + '0';
	}
	else if (udata > 99) {
		buf[i++] = udata/100 + '0';
		temp = udata%100;
		buf[i++] = temp/10 + '0';
		temp = temp%10;
		buf[i++] = temp + '0';
	}
	else if (udata > 9) {
		buf[i++] = udata/10 + '0';
		temp = udata%10;
		buf[i++] = temp + '0';
	}
	else {
		buf[i++] = udata + '0';
	}
	buf[i] = 0;

#else
	int i, j;
	//uint8_t temp;
	uint32_t temp32;
	uint8_t tempbuf[22];
	//uint32_t is 0 ~ 4294967295

	//if (maxbuf < 20) return;
	i = 0;
	tempbuf[0] = data%10 + '0';
	if (data > 9) {
		for (i = 1; i < maxbuf; i++) {
			if (i >= maxbuf) break;
			temp32 = data/Multiplier(10, i);
			if (temp32 == 0) {
				tempbuf[i] = 0;
				break;
			}
			else if (temp32 > 9) tempbuf[i] = temp32%10 + '0';
			else tempbuf[i] = (uint8_t)temp32 + '0';
		}
		i--;
	}
	j = i;
	for (; i >= 0; i--) {
		buf[i] = tempbuf[j-i];
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
uint16_t MovingAvg_Process(MOVING_AVG *pMvAvg, uint32_t data)
{
	uint16_t result;
	
	pMvAvg->pBuf[pMvAvg->savePos] = data;
	pMvAvg->sum += data;
	pMvAvg->savePos++;

	if (pMvAvg->savePos >= pMvAvg->bufSize) {
		pMvAvg->savePos = 0;
	}

	result = pMvAvg->sum/(pMvAvg->bufSize);
	pMvAvg->sum -= pMvAvg->pBuf[pMvAvg->delPos];
	pMvAvg->delPos++;
	if (pMvAvg->delPos >= pMvAvg->bufSize) {
		pMvAvg->delPos = 0;
	}
	return result;
}

void MovingAvg_Init(MOVING_AVG *pMvAvg, uint32_t *buf, uint16_t bufSize, uint16_t initData)
{
	uint16_t i;
	
	pMvAvg->pBuf = buf;
	pMvAvg->bufSize = bufSize;
	
	for (i = 0; i < bufSize; i++) {
		pMvAvg->pBuf[i] = initData;
		pMvAvg->sum += initData;
	}
	pMvAvg->delPos = 1;
}

