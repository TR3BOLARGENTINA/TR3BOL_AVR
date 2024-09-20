/**
 * @file pcf8574.c
 *
 * @brief	Libreria con funciones necesarias para trabajar
 *			con modulo PCF8574 o PCF8574A mediante protocolo I2C
 *
 *	Contiene funcion de inicializacion del modulo, escritura
 *	de salidas y lectura de entradas.
 *	Los codigo de errores son los siguientes:
 *	- 0: Escritura exitosa
 *	- -1: Error de comunicacion I2C entre maestro y esclavo
 *	- -2: Error en un parametro al llamar a la funcion
 *
 * @date 12/3/2020
 * @author Gonzalo G. Fernandez
 */ 

#include "pcf8574.h"

void pcf8574_init( void )
{
	i2c_init(); // Inicialización de interfaz I2C del maestro
	//_delay_us( 10 );
	_delay_ms(2);
	
	// Reseteo del estado de los pines
	for( uint8_t i=0; i<PCF8574_MAXDEVICES; i++ )
	{
		pcf8574_pinstatus[i] = 0x00;
	}
}

int8_t pcf8574_setoutput( uint8_t deviceid, uint8_t data )
{
	if( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) )
	{
		pcf8574_pinstatus[deviceid] = data;
	
		if( i2c_start( ( (PCF8574_ADDRBASE | deviceid) << 1 ) | I2C_WRITE ) )
		{
			return -1;
		}
		
		if( i2c_write( data ) )
		{
			return -1;
		}
		
		if( i2c_stop() )
		{
			return -1;
		}
		
		return 0;
	}
	
	return -2;
}

int8_t pcf8574_getoutput( uint8_t deviceid )
{
	// Verificacion de que el ID del dispositivo se encuentra
	// en el rango admisible
	if( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) )
	{
		return pcf8574_pinstatus[deviceid];
	}
	
	return -2;
}

int8_t pcf8574_setoutputpin( uint8_t deviceid, uint8_t pin, uint8_t status )
{
	if( ( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) ) &&
			( (pin >= 0) && (pin < PCF8574_MAXPINS) ) )
	{
		uint8_t data = pcf8574_pinstatus[deviceid];
		
		if( status == 1)
		{
			data |= ( 1 << pin );
		}
		else if( status == 0)
		{
			data &= ~( 1 << pin );
		}
		
		pcf8574_pinstatus[deviceid] = data;
		
		return pcf8574_setoutput( deviceid, data );
	}
	
	return -2;
}

int8_t pcf8574_setoutputpinhigh( uint8_t deviceid, uint8_t pin )
{
	return pcf8574_setoutputpin( deviceid, pin, 1 );
}

int8_t pcf8574_setoutputpinlow( uint8_t deviceid, uint8_t pin )
{
	return pcf8574_setoutputpin( deviceid, pin, 0 );
}

int8_t pcf8574_getoutputpin( uint8_t deviceid, uint8_t pin )
{
	// Verificacion de que el ID del dispositivo se encuentra
	// en el rango admisible, al igual que el pin a leer
	if( ( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) ) &&
			( (pin >= 0) && (pin < PCF8574_MAXPINS) ) )
	{
		int8_t data = pcf8574_pinstatus[deviceid];
		
		// Lectura unicamente del pin a leer
		data = ( data >> pin ) & 0b00000001;
		
		return data;
	}
	
	return -2;
}

int8_t pcf8574_getinput( uint8_t deviceid )
{	
	// Verificacion de que el ID del dispositivo se encuentra
	// en el rango admisible
	if( ( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) ) )
	{
		if( i2c_start( ( (PCF8574_ADDRBASE | deviceid) << 1 ) | I2C_READ ) )
		{
			return -1;
		}
		
		
		// Lectura de un byte del dispositivo I2C
		int8_t data = ~i2c_readNak();
		
		//int8_t data = i2c_readNak();
		//if( data < 0 )
		//{
		//	return -1;
		//}
		//data = ~data;
		
		if( i2c_stop() )
		{
			return -1;
		}
		
		return data;
	}
	
	return -2;
}

int8_t pcf8574_getinputpin( uint8_t deviceid, uint8_t pin )
{
	// Verificacion de que el ID del dispositivo se encuentra
	// en el rango admisible, al igual que el pin a leer
	if( ( (deviceid >= 0) && (deviceid < PCF8574_MAXDEVICES) ) &&
			( (pin >= 0) && (pin < PCF8574_MAXPINS) ) )
	{
		int8_t data = pcf8574_getinput( deviceid );
		
		// Verificacion de que la lectura se realizo exitosamente
		if( data < 0 )
		{
			return data;
		}
		
		// Lectura unicamente del pin a leer
		data = (data >> pin) & 0b00000001;
		
		return data;
	}
	
	return -2;
}