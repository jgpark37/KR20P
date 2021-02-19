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
* source
* code is strictly prohibited, unless the prior written consent of Hexar Systems, Inc.
* is obtained.
*
* Filename		: hx_i2c.c
* Programmer(s)	: PJG
*                   	  Other name if it be
* MCU 			: STM32F407
* Compiler		: IAR
* Created      	: 2020/02/19
* Description	: I2C Driver
*******************************************************************************
*
*/

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "hx_Message.h"

/* Private define ------------------------------------------------------------*/
#define USE_I2C_TYPE1

/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern HWND hDeskTop, hParent, hChild[WINDOW_CHILD_NUM];


/* Private function prototypes -----------------------------------------------*/
//extern void OS_Init(void);


////////////////////////////Function////////////////////////////////////////
#ifdef USE_I2C_TYPE1
//출처: https://progdev.tistory.com/9 [플머의 개발 연구소]
void i2c_init(void)
{
    P0IO |= 0xC0;  //Set SCL, SDA to output
    SCL = 1;  // Set SCL, SDA High
    SDA = 1;
}
 
void i2c_start(void)
{
    //start
    P0IO |= 0xC0;  //Set SCL, SDA to output
    SDA = 1;
    SCL = 1;  // Set SCL, SDA High
    Delay_us(7);
 
    SDA = 0;    // Clear SDA
    Delay_us(7);
    SCL = 0;    // Clear SCL
    Delay_us(7);
}
 
void i2c_stop(void)
{
    P0IO |= 0x80;   // Set SDA to output
    SDA = 0;    // Clear SDA Low
    Delay_us(7);
 
    SCL = 1;    // Set SCL High
    Delay_us(7); 
    SDA = 1; // Set SDA High
 
    P0IO &= (~0xC0);    // Set SDA to Input
}
 
void write_i2c_byte(unsigned char byte)
{
    unsigned char i = 0;
 
    P0IO |= 0x80;       // Set SDA to output
     
    for (i = 0; i < 8 ; i++)
    {
        if((byte & 0x80)==0x80) SDA = 1;    // Set SDA High
        else                    SDA = 0;    // Clear SDA Low
 
        SCL = 1;        // Set SCL High, Clock data
        _nop_();
        byte = byte << 1; // Shift data in buffer right one
        SCL = 0;        // Clear SCL
        _nop_();
    }
    SDA = 0; //listen for ACK
    P0IO &= (~0x80);
 
    SCL = 1;        
    _nop_();_nop_();
    SCL = 0; 
    _nop_();_nop_(); //Clear SCL.
    P0IO |= 0x80;       // Set SDA to Output
}
 
unsigned char read_i2c_byte(unsigned char ch)
{
    unsigned char i, buff=0;
 
    P0IO &= (~0x80);    // Set SDA to input
 
    for(i=0; i<8; i++)
    {
        _nop_();_nop_();
        SCL = 1;    
        _nop_();_nop_();// Set SCL High, Clock bit out
        buff <<= 1;
 
        // Read data on SDA pin
        if ((P0&0x80) == 0x80) {
            buff |= 0x01;
        }
        SCL = 0; // Clear SCL
        _nop_();_nop_();
    }
    if(ch == 0) //ACK
    {
        SDA = 1; //SDA HIGH.
    }
    else //NACK.
    {
        SDA = 0; //SDA LOW.
    }
    SCL = 1;        
    _nop_();_nop_();
    SCL = 0; //SCL LOW.
    SDA = 1; //SDA HIGH.
    _nop_();_nop_();
    P0IO |= 0x80;   // Set SDA to Output
 
    return buff;
}
 
unsigned int read_word(unsigned char slave, unsigned char page_address, unsigned char index_address)
{
    unsigned int temp;
    unsigned char ucHibyte = 0;
    unsigned char ucLobyte = 0;
    unsigned char ucNack = 0;
 
    ucHibyte=0;
    ucLobyte=0;
 
    i2c_start();
    write_i2c_byte(slave);
    write_i2c_byte(page_address);
    write_i2c_byte(index_address);
 
    i2c_start();
    write_i2c_byte(slave+1);
 
    ucLobyte=read_i2c_byte(1);
    //ucHibyte=read_i2c_byte(1);
     
    i2c_stop();
 
    temp = (ucHibyte<<8) + ucLobyte;
 
    return temp;
}
 
void write_word(unsigned char slave, unsigned char page_address, unsigned char index_address, unsigned char value)
{
    i2c_start();
    write_i2c_byte(slave);
    write_i2c_byte(page_address);
    write_i2c_byte(index_address);
    write_i2c_byte(value);
    i2c_stop();
}

#else

// Port for the I2C
#define I2C_DDR DDRD
#define I2C_PIN PIND
#define I2C_PORT PORTD

// Pins to be used in the bit banging
#define I2C_CLK 0
#define I2C_DAT 1

#define I2C_DATA_HI() I2C_DDR &= ~( 1 << I2C_DAT );
#define I2C_DATA_LO() I2C_DDR |= ( 1 << I2C_DAT );

#define I2C_CLOCK_HI() I2C_DDR &= ~( 1 << I2C_CLK );
#define I2C_CLOCK_LO() I2C_DDR |= ( 1 << I2C_CLK );

void I2C_WriteBit( unsigned char c )
{
	if ( c > 0 )
	{
		I2C_DATA_HI();
	}
	else
	{
		I2C_DATA_LO();
	}

	I2C_CLOCK_HI();
	_delay_ms(1);

	I2C_CLOCK_LO();
	_delay_ms(1);

	if ( c > 0 )
	{
		I2C_DATA_LO();
	}

	_delay_ms(1);

}

unsigned char I2C_ReadBit()
{
	I2C_DATA_HI();

	I2C_CLOCK_HI();
	_delay_ms(1);

	unsigned char c = I2C_PIN;

	I2C_CLOCK_LO();
	_delay_ms(1);

	return ( c >> I2C_DAT ) & 1;
}

// Inits bitbanging port, must be called before using the functions below
//
void I2C_Init()
{
	I2C_PORT &= ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );

	I2C_CLOCK_HI();
	I2C_DATA_HI();

	_delay_ms(1);
}

// Send a START Condition
//
void I2C_Start()
{
	// set both to high at the same time
	I2C_DDR &= ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );
	_delay_ms(1);

	I2C_DATA_LO();
	_delay_ms(1);

	I2C_CLOCK_LO();
	_delay_ms(1);
}

// Send a STOP Condition
//
void I2C_Stop()
{
	I2C_CLOCK_HI();
	_delay_ms(1);

	I2C_DATA_HI();
	_delay_ms(1);
}

// write a byte to the I2C slave device
//
unsigned char I2C_Write( unsigned char c )
{
	for ( char i=0;i<8;i++)
	{
		I2C_WriteBit( c & 128 );
		c<<=1;
	}

	return I2C_ReadBit();
}


// read a byte from the I2C slave device
//
unsigned char I2C_Read( unsigned char ack )
{
	unsigned char res = 0;

	for ( char i=0;i<8;i++)
	{
		res <<= 1;
		res |= I2C_ReadBit();
	}

	if ( ack > 0)
	{
		I2C_WriteBit( 0 );
	}
	else
	{
		I2C_WriteBit( 1 );
	}

	_delay_ms(1);

	return res;
}
#endif


