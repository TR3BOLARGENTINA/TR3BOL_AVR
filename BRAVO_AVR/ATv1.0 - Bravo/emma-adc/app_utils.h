/*
 * app_utils.h
 *
 * Created: 13/4/2020 16:26:47
 *  Author: Acer
 */ 

#ifndef APP_UTILS_H_
#define APP_UTILS_H_
		
	#define UART_ESP	
	
	#define TIMER0_ON			(TCCR0B |=  (3<<CS00))
	#define TIMER0_OFF			(TCCR0B &=~ (7<<CS00))
	#define TIMER2_ON			(TCCR2B |=  (1<<CS22))
	#define TIMER2_OFF			(TCCR2B &=~ (7<<CS20))
	#define TIMER1_ON			(TCCR1B |=  (1<<CS10))
	#define TIMER1_OFF			(TCCR1B &=~ (7<<CS10))
	
	//PB0/PCINT0 	-->	P1
	//PD5/PCINT21	-->	P2
	//PD4/PCINT20	-->	P3
	//PD2/PCINT18	-->	P4
	
	#define P1_Test		(PINB &  (1<<PINB0))
	#define P2_Test		(PIND &  (1<<PIND5))
	#define P3_Test		(PIND &  (1<<PIND4))
	#define P4_Test		(PIND &  (1<<PIND2))
	
	#define LOOP_DELAY    100
	#define BUZZER_DELAY  80
	#define FIRST_DELAY   70
	#define SECOND_DELAY  7
	#define SHORT_DELAY   1

	#include <avr/eeprom.h>
	
	#include "uart.h"
	#include "wd.h"
	//#include "lcdpcf8574.h"
	//#include "i2c_restore.h"
	//#include "mi_ADC_AVR.h"
	#include "RTC_hal.h"
		
	#define pinCS   PORTB1
	#define pinMISO PORTB4
	#define pinSCK  PORTB5

	#define CS_OFF  (PORTB &=~(1<<pinCS))
	#define CS_ON   (PORTB |= (1<<pinCS))
	#define CS_OUT  (DDRB |= (1<<pinCS))

	#define ifMISO   ( PINB   & (1<<pinMISO))

	#define SCK_OFF  ( PORTB &=~(1<<pinSCK) )
	#define SCK_ON   ( PORTB |= (1<<pinSCK))

	/*-------------	Salidas Dv1.1 ------------
		PD6 		-> 	buzzer (HIGH)
	*/
	
	#define buzzer_OUT		(DDRD  |=  (1<<DDD6))
	#define buzzer_OFF		(PORTD &=~ (1<<PORTD6))
	#define buzzer_ON 		(PORTD |=  (1<<PORTD6))
	#define buzzer_Toggle	(PORTD ^=  (1<<PORTD6))
	//#define buzzer_ON 		(buzzer_OFF)
	//#define buzzer_Toggle	(buzzer_OFF)
	
	//-----Modo 0: apagado, 1: purga, 2: timer, 3: configuraTimer, 4: enclavado, 5: accessPoint 1, 6: accessPoint 2
	enum tEstado {s_reposo, s_enviaVolt, s_pulsadores};
	volatile enum tEstado estado;
		
	
	volatile uint16_t centenas, decenas, unidades; 
	
	volatile uint8_t flagTimer0;
	volatile uint8_t flagTimer1;
	volatile uint8_t flagTimer2;
	
	volatile uint8_t contTimer1;
	volatile uint8_t contTimer0;
	volatile uint8_t contTimer2;
	
	volatile uint8_t  pDIA_SEMANA, pHORAS, pMINUTOS, /*pSEGUNDOS, */pDIA, pMES, pANIO;
	
	//extern uint8_t EEMEM eeprom_HorasACT[cantidad_Horas];
	//extern uint8_t EEMEM eeprom_anchoA;

	//volatile uint8_t  lunes, martes, miercoles, jueves, viernes, sabado, domingo;

	void buzzer_N_times (uint8_t N_times);
	
	void iniciaPines( void) ;

	void iniciaVariables( void );

	#ifdef UART_DEBUG_
		void buffer_print( struct DisplayBuffer *dispbuffer );
	#endif // UART_DEBUG_
	
	void miDelay_ms_reposo(uint16_t kMax);
	
	
	//------------ Máquina de Estados ------- //
	void rutinaTimer ( void );
	
	void rutinaCambioParametro ( void );
	
	void rutinaPulsador ( void );
	
	void habilitaPulsadores ( void );
			
	uint8_t recuperaPx0 ( void );
					
#endif /* APP_UTILS_H_ */