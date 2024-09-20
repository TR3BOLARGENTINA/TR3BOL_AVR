/*
 * @file display.c
 *
 * @author Gonzalo G. Fernandez
 */

// TO DO: REVISAR DELAY EN EL TOGGLE DE E

#include "lcdpcf8574.h"

volatile uint8_t dataport = 0x00;

int8_t lcd_init( void )
{
	pcf8574_init();
	
	// Se inicializa todos los pines de salida del modulo en 0
	dataport = 0x00;
	
	if( pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport ) == -1)
	{
		return -1;
	}
	
	/**
	* Delay de 15ms para que la tension de alimentacion del display
	* se establezca una vez alcanzados los 4.5V
	*/
	_delay_ms( 15 );
	
	/**
	*	Todavia con el display con interfaz de 8 bits, se realiza
	*	una escritura con 1s en D4 y D5
	*	dataport = 0b00110000
	*/
	dataport |= ( 1 << LCD_DATA4_PIN );
	dataport |= ( 1 << LCD_DATA5_PIN );
	
	if( pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport ) == -1)
	{
		return -1;
	}
	
	if( lcd_toggle_e() == -1 )
	{
		return -1;
	}
	
	// Delay de 4.1ms o mas (ver datasheet)
	_delay_ms( 8 );
		
	// Se repite el comando anterior
	if( lcd_toggle_e() == -1 )
	{
		return -1;
	}
	
	// Delay de 4.1ms o mas (ver datasheet)
	_delay_ms( 5 );
	
	// Se escribe por tercera y ultima vez el mismo comando
	if( lcd_toggle_e() == -1 )
	{
		return -1;
	}
	
	// Delay de 100us o mas
	_delay_us( 200 );
	
	/**
	*	Function set (Set interface to be 4 bits long.)
	*	Interface is 8 bits in length.
	*	dataport = 0b00100000
	*/
	dataport = 0x00;
	dataport |= ( 1 << LCD_DATA5_PIN );
	
	if( pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport ) == -1 )
	{
		return -1;
	}
	
	if( lcd_toggle_e() == -1 )
	{
		return -1;
	}
	
	_delay_us( 200 );
	
	if( lcd_command( lcd_function_set( 0, 1, 0 ) ) == -1 )
	{
		return -1;
	}
	
	if( lcd_command( LCD_DISPLAY_OFF ) == -1 )
	{
		return -1;
	}
	
	if( lcd_command( LCD_CLEAR_DISPLAY ) == -1 )
	{
		return -1;
	}
	
	if( lcd_command( lcd_entry_mode_set( 1, 0 ) ) == -1 )
	{
		return -1;
	}
	
	if( lcd_command( lcd_display_onoff( 1, 0, 0 ) ) == -1 )
	{
		return -1;
	}
	
	return 0;
}

int8_t lcd_read( uint8_t rs_state )
{
	// Ver datasheet HD44780U, pag. 9
	
	uint8_t data = 0x00;
	
	dataport |= ( 1 << LCD_RW_PIN ); // Lectura
	
	if( rs_state )
	{
		// Si RS=1 y R/W=1 (lectura), DR lee como operacion interna
		// DDRAM o CGRAM 
		dataport |= ( 1 << LCD_RS_PIN );
	}
	else
	{
		// Si RS=0 y R/W=1 (lectura), se lee busyflag (DB7) 
		// y AC (DB0 a DB6)
		dataport &= ~( 1 << LCD_RS_PIN );
	}
	
	int8_t aux;
	aux = pcf8574_setoutput(LCD_PCF8574_DEVICEID, dataport);
	if( aux < 0 )
	{
		return aux;
	}
	
	// Ver datasheet HD44780U, pag. 22
	
	// En interfaz de 4 bits, la transferencia de informacion se
	// realiza en dos operaciones: primero los 4 bits de mayor orden
	// y luego los 4 bits de menor orden
	
	// Lectura de los 4 bits de mayor orden:
	
	aux = pcf8574_setoutputpinhigh( LCD_PCF8574_DEVICEID, LCD_E_PIN ); // Pin E en ALTO
	if( aux < 0 )
	{
		return aux;
	}
	
	LCD_DELAY;
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA4_PIN ) )
	{
		data |= ( 1 << 4 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA5_PIN ) )
	{
		data |= ( 1 << 5 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA6_PIN ) ) 
	{
		data |= ( 1 << 6 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA7_PIN ) )
	{
		data |= ( 1 << 7 );
	}
	
	aux = pcf8574_setoutputpinlow( LCD_PCF8574_DEVICEID, LCD_E_PIN ); // Pin E en BAJO
	if( aux < 0 )
	{
		return aux;
	}
	
	LCD_DELAY;
	
	// Lectura de los 4 bits de menor orden:
	aux = pcf8574_setoutputpinhigh( LCD_PCF8574_DEVICEID, LCD_E_PIN ); // Pin E en ALTO
	if( aux < 0 )
	{
		return aux;
	}
	
	LCD_DELAY;
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA4_PIN ) )
	{
		data |= ( 1 << 0 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA5_PIN ) )
	{
		data |= ( 1 << 1 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA6_PIN ) )
	{
		data |= ( 1 << 2 );
	}
	
	if( !pcf8574_getinputpin( LCD_PCF8574_DEVICEID, LCD_DATA7_PIN ) )
	{
		data |= ( 1 << 3 );
	}
	
	aux = pcf8574_setoutputpinlow( LCD_PCF8574_DEVICEID, LCD_E_PIN ); // Pin E en BAJO
	if( aux < 0 )
	{
		return aux;
	}

	return data;
}

int8_t lcd_write( uint8_t data, uint8_t rs_state )
{
	// Ver datasheet HD44780U, pag. 9
	
	dataport &= ~( 1 << LCD_RW_PIN ); // Escritura
	
	if( rs_state )
	{
		// Si RS=1 y R/W=0 (escritura), DR escribe como operacion interna
		// DDRAM o CGRAM
		dataport |= ( 1 << LCD_RS_PIN );
	}
	else
	{
		// Si RS=0 y R/W=0 (lectura), IR escribe como operacion interna
		// (display clear, etc.)
		dataport &= ~( 1 << LCD_RS_PIN );
	}
	
	int8_t aux;
	aux = pcf8574_setoutput(LCD_PCF8574_DEVICEID, dataport);
	if( aux < 0 )
	{
		return aux;
	}
	
	// Ver datasheet HD44780U, pag. 33
	
	// En interfaz de 4 bits, la transferencia de informacion se
	// realiza en dos operaciones: primero los 4 bits de mayor orden
	// y luego los 4 bits de menor orden
	
	// Escritura de los 4 bits de mayor orden:
	
	// Reset de los bits a escribir
	dataport &= ~( 1 << LCD_DATA7_PIN );
	dataport &= ~( 1 << LCD_DATA6_PIN );
	dataport &= ~( 1 << LCD_DATA5_PIN );
	dataport &= ~( 1 << LCD_DATA4_PIN );
	
	if( data & ( 1 << 7 ) )
	{
		dataport |= ( 1 << LCD_DATA7_PIN );
	}
	
	if( data & ( 1 << 6 ) )
	{
		dataport |= ( 1 << LCD_DATA6_PIN );
	}
	
	if( data & ( 1 << 5 ) )
	{
		dataport |= ( 1 << LCD_DATA5_PIN );
	}
	
	if( data & ( 1 << 4 ) )
	{
		dataport |= ( 1 << LCD_DATA4_PIN );
	}
	
	aux = pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport );
	if( aux < 0 )
	{
		return aux;
	}
	
	aux = lcd_toggle_e();
	if( aux < 0 )
	{
		return aux;
	}
	
	// Escritura de los 4 bits de menor orden:
	
	// Reset de los bits a escribir
	dataport &= ~( 1 << LCD_DATA7_PIN );
	dataport &= ~( 1 << LCD_DATA6_PIN );
	dataport &= ~( 1 << LCD_DATA5_PIN );
	dataport &= ~( 1 << LCD_DATA4_PIN );
	
	if( data & ( 1 << 3 ) )
	{
		dataport |= ( 1 << LCD_DATA7_PIN );
	}
	
	if( data & ( 1 << 2 ) )
	{
		dataport |= ( 1 << LCD_DATA6_PIN );
	}
	
	if( data & ( 1 << 1 ) )
	{
		dataport |= ( 1 << LCD_DATA5_PIN );
	}
	
	if( data & ( 1 << 0 ) )
	{
		dataport |= ( 1 << LCD_DATA4_PIN );
	}
	
	aux = pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport );
	if( aux < 0 )
	{
		return aux;
	}
	
	aux = lcd_toggle_e();
	if( aux < 0 )
	{
		return aux;
	}
	
	// Pines en ALTO (inactivos)
	dataport |= ( 1 << LCD_DATA7_PIN );
	dataport |= ( 1 << LCD_DATA6_PIN );
	dataport |= ( 1 << LCD_DATA5_PIN );
	dataport |= ( 1 << LCD_DATA4_PIN );
	
	aux = pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport );
	if( aux < 0 )
	{
		return aux;
	}
	
	return 0;
}

int8_t lcd_command( uint8_t command )
{
	int8_t aux;
	aux = lcd_waitbusy();
	if( aux < 0 )
	{
		return aux;
	}
	aux = lcd_write( command, 0 );
	if( aux < 0 )
	{
		return aux;
	}
	
	return 0;
}

uint8_t lcd_entry_mode_set( uint8_t optionID, uint8_t optionS )
{
	uint8_t command = 0b00000100;
	
	if( optionID )
	{
		command |= ( 1 << LCD_ENTRY_MODE_ID );
	}
	
	if( optionS )
	{
		command |= ( 1 << LCD_ENTRY_MODE_S );
	}
	
	return command;
}

uint8_t lcd_display_onoff( uint8_t optionD, uint8_t optionC, uint8_t optionB )
{
	uint8_t command = 0b00001000;
	
	if( optionD )
	{
		command |= ( 1 << LCD_DISPLAY_ONOFF_D );
	}
	
	if( optionC )
	{
		command |= ( 1 << LCD_DISPLAY_ONOFF_C );
	}
	
	if( optionB )
	{
		command |= ( 1 << LCD_DISPLAY_ONOFF_B );
	}
	
	return command;
}

uint8_t lcd_function_set( uint8_t optionDL, uint8_t optionN, uint8_t optionF )
{
	uint8_t command = 0b00100000;
	
	if( optionDL )
	{
		command |= ( 1 << LCD_FUNCTION_SET_DL );
	}
	
	if( optionN )
	{
		command |= ( 1 << LCD_FUNCTION_SET_N );
	}
	
	if( optionF )
	{
		command |= ( 1 << LCD_FUNCTION_SET_F );
	}
	
	return command;
}

int8_t lcd_set_ddram( uint8_t address )
{
	if( address > 0x7F )
	{
		return -2;
	}
	
	uint8_t command = 0b10000000 + address;
	
	return command;
}

int8_t lcd_read_bf( void )
{
	int8_t aux;
	aux = lcd_read( 0 );
	
	if( aux < 0 )
	{
		return aux;
	}
	
	if ( aux & (1 << LCD_BUSY_FLAG ) )
	{
		return 1;
	}
	
	return 0;
}

int8_t lcd_waitbusy( void )
{
	int8_t aux;
	aux = lcd_read_bf();
	if( aux < 0 )
	{
		return aux;
	}
	
	uint8_t iter = 0;
	
	while( aux )
	{
		// Esperar que se desocupe el display
		_delay_us( 1 ); //1us
		if( iter > BUSY_LIMIT )
		{
			return -1;
		}
		iter++;
	}
	
	// Ver datasheet HD44780U, pag. 25
	// Actualizacion de Adress Counter (4us??)
	_delay_us( 4 );
	
	return lcd_read( 0 ); // Devolver lectura AC
}

int8_t lcd_toggle_e( void )
{
	int8_t aux;
	aux = pcf8574_setoutputpin( LCD_PCF8574_DEVICEID, LCD_E_PIN, 1 );
	if( aux < 0 )
	{
		return aux;
	}
	
	LCD_DELAY;
	
	aux = pcf8574_setoutputpin( LCD_PCF8574_DEVICEID, LCD_E_PIN, 0 );
	if( aux < 0 )
	{
		return aux;
	}
	
	return 0;
}

void lcd_led( uint8_t led_status )
{
	if( led_status )
	{
		dataport &= ~( 1 << LCD_LED_PIN );
	}
	else
	{
		dataport |= ( 1 << LCD_LED_PIN );
	}
	
	pcf8574_setoutput( LCD_PCF8574_DEVICEID, dataport );
}

int8_t lcd_gotoxy( uint8_t x, uint8_t y )
{
	// TO DO: x validation
	if( y == 0 )
	{
		if( lcd_command( lcd_set_ddram( LCD_START_LINE1 + x ) ) == -1 )
		{
			return -1;
		}
	}
	else if( y == 1 )
	{
		if( lcd_command( lcd_set_ddram( LCD_START_LINE2 + x ) ) == -1 )
		{
			return -1;
		}
	}
	else
	{
		return -2;
	}
	
	return 0;
}

void lcd_newline( uint8_t currentPosition )
{
	register uint8_t addressCounter;
	
	if( currentPosition < LCD_START_LINE2 )
	{
		addressCounter = LCD_START_LINE2;
	}
	else
	{
		addressCounter = LCD_START_LINE1;
	}
	
	lcd_command( lcd_set_ddram( addressCounter ) );
}

void lcd_putc( char c )
{
	uint8_t currentPosition;
	
	currentPosition = lcd_waitbusy();
	
	if ( c == '\n' )
	{
		lcd_newline( currentPosition );
	}
	else
	{
		if ( currentPosition == LCD_START_LINE1 + LCD_DISP_LENGTH )
		{
			lcd_write( lcd_set_ddram( LCD_START_LINE2 ), 0 );
		}
		else if( currentPosition == LCD_START_LINE2 + LCD_DISP_LENGTH )
		{
			lcd_write( lcd_set_ddram( LCD_START_LINE1 ), 0 );
		}
		
		currentPosition = lcd_waitbusy();
		
		lcd_write( c, 1 );
	}
}

void lcd_puts( const char *s )
{
	register char c;
	 
	while ( ( c = *s++ ) ) {
		lcd_putc(c);
	}
}