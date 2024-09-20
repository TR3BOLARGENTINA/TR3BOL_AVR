/**
 * @file pcf8574.h
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

#ifndef PCF8574_H_
#define PCF8574_H_

#include "i2cmaster.h"

//-----------------------------------------------------------------
// DIRECCIÓN DEL DISPOSITIVO
/*
	Los 8 bits de direccion del esclavo consisten en los siguiente:
	
	- Para módulo PCF8574
	| X | 0 | 1 | 0 | 0 | A2 | A1 | A0 |
	
	- Para módulo PCF8574A
	| X | 0 | 1 | 1 | 1 | A2 | A1 | A0 |
	
	Donde A0, A1 y A2 son programables por hardware en el modulo.
*/
uint8_t PCF8574_ADDRBASE;
uint8_t LCD_PCF8574_DEVICEID;

//-----------------------------------------------------------------

/* Maxima cantidad de dispositivos, depende de la dirección (3 bit) */
#define PCF8574_MAXDEVICES	8

/* Maxima cantidad de pines de modulo PCF8574 */
#define PCF8574_MAXPINS		8

/**
*	Definiciones de pines de modulo PCF8574
*/
#define PCF8574_P0	0
#define PCF8574_P1	1
#define PCF8574_P2	2
#define PCF8574_P3	3
#define PCF8574_P4	4
#define PCF8574_P5	5
#define PCF8574_P6	6
#define PCF8574_P7	7

volatile uint8_t pcf8574_pinstatus[PCF8574_MAXDEVICES];

/**
 *	@brief	Inicializacion del modulo PCF8574 que permite la comunicacion
 *			mediante protocolo I2C con el display.
 
 *	@param  void
 *	@return none
 */
void pcf8574_init( void );

/**
 *	@brief	Escritura de las 8 salidas del modulo PCF8574 (P0 a P7) a traves
 *			de comunicacion I2C.
 *
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param data		Entero sin signo de 8 bits, que corresponden los 
 *					respectivos valores a escribir de P0 a P7
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	0 si la escritura se realizo con exito
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					no es valido
 */
int8_t pcf8574_setoutput( uint8_t deviceid, uint8_t data );

/**
 *	@brief	Lectura de las 8 salidas del modulo PCF8574 (P0 a P7) a traves
 *			de comunicacion I2C.
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	entero de 8 bits con el estado de las salidas 
 *					del modulo PCF8574
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					no es valido
 */
int8_t pcf8574_getoutput( uint8_t deviceid );

/**
 *	@brief	Escritura de un unico pin de salida del modulo PCF8574 a traves de 
 *			comunicacion I2C, dejando el resto de los pines en el mismo
 *			estado en el que estaban.
 *
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param pin		Entero sin signo de 8 bits, que corresponde a algun pin
 *					del modulo, entre P0 y P7
 *	@param status	Estado del pin que se desa escribit, 1 o 0
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	0 si la escritura se realizo con exito
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					o el pin a escribir no es valido
 */
int8_t pcf8574_setoutputpin( uint8_t deviceid, uint8_t pin, uint8_t status );

/**
 *	@brief	Escritura de un unico pin de salida del modulo PCF8574 a traves de 
 *			comunicacion I2C en estado ALTO, dejando el resto de los pines en el mismo
 *			estado en el que estaban.
 *
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param pin		Entero sin signo de 8 bits, que corresponde a algun pin
 *					del modulo, entre P0 y P7
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	0 si la escritura se realizo con exito
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					o el pin a escribir no es valido
 */
int8_t pcf8574_setoutputpinhigh( uint8_t deviceid, uint8_t pin );

/**
 *	@brief	Escritura de un unico pin de salida del modulo PCF8574 a traves de 
 *			comunicacion I2C en estado BAJO, dejando el resto de los pines en el mismo
 *			estado en el que estaban.
 *
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param pin		Entero sin signo de 8 bits, que corresponde a algun pin
 *					del modulo, entre P0 y P7
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	0 si la escritura se realizo con exito
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					o el pin a escribir no es valido
 */
int8_t pcf8574_setoutputpinlow( uint8_t deviceid, uint8_t pin );

/**
 *	@brief	Lectura de un unico pin de salida del modulo PCF8574 a traves de 
 *			comunicacion I2C, dejando el resto de los pines en el mismo
 *			estado en el que estaban.
 *
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param pin		Entero sin signo de 8 bits, que corresponde a algun pin
 *					del modulo, entre P0 y P7
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	Entero con signo de 8 bits, con el estado del pin 
 *					leido 0 o 1
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					o el pin a escribir no es valido
 */
int8_t pcf8574_getoutputpin( uint8_t deviceid, uint8_t pin );

/**
 *	@brief	Lectura de las 8 entradas del modulo PCF8574 (P0 a P7) a traves
 *			de comunicacion I2C.
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	entero de 8 bits con la informacion de la lectura realizada
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					no es valido
 */
int8_t pcf8574_getinput( uint8_t deviceid );

/**
 *	@brief	Lectura de un unico pin del modulo PCF8574 a traves de 
 *			comunicacion I2C.
 *
 *	@param deviceid	ID del dispositivo a escribir. Puede ir de 0
 *					a la maxima cantidad de dispositivos (definicion 
 *					PCF8574_MAXDEVICES)
 *	@param pin		Entero sin signo de 8 bits, que corresponde a algun pin
 *					del modulo, entre P0 y P7
 *	
 *	@return			Los posibles resultados devueltos son los siguientes:
 *					-	el estado del pin leido (0 o 1)
 *					-	-1 Error: Problema de comunicacion con el esclavo,
 *					el dispositivo se encuentra ocupado
 *					-	-2 Error: El identificador de dispositivo (deviceid)
 *					o el pin a leer no es valido
 */
int8_t pcf8574_getinputpin( uint8_t deviceid, uint8_t pin );

#endif /* PCF8574_H_ */