#include <inttypes.h>
#include <compat/twi.h>

#include "i2cmaster.h"

#ifdef UART_DEBUG_
#include "uart.h"
#endif // UART_DEBUG_

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000
#endif

#define ERR_ITER_MAX	1000000

/* I2C clock in Hz */
#define SCL_CLOCK  15250


/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void)
{
	  TWSR &=~ (3<<TWPS0);
	  TWBR = 72;
}/* i2c_init */


/*************************************************************************	
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
    //uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	
	// Chequeo por iteraciones
	uint8_t err_iter = 0;

	// wait until transmission completed
	while( !(TWCR & (1<<TWINT)) )
	{
		_delay_us(1);
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
				printf("Err: Timeout i2c_start\n");
			#endif // UART_DEBUG_
			return 1;
		}
	}

	// check value of TWI Status Register. Mask prescaler bits.
	if( (TWSR&0xF8)!=TW_START )	//Asegura que la condición de START fue transmitida
	return 1;
	//twst = TW_STATUS & 0xF8;
	//if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	err_iter = 0;

	// wail until transmission completed and ACK/NACK has been received
	while( !(TWCR & (1<<TWINT)) )
	{
		_delay_us(1);
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
				printf("Err: Timeout i2c_start\n");
			#endif // UART_DEBUG_
			return 1;
		}
	}

	// check value of TWI Status Register. Mask prescaler bits.
	//twst = TW_STATUS & 0xF8;
	//if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
	uint8_t twst = TW_STATUS&0xF8;	//Asegura que el esclavo mando ACK al modo R/W
	if ( (twst!=TW_MT_SLA_ACK) && (twst!=TW_MR_SLA_ACK) )
	return 1;
	
	return 0;

}/* i2c_start */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready
 
 Input:   address and transfer direction of I2C device
*************************************************************************/
char i2c_start_wait(unsigned char address)
{
    uint8_t   twst;

	// Chequeo por iteraciones
	uint8_t err_iter;

    while ( 1 )
    {
	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
		
		err_iter = 0;
    
    	// wait until transmission completed
    	while(!(TWCR & (1<<TWINT)))
		{
			_delay_us(1);
			
			err_iter++;
			
			if( err_iter > ERR_ITER_MAX )
			{
				#ifdef UART_DEBUG_
				printf("Err: Timeout i2c_start_wait\n");
				#endif // UART_DEBUG_
				return -1;
			}
		}
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	// send device address
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    
    	// wail until transmission completed
    	while(!(TWCR & (1<<TWINT)))
		{
			_delay_us(1);
			
			err_iter++;
			
			if( err_iter > ERR_ITER_MAX )
			{
				#ifdef UART_DEBUG_
				printf("Err: Timeout i2c_start_wait\n");
				#endif // UART_DEBUG_
				return -1;
			}
		}
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ) 
    	{    	    
    	    /* device busy, send stop condition to terminate write operation */
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        
	        // wait until stop condition is executed and bus released
	        while(TWCR & (1<<TWSTO))
			{
				_delay_us(1);
				
				err_iter++;
				
				if( err_iter > ERR_ITER_MAX )
				{
					#ifdef UART_DEBUG_
					printf("Err: Timeout i2c_start_wait\n");
					#endif // UART_DEBUG_
					return -1;
				}
			}
	        
    	    continue;
    	}
    	//if( twst != TW_MT_SLA_ACK) return 1;
    	break;
     }
	 
	 return 0;

}/* i2c_start_wait */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
          1 failed to access device
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
unsigned char i2c_stop(void)
{
    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// Chequeo por iteraciones
	uint8_t err_iter = 0;
	
	// wait until stop condition is executed and bus released
	while(TWCR & (1<<TWSTO))
	{
		_delay_us(1);
		
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
				printf("Err: Timeout i2c_stop\n");
			#endif // UART_DEBUG_
			return 1;
		}
	}
	
	return 0;

}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device
  
  Input:    byte to be transfered
  Return:   0 write successful 
            1 write failed
*************************************************************************/
unsigned char i2c_write( unsigned char data )
{	
    //uint8_t   twst;
    
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// Chequeo por iteraciones
	uint8_t err_iter = 0;

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)))
	{
		_delay_us(1);
		
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
			printf("Err: Timeout i2c_write\n");
			#endif // UART_DEBUG_
			return 1;
		}
	}

	// check value of TWI Status Register. Mask prescaler bits
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK )
	return 1;
	//twst = TW_STATUS & 0xF8;
	//if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}/* i2c_write */


/*************************************************************************
 Read one byte from the I2C device, request more data from device 
 
 Return:  byte read from I2C device
*************************************************************************/
char i2c_readAck(void)
{
	// Chequeo por iteraciones
	uint8_t err_iter = 0;
	
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	while(!(TWCR & (1<<TWINT)))
	{
		_delay_us(1);
		
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
			printf("Err: Timeout i2c_readAck\n");
			#endif // UART_DEBUG_
			return -1;
		}
	}

    return TWDR;

}/* i2c_readAck */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition 
 
 Return:  byte read from I2C device
*************************************************************************/
char i2c_readNak(void)
{
	// Chequeo por iteraciones
	uint8_t err_iter = 0;
	
	TWCR = (1<<TWINT) | (1<<TWEN);
	//TWCR &=~ (1<<TWEA);
	
	while(!(TWCR & (1<<TWINT)))
	{
		_delay_us(1);
		
		err_iter++;
		
		if( err_iter > ERR_ITER_MAX )
		{
			#ifdef UART_DEBUG_
			printf("Err: Timeout i2c_readNak\n");
			#endif // UART_DEBUG_
			return -1;
		}
	}
	
    return TWDR;

}/* i2c_readNak */
