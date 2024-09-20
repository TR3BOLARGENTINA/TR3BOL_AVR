/*
 * display.h
 *
 * Created: 12/3/2020 10:05:45
 *	@author Gonzalo G. Fernandez
 */ 

/**
*	TO DO:
*		- Revisar que PC8574_P3 este conectado al LED del display
*/


#ifndef LCDPCF8574_H_
#define LCDPCF8574_H_

#include <string.h>

#include "pcf8574.h"

/**
*	Definicion de parametros del display
*/

/// Longitud del display en cantidad de caracteres
#define LCD_DISP_LENGTH		40

/// Longitud interna del display
#define LCD_LINE_LENGTH		0x40

/// Direccion DDRAM del primer caracter de la primera linea
#define LCD_START_LINE1  0x00

/// Direccion DDRAM del primer caracter de la segunda linea
#define LCD_START_LINE2  0x40

/// Direccion DDRAM del primer caracter de la tercera linea
//#define LCD_START_LINE3  0x14

/// Direccion DDRAM del primer caracter de la cuarta linea
//#define LCD_START_LINE4  0x54

/**
*	Definicion de conexion de display con modulo PCF8574
*/
#define LCD_DATA4_PIN	PCF8574_P4
#define LCD_DATA5_PIN	PCF8574_P5
#define LCD_DATA6_PIN	PCF8574_P6
#define LCD_DATA7_PIN	PCF8574_P7

#define LCD_RS_PIN		PCF8574_P0
#define LCD_RW_PIN		PCF8574_P1
#define LCD_E_PIN		PCF8574_P2
#define LCD_LED_PIN		PCF8574_P3


//#define LCD_DELAY	_delay_us( 10 )
#define LCD_DELAY	__asm__ __volatile__( "rjmp 1f\n 1:" );

/// Busy flag en DB7 en lectura (RS=0 y R/W=1)
#define LCD_BUSY_FLAG	7

/**
*	Instruccion Clear Display (Ver datasheet HD44780U, pag. 24 y 26):
*
*	| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 |
*/

#define LCD_CLEAR_DISPLAY	0x01	// 0b00000001

/**
*	Instruccion Return home (Ver datasheet HD44780U, pag. 24 y 26):
*
*	| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 |
*/

#define LCD_RETURN_HOME		0x02	// 0b00000010

/**
*	Instruccion Entry mode set (Ver datasheet HD44780U, pag. 24 y 26):
*
*	| 0 | 0 | 0 | 0 | 0 | 1 | I/D | S |
*
*	- I/D: 1 si incrementa DDRAM cuando se escribe caracter, 0 si decrementa
*	- S: 0 si hace shift del display a la derecha, 1 si hace shift a la izquierda
*/

#define LCD_ENTRY_MODE_ID	1
#define LCD_ENTRY_MODE_S	0

/**
*	Instruccion Display on/off control (Ver datasheet HD44780U, pag. 24 y 26):
*
*	| 0 | 0 | 0 | 0 | 1 | D | C | B |
*
*	- D: 1 si display ON y 0 si display OFF
*	- C: 1 si cursor se muestra y 0 si cursor se oculta
*	- B: 1 si cursor titila
*/

#define LCD_DISPLAY_ONOFF_D		2
#define LCD_DISPLAY_ONOFF_C		1
#define LCD_DISPLAY_ONOFF_B		0

/// Display on/off control: Display off
#define LCD_DISPLAY_OFF		0x08	//0b00001000

/**
*	Instruccion Function Set (Ver datasheet HD44780U, pag. 24 y 27):
*	
*	| 0 | 0 | 1 | DL | N | F | - | - |
*
*	- DL: 1 para interfaz de 8 bits, 0 para interfaz de 4 bits
*	- N: Numero de lineas del display (1 para display de dos lineas)
*	- F: Seteo de la fuente
*/

/// Function Set: Interfaz de 4 bits
#define LCD_FUNCTION_SET_DL	4
/// Function Set: Display de 2 lineas
#define LCD_FUNCTION_SET_N	3
/// Function Set: Fuente 5x7 puntos
#define LCD_FUNCTION_SET_F	2

// Variable asociada a salida del modulo I2C
extern volatile uint8_t dataport;

/**
*	@brief	Inicializacion del display
*	@param	void
*	@return none
*/
int8_t lcd_init( void );

/**
*	@brief
*	@param
*	@return
*/
int8_t lcd_read( uint8_t rs_state );

int8_t lcd_write( uint8_t data, uint8_t rs_state );

int8_t lcd_command( uint8_t command );

uint8_t lcd_entry_mode_set( uint8_t optionID, uint8_t optionS );

uint8_t lcd_display_onoff( uint8_t optionD, uint8_t optionC, uint8_t optionB );

uint8_t lcd_function_set( uint8_t optionDL, uint8_t optionN, uint8_t optionF );

int8_t lcd_set_ddram( uint8_t address );

int8_t lcd_read_bf( void );

#define BUSY_LIMIT	3000

int8_t lcd_waitbusy( void );

/**
*	@brief	Toggle del pin E del display
*	@param	void
*	@return none
*/
int8_t lcd_toggle_e( void );

void lcd_led( uint8_t led_status );

int8_t lcd_gotoxy( uint8_t x, uint8_t y );

void lcd_newline( uint8_t currentPosition );

void lcd_putc( char c );

void lcd_puts( const char *s );

#endif /* LCDPCF8574_H_ */