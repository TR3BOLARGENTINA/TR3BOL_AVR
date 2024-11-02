/*
 * IncFile1.h
 *
 * Created: 9/6/2017 9:45:45 a. m.
 *  Author: Emmanuel Jordán
 */ 

#ifndef INCFILE1_H_
#define INCFILE1_H_

#include <avr/interrupt.h>
#include <stdio.h>

#include "328p_interrupcionPorCambio_2020.h"
#include "app_utils.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

uint8_t flagDisparo = 0;

//Rutina de interrupcion externa INT0
//PD2/INT0		→ 	Más Bomba B
/*ISR(INT0_vect)
{
	if(estado==s_reposo)
	{
		_delay_ms(10);
	}
	EIFR  |= (1<<INTF0);
}*/

//Rutina de interrupcion externa INT1
//PD3/INT1		→ 	Paso por Cero
#define px0_Test	(PIND   &  (1<<PIND3))
ISR(INT1_vect)
{
	flag_px0 = 1;
	EIFR  |= (1<<INTF1);
}

void configuraINT0(void)
{
	DDRD  &=~ (1<<DDD2);	//INT0_entrada;
	//PORTD |=  (1<<PIND2); //INT0_pullUp;
	EICRA &=~ (1<<ISC00);	//INT0_flancoBajada
	EICRA |=  (1<<ISC01);
	EIFR  |=  (1<<INTF0);	//flagINT0_OFF;
	EIMSK |=  (1<<INT0);	//INT0_ON;
}

void configuraINT1(void)
{
	DDRD  &=~ (1<<DDD3);	//INT1_entrada;
	//PORTD |=  (1<<PIND3); //INT1_pullUp;
	EICRA &=~ (1<<ISC10); //INT1_flancoBajada
	EICRA |=  (1<<ISC11);
	//EICRA |= (3<<ISC10);	//INT1_flancoSubida
	
	//EIFR  |=  (1<<INTF1);	//flagINT1_OFF;
	//EIMSK |=  (1<<INT1);	//INT1_ON;
}

//PD2/INT0		→ 	Más Bomba B
//PD3/INT1		→ 	Paso por Cero
void configuraInterrupcionExterna()
{
	//configuraINT0();
	configuraINT1(); //no habilita inicialmente (segun flag PX0)
}

#endif /* INCFILE1_H_ */