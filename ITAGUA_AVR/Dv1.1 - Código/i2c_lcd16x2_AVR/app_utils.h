/*
 * app_utils.h
 *
 * Created: 13/4/2020 16:26:47
 *  Author: Acer
 */ 

#ifndef APP_UTILS_H_
#define APP_UTILS_H_
		
	#define PASO_X_CERO
	#define UART_ESP
	//#define UART_DEBUG_
	//#define CODIGO_PRUEBA
	
	
	#define N_CLEARBUS	5				 //Número de reintentos de recuperación i2c
	
	#define TIMER0_ON			(TCCR0B |=  (3<<CS00))
	#define TIMER0_OFF			(TCCR0B &=~ (7<<CS00))
	#define TIMER2_ON			(TCCR2B |=  (1<<CS22))
	#define TIMER2_OFF			(TCCR2B &=~ (7<<CS20))
	#define TIMER1_ON			(TCCR1B |=  (1<<CS10))
	#define TIMER1_OFF			(TCCR1B &=~ (7<<CS10))
	
	#define menos_Bomba_A_Test	(PINB &  (1<<PINB0))
	#define mas_Bomba_A_Test	(PIND &  (1<<PIND5))
	#define mas_Bomba_B_Test	(PIND &  (1<<PIND2))
	#define menos_Bomba_B_Test	(PIND &  (1<<PIND4))
	
	#define LOOP_DELAY    100
	#define BUZZER_DELAY  80
	#define FIRST_DELAY   70
	#define SECOND_DELAY  7
	#define SHORT_DELAY   1

	#include <avr/eeprom.h>
	
	#include "uart.h"
	#include "wd.h"
	#include "lcdpcf8574.h"
	#include "i2c_restore.h"
	#include "mi_ADC_AVR.h"
	#include "RTC_hal.h"
		
	#define disp_4				 4
	#define disp_Duracion		 5
	#define disp_6				 6
	#define disp_DiaIzq			 7
	#define disp_14				14
	#define disp_DiaDer			18
	#define disp_17				17
	#define disp_Dia_Semana		20
	#define disp_Dia			24
	#define disp_A_Dur_Ciclos	25
	#define disp_26				26
	#define disp_Mes			27
	#define disp_Ancho			28
	#define disp_Ano			30
	#define disp_Hora			33
	#define disp_Dur			34
	#define disp_Dosis_B		35
	#define disp_Min			36
	#define disp_37				37
				
	/*-------------	Salidas Dv1.1 ------------
		PB3 		-> 	salida Bomba B
		PC3			->	Habilita Display (HIGH)
		PD1 		->	Uart Tx (Led PCB, HIGH)
		PD6 		-> 	buzzer (HIGH)
		PD7 		-> 	salida Bomba A	
	*/
	#define bomba_B_OUT		(DDRB  |=  (1<<DDB3))
	#define bomba_B_ON		(PORTB |=  (1<<PORTB3))
	#define bomba_B_OFF		(PORTB &=~ (1<<PORTB3))
	
	#define test_OUT		(DDRB  |=  (1<<DDB5))
	#define test_ON			(PORTB |=  (1<<PORTB5))
	#define test_OFF		(PORTB &=~ (1<<PORTB5))
	#define test_Toggle		(PORTB ^=  (1<<PORTB5))
	
	#define display_OUT		(DDRC  |=  (1<<DDC3))
	#define display_ON		(PORTC |=  (1<<PORTC3))
	#define display_OFF		(PORTC &=~ (1<<PORTC3))

	
	#define buzzer_OUT		(DDRD  |=  (1<<DDD6))
	
	#define buzzer_OFF		(PORTD &=~ (1<<PORTD6))
	
	#define buzzer_ON 		(PORTD |=  (1<<PORTD6))
	#define buzzer_Toggle	(PORTD ^=  (1<<PORTD6))
	
	//#define buzzer_ON 		(buzzer_OFF)
	//#define buzzer_Toggle	(buzzer_OFF)
	
	#define bomba_A_OUT		(DDRD  |=  (1<<DDD7))
	#define bomba_A_ON		(PORTD |=  (1<<PORTD7))
	#define bomba_A_OFF		(PORTD &=~ (1<<PORTD7))
		
	#define	CH_WORD			"   "	// Palabra o caracteres a verificar para evitar símbolos chinos.
	
	// Parámetros para Dosis A
	#define size_Dosis    12
		extern const uint16_t	lista_Tiempos[size_Dosis];
		extern const uint16_t	lista_Dosis[size_Dosis];
	#define	PRECISION_Dosis 1		// Discretización del valor de Dosis A mostrar en Display
			
	uint8_t estadoAnterior;
	//-----Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: accessPoint 1, 6: accessPoint 2
	enum tEstado {s_apagado, s_purga, s_timer, s_configuraTimer, s_enclavado, s_cambioParametro, s_menu, s_pulsadores, s_uart, s_errorPx0, s_cambioModo};
	volatile enum tEstado estado;
	
	enum subEstado {s_nada, s_menos_Dosis_A, s_mas_Dosis_A, s_menos_Dosis_B, s_mas_Dosis_B};
	volatile enum subEstado subestado;
	
	#define desfasaje_Min	   0
	#define desfasaje_Max	 100
	#define ancho_Min		  20
	#define ancho_Max		  70
	#define curva_Min		   0
	#define curva_Max		  50
	//---------- Bombas A y B -----------//
	#define dosis_Min		  2
	#define dosis_Max	      100
		
	#define duracion_Min	   1
	#define duracion_Max	  30
	
	#define ciclos_Min	       1
	#define ciclos_Max	      12
	
	#define hora_Min	       0
	#define hora_Max	      23
	
	#define cantidad_Horas	  12
	
	//Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: WiFi 1, 6: WiFi 2
	#define modo_Min	       0
	#define modo_Max	       6
		
	extern uint8_t  EEMEM eeprom_PCF;
	volatile uint8_t  tipoPCF8574;
	
	volatile uint8_t  pDIA_SEMANA, pHORAS, pMINUTOS, pSEGUNDOS,pDIA, pMES, pANIO;
	volatile uint16_t tiempo_A, tiempo_B;
	
	volatile uint16_t cont_Tiempo_A;
	volatile uint16_t cont_Tiempo_B;
	
	volatile uint8_t flag_px0;
	
	//Se podrían agrupar cadenas del mismo tamaño!! no hace falta una para cada variable!
	char cadena_Dia_Semana[5];
	char cadena_Dia[6];
	char cadena_Mes[7];
	char cadena_Anio[8];   //bug, no pueden ser del mismo tamaño escrituras consecutivas del display???
	char cadena_Hora[4];
	char cadena_Min[5];   

	char cadena_Dosis_A[6];
	char cadena_Dosis_B[7];
	
	char cadena_Tiempo_Purga[6];
	char cadena_HoraAct[6];
	
	char cadena_Display2[3];
			
			
	volatile uint16_t lecturaActualAD, lectura1, lectura2, lectura3;
	volatile uint16_t centenas, decenas, unidades; 
	
	volatile uint8_t flagTimer0;
	volatile uint8_t flagTimer1;
	volatile uint8_t flagTimer2;
	
	volatile uint8_t contTimer1;
	volatile uint8_t contTimer0;
	volatile uint8_t contTimer2;
	
	/// Tipo buffer para display, mensaje en pantalla
	struct DisplayBuffer {
		char line0[LCD_DISP_LENGTH];
		char line1[LCD_DISP_LENGTH];
		uint8_t x;
		uint8_t y;
		uint8_t state;
	};

	extern volatile uint8_t display_watchdog_state;
	extern struct DisplayBuffer main_dispbuffer; /// Buffer de mensaje en display para recover
	
	extern uint8_t EEMEM eeprom_HorasACT[cantidad_Horas];
	extern uint8_t EEMEM eeprom_anchoA;
	extern uint8_t EEMEM eeprom_anchoB;
	extern uint8_t EEMEM eeprom_curvaA;
	extern uint8_t EEMEM eeprom_curvaB;
	extern uint8_t EEMEM eeprom_Desfasaje;
	extern uint8_t EEMEM eeprom_Lunes;
	extern uint8_t EEMEM eeprom_Martes;
	extern uint8_t EEMEM eeprom_Miercoles;
	extern uint8_t EEMEM eeprom_Jueves;
	extern uint8_t EEMEM eeprom_Viernes;
	extern uint8_t EEMEM eeprom_Sabado;
	extern uint8_t EEMEM eeprom_Domingo;
	extern uint8_t EEMEM eeprom_Ciclos;
	extern uint8_t EEMEM eeprom_Modo;
	extern uint8_t EEMEM eeprom_Duracion;
	extern uint8_t EEMEM eeprom_Dosis_A;
	extern uint8_t EEMEM eeprom_Dosis_B;
			
	volatile uint8_t  horas_act[cantidad_Horas];
	volatile uint8_t  ANCHO_PULSO_A, ANCHO_PULSO_B, DESFASAJE, CORRECCION_CURVA_A, CORRECCION_CURVA_B;
	volatile uint8_t  lunes, martes, miercoles, jueves, viernes, sabado, domingo;
	volatile uint8_t  modo, duracion, ciclos;
	volatile uint8_t  dosis_A,  dosis_B;
	volatile uint8_t  bomba_A_APAGADA, bomba_B_APAGADA;
	
	void buzzer_N_times (uint8_t N_times);
	
	void iniciaPines( void) ;

	void iniciaVariables( void );

	void Bruto_a_char( float bruto );

	void buffer_clean( struct DisplayBuffer *dispbuffer );

	void buffer_newline( struct DisplayBuffer *dispbuffer );

	void buffer_setxy( struct DisplayBuffer *dispbuffer, uint8_t x, uint8_t y );

	void buffer_puts( struct DisplayBuffer *dispbuffer, const char *s );

	#ifdef UART_DEBUG_
		void buffer_print( struct DisplayBuffer *dispbuffer );
	#endif // UART_DEBUG_

	void buffer_recover( struct DisplayBuffer *dispbuffer );

	void display_init( uint8_t recover_flag );

	void display_clear ( void );
	
	void display_clear2 ( void );
	
	void display_clear2_wait ( void );
	
	void display_clear3 ( void );
	
	void display_gotoxy( uint8_t x, uint8_t y );

	void display_puts( const char *s );

	void display_restore( uint8_t bus_state );

	void display_watchdog( void );

	//void codigoDePrueba();

	void itaguaDisplay();
	
	void iniciaLCD();

	/*void puntitos( void );
	
	void esperePorFavor( void );
	
	void inicializando( void );*/

	void saludoInicial( void );
	
	void cambia_Fecha ( void );
	
	void cambia_Hora ( void );
	
	void cambia_Horas_Activacion ( uint8_t );
	
	void cambia_Dias ( uint8_t );
	
	void cambia_Desfasaje ( void );
	
	void cambia_ancho ( uint8_t );
	
	void cambia_correccion_curva ( uint8_t );
	
	void cambia_Ciclos ( void );
	
	void cambia_Modo ( void );
				
	void cambia_Duracion ( void );
	
	void cambia_Tiempo_Purga (uint16_t timePurga);
	
	//void cambia_Version ( void );
	
	void cambia_Dosis_A ( void );
	
	void cambia_Dosis_B ( void );
	
	uint8_t check_ITAGUA( void );
	
	char lcd_read_ITAGUA( void );
	
	void miDelay_ms_enclavado(uint16_t kMax);
	
	void miDelay_ms_apagado(uint16_t kMax);
	
	void miDelay_ms_timer(uint16_t kMax);
	
	void dosis_a_tiempo_A( void );
	
	void dosis_a_tiempo_B( void );
	
	void display_Dosis( void );
	
	void display_Dia_Semana( void );
	
	void display_Fecha( void );
	
	void display_Hora( void );
	
	void mem_Dosis_A ( void );
	
	void mem_Dosis_B ( void );
	
	void setDosisAB ( void );
	
	void mem_Modo ( void );
	
	//------------ Máquina de Estados ------- //
	
	void rutinaApagado ( void );
	
	void rutinaEnclavado ( void );
	
	void modoAnterior ( void );
		
	void rutinaTimer ( void );
	
	void rutinaCambioParametro ( void );
	
	void seleccione_desfasaje ( void );
		
	void seleccione_anchos ( void );
	
	void seleccione_correccion_curvas ( void );
	
	void seleccioneModo ( void );
	
	void seleccioneDuracion ( void );
	
	void seleccioneCiclos ( void );
	
	void seleccioneHorarios ( void );
	
	void seleccioneDias ( void );
	
	void rutinaMenu ( void );
	
	void rutinaPurga ( void );
	
	//void rutinaContinuo ( void );
	
	void rutinaPulsador ( void );
	
	void habilitaPulsadores ( void );
	
	void desactivacion ( void );
	
	void activacion ( void  );
			
	uint8_t recuperaPx0 ( void );
					
#endif /* APP_UTILS_H_ */