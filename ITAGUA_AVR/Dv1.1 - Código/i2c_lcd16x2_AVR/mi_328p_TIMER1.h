#include "mi_TC1.h"

void configura_OndaCuadrada_TC1(float T)
{
		T = T/2.0;
		configura_Modo_TC1(4);		//4: CTC
		configura_Prescaler_TC1(T);
		OCR1A = (uint16_t)(  ( (T/1000.0)*(F_CPU/prescaler_TC1)+1 ) ); 
		#ifdef UART_DEBUG_
			printf("OCR1A = %d\r\n",OCR1A);
		#endif
}

void configuraTIMER1()
 {
		/*Reducción de energía para timer1 */
		PRR &=~ (1<<PRTIM1);
				
		/*para onda cuadrada*/
		configura_OndaCuadrada_TC1 (2.0); // período [ms]
		//TCCR1B &=~ (7<<CS10);			  // Apaga timer 1
		
		//modo salidas (pines asociados)//
		configura_ModoSalidas_TC1(0,0);	//(salidaA,salidaB) --> 0: off, 1: toggle; 2: clear; 3: set
		//ejemplo: (1,0) para onda cuadrada en A (y B apagado);
		//         (2,3) para PWM complementario en A y B (clear: PWM al derecho, set: PWM invertido)
		
		/*habilita interrupciones*/
		interrupciones_TC1(0,1,0,0); //(InputCapture,CompareA,CompareB,Overflow)
 }

uint8_t waitPx0(void) //0: ok, 1: error
{
	TCCR1B &=~ (7<<CS10);   //Apaga Timer 1
	
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
	
	TCCR1B |= (1<<CS10);	//Prende Timer 1
	EIMSK &=~  (1<<INT1);	//INT1_OFF;
	return 0;
}

//Por comparación A
uint8_t error_Px0 = 0;
ISR(TIMER1_COMPA_vect)
{
	//led_Toggle; //Verificado entra cada 1ms
	cont_Tiempo_A++;	
	if(cont_Tiempo_A >= tiempo_A)
	{
		cont_Tiempo_A = 0;	
		error_Px0 = 0;
		#ifdef PASO_X_CERO
			error_Px0 = waitPx0();	//0: ok, 1: error
		#endif
		if(error_Px0)
		{
			desactivacion();
			if(estado != s_purga) estadoAnterior = estado;
			estado = s_errorPx0;
		}
		else if(bomba_A_APAGADA==0)
		{
			bomba_A_ON; 			//Prende Bomba A
			TCNT0 = 0;			    //Resetea Timer 0
			contTimer0 = 0;			//Resetea contador Timer 0
			TIFR0  |= (1<<OCF0A);   //apaga flag Timer_0_COMPA
			TIMER0_ON;				//Habilita Timer 0 <-- apaga bomba A al completar 40ms
		}
	}
	if(error_Px0==0)
	{
		cont_Tiempo_B++;
		if(cont_Tiempo_B >= tiempo_B)
		{
			cont_Tiempo_B = 0;
			error_Px0 = 0;
			#ifdef PASO_X_CERO
				error_Px0 = waitPx0();	//0: ok, 1: error
			#endif
			if(error_Px0)
			{
				desactivacion();
				if(estado != s_purga) estadoAnterior = estado;
				estado = s_errorPx0;
			}
			else if(bomba_B_APAGADA==0)
			{
				bomba_B_ON;			   //Prende Bomba B
				TCNT2 = 0;			   //Resetea Timer 2
				contTimer2 = 0;		   //Resetea contador Timer2
				TIFR2  |= (1<<OCF2A);  //apaga flag Timer_2_COMPA
				TIMER2_ON;			   //Habilita Timer 2 <-- apaga Bomba B al completar 40ms
			}	
		}
	}
	TIFR1  |= (1<<OCF1A); //apaga flag Timer 1
}

//Por input capture
/*ISR(TIMER1_CAPT_vect)
{
	PORTB ^= (1<<PORTB5);
}*/

/*
//Por overflow
ISR(TIMER1_OVF_vect)
{
	//B3_toggle;
}

//Por comparación B
ISR(TIMER1_COMPB_vect)
{
	//B3_toggle;
}*/