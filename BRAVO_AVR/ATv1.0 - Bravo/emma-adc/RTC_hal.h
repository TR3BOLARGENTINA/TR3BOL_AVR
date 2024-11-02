/*
 * twi_hal.h
 *
 * Created: 2020/11/01 18:07:26
 *  Author: DevilBinder
 */ 


#ifndef TWI_HAL_H_
#define TWI_HAL_H_

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

#define RTC_ADDR (0x68)

#define TWI_TIMEOUT 16000

#define TWI_START		0x08
#define TWI_RSTART		0x10
#define TWIT_ADDR_ACK	0x18
#define TWIT_ADDR_NACK	0x20
#define TWIT_DATA_ACK	0x28
#define TWIT_DATA_NACK	0x30

#define TWIR_ADDR_ACK	0x40
#define TWIR_ADDR_NACK	0x48
#define TWIR_DATA_ACK	0x50
#define TWIR_DATA_NACK	0x58

#define TWI_ERROR		0x38
#define TWI_NONE		0xF8

enum{
	TWI_OK,
	TWI_ERROR_START,
	TWI_ERROR_RSTART,
	TWI_NACK
};

// -------------------- SEG --MIN --HORA----------DIA---MES---AÑO
extern uint8_t rtc_data[7]; // = {0x00,0x25, 0x14,0x07,0x10,0x01,0x23};
//----------------- 80 , 89 , 35 ,  7 , 49 , 16 , 32

extern volatile uint8_t fT;

void configuraRTC ( void );
void leeRTC ();
uint8_t RTC_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);
uint8_t RTC_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);

#endif /* TWI_HAL_H_ */