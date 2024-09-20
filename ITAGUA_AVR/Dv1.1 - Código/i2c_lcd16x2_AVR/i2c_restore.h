/*
 * i2c_restore.h
 *
 * Created: 2/4/2020 17:46:02
 *  Author: Acer
 */ 


#ifndef I2C_RESTORE_H_
#define I2C_RESTORE_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>

#include "i2cmaster.h"

uint8_t i2c_clearbus( void );

#endif /* I2C_RESTORE_H_ */