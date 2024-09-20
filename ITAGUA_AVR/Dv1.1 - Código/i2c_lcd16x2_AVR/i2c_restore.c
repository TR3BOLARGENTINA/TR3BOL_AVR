/*
 * i2c_restore.c
 *
 * Created: 2/4/2020 17:45:44
 *  Author: Acer
 */ 

#include "i2c_restore.h"

uint8_t i2c_clearbus( void )
{
	#if defined(TWCR) && defined(TWEN)
	TWCR &= ~(1 << TWEN); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly	
	#endif
	
	// Make SDA (data) and SCL (clock) pins Inputs with pullup.
	DDRC &= ~(1 << DDC4);	// SDA
	
	DDRC &= ~(1 << DDC5);	// SCL
	
	_delay_ms(1000);
	
	uint8_t SCL_STATE = (PINC & (1 << PINC5)); // Estado de SCL
	
	if ( !SCL_STATE ) // Chequear si SCL está en BAJO
	{
		//printf("Error 1: Si la linea SCL no se libera, la placa no puede tomar el rol de maestro en la comunicación.\r\n");
		return 1; // Error en bus I2C. 
		/* Si la linea SCL no se libera, la placa no puede tomar el rol
		de maestro en la comunicación.*/ 
	}
	
	uint8_t SDA_STATE = ~(PINC & (1 << PINC4)); // Estado de SDA
	int8_t clockCount = 20;
	
	while (!SDA_STATE && (clockCount > 0) )
	{
		clockCount--;
		
		// Release de la línea SCL
		PORTC &= ~(1 << PORTC5);
		DDRC  |=  (1<<DDC5);	// Clock SCL BAJO
		
		_delay_us(10);
		
		// Release de la línea SCL
		PORTC &= ~(1 << PORTC5);
		DDRC  |=  (1<<DDC5);
		
		DDRC &= ~(1 << DDC5);
		
		_delay_us(10);
		
		SCL_STATE = (PINC & (1 << PINC5)); // Estado de SCL
		
		int8_t counter = 20;
		
		while ( !SCL_STATE && (counter > 0) )
		{
			counter--;
			_delay_ms(100);
			SCL_STATE = (PINC & (1 << PINC5)); // Estado de SCL
		}
		
		if ( !SCL_STATE ) // Chequear si SCL sigue en BAJO
		{
			//printf("Error 2: No se pudo limpiar el bus. Linea SCL mantenida en BAJO por dispositivo esclavo.\r\n");
			return 2; // Error en bus I2C
			/* No se pudo limpiar el bus. Linea SCL mantenida en BAJO 
			por dispositivo esclavo */
		}
		
		SDA_STATE = ~(PINC & (1 << PINC4)); // Estado de SDA
	}
	
	if ( !SDA_STATE ) // Chequear si SDA sigue en BAJO
	{
		//printf("Error 3: No se pudo limpiar el bus. Linea SDA mantenida en BAJO por dispositivo esclavo.\r\n");
		return 3; // Error en bus I2C
		/* No se pudo limpiar el bus. Linea SDA mantenida en BAJO
		por dispositivo esclavo */
	}
	
	i2c_init();
	//printf("CLEAN BUS\r\n");
	
	return 0;
}