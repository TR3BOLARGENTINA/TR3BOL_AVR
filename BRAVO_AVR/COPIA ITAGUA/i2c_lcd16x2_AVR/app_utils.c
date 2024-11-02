/*
 * app_utils.c
 *
 * Created: 13/4/2020 16:26:27
 *  Author: Acer
 */ 

#include "app_utils.h"

const uint16_t	lista_Tiempos[size_Dosis]	=	{24000,5200, 2607, 1317, 896, 685, 556, 468, 409, 363, 326, 298};
const uint16_t	lista_Dosis[size_Dosis]     =	{    1,   5,   10,   20,  30,  40,  50,  60,  70,  80,  90, 100};
				
volatile uint8_t display_watchdog_state = 0;
struct DisplayBuffer main_dispbuffer;

uint8_t EEMEM eeprom_HorasACT[cantidad_Horas] = {1,2,3,4,5,6,7,8,9,10,11,12};
uint8_t EEMEM eeprom_anchoA;
uint8_t EEMEM eeprom_anchoB;
uint8_t EEMEM eeprom_curvaA;
uint8_t EEMEM eeprom_curvaB;
uint8_t EEMEM eeprom_Desfasaje;
uint8_t EEMEM eeprom_Lunes;
uint8_t EEMEM eeprom_Martes;
uint8_t EEMEM eeprom_Miercoles;
uint8_t EEMEM eeprom_Jueves;
uint8_t EEMEM eeprom_Viernes;
uint8_t EEMEM eeprom_Sabado;
uint8_t EEMEM eeprom_Domingo;
uint8_t EEMEM eeprom_Ciclos;
uint8_t EEMEM eeprom_Duracion;
uint8_t EEMEM eeprom_Modo;
uint8_t EEMEM eeprom_Dosis_A;
uint8_t EEMEM eeprom_Dosis_B;
uint8_t EEMEM eeprom_PCF;


void iniciaPines(void)
{
	/*-------------	Placa Dv1.1 ------------
		PB0/PCINT0 	-> 	entrada P1		->   Menos Bomba A
		PB1			->	salida S2 (no usada)
		PB2			->	salida S1 (no usada)
		PB3 		->	salida Bomba B
		PB4			->	salida S3 (no usada)
		PB5			->	salida S4 (no usada)
		PB6-7		->	Cristal externo 16MHz
		PC0-1		->	Entradas analógicas (no usadas)
		PC2			->	entrada P5 (no usada)
		PC3			->	Habilita Display (HIGH)
		PC4-5		->	SDA/SCL, i2c lcd (pull-Up externo)
		PC6			->	Reset
		PD0			->	Uart Rx
		PD1 		->	Uart Tx
		PD2/PCINT18 ->	entrada P4		->	Más Bomba B (INT0)
		PD3/INT1 	-> 	entrada Px0		->	Paso por Cero
		PD4/PCINT20 -> 	entrada P3		->	Menos Bomba B
		PD5/PCINT21 -> 	entrada P2		->	Más Bomba A
		PD6 		-> 	buzzer (HIGH)
		PD7 		-> 	salida Bomba A	
	*/
	
	bomba_A_OUT;	bomba_A_OFF;
	bomba_B_OUT;	bomba_B_OFF;
	display_OUT;	display_OFF;
	buzzer_OUT;		buzzer_OFF;
	
	//Pines no usados --> salidas apagadas
	DDRB |=  (3<<DDB1);  PORTB &=~ (3<<PORTB1);  // PB1-2
	DDRB |=  (3<<DDB4);  PORTB &=~ (3<<PORTB4);  // PB4-5
	DDRC |=  (7<<DDC0);  PORTC &=~ (7<<PORTC0);  // PC0-1-2
	
	buzzer_N_times(3);
}

void iniciaVariables(void)
{
	tipoPCF8574 = eeprom_read_byte(&eeprom_PCF);
	if(tipoPCF8574  > 3)
	{
		tipoPCF8574  = 0;
		eeprom_write_byte(&eeprom_PCF,tipoPCF8574);
	}
	
	bomba_A_APAGADA = 0;
	bomba_B_APAGADA = 0;
	
	for(uint8_t j=0; j<cantidad_Horas; j++)
	{
		horas_act[j] = eeprom_read_byte(&eeprom_HorasACT[j]);
		if(horas_act[j]>23) 
		{
			horas_act[j] = 0;
			eeprom_write_byte(&eeprom_HorasACT[j],horas_act[j]);
		}
	}
	
	ANCHO_PULSO_A = eeprom_read_byte(&eeprom_anchoA);
	ANCHO_PULSO_B = eeprom_read_byte(&eeprom_anchoB);
	if(ANCHO_PULSO_A  < ancho_Min  || ANCHO_PULSO_A  > ancho_Max)
	{
		ANCHO_PULSO_A  = 40;
		eeprom_write_byte(&eeprom_anchoA,ANCHO_PULSO_A);
	}
	if(ANCHO_PULSO_B  < ancho_Min  || ANCHO_PULSO_B  > ancho_Max)
	{
		ANCHO_PULSO_B  = 40;
		eeprom_write_byte(&eeprom_anchoB,ANCHO_PULSO_B);
	}
	
	DESFASAJE = eeprom_read_byte(&eeprom_Desfasaje);
	if(DESFASAJE  < desfasaje_Min  || DESFASAJE  > desfasaje_Max)
	{
		DESFASAJE  = 50;
		eeprom_write_byte(&eeprom_Desfasaje,DESFASAJE);
	}
	
	CORRECCION_CURVA_A = eeprom_read_byte(&eeprom_curvaA);
	CORRECCION_CURVA_B = eeprom_read_byte(&eeprom_curvaB);
	if(CORRECCION_CURVA_A  < curva_Min  || CORRECCION_CURVA_A  > curva_Max)
	{
		CORRECCION_CURVA_A  = 0;
		eeprom_write_byte(&eeprom_curvaA,CORRECCION_CURVA_A);
	}
	if(CORRECCION_CURVA_B  < curva_Min  || CORRECCION_CURVA_B  > curva_Max)
	{
		CORRECCION_CURVA_B  = 0;
		eeprom_write_byte(&eeprom_curvaB,CORRECCION_CURVA_B);
	}
	
	lunes     = eeprom_read_byte(&eeprom_Lunes);
	martes    = eeprom_read_byte(&eeprom_Martes);
	miercoles = eeprom_read_byte(&eeprom_Miercoles);
	jueves    = eeprom_read_byte(&eeprom_Jueves);
	viernes   = eeprom_read_byte(&eeprom_Viernes);
	sabado    = eeprom_read_byte(&eeprom_Sabado);
	domingo   = eeprom_read_byte(&eeprom_Domingo);
	
	if(lunes     > 1) lunes = 0;
	if(martes    > 1) martes = 0;
	if(miercoles > 1) miercoles = 0;
	if(jueves    > 1) jueves = 0;
	if(viernes   > 1) viernes = 0;
	if(sabado    > 1) sabado = 0;
	if(domingo   > 1) domingo = 0;
	
	dosis_A  = eeprom_read_byte(&eeprom_Dosis_A);
	dosis_B  = eeprom_read_byte(&eeprom_Dosis_B);
	duracion = eeprom_read_byte(&eeprom_Duracion);
	ciclos   = eeprom_read_byte(&eeprom_Ciclos);
	
	if(dosis_A == dosis_Min-1) bomba_A_APAGADA = 1;
	else if(dosis_A  < dosis_Min  || dosis_A  > dosis_Max)
	{
		dosis_A  = 50;
		eeprom_write_byte(&eeprom_Dosis_A,dosis_A);
	}
	if(dosis_B == dosis_Min-1) bomba_B_APAGADA = 1;
	else if(dosis_B  < dosis_Min  || dosis_B  > dosis_Max)
	{
		dosis_B  = 50;
		eeprom_write_byte(&eeprom_Dosis_B,dosis_B);
	}
	if(duracion < duracion_Min || duracion > duracion_Max)
	{
		duracion = 15;
		eeprom_write_byte(&eeprom_Duracion,duracion);
	}
	if(ciclos   < ciclos_Min   || ciclos   > ciclos_Max)
	{
		ciclos   = 1;
		eeprom_write_byte(&eeprom_Ciclos,ciclos);
	}
	
	//Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: WiFi 1, 6: WiFi 2
	modo     = eeprom_read_byte(&eeprom_Modo);	 
	if (modo==2)	  estado = s_timer;
	else if(modo==4)  estado = s_enclavado;
	else 
	{
		modo = 0;
		eeprom_write_byte(&eeprom_Modo, modo);
		estado = s_apagado;
	}
	estadoAnterior = estado;
		
	flag_px0 = 0;
	
	tiempo_A = 0;
	tiempo_B = 0;
		
	cadena_Display2[0] = ' ';
	cadena_Display2[1] = ' ';
	cadena_Display2[2] = '\n';
		 
	cadena_Tiempo_Purga[0]=' ';
	cadena_Tiempo_Purga[1]=':';
	cadena_Tiempo_Purga[2]=' ';
	cadena_Tiempo_Purga[3]=' ';
	cadena_Tiempo_Purga[4]='\n';
	
	cadena_HoraAct[0]=' ';
	cadena_HoraAct[1]=' ';
	cadena_HoraAct[2]='h';
	cadena_HoraAct[3]='s';
	cadena_HoraAct[4]='\n';
	
	cadena_Dia_Semana[0]=' ';
	cadena_Dia_Semana[1]=' ';
	cadena_Dia_Semana[2]=' ';
	cadena_Dia_Semana[3]='\n';
    
	cadena_Dia[0] = ' ';
	cadena_Dia[1] = ' ';
	cadena_Dia[2] = '\n';
	
	cadena_Mes[0] = ' ';
	cadena_Mes[1] = ' ';
	cadena_Mes[2] = '\n';
	
	/*cadena_Anio[0] = ' ';
	cadena_Anio[1] = ' ';
	cadena_Anio[2] = '\n';*/
	
	cadena_Hora[0] = ' ';
	cadena_Hora[1] = ' ';
	cadena_Hora[2] = '\n';
	
	cadena_Min[0] = ' ';
	cadena_Min[1] = ' ';
	cadena_Min[2] = '\n';
	
	cadena_Dosis_A[0] = ' ';
	cadena_Dosis_A[1] = ' ';
	cadena_Dosis_A[2] = ' ';
	cadena_Dosis_A[3] = '\n';
	
	cadena_Dosis_B[0] = ' ';
	cadena_Dosis_B[1] = ' ';
	cadena_Dosis_B[2] = ' ';
	cadena_Dosis_B[3] = '\n';
	
	centenas = 0;
	decenas  = 0;
	unidades = 0;
		
	contTimer0 = 0;
	contTimer1 = 0;
	contTimer2 = 0;
	
	flagTimer0 = 0;
	flagTimer1 = 0;
	flagTimer2 = 0;
	
	flagHora  = 1;
	flagFecha = 1;
	
	DIA_ACTIVADO  = 0;
	HORA_ACTIVADO = 0;
	MIN_ACTIVADO  = 0;
}

void dosis_a_tiempo_A()
{
	if(dosis_A < lista_Dosis[0])
	{
		tiempo_A = lista_Tiempos[0];
	}
	else if(dosis_A >= lista_Dosis[size_Dosis-1])
	{
		tiempo_A = lista_Tiempos[size_Dosis-1];
	}
	else
	{
		uint16_t minDosis = 0, maxDosis = 0;
		uint16_t minTiempo = 0, maxTiempo = 0;
		float deltaDosis = 0.0, deltaTiempo = 0.0;
		float difDosis = 0.0, difTiempo = 0.0;
		
		for(uint16_t i=0; i<size_Dosis; i++)
		{
			if(dosis_A <= lista_Dosis[i+1])
			{
				maxTiempo = lista_Tiempos[i];
				minTiempo = lista_Tiempos[i+1];
				minDosis  = lista_Dosis[i];
				maxDosis  = lista_Dosis[i+1];
				break;
			}
		}
		/*#ifdef UART_DEBUG_
			printf("minDosis  = %d\r\n",minDosis);
			printf("maxDosis  = %d\r\n",maxDosis);
			printf("minTiempo = %d\r\n",minTiempo);
			printf("maxTiempo = %d\r\n",maxTiempo);
			printf("auxLee    = %d\r\n\r\n",auxLee);
		#endif*/
		
		// Interpolación, valor de Dosis A no discretizado.
		deltaDosis  = maxDosis  - minDosis;				   //printf("deltaDosis  = %.1f\r\n",deltaDosis);
		deltaTiempo = maxTiempo - minTiempo;			   //printf("deltaTiempo = %.1f\r\n",deltaTiempo);
		difDosis    = (float) maxDosis  - (float) dosis_A; //printf("difDosis    = %.1f\r\n",difDosis);
		difTiempo   = (difDosis*deltaTiempo)/deltaDosis;   //printf("difTiempo   = %.1f\r\n",difTiempo);
		tiempo_A    = round (minTiempo + difTiempo);	   //printf("Tiempo      = %.1f\r\n",Tiempo);
	}
	tiempo_A = tiempo_A -CORRECCION_CURVA_A;
	#ifdef UART_DEBUG_
		printf("tiempo A %d ms\r\n", tiempo_A); 
	#endif
}

void dosis_a_tiempo_B()
{
	// Cambiar tamaño de variable si la salida puede ser mayor a 255.
	if(dosis_B < lista_Dosis[0])
	{
		tiempo_B = lista_Tiempos[0];
	}
	else if(dosis_B >= lista_Dosis[size_Dosis-1])
	{
		tiempo_B = lista_Tiempos[size_Dosis-1];
	}
	else
	{
		uint16_t minDosis = 0, maxDosis = 0;
		uint16_t minTiempo = 0, maxTiempo = 0;
		float deltaDosis = 0.0, deltaTiempo = 0.0;
		float difDosis = 0.0, difTiempo = 0.0;
		
		for(uint16_t i=0; i<size_Dosis; i++)
		{
			if(dosis_B <= lista_Dosis[i+1])
			{
				maxTiempo = lista_Tiempos[i];
				minTiempo = lista_Tiempos[i+1];
				minDosis  = lista_Dosis[i];
				maxDosis  = lista_Dosis[i+1];
				break;
			}
		}
		/*#ifdef UART_DEBUG_
			printf("minDosis  = %d\r\n",minDosis);
			printf("maxDosis  = %d\r\n",maxDosis);
			printf("minTiempo = %d\r\n",minTiempo);
			printf("maxTiempo = %d\r\n",maxTiempo);
			printf("auxLee    = %d\r\n\r\n",auxLee);
		#endif*/
		// Interpolación, valor de Dosis B no discretizado.
		deltaDosis  = maxDosis  - minDosis;				  //printf("deltaDosis  = %.1f\r\n",deltaDosis);
		deltaTiempo = maxTiempo - minTiempo;			  //printf("deltaTiempo = %.1f\r\n",deltaTiempo);
		difDosis    = (float) maxDosis  - (float) dosis_B; //printf("difDosis    = %.1f\r\n",difDosis);
		difTiempo   = (difDosis*deltaTiempo)/deltaDosis;  //printf("difTiempo   = %.1f\r\n",difTiempo);
		tiempo_B    = round (minTiempo + difTiempo);	  //printf("Tiempo      = %.1f\r\n",Tiempo);
	}
	tiempo_B = tiempo_B -CORRECCION_CURVA_B;
	#ifdef UART_DEBUG_
		printf("tiempo B %d ms\r\n", tiempo_B); 
	#endif
}

void buffer_clean( struct DisplayBuffer *dispbuffer )
{
	strcpy( dispbuffer->line0, "                " );
	strcpy( dispbuffer->line1, "                " );
	dispbuffer->state = 0;
}

void buffer_newline( struct DisplayBuffer *dispbuffer )
{
	if( dispbuffer->y == 0 )
	{
		dispbuffer->y = 1;
	}
	else if( dispbuffer->y == 1 )
	{
		dispbuffer->y = 0;
	}
	dispbuffer->state = 1;
}

void buffer_setxy( struct DisplayBuffer *dispbuffer, uint8_t x, uint8_t y )
{
	dispbuffer->x = x;
	dispbuffer->y = y;
	dispbuffer->state = 1;
}

void buffer_puts( struct DisplayBuffer *dispbuffer, const char *s )
{
	register char c;
	
	dispbuffer->state = 1;
	
	uint8_t i = 0;
	
	while ( ( c = *s++ ) ) {
		if( c == '\n' )
		{
			buffer_newline( dispbuffer );
			i = 0;
		}
		else
		{
			if( i == LCD_DISP_LENGTH )
			{
				buffer_newline( dispbuffer );
				i = 0;
			}
			if( dispbuffer->y == 0 )
			{
				dispbuffer->line0[dispbuffer->x + i] = c;
			}
			else if( dispbuffer->y == 1 )
			{
				dispbuffer->line1[dispbuffer->x + i] = c;
			}
			i++;
		}
	}
}

#ifdef UART_DEBUG_
	void buffer_print( struct DisplayBuffer *dispbuffer )
	{
		printf( "%s\n%s\n", dispbuffer->line0, dispbuffer->line1 );
	}
#endif // UART_DEBUG_

void buffer_recover( struct DisplayBuffer *dispbuffer )
{
	uint8_t bus_state = 0;
	while( lcd_gotoxy( 0, 0 ) == -1 )
	{
		#ifdef UART_DEBUG_
			printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
			display_watchdog_state = 1;
			return;
		}
		else
		{
			bus_state++;
		}
	}
	
	lcd_puts( dispbuffer->line0 );
	
	bus_state = 0;
	while( lcd_gotoxy( 0, 1 ) == -1 )
	{
		#ifdef UART_DEBUG_
		printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
			display_watchdog_state = 1;
			return;
		}
		else
		{
			bus_state++;
		}
	}
	
	lcd_puts( dispbuffer->line1 );
}

void seteaDireccion()
{
	switch(tipoPCF8574)
	{
		case 0:
				PCF8574_ADDRBASE	 = 0x38;
				LCD_PCF8574_DEVICEID = 0x00;
				break;
		case 1:
				PCF8574_ADDRBASE	 = 0x20;
				LCD_PCF8574_DEVICEID = 0x07;
				break;
		case 2:
				PCF8574_ADDRBASE	 = 0x38;
				LCD_PCF8574_DEVICEID = 0x07;
				break;
		case 3:
				PCF8574_ADDRBASE	 = 0x20;
				LCD_PCF8574_DEVICEID = 0x00;
				break;
		default:
				break;
	}
}

void display_init( uint8_t recover_flag )
{
	seteaDireccion();
	
	if( recover_flag == 0 )
	{
		buffer_clean( &main_dispbuffer );
	}
	
try_init:
	display_ON;
	_delay_ms(80);
	
	uint8_t bus_state = 0;
	while( lcd_init() == -1 )
	{
		tipoPCF8574++;
		if(tipoPCF8574>3) tipoPCF8574=0;
		eeprom_write_byte(&eeprom_PCF,tipoPCF8574);
		seteaDireccion();
		
		#ifdef UART_DEBUG_
			printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
		}
		else
		{
			bus_state++;
		}
	}
	bus_state = 0;
	
	while( lcd_command( LCD_RETURN_HOME ) == -1 )
	{
		#ifdef UART_DEBUG_
			printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
			goto try_init;
		}
		else
		{
			bus_state++;
		}
	}
	bus_state = 0;
	
	lcd_led( 0 );
	
	while( lcd_command( LCD_CLEAR_DISPLAY ) == -1 )
	{
		#ifdef UART_DEBUG_
			printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
			goto try_init;
		}
		else
		{
			bus_state++;
		}
	}
	_delay_ms(50);
}

void buzzer_N_times (uint8_t N_times)
{
	for(uint8_t i=0; i<2*N_times; i++)
	{
		buzzer_Toggle;
		_delay_ms(BUZZER_DELAY); wdt_reset();
	}
}


void display_clear ( void )
{
	display_gotoxy (0,0); display_puts("                    "); wdt_reset();
	display_gotoxy( 0,1); display_puts("                    "); wdt_reset();
	display_gotoxy(20,0); display_puts("                    "); wdt_reset();
	display_gotoxy(20,1); display_puts("                    "); wdt_reset();
}

void display_clear2_wait ( void )
{
	display_gotoxy( 0,1); display_puts("                    "); wdt_reset();
	display_gotoxy(20,0); display_puts("                    "); wdt_reset();
	display_gotoxy(20,1); display_puts("                    "); wdt_reset();
}

void display_clear2 ( void )
{
	display_gotoxy( 0,1); display_puts("                    "); wdt_reset();
	display_gotoxy(20,0); display_puts("                    "); wdt_reset();
	display_gotoxy(20,1); display_puts("                    "); wdt_reset();
}

void display_clear3 ( void )
{
	display_gotoxy(20,0); display_puts("                    "); wdt_reset();
	display_gotoxy(20,1); display_puts("                    "); wdt_reset();
}

void display_gotoxy( uint8_t x, uint8_t y )
{
	buffer_setxy( &main_dispbuffer, x, y );
	
	uint8_t bus_state = 0;
	while( lcd_gotoxy( x, y ) == -1 )
	{
		#ifdef UART_DEBUG_
			printf( "Lost connection...\n" );
		#endif // UART_DEBUG_
		
		display_restore( bus_state );
		_delay_ms( 50 );
		if( bus_state > 1 )
		{
			bus_state = 0;
			return;
		}
		else
		{
			bus_state++;
		}
	}
}

void display_puts( const char *s )
{
	buffer_puts( &main_dispbuffer, s );
	lcd_puts( s );
}

void display_restore( uint8_t bus_state )
{
	if( bus_state < N_CLEARBUS )
	{
		uint8_t aux = i2c_clearbus();
		if( aux == 0 )
		{
			#ifdef UART_DEBUG_
				printf( "I2C bus clean.\n" );
			#endif // UART_DEBUG_
		}
		else
		{
			#ifdef UART_DEBUG_
				printf( "Error %d in I2C bus clear\n", aux );
			#endif // UART_DEBUG_
		}
	}
	else
	{
		display_OFF;
		_delay_ms( 200 );
		display_ON;
		
		#ifdef UART_DEBUG_
		printf( "Display power supply reset.\n" );
		#endif // UART_DEBUG_
	}
	
	display_watchdog_state = 1;
}

void display_watchdog( void )
{
	if( display_watchdog_state == 1)
	{
		display_init( 1 );
		display_watchdog_state = 0;
		buffer_recover( &main_dispbuffer );
	}
	else if(check_ITAGUA())
	{
		display_init( 1 );
		buffer_recover( &main_dispbuffer );
	}
}

void display_Dosis()
{
	display_gotoxy(disp_A_Dur_Ciclos,  0);   display_puts(&cadena_Dosis_A[0]);		wdt_reset();
	display_gotoxy(disp_Dosis_B,  0);		 display_puts(&cadena_Dosis_B[0]);		wdt_reset();
}

void display_Dia_Semana()
{
	display_gotoxy(disp_Dia_Semana,1);   display_puts(&cadena_Dia_Semana[0]);   wdt_reset();
}

void display_Fecha()
{
	display_gotoxy(disp_Dia,       1);   display_puts(&cadena_Dia[0]);			wdt_reset();
	display_gotoxy(disp_Mes,       1);   display_puts(&cadena_Mes[0]);			wdt_reset();
	display_gotoxy(disp_Ano,       1);   display_puts(&cadena_Anio[0]);			wdt_reset();
}

void display_Hora()
{
	display_gotoxy(disp_Hora,      1);   display_puts(&cadena_Hora[0]);					wdt_reset();
	display_gotoxy(disp_Min,       1);   display_puts(&cadena_Min[0]);			wdt_reset();
}

/*void codigoDePrueba()
{
	display_gotoxy( 0,0); 				display_puts("       Itagua       ");   wdt_reset();
	display_gotoxy( 0,1); 				display_puts("Ingenieria en Aguas ");   wdt_reset();
	display_gotoxy(20,0);				display_puts("  Codigo de Prueba  ");	wdt_reset();
}*/

void itaguaDisplay ( void )
{
	display_gotoxy( 7,0); 				display_puts("Itagua");   wdt_reset();
	display_gotoxy( 0,1); 				display_puts("Ingenieria en Aguas ");   wdt_reset();
}
void iniciaLCD()
{
	itaguaDisplay();
	
	if(estado == s_timer)
	{
		display_gotoxy(20,0);			display_puts("  A =   %   B =   % ");	wdt_reset();
		display_Dosis();				wdt_reset();
		display_gotoxy(20,1);			display_puts("      /  /     :  hs");	wdt_reset();
		
		display_Dia_Semana();			wdt_reset();
		display_Fecha();				wdt_reset();
		display_Hora();					wdt_reset();
	}
	else if (estado == s_apagado)
	{
		display_gotoxy(20,0);				display_puts("    Modo Apagado    ");	wdt_reset();
		display_gotoxy(20,1);				display_puts("                    ");	wdt_reset();
	}
	else if (estado == s_enclavado)
	{
		display_gotoxy(20,0);				display_puts("  A =   %   B =   % ");	wdt_reset();
		display_Dosis();					wdt_reset();
		display_gotoxy(20,1);				display_puts("   Modo Enclavado   ");	wdt_reset();
	}
}

/*void puntitos(void)
{
	display_gotoxy(29,1);  display_puts(".          ");
	display_gotoxy(29,1);  display_puts("...        ");
	display_gotoxy(29,1);  display_puts("  ...      ");
	display_gotoxy(29,1);  display_puts("    ...    ");
	display_gotoxy(29,1);  display_puts("      ...  ");
	display_gotoxy(29,1);  display_puts("        ...");
	display_gotoxy(29,1);  display_puts("          .");
	display_gotoxy(29,1);  display_puts("           ");
}

void esperePorFavor()
{
	display_gotoxy(0,0);	display_puts("Espere por favor");
	_delay_ms(200); wdt_reset();
	puntitos();		wdt_reset();
}

void inicializando(void)
{
	display_gotoxy(20,1);
	display_puts("Iniciando..."); wdt_reset();
	
	display_puts("I"); _delay_ms(40); wdt_reset();
	display_puts("n"); _delay_ms(40); wdt_reset();
	display_puts("i"); _delay_ms(40); wdt_reset();
	display_puts("c"); _delay_ms(40); wdt_reset();
	display_puts("i"); _delay_ms(40); wdt_reset();
	display_puts("a"); _delay_ms(40); wdt_reset();
	display_puts("n"); _delay_ms(40); wdt_reset();
	display_puts("d"); _delay_ms(40); wdt_reset();
	display_puts("o"); _delay_ms(40); wdt_reset();

	//puntitos();		wdt_reset();
}*/

/*void itaguaInteractivo(void)
{
	display_puts("I"); _delay_ms(40);   wdt_reset();
	display_puts("t"); _delay_ms(40);   wdt_reset();
	display_puts("a"); _delay_ms(40);   wdt_reset();
	display_puts("g"); _delay_ms(40);   wdt_reset();
	display_puts("u"); _delay_ms(40);   wdt_reset();
	display_puts("a");
	
	_delay_ms(300);  wdt_reset();
	
	display_gotoxy(0,1);
	display_puts("I"); _delay_ms(40);  wdt_reset();
	display_puts("n"); _delay_ms(40);  wdt_reset();
	display_puts("g"); _delay_ms(40);  wdt_reset();
	display_puts("e"); _delay_ms(40);  wdt_reset();
	display_puts("n"); _delay_ms(40);  wdt_reset();
	display_puts("i"); _delay_ms(40);  wdt_reset();
	display_puts("e"); _delay_ms(40);  wdt_reset();
	display_puts("r"); _delay_ms(40);  wdt_reset();
	display_puts("i"); _delay_ms(40);  wdt_reset();
	display_puts("a"); _delay_ms(40);  wdt_reset();
	display_puts(" "); _delay_ms(40);  wdt_reset();
	display_puts("e"); _delay_ms(40);  wdt_reset();
	display_puts("n"); _delay_ms(40);  wdt_reset();
	display_puts(" "); _delay_ms(40);  wdt_reset();
	display_puts("A"); _delay_ms(40);  wdt_reset();
	display_puts("g"); _delay_ms(40);  wdt_reset();
	display_puts("u"); _delay_ms(40);  wdt_reset();
	display_puts("a"); _delay_ms(40);  wdt_reset();
	display_puts("s");
}*/
	
void saludoInicial(void)
{
	itaguaDisplay(); wdt_reset();
	
	_delay_ms(400);  wdt_reset();
	
	display_gotoxy(20,0);	display_puts("  Version 240902  "); wdt_reset();
	_delay_ms(400);  wdt_reset();
	
	display_gotoxy(20,1);	display_puts("Iniciando..."); wdt_reset();
	_delay_ms(400);  wdt_reset();
	_delay_ms(400);  wdt_reset();
}

void cambia_Fecha()
{
	decenas  = pDIA/10;
	unidades = pDIA-10*decenas;
	
	if(decenas==0 && centenas==0 ) cadena_Dia[0]='0';
	else cadena_Dia[0]=decenas+48;
	cadena_Dia[1]=unidades+48;
	
	decenas  = pMES/10;
	unidades = pMES-10*decenas;
	
	if(decenas==0 && centenas==0 ) cadena_Mes[0]='0';
	else cadena_Mes[0]=decenas+48;
	cadena_Mes[1]=unidades+48;
	
	decenas  = pANIO/10;
	unidades = pANIO-10*decenas;
	
	if(decenas==0 && centenas==0 ) cadena_Anio[0]='0';
	else cadena_Anio[0]=decenas+48;
	cadena_Anio[1]=unidades+48;
}

void cambia_Hora()
{
	decenas  = pHORAS/10;
	unidades = pHORAS-10*decenas;
		
	if(decenas==0 && centenas==0 ) cadena_Hora[0]='0';
	else cadena_Hora[0]	=	decenas+48;
	cadena_Hora[1]		=	unidades+48;
	
	decenas  = pMINUTOS/10;
	unidades = pMINUTOS-10*decenas;
	
	if(decenas==0 && centenas==0 ) cadena_Min[0]='0';
	else cadena_Min[0]=decenas+48;
	cadena_Min[1]=unidades+48;
}

void cambia_Ciclos()
{
	uint16_t ciclos_Print = ciclos;
	eeprom_write_byte(&eeprom_Ciclos, ciclos);
	
	decenas  = ciclos_Print/10;
	unidades = ciclos_Print - 10*decenas;
	
	if(decenas==0) cadena_Display2[0]=' ';
	else cadena_Display2[0]=decenas+48;
	
	cadena_Display2[1]=unidades+48;
	
	//display_gotoxy(disp_A_Dur_Ciclos,1);
	//display_puts("   ");
	buzzer_ON;
	display_gotoxy(disp_Ancho, 0);
	display_puts(&cadena_Display2[0]);
	buzzer_OFF;
}

void cambia_Horas_Activacion (uint8_t hora)
{
	eeprom_write_byte(&eeprom_HorasACT[hora], horas_act[hora]);
	
	decenas  = horas_act[hora]/10;
	unidades = horas_act[hora]-10*decenas;
	
	if(decenas==0) cadena_HoraAct[0]=' ';
	else cadena_HoraAct[0]=decenas+48;
	cadena_HoraAct[1]=unidades+48;
	
	buzzer_ON;
	switch(hora)
	{
		case 0:
					display_gotoxy(disp_4,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 1:
					display_gotoxy(disp_4,1);
					display_puts(&cadena_HoraAct[0]);
					break;			
		case 2:
					display_gotoxy(disp_Dia,0);
					display_puts(&cadena_HoraAct[0]);
					break;	
		case 3:
					display_gotoxy(disp_Dia,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 4:
					display_gotoxy(disp_14, 0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 5:
					display_gotoxy(disp_14,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 6:
					display_gotoxy(disp_Dur,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 7:
					display_gotoxy(disp_Dur,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 8:
					display_gotoxy(disp_4, 0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 9:
					display_gotoxy(disp_4,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 10:
					display_gotoxy(disp_Dia,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 11:
					display_gotoxy(disp_Dia,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 12:
					display_gotoxy(disp_14, 0);
					display_puts(&cadena_HoraAct[0]);
					break;
		/*case 13:
					display_gotoxy(14,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(14,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 14:
					display_gotoxy(34,0);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(34,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 15:
					display_gotoxy(34,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(34,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 16:
					display_gotoxy(4,0);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(4, 0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 17:
					display_gotoxy(4,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(4,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 18:
					display_gotoxy(24,0);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(24,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 19:
					display_gotoxy(24,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(24,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 20:
					display_gotoxy(14,0);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(14, 0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 21:
					display_gotoxy(14,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(14,1);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 22:
					display_gotoxy(34,0);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(34,0);
					display_puts(&cadena_HoraAct[0]);
					break;
		case 23:
					display_gotoxy(34,1);
					display_puts("    ");
					buzzer_ON;
					display_gotoxy(34,1);
					display_puts(&cadena_HoraAct[0]);
					break;*/
		
		default:	
					display_gotoxy(34,0);
					display_puts("eee");
					break;
	}
	buzzer_OFF;
}

void cambia_Desfasaje ( void )
{
	eeprom_write_byte(&eeprom_Desfasaje, DESFASAJE);
		
	decenas  = DESFASAJE/10;
	unidades = DESFASAJE - 10*decenas;
		
	if(decenas==0) cadena_Display2[0]=' ';
	else cadena_Display2[0]=decenas+48;
		
	cadena_Display2[1]=unidades+48;
		
	//display_gotoxy(disp_Ancho,0);
	//display_puts("   ");
	buzzer_ON;
	display_gotoxy(disp_Ancho, 0);
	display_puts(&cadena_Display2[0]);
	buzzer_OFF;
}

void cambia_ancho (uint8_t bomba)
{
	switch(bomba)
	{
		case 0: //bomba A
				eeprom_write_byte(&eeprom_anchoA, ANCHO_PULSO_A);
			
				decenas  = ANCHO_PULSO_A/10;
				unidades = ANCHO_PULSO_A - 10*decenas;
			
				if(decenas==0) cadena_Display2[0]=' ';
				else cadena_Display2[0]=decenas+48;
			
				cadena_Display2[1]=unidades+48;
			
				//display_gotoxy(disp_Ancho,0);
				//display_puts("   ");
				buzzer_ON;
				display_gotoxy(disp_Ancho, 0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
		case 1: //bomba B
				eeprom_write_byte(&eeprom_anchoB, ANCHO_PULSO_B);
				
				decenas  = ANCHO_PULSO_B/10;
				unidades = ANCHO_PULSO_B - 10*decenas;
				
				if(decenas==0) cadena_Display2[0]=' ';
				else cadena_Display2[0]=decenas+48;
				
				cadena_Display2[1]=unidades+48;
				
				//display_gotoxy(disp_Ancho,0);
				//display_puts("   ");
				buzzer_ON;
				display_gotoxy(disp_Ancho,0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
		default:
				break;
	}
}

void cambia_correccion_curva (uint8_t bomba)
{
	switch(bomba)
	{
		case 0: //bomba A
				eeprom_write_byte(&eeprom_curvaA, CORRECCION_CURVA_A);
		
				decenas  = CORRECCION_CURVA_A/10;
				unidades = CORRECCION_CURVA_A - 10*decenas;
		
				if(decenas==0) cadena_Display2[0]=' ';
				else cadena_Display2[0]=decenas+48;
		
				cadena_Display2[1]=unidades+48;
		
				//display_gotoxy(disp_Ancho,0);
				//display_puts("   ");
				buzzer_ON;
				display_gotoxy(disp_Ancho, 0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
		case 1: //bomba B
				eeprom_write_byte(&eeprom_curvaB, CORRECCION_CURVA_B);
				
				decenas  = CORRECCION_CURVA_B/10;
				unidades = CORRECCION_CURVA_B - 10*decenas;
				
				if(decenas==0) cadena_Display2[0]=' ';
				else cadena_Display2[0]=decenas+48;
				
				cadena_Display2[1]=unidades+48;
				
				//display_gotoxy(disp_Ancho,0);
				//display_puts("   ");
				buzzer_ON;
				display_gotoxy(disp_Ancho,0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
		default:
				break;
	}
}

void diaSiNo (uint8_t dia)
{
	if(dia == 1)
	{
		cadena_Display2[0] = 'S';
		cadena_Display2[1] = 'I';
	}
	else
	{
		cadena_Display2[0] = 'N';
		cadena_Display2[1] = 'O';
	}
}

void cambia_Dias(uint8_t dia)
{
	switch(dia)
	{
		case 0: //Lunes
				eeprom_write_byte(&eeprom_Lunes, lunes);
				diaSiNo(lunes);
				display_gotoxy(disp_DiaIzq,0);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaIzq, 0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
		case 1:
				eeprom_write_byte(&eeprom_Martes, martes);
				diaSiNo(martes);
				display_gotoxy(disp_DiaIzq,1);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaIzq, 1);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		case 2:
				eeprom_write_byte(&eeprom_Miercoles, miercoles);
				diaSiNo(miercoles);
				display_gotoxy(disp_DiaIzq+20,0);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaIzq+20, 0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		case 3:
				eeprom_write_byte(&eeprom_Jueves, jueves);
				diaSiNo(jueves);
				display_gotoxy(disp_DiaIzq+20,1);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaIzq+20, 1);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		case 4:
				eeprom_write_byte(&eeprom_Viernes, viernes);
				diaSiNo(viernes);
				display_gotoxy(disp_DiaDer,0);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaDer,0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		case 5:
				eeprom_write_byte(&eeprom_Sabado, sabado);
				diaSiNo(sabado);
				display_gotoxy(disp_DiaDer,1);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaDer,1);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		case 6:
				eeprom_write_byte(&eeprom_Domingo, domingo);
				diaSiNo(domingo);
				display_gotoxy(disp_DiaDer+20,0);
				display_puts("  ");
				buzzer_ON;
				display_gotoxy(disp_DiaDer+20,0);
				display_puts(&cadena_Display2[0]);
				buzzer_OFF;
				break;
				
		default:
				break;
	}	
}


void mem_Modo ( void )
{
	eeprom_write_byte(&eeprom_Modo, modo);
	#ifdef UART_ESP
		printf(":C%d\n", modo);
	#endif
	_delay_ms(500); wdt_reset();
}

void cambia_Modo()
{
	display_gotoxy(disp_A_Dur_Ciclos,0);
	display_puts("          ");
	buzzer_ON;
	display_gotoxy(disp_A_Dur_Ciclos, 0);
	if(modo==0)		 //Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: WiFi 1, 6: WiFi 2
	{
		display_puts(" Apagado");
		estado = s_apagado;
	}
	else if(modo==1) 
	{
		display_puts("  Purga");
		estado = s_purga;
	}
	else if(modo==2) 
	{
		display_puts("  Timer");
		estado = s_timer;
	}
	else if(modo==3)
	{
		display_puts("Conf Timer");
		estado = s_configuraTimer;
	}
	else if(modo==4)
	{
		display_puts("Enclavado");
		estado = s_enclavado;
	}
	else if(modo==5)
	{
		display_puts("  WiFi 1");
		estado = s_apagado;
	}
	else if(modo==6)
	{ 
		display_puts("  WiFi 2");
		estado = s_apagado;
	}
	buzzer_OFF;
}

void cambia_Duracion()
{	
	uint16_t duracion_Print = duracion;
	eeprom_write_byte(&eeprom_Duracion, duracion);
					
	decenas  = duracion_Print/10;
	unidades = duracion_Print - 10*decenas;
		
	if(decenas==0) cadena_Display2[0]=' ';
	else cadena_Display2[0]=decenas+48;
	
	cadena_Display2[1]=unidades+48;
	
	//display_gotoxy(disp_Mes,0);
	//display_puts("   ");
	buzzer_ON;
	display_gotoxy(disp_Mes, 0);
	display_puts(&cadena_Display2[0]);
	buzzer_OFF;
}

void cambia_Tiempo_Purga(uint16_t timePurga)
{		
	if(timePurga>=180)
	{
		cadena_Tiempo_Purga[0] = '3';
		timePurga = timePurga-180;
	}
	else if(timePurga>=120)
	{
		cadena_Tiempo_Purga[0] = '2';
		timePurga = timePurga-120;
	}
	else if(timePurga>=60)
	{
		cadena_Tiempo_Purga[0] = '1';
		timePurga = timePurga-60;
	}
	else
	{
		cadena_Tiempo_Purga[0] = '0';
	}
	
	decenas  = timePurga/10;
	unidades = timePurga-10*decenas ;
	
	if(decenas==0)	cadena_Tiempo_Purga[2]='0';
	else cadena_Tiempo_Purga[2]=decenas+48;
	
	cadena_Tiempo_Purga[3]=unidades+48;
	
	display_gotoxy(28,1);
	display_puts("   ");
	buzzer_ON;
	display_gotoxy(28,1);
	display_puts(&cadena_Tiempo_Purga[0]);
	buzzer_OFF;
}

void mem_Dosis_A ( void )
{
	eeprom_write_byte(&eeprom_Dosis_A, dosis_A);
	_delay_ms(200); wdt_reset();
	#ifdef UART_ESP
		printf(":A%d\n",dosis_A);
	#endif
	_delay_ms(700); wdt_reset();
}

void mem_Dosis_B ( void )
{
	eeprom_write_byte(&eeprom_Dosis_B,dosis_B);
	_delay_ms(200); wdt_reset();
	#ifdef UART_ESP
		printf(":B%d\n",dosis_B);
	#endif
	_delay_ms(700); wdt_reset();
}

void setDosisAB(void)
{
	uint16_t dosis_A_Print = dosis_A ;
	uint16_t dosis_B_Print = dosis_B ;
	
	centenas = dosis_A_Print/100;
	decenas  = ( dosis_A_Print -100*centenas )/10;
	unidades = ( dosis_A_Print -100*centenas -10*decenas );
	if(centenas==0 ) cadena_Dosis_A[0]=' ';
	else cadena_Dosis_A[0]=centenas+48;
	if(decenas==0 && centenas==0 ) cadena_Dosis_A[1]=' ';
	else cadena_Dosis_A[1]=decenas+48;
	cadena_Dosis_A[2]=unidades+48;
	cadena_Dosis_A[3]='%';
	
	centenas = dosis_B_Print/100;
	decenas  = ( dosis_B_Print -100*centenas )/10;
	unidades = ( dosis_B_Print -100*centenas -10*decenas );
	if(centenas==0 ) cadena_Dosis_B[0]=' ';
	else cadena_Dosis_B[0]=centenas+48;
	if(decenas==0 && centenas==0 ) cadena_Dosis_B[1]=' ';
	else cadena_Dosis_B[1]=decenas+48;
	cadena_Dosis_B[2]=unidades+48;
	cadena_Dosis_B[3]='%';
}

void cambia_Dosis_A ( void )
{	
	cont_Tiempo_A = 0;
	uint16_t dosis_A_Print = dosis_A ;
	
	if(bomba_A_APAGADA)
	{
		cadena_Dosis_A[1] = 'O';
		cadena_Dosis_A[2] = 'F';
		cadena_Dosis_A[3] = 'F';
	}
	else
	{
		centenas = dosis_A_Print/100;
		decenas  = ( dosis_A_Print -100*centenas )/10;
		unidades = ( dosis_A_Print -100*centenas -10*decenas );
		if(centenas==0 ) cadena_Dosis_A[0]=' ';
		else cadena_Dosis_A[0]=centenas+48;
		if(decenas==0 && centenas==0 ) cadena_Dosis_A[1]=' ';
		else cadena_Dosis_A[1]=decenas+48;
		cadena_Dosis_A[2]=unidades+48;
		cadena_Dosis_A[3]='%';
	}
	
	if(estadoAnterior!=s_apagado)
	{
		display_gotoxy(disp_A_Dur_Ciclos,0);
		display_puts("   ");
		buzzer_ON;
		display_gotoxy(disp_A_Dur_Ciclos, 0);
		display_puts(&cadena_Dosis_A[0]);
		buzzer_OFF;
	}
	dosis_a_tiempo_A();
}

void cambia_Dosis_B ( void )
{
	cont_Tiempo_B = 0;
	
	uint16_t dosis_B_Print = dosis_B ;
	
	/*#ifdef UART_DEBUG_
		printf("Dosis B -------> %d\r\n", dosis_B_Print);
	#endif*/

	if(bomba_B_APAGADA)
	{
		cadena_Dosis_B[1] = 'O';
		cadena_Dosis_B[2] = 'F';
		cadena_Dosis_B[3] = 'F';
	}
	else
	{
		centenas = dosis_B_Print/100;
		decenas  = ( dosis_B_Print -100*centenas )/10;
		unidades = ( dosis_B_Print -100*centenas -10*decenas );	
		if(centenas==0 ) cadena_Dosis_B[0]=' ';
		else cadena_Dosis_B[0]=centenas+48;		
		if(decenas==0 && centenas==0 ) cadena_Dosis_B[1]=' ';
		else cadena_Dosis_B[1]=decenas+48;		
		cadena_Dosis_B[2]=unidades+48;
		cadena_Dosis_B[3]='%';
	}
	
	if(estadoAnterior!=s_apagado)
	{
		display_gotoxy(disp_Dosis_B,0);
		display_puts("   ");	
		buzzer_ON;
		display_gotoxy(disp_Dosis_B,0);
		display_puts(&cadena_Dosis_B[0]);
		buzzer_OFF;
	}
	
	dosis_a_tiempo_B();
}

uint8_t check_ITAGUA(){
	char checkWord[3] = CH_WORD;
	for(int i = 0; i < sizeof(checkWord); i++){
		display_gotoxy(i, 0);
		if(checkWord[i] != lcd_read_ITAGUA()){
			#ifdef UART_DEBUG_
				printf("Símbolos chinos.\r\n");
			#endif
			return 1;
		}
	}
	return 0;
}

char lcd_read_ITAGUA(){
	uint8_t data = 0x00, bina1 = 0x00, bina2 = 0x00;
	char bina;
	dataport |= ( 1 << LCD_RW_PIN );	//Lectura
	dataport |= ( 1 << LCD_RS_PIN );	//rs_state = 1;
	int8_t aux;
	aux = pcf8574_setoutput(LCD_PCF8574_DEVICEID, dataport);
	if( aux < 0 ) return aux;
	aux = pcf8574_setoutputpinhigh( LCD_PCF8574_DEVICEID, LCD_E_PIN );
	if( aux < 0 ) return aux;
	LCD_DELAY;
	data = pcf8574_getinput(LCD_PCF8574_DEVICEID);
	aux = pcf8574_setoutputpinlow( LCD_PCF8574_DEVICEID, LCD_E_PIN );
	if( aux < 0 ) return aux;
	bina1 = data & 0b11110000;
	bina1 ^= 0b11111111;
	LCD_DELAY;
	aux = pcf8574_setoutputpinhigh( LCD_PCF8574_DEVICEID, LCD_E_PIN );
	if( aux < 0 ) return aux;
	LCD_DELAY;
	data = pcf8574_getinput(LCD_PCF8574_DEVICEID);
	aux = pcf8574_setoutputpinlow( LCD_PCF8574_DEVICEID, LCD_E_PIN );
	if( aux < 0 ) return aux;
	bina2 = (data >> 4);
	bina2 ^= 0b11111111;
	bina = (char)(bina1 & bina2);
	return bina;
}

void miDelay_ms_apagado(uint16_t kMax)
{
	for(uint16_t k=0; k<kMax; k++)
	{
		if(estado==s_apagado)
		{
			_delay_ms(1);	wdt_reset();
		}
		else
		{
			k=kMax;
			break;
		}
	}
}

void miDelay_ms_enclavado(uint16_t kMax)
{
	for(uint16_t k=0; k<kMax; k++)
	{
		if(estado==s_enclavado)
		{
			_delay_ms(1);	wdt_reset();
		}
		else
		{
			k=kMax;
			break;
		}
	}
}

void miDelay_ms_timer(uint16_t kMax)
{
	for(uint16_t k=0; k<kMax; k++)
	{
		if(estado==s_timer)
		{
			_delay_ms(1);
			wdt_reset();
		}
		else 
		{
			k=kMax;
			break;
		}
	}
}

//----------------------------------------------//
//--------------- Máquina de Estados ---------- //
//----------------------------------------------//
void rutinaApagado ( void )
{
	if(estado == s_apagado)
	{
		miDelay_ms_apagado(500);
		wdt_reset();
	}
	if(estado == s_apagado)
	{
		display_watchdog();
		wdt_reset();
	}
}

void modoAnterior ( void )
{
	//-----Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: accessPoint 1, 6: accessPoint 2
	if(estado==s_apagado)				modo = 0;
	else if(estado == s_purga)			modo = 1;
	else if(estado == s_timer)			modo = 2;
	else if(estado == s_configuraTimer)	modo = 3;
	else if(estado == s_enclavado)		modo = 4;
	else if(estado == s_cambioModo)		modo = 11; //<--
	estadoAnterior = estado; //agregado!
	mem_Modo(); _delay_ms(300); wdt_reset();
}

void rutinaEnclavado ( void )
{
	if(estado == s_enclavado)
	{
		miDelay_ms_enclavado(500);
		wdt_reset();
	}
	if(estado == s_enclavado)
	{
		display_watchdog();
		wdt_reset();
	}
}

uint8_t timerCount = 0;
void rutinaTimer ( void )
{
	if(estado == s_timer)
	{
		miDelay_ms_timer(850);
		wdt_reset();
	}
	if(estado == s_timer)
	{
		display_watchdog();
		wdt_reset();
	}
	if(estado == s_timer)
	{
		leeRTC();
		wdt_reset();
	}
	if(estado == s_timer && flagFecha == 1)
	{
		cambia_Fecha();
		display_Dia_Semana();
		display_Fecha();
		flagFecha = 0;
		wdt_reset();
	}
	if(estado == s_timer && flagHora == 1)
	{
		cambia_Hora();
		display_Hora();
		flagHora = 0;
		wdt_reset();
	}
	
	if(DIA_ACTIVADO && HORA_ACTIVADO && MIN_ACTIVADO)
	{
		/* MUY RARO 
		única solución que encontré a que vuelva a andar modo timer
		- agregar este UART_ESP
		- cambiar los ":D%d\n",0 a ":D0\n" por ejemplo
		- imprimir flag como "F%d\n" aunque moleste al ESP32
		IMPRIMIR ESO MOLESTA AL ESP!
		*/
		/*#ifdef UART_ESP 
			//printf("%d\n",FLAG_ACTIVADO); //<-- me imprime 52,53...,40,50,51... cualquiera... ya sea con uint8_t o int
			printf("F%d\n",fT); //así  me lo imprime bien y lo toma
		#endif*/
		//fT = (uint8_t) fT;
		if(fT==0) 
		{
			//buzzer_3_times();
			activacion();
		}
	}
	else
	{
		desactivacion();
	}
	
}

void rutinaCambioParametro ( void )
{	
	uint16_t wait_pulsador = 0;
	switch(subestado)
	{
		case s_nada:
				break;
				
		case s_mas_Dosis_A:

				wdt_reset();
				dosis_A++;
				bomba_A_APAGADA = 0;
				if(dosis_A>dosis_Max) dosis_A=dosis_Max;
				else cambia_Dosis_A();
				wait_pulsador = 0;
				while(!mas_Bomba_A_Test)
				{
					wait_pulsador++;
					if(wait_pulsador>=1000)
					{
						wait_pulsador = 0;
						while(!mas_Bomba_A_Test)
						{
							dosis_A++;
							if(dosis_A>dosis_Max) dosis_A=dosis_Max;
							else cambia_Dosis_A();
							wdt_reset();
						}
					}
					else 
					{
						_delay_ms(1); wdt_reset();
					}
				}
				mem_Dosis_A();
				break;
		
		case s_menos_Dosis_A:
		
				wdt_reset();
				dosis_A--;
				if(dosis_A<dosis_Min)  
				{
					dosis_A = dosis_Min-1;
					bomba_A_APAGADA = 1;
				}
				else bomba_A_APAGADA = 0;
				cambia_Dosis_A();
				wait_pulsador = 0;
				while(!menos_Bomba_A_Test)
				{
					wait_pulsador++;
					if(wait_pulsador >= 1000)
					{
						while(!menos_Bomba_A_Test)
						{
							dosis_A--;
							if(dosis_A<dosis_Min) 
							{
								dosis_A = dosis_Min-1;
								bomba_A_APAGADA = 1;
							}
							else bomba_A_APAGADA = 0;
							cambia_Dosis_A();
							wdt_reset();
						}
					}
					else
					{
						_delay_ms(1); wdt_reset();
					}
				}
				mem_Dosis_A();
				break;
		
		case s_mas_Dosis_B:

				wdt_reset();
				dosis_B++;
				bomba_B_APAGADA = 0;
				if(dosis_B>dosis_Max) dosis_B=dosis_Max;
				else cambia_Dosis_B();
				wait_pulsador = 0;
				while(!mas_Bomba_B_Test)
				{
					wait_pulsador++;
					if(wait_pulsador>=1000)
					{
						wait_pulsador = 0;
						while(!mas_Bomba_B_Test)
						{
							dosis_B++;
							if(dosis_B>dosis_Max) dosis_B=dosis_Max;
							else cambia_Dosis_B();
							wdt_reset();
						}
					}
					else
					{
						_delay_ms(1); wdt_reset();
					}
				}
				mem_Dosis_B();
				break;
		
		case s_menos_Dosis_B:
		
				wdt_reset();
				dosis_B--;
				if(dosis_B<dosis_Min)  
				{
					dosis_B = dosis_Min-1;
					bomba_B_APAGADA = 1;
				}
				else bomba_B_APAGADA = 0;	  
				cambia_Dosis_B();
				wait_pulsador = 0;
				while(!menos_Bomba_B_Test)
				{
					wait_pulsador++;
					if(wait_pulsador >= 1000)
					{
						while(!menos_Bomba_B_Test)
						{
							dosis_B--;
							if(dosis_B<dosis_Min-1)  
							{
								dosis_B = dosis_Min-1;
								bomba_B_APAGADA = 1;
							}
							else bomba_B_APAGADA = 0;
							cambia_Dosis_B();
							wdt_reset();
						}
					}
					else
					{
						_delay_ms(1); wdt_reset();
					}
				}
				mem_Dosis_B();
				break;
		
		default:
		
				_delay_ms(1);
				break;
	}
	cont_Tiempo_A = 0;
	cont_Tiempo_B = 0;
	contTimer1 = 0;
	contTimer0 = 0;
	contTimer2 = 0;
}

uint8_t escape ( void ) //0: escape, 1: continua
{
	if(!mas_Bomba_B_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!mas_Bomba_B_Test) return 0;
	}
	else if(!menos_Bomba_B_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!menos_Bomba_B_Test) return 0;
	}
	else if(!mas_Bomba_A_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!mas_Bomba_A_Test) return 0;
	}
	else if(!menos_Bomba_A_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!menos_Bomba_A_Test) return 0;
	}
	return 1;
}

uint8_t esperaPurgaContinuo ( void ) //0: escape, 1: termina ok
{ 
	for(uint16_t p= 0; p<850; p++)
	{
		if( escape() ) //0: escape, 1: continua
		{
			_delay_ms(1); wdt_reset();
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

void rutinaPurga ( void )
{
	display_gotoxy (0,0); display_puts("---- Modo Purga ----"); wdt_reset();
	display_gotoxy( 0,1); display_puts("  A=100%   B=100%   ");	wdt_reset();
	display_gotoxy(20,0); display_puts("  tiempo restante   "); wdt_reset();
	display_gotoxy(20,1); display_puts("       03:00        "); wdt_reset();
	
	uint16_t tiempo_A_ant = tiempo_A;
	uint16_t tiempo_B_ant = tiempo_B;
	tiempo_A = 430; //Máximo para 100%
	tiempo_B = 430; //Máximo para 100%
	
	activacion();	sei();
	for(uint16_t iPurga=180; iPurga>0; iPurga--)
	{
		cambia_Tiempo_Purga(iPurga); wdt_reset();
		if(!esperaPurgaContinuo() || estado==s_errorPx0 || estado == s_uart)  //0: escape, 1: termina ok
		{
			iPurga = 0;
			break;
		}
	}
	desactivacion();
	cli();
	tiempo_A = tiempo_A_ant;
	tiempo_B = tiempo_B_ant;
	
	buzzer_N_times(3);
}

/*void rutinaContinuo ( void )
{
	display_gotoxy (0,0); display_puts("-- Modo Continuo -- "); wdt_reset();
	display_gotoxy( 0,1); display_puts("   A=50%    B=50%   ");	wdt_reset();
	display_gotoxy(20,0); display_puts("                    "); wdt_reset();
	display_gotoxy(20,1); display_puts("                    "); wdt_reset();
	
	uint16_t tiempo_A_ant = tiempo_A;
	uint16_t tiempo_B_ant = tiempo_B;
	tiempo_A = 856; //50%
	tiempo_B = 856; //50%
	
	activacion();	sei();
	while(esperaPurgaContinuo())  //0: escape, 1: termina ok
	{
		buzzer_ON;
		_delay_ms(FIRST_DELAY);
		buzzer_OFF;
		if(estado == s_errorPx0) break;
	}
	desactivacion();	cli();
	
	tiempo_A = tiempo_A_ant;
	tiempo_B = tiempo_B_ant;
	buzzer_N_times(3);
}*/

void rutinaMenu ( void )
{
	buzzer_N_times(3);
	display_clear(); _delay_ms(100);  wdt_reset();
	display_gotoxy (0,0); display_puts("-- Configuracion --"); wdt_reset();
	while ( !mas_Bomba_A_Test && !mas_Bomba_B_Test )
	{
		_delay_ms(SHORT_DELAY); wdt_reset();
	}
	
	seleccioneModo();  buzzer_N_times(3);
	
	uint8_t Wnum = 0;
	if(modo==5)
	{
		Wnum=1;
		#ifdef UART_ESP
			printf(":W%d\n",Wnum);
		#endif
		buzzer_OFF;
		_delay_ms(1000); wdt_reset();
		modo = 0;
	}
	else if(modo==6)
	{
		Wnum=2;
		#ifdef UART_ESP
			printf(":W%d\n",Wnum);
		#endif
		buzzer_OFF;
		_delay_ms(1000); wdt_reset();
		modo = 0;
	}
	
	if(estado == s_configuraTimer)
	{
		display_clear2_wait();
		seleccioneDuracion();				buzzer_N_times(3);
		display_clear2_wait();
		seleccioneCiclos();					buzzer_N_times(3);
		display_clear2_wait();
		seleccioneHorarios();				buzzer_N_times(3);
		display_clear2_wait();
		seleccioneDias();					buzzer_N_times(3);
		estado = estadoAnterior;
	}
	display_clear(); _delay_ms(100);  wdt_reset();
}

uint8_t continuarMenu ( void )
{
	if(!mas_Bomba_B_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!mas_Bomba_B_Test)
		{
			while(!mas_Bomba_B_Test)
			{
				_delay_ms(1); wdt_reset();
			}
			return 0;
		}
	}
	return 1;
}

uint8_t continuarMenu2 ( void )
{
	if(!mas_Bomba_B_Test)
	{
		_delay_ms(SECOND_DELAY);
		if(!mas_Bomba_B_Test)
		{
			buzzer_ON; _delay_ms(50); wdt_reset();
			buzzer_OFF;
			while(!mas_Bomba_B_Test)
			{
				_delay_ms(1); wdt_reset();
			}
			return 0;
		}
	}
	return 1;
}

void seleccioneModo( void )
{
	display_gotoxy (7,1); display_puts("Modo:"); wdt_reset();
	cambia_Modo();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS MODO
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(modo < modo_Max) modo++;
				else modo = modo_Min;
				cambia_Modo();
				while(!mas_Bomba_A_Test)
				{
					_delay_ms(SHORT_DELAY); wdt_reset();
				}
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS MODO
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(modo > modo_Min) modo --;
				else modo = modo_Max;
				cambia_Modo();
				while(!menos_Bomba_A_Test)
				{
					_delay_ms(SHORT_DELAY); wdt_reset();
				}
			}
		}
		wdt_reset();
	}
	//mem_Modo(); YA LO HACE EN EL LOOP. No mandar otros estados intermedios
}

void seleccioneDuracion( void )
{
	display_gotoxy (disp_Duracion,1); display_puts("Duracion:"); wdt_reset();
	display_gotoxy (disp_Mes,0);	  display_puts("   min "); wdt_reset();
	
	cambia_Duracion();
	
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(duracion < duracion_Max) duracion ++;
				cambia_Duracion();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(duracion > duracion_Min) duracion --;
				cambia_Duracion();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
}

void seleccione_desfasaje ( void )
{		
	//display_gotoxy(0,0); display_puts("    Seleccione      "); wdt_reset();
	display_gotoxy(4,1); display_puts("Desfasaje:"); wdt_reset();
	cambia_Desfasaje();
	display_gotoxy(31,0); display_puts("ms"); wdt_reset();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(DESFASAJE < desfasaje_Max) DESFASAJE++;
				cambia_Desfasaje();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(DESFASAJE > desfasaje_Min) DESFASAJE --;
				cambia_Desfasaje();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
	
	buzzer_N_times(2);
	display_clear();
}

void seleccione_anchos ( void )
{
	buzzer_N_times(3);
	display_clear();
	
	while( !menos_Bomba_A_Test && !menos_Bomba_B_Test)
	{
		_delay_ms(1); wdt_reset();
	}
	
	//display_gotoxy(0,0); display_puts(" Seleccione Ancho   "); wdt_reset();
	display_gotoxy(7,1);  display_puts("Ancho A:"); wdt_reset();	
	cambia_ancho(0);	
	display_gotoxy(31,0); display_puts("ms"); wdt_reset();	
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(ANCHO_PULSO_A < ancho_Max) ANCHO_PULSO_A++;
				cambia_ancho(0);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(ANCHO_PULSO_A > ancho_Min) ANCHO_PULSO_A --;
				cambia_ancho(0);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
	
	buzzer_N_times(2);
	display_clear();
		
	//display_gotoxy(0,0); display_puts(" Seleccione Ancho   "); wdt_reset();
	display_gotoxy(7,1);  display_puts("Ancho B:"); wdt_reset();
	cambia_ancho(1);
	display_gotoxy(31,0); display_puts("ms"); wdt_reset();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(ANCHO_PULSO_B < ancho_Max) ANCHO_PULSO_B++;
				cambia_ancho(1);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(ANCHO_PULSO_B > ancho_Min) ANCHO_PULSO_B --;
				cambia_ancho(1);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
	buzzer_N_times(2);
	display_clear();
}

void seleccione_correccion_curvas ( void )
{
	while( !menos_Bomba_A_Test && !menos_Bomba_B_Test)
	{
		_delay_ms(1); wdt_reset();
	}
	
	//display_gotoxy(0,0); display_puts("     Seleccione     "); wdt_reset();
	display_gotoxy(3,1); display_puts("Correccion A:"); wdt_reset();
	cambia_correccion_curva(0);
	display_gotoxy(31,0); display_puts("ms"); wdt_reset();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(CORRECCION_CURVA_A < curva_Max) CORRECCION_CURVA_A++;
				cambia_correccion_curva(0);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(CORRECCION_CURVA_A > curva_Min) CORRECCION_CURVA_A --;
				cambia_correccion_curva(0);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
	
	buzzer_N_times(2);
	display_clear();
	
	display_gotoxy(3,1); display_puts("Correccion B:"); wdt_reset();
	cambia_correccion_curva(1);
	display_gotoxy(31,0); display_puts("ms"); wdt_reset();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(CORRECCION_CURVA_B < curva_Max) CORRECCION_CURVA_B++;
				cambia_correccion_curva(1);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(CORRECCION_CURVA_B > curva_Min) CORRECCION_CURVA_B --;
				cambia_correccion_curva(1);
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
	buzzer_N_times(2);
	display_clear();
}

void seleccioneCiclos ( void )
{
	display_gotoxy (2,1); display_puts("Ciclos por dia:"); wdt_reset();
	cambia_Ciclos();
	while( continuarMenu() )
	{
		if(!mas_Bomba_A_Test) //MÁS DURACIÓN
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(ciclos < ciclos_Max) ciclos++;
				cambia_Ciclos();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		else if(!menos_Bomba_A_Test) //MENOS DURACIÓN
		{
			_delay_ms(15);
			if(!menos_Bomba_A_Test)
			{
				if(ciclos > ciclos_Min) ciclos --;
				cambia_Ciclos();
				_delay_ms(LOOP_DELAY); wdt_reset();
			}
		}
		wdt_reset();
	}
}

void wait_mas_A ( void )
{
	while(!mas_Bomba_A_Test)
	{
		_delay_ms(SHORT_DELAY); wdt_reset();
	}
}

void wait_menos_A ( void )
{
	while(!menos_Bomba_A_Test)
	{
		_delay_ms(SHORT_DELAY); wdt_reset();
	}
}

void cambiaDiaLunes ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA LUNES
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(lunes==0) lunes = 1;
				else lunes = 0;
				cambia_Dias(0); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(lunes==0) lunes = 1;
				else lunes = 0;
				cambia_Dias(0); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
}

void cambiaDiaMartes ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA MARTES
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(martes==0) martes = 1;
				else martes = 0;
				cambia_Dias(1); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(martes==0) martes = 1;
				else martes = 0;
				cambia_Dias(1); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
}

void cambiaDiaMiercoles ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA MIERCOLES
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(miercoles==0) miercoles = 1;
				else miercoles = 0;
				cambia_Dias(2); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(miercoles==0) miercoles = 1;
				else miercoles = 0;
				cambia_Dias(2); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
	estado = s_configuraTimer;
}

void cambiaDiaJueves ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA JUEVES
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(jueves==0) jueves = 1;
				else jueves = 0;
				cambia_Dias(3); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(jueves==0) jueves = 1;
				else jueves = 0;
				cambia_Dias(3); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
	estado = s_configuraTimer;
}

void cambiaDiaViernes ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA VIERNES
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(viernes==0) viernes = 1;
				else viernes = 0;
				cambia_Dias(4); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(viernes==0) viernes = 1;
				else viernes = 0;
				cambia_Dias(4); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
	estado = s_configuraTimer;
}

void cambiaDiaSabado ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA SABADO
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(sabado==0) sabado = 1;
				else sabado = 0;
				cambia_Dias(5); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(sabado==0) sabado = 1;
				else sabado = 0;
				cambia_Dias(5); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
	estado = s_configuraTimer;
}

void cambiaDiaDomingo ( void )
{
	while ( continuarMenu2() )
	{
		if(!mas_Bomba_A_Test) //CAMBIA DOMINGO
		{
			_delay_ms(SECOND_DELAY);
			if(!mas_Bomba_A_Test)
			{
				if(domingo==0) domingo = 1;
				else domingo = 0;
				cambia_Dias(6); wdt_reset();
			}
			wait_mas_A();
		}
		else if(!menos_Bomba_A_Test)
		{
			_delay_ms(SECOND_DELAY);
			if(!menos_Bomba_A_Test)
			{
				if(domingo==0) domingo = 1;
				else domingo = 0;
				cambia_Dias(6); wdt_reset();
			}
			wait_menos_A();
		}
		wdt_reset();
	}
	estado = s_configuraTimer;
}

void seleccioneHorarios (void)
{
	display_gotoxy (0,0);   display_puts("-- Configuracion --"); wdt_reset();
	display_gotoxy (2,1);   display_puts("Horas activacion"); wdt_reset();
	
	_delay_ms(800); wdt_reset();
	
	if(ciclos>0)
	{
		display_clear();		
		display_gotoxy  (1,0); display_puts("1-        5-"); wdt_reset();
		display_gotoxy  (1,1); display_puts("2-        6-"); wdt_reset();
		display_gotoxy (21,0); display_puts("3-        7-"); wdt_reset();
		display_gotoxy (21,1); display_puts("4-        8-"); wdt_reset();
		
		for(uint8_t i=0; ( (i<ciclos)&&(i<8) ); i++) //Página 1
		{
			cambia_Horas_Activacion(i);
			while( continuarMenu() )
			{
				if(!mas_Bomba_A_Test) //MÁS
				{
					_delay_ms(SECOND_DELAY);
					if(!mas_Bomba_A_Test)
					{
						if(horas_act[i] < hora_Max) horas_act[i]++;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				else if(!menos_Bomba_A_Test) //MENOS
				{
					_delay_ms(SECOND_DELAY);
					if(!menos_Bomba_A_Test)
					{
						if(horas_act[i] > hora_Min) horas_act[i] --;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				wdt_reset();
			}	
		}
	}	
	if(ciclos>8) //Página 2
	{	
		display_clear();
		display_gotoxy  (0,0); display_puts(" 9-"); wdt_reset();
		display_gotoxy  (0,1); display_puts("10-"); wdt_reset();
		display_gotoxy (20,0); display_puts("11-"); wdt_reset();
		display_gotoxy (20,1); display_puts("12-"); wdt_reset();
		
		for(uint8_t i=8; ( (i<ciclos)&&(i<cantidad_Horas) ); i++)
		{
			cambia_Horas_Activacion(i);
			while( continuarMenu() )
			{
				if(!mas_Bomba_A_Test) //MÁS
				{
					_delay_ms(SECOND_DELAY);
					if(!mas_Bomba_A_Test)
					{
						if(horas_act[i] < hora_Max) horas_act[i]++;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				else if(!menos_Bomba_A_Test) //MENOS
				{
					_delay_ms(SECOND_DELAY);
					if(!menos_Bomba_A_Test)
					{
						if(horas_act[i] > hora_Min) horas_act[i] --;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				wdt_reset();
			}
		}
	}
	/*if(ciclos>16) //Página 3
	{		
		display_gotoxy  (0,0); display_puts("17-       21-       "); wdt_reset();
		display_gotoxy ( 0,1); display_puts("18-       22-       "); wdt_reset();
		display_gotoxy (20,0); display_puts("19-       23-       "); wdt_reset();
		display_gotoxy (20,1); display_puts("20-       24-       "); wdt_reset();
		
		for(uint8_t i=16; (i<ciclos&&i<24); i++)
		{
			cambia_Horas_Activacion(i);
			
			while( continuarMenu() )
			{
				if(!mas_Bomba_A_Test) //MÁS
				{
					_delay_ms(SECOND_DELAY);
					if(!mas_Bomba_A_Test)
					{
						if(horas_act[i] < hora_Max) horas_act[i]++;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				else if(!menos_Bomba_A_Test) //MENOS
				{
					_delay_ms(SECOND_DELAY);
					if(!menos_Bomba_A_Test)
					{
						if(horas_act[i] > hora_Min) horas_act[i] --;
						cambia_Horas_Activacion(i);
						_delay_ms(LOOP_DELAY); wdt_reset();
					}
				}
				wdt_reset();
			}
		}
	}*/	
}

void seleccioneDias ( void )
{
	display_gotoxy (0,0);   display_puts("-- Configuracion --"); wdt_reset();
	display_gotoxy (2,1);   display_puts("Dias de trabajo"); wdt_reset();
	
	_delay_ms(800); wdt_reset();
	
	display_gotoxy  (0,0); display_puts("Lunes     Viernes   "); wdt_reset();
	display_gotoxy ( 0,1); display_puts("Martes    Sabado    "); wdt_reset();
	display_gotoxy (20,0); display_puts("Mierc.    Domingo   "); wdt_reset();
	display_gotoxy (20,1); display_puts("Jueves              "); wdt_reset();
	
	for (uint8_t i=0; i<7; i++)
	{
		cambia_Dias(i);
		_delay_ms(50); wdt_reset();
	}
	
	display_gotoxy (disp_6,0);    display_puts("-"); wdt_reset();
	cambiaDiaLunes();
	display_gotoxy (disp_6,1);    display_puts("-"); wdt_reset();
	cambiaDiaMartes();
	display_gotoxy (disp_26,0);   display_puts("-"); wdt_reset();
	cambiaDiaMiercoles();
	display_gotoxy (disp_26,1);   display_puts("-"); wdt_reset();
	cambiaDiaJueves();
	display_gotoxy (disp_17,0);   display_puts("-"); wdt_reset();
	cambiaDiaViernes();
	display_gotoxy (disp_17,1);   display_puts("-"); wdt_reset();
	cambiaDiaSabado();
	display_gotoxy (disp_37,0);   display_puts("-"); wdt_reset();
	cambiaDiaDomingo();
}

void desactivacion ()
{
	if(fT==1) //flag timer activado
	{
		#ifdef UART_ESP
			printf(":D0\n");
		#endif
		_delay_ms(500); wdt_reset();
	}
	fT = 0;
	bomba_A_OFF;
	bomba_B_OFF;
	TIMER1_OFF;
	TIMER0_OFF;
	TIMER2_OFF;
	bomba_A_OFF;
	bomba_B_OFF;
}

uint8_t recuperaPx0 (void) //0: ok, 1: error
{
	flag_px0 = 0;
	EIFR  |=  (1<<INTF1);	//flagINT1_OFF;
	EIMSK |=  (1<<INT1);	//INT1_ON;
	sei();					//al llamarse desde dentro de una ISR, se desactivó interrupción global
	uint16_t countPx0 = 0;
	while(flag_px0==0)
	{
		countPx0++;
		if(countPx0>=12000) //12ms > 10ms de semi-período, no hay Px0
		{
			EIMSK &=~  (1<<INT1);	//INT1_OFF;
			return 1;
		}
		else _delay_us(1); wdt_reset();
	}
	EIMSK &=~  (1<<INT1);	//INT1_OFF;
	return 0;
}

void habilitaPulsadores ( void )
{
	PCIFR |= (1<<PCIF0);  //flagPCINT0_OFF;
	PCICR |=  (1<<PCIE0); //PCINT0_ON;
	PCIFR |= (1<<PCIF2);  //flagPCINT2_OFF;
	PCICR |= (1<<PCIE2);  //PCINT2_ON;
}

void activacion ( void )
{
	TCNT0 = 0;
	TCNT2 = 0;
	TCNT1 = 0;
	
	cont_Tiempo_A = 0;
	if(tiempo_A>tiempo_B)
	{
		if( (tiempo_A-tiempo_B) <= DESFASAJE)
		{
			cont_Tiempo_B = tiempo_A-tiempo_B+DESFASAJE;
		}
	}
	else
	{
		if( (tiempo_B-tiempo_A) <= DESFASAJE)
		{
			cont_Tiempo_B = tiempo_B-tiempo_A+DESFASAJE;
		}
	}
	
	contTimer0  = 0;
	contTimer2  = 0;
	if(estado == s_timer)
	{
		if(DIA_ACTIVADO && HORA_ACTIVADO && MIN_ACTIVADO)
		{
			fT = 1;
			#ifdef UART_ESP
				printf(":D1\n");
			#endif
			habilitaPulsadores();
			_delay_ms(500); wdt_reset();
			TIMER1_ON;
		}
	}
	else if(estado == s_enclavado || estado == s_purga )
	{
		fT = 1;
		#ifdef UART_ESP
			printf(":D1\n");
		#endif
		habilitaPulsadores();
		_delay_ms(500); wdt_reset();
		TIMER1_ON;
	}
}

void rutinaPulsador (void)
{
	if(estadoAnterior==s_timer || estadoAnterior==s_enclavado || estadoAnterior == s_apagado)
	{
		_delay_ms(20); //Más tiempo para ambos pulsadores en modo Configuración
		if(!mas_Bomba_A_Test && !mas_Bomba_B_Test)
		{
			_delay_ms(2);
			if(!mas_Bomba_A_Test && !mas_Bomba_B_Test)
			{
				estado = s_menu;
			}
			else estado = estadoAnterior;
		}
		else estado = estadoAnterior;
	}
	else estado = estadoAnterior;

	if(estado != s_menu)
	{
		if(estadoAnterior==s_timer || estadoAnterior==s_enclavado)
		{
			_delay_ms(8);
			if(!mas_Bomba_B_Test)
			{
				_delay_ms(1);
				if(!mas_Bomba_B_Test)
				{
					estado    = s_cambioParametro;
					subestado = s_mas_Dosis_B;
				}
				else estado = estadoAnterior;
			}
			else if(!menos_Bomba_B_Test)
			{
				_delay_ms(1);
				if(!menos_Bomba_B_Test)
				{
					estado    = s_cambioParametro;
					subestado = s_menos_Dosis_B;
				}
				else estado = estadoAnterior;
			}
			else if(!mas_Bomba_A_Test)
			{
				_delay_ms(1);
				if(!mas_Bomba_A_Test)
				{
					estado    = s_cambioParametro;
					subestado = s_mas_Dosis_A;
				}
				else estado = estadoAnterior;
			}
			else if(!menos_Bomba_A_Test)
			{
				_delay_ms(1);
				if(!menos_Bomba_A_Test)  //doble verificación
				{
					estado    = s_cambioParametro;
					subestado = s_menos_Dosis_A;
				}
				else estado = estadoAnterior;
			}
			else estado = estadoAnterior;
		}
		else estado = estadoAnterior;
	}
	if(estado == estadoAnterior) habilitaPulsadores();
	PCIFR |= (1<<PCIF0);
	PCIFR |= (1<<PCIF2);
}


