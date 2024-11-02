#include "mi_TC0.h"
#include "mi_TC2.h"

//----------------------------------------------------------//
//                   TIMER 0 (8 bits)                       //
//----------------------------------------------------------//
void configura_PWM_TC0(float T, float dutyA, float dutyB)		//(período,dutyCicleA,dutyCicleB) [ms]
{
	configura_Modo_TC0(3);	   //3 fast PWM (Ton con OCRA, T = 0xFF)
	configuraPrescaler_TC0(T); //(período) [ms]
	OCR0A = (uint8_t)((dutyA/1000)*(F_CPU/prescaler_TC0)+1);
	OCR0B = (uint8_t)((dutyB/1000)*(F_CPU/prescaler_TC0)+1);
}

void configura_OndaCuadrada_TC0(float T)					   //(período) [ms]
{
	T=T/2.0;
	configura_Modo_TC0(2);		  //2 CTC (ídem modo 4 timer1)
	configuraPrescaler_TC0(T);   //(período) [ms]
	OCR0A = (uint8_t)(  ( (float)(T/1000.0)*(float)(F_CPU/prescaler_TC0)+1.0 ) );
	#ifdef UART_DEBUG_
		printf("OCR0A = %d\r\n",OCR0A);
	#endif
}

void configura_OndaCuadrada_TC2(float T)					   //(período) [ms]
{
	T=T/2.0;
	configura_Modo_TC2(2);		  //2 CTC (ídem modo 4 timer1)
	configuraPrescaler_TC2(T);   //(período) [ms]
	OCR2A = (uint8_t)(  ( (float)(T/1000.0)*(float)(F_CPU/prescaler_TC2)+1.0 ) );
	#ifdef UART_DEBUG_
		printf("OCR2A = %d\r\n",OCR2A);
	#endif
}

void configuraTIMER0()
{
		prescaler_TC0 = 0;
		contTimer0    = 0;
	 
		PRR &=~ (1<<PRTIM0); //reducción de energía para timer 0
		
		/*para Modo PWM*/
		//configura_PWM_TC0(16.0,1.5,1.5);		//(período,dutyCicleA,dutyCicleB) [ms]
		
		/*para onda cuadrada*/ 
		configura_OndaCuadrada_TC0 (2.0);		//(período) [ms]
		
		/*modo salidas OC0A y OC0B*/ 
		configura_ModoSalidas_TC0(0,0);			//(salidaA,salidaB) --> 0: off, 1: toggle; 2: clear; 3: set
		//ejemplo: (1,0) para onda cuadrada en A, (2,3) para PWM complementario en A,B
		//clear es modo al derecho en PWM, set es modo invertido en PWM
		
		/*habilitación de interrupciones*/ 
		interrupciones_TC0(1,0,0); // habilita interrupciones (CompareA,CompareB,Overflow)
 }

void configuraTIMER2()
{
	prescaler_TC2 = 0;
	contTimer2    = 0;
	
	PRR &=~ (1<<PRTIM2); //reducción de energía para timer 0
	
	/*para Modo PWM*/
	//configura_PWM_TC0(16.0,1.5,1.5);		//(período,dutyCicleA,dutyCicleB) [ms]
	
	/*para onda cuadrada*/
	configura_OndaCuadrada_TC2 (2.0);		//(período) [ms]
	
	/*modo salidas OC0A y OC0B*/
	configura_ModoSalidas_TC2(0,0);			//(salidaA,salidaB) --> 0: off, 1: toggle; 2: clear; 3: set
	//ejemplo: (1,0) para onda cuadrada en A, (2,3) para PWM complementario en A,B
	//clear es modo al derecho en PWM, set es modo invertido en PWM
	
	/*habilitación de interrupciones*/
	interrupciones_TC2(1,0,0); // habilita interrupciones (CompareA,CompareB,Overflow)
}

//Por comparación A

/*volatile static uint32_t millis_c = 0;

uint32_t millis(void){
	return millis_c;
}*/

ISR(TIMER0_COMPA_vect)
{
	contTimer0++;
	if(contTimer0>=ANCHO_PULSO_A)
	{
		contTimer0 = 0;
		flagTimer0 = 1;
		bomba_A_OFF;
		TIMER0_OFF;
	}
	TIFR0 |= (1<<OCF0A); //apaga flag Timer_0_COMPA
}

//Por comparación A
ISR(TIMER2_COMPA_vect)
{
	contTimer2++;
	if(contTimer2>=ANCHO_PULSO_B)
	{
		contTimer2 = 0;
		flagTimer2 = 1;
		bomba_B_OFF;
		TIMER2_OFF;
	}
	TIFR2 |= (1<<OCF2A); //apaga flag Timer_2_COMPA
}
