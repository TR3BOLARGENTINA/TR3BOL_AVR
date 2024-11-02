/*
 * interrupcionPorCambio.h
 *
 * Created: 9/6/2017 10:16:46 a. m.
 *  Author: Emmanuel Jordán
 */ 

#ifndef INTERRUPCIONPORCAMBIO_H_
#define INTERRUPCIONPORCAMBIO_H_

#ifndef F_CPU
	#define F_CPU 16000000
#endif

#include <avr/interrupt.h>
#include <stdio.h>

#include "app_utils.h"

#define PCINT0_in     (DDRB &=~  (1<<PINB0))
#define PCINT1_in     (DDRB &=~  (1<<PINB1))
#define PCINT2_in     (DDRB &=~  (1<<PINB2))
#define PCINT3_in     (DDRB &=~  (1<<PINB3))
#define PCINT4_in     (DDRB &=~  (1<<PINB4))
#define PCINT5_in     (DDRB &=~  (1<<PINB5))
#define PCINT6_in     (DDRB &=~  (1<<PINB6))
#define PCINT7_in     (DDRB &=~  (1<<PINB7))
#define PCINT8_in     (DDRC &=~  (1<<PINC0))
#define PCINT9_in     (DDRC &=~  (1<<PINC1))
#define PCINT10_in    (DDRC &=~  (1<<PINC2))
#define PCINT11_in    (DDRC &=~  (1<<PINC3))
#define PCINT12_in    (DDRC &=~  (1<<PINC4))
#define PCINT13_in    (DDRC &=~  (1<<PINC5))
#define PCINT14_in    (DDRC &=~  (1<<PINC6))
#define PCINT16_in    (DDRD &=~  (1<<PIND0))
#define PCINT17_in    (DDRD &=~  (1<<PIND1))
#define PCINT18_in    (DDRD &=~  (1<<PIND2))
#define PCINT19_in    (DDRD &=~  (1<<PIND3))
#define PCINT20_in    (DDRD &=~  (1<<PIND4))
#define PCINT21_in    (DDRD &=~  (1<<PIND5))
#define PCINT22_in    (DDRD &=~  (1<<PIND6))
#define PCINT23_in    (DDRD &=~  (1<<PIND7))

#define PCINT0_PU     (PORTB |=  (1<<PINB0))
#define PCINT1_PU     (PORTB |=  (1<<PINB1))
#define PCINT2_PU     (PORTB |=  (1<<PINB2))
#define PCINT3_PU     (PORTB |=  (1<<PINB3))
#define PCINT4_PU     (PORTB |=  (1<<PINB4))
#define PCINT5_PU     (PORTB |=  (1<<PINB5))
#define PCINT6_PU     (PORTB |=  (1<<PINB6))
#define PCINT7_PU     (PORTB |=  (1<<PINB7))
#define PCINT8_PU     (PORTC |=  (1<<PINC0))
#define PCINT9_PU     (PORTC |=  (1<<PINC1))
#define PCINT10_PU    (PORTC |=  (1<<PINC2))
#define PCINT11_PU    (PORTC |=  (1<<PINC3))
#define PCINT12_PU    (PORTC |=  (1<<PINC4))
#define PCINT13_PU    (PORTC |=  (1<<PINC5))
#define PCINT14_PU    (PORTC |=  (1<<PINC6))
#define PCINT16_PU    (PORTD |=  (1<<PIND0))
#define PCINT17_PU    (PORTD |=  (1<<PIND1))
#define PCINT18_PU    (PORTD |=  (1<<PIND2))
#define PCINT19_PU    (PORTD |=  (1<<PIND3))
#define PCINT20_PU    (PORTD |=  (1<<PIND4))
#define PCINT21_PU    (PORTD |=  (1<<PIND5))
#define PCINT22_PU    (PORTD |=  (1<<PIND6))
#define PCINT23_PU    (PORTD |=  (1<<PIND7))

#define PCINT0_test     (PINB &  (1<<PINB0))
#define PCINT1_test     (PINB &  (1<<PINB1))
#define PCINT2_test     (PINB &  (1<<PINB2))
#define PCINT3_test     (PINB &  (1<<PINB3))
#define PCINT4_test     (PINB &  (1<<PINB4))
#define PCINT5_test     (PINB &  (1<<PINB5))
#define PCINT6_test     (PINB &  (1<<PINB6))
#define PCINT7_test     (PINB &  (1<<PINB7))
#define PCINT8_test     (PINC &  (1<<PINC0))
#define PCINT9_test     (PINC &  (1<<PINC1))
#define PCINT10_test    (PINC &  (1<<PINC2))
#define PCINT11_test    (PINC &  (1<<PINC3))
#define PCINT12_test    (PINC &  (1<<PINC4))
#define PCINT13_test    (PINC &  (1<<PINC5))
#define PCINT14_test    (PINC &  (1<<PINC6))
#define PCINT16_test    (PIND &  (1<<PIND0))
#define PCINT17_test    (PIND &  (1<<PIND1))
#define PCINT18_test    (PIND &  (1<<PIND2))
#define PCINT19_test    (PIND &  (1<<PIND3))
#define PCINT20_test    (PIND &  (1<<PIND4))
#define PCINT21_test    (PIND &  (1<<PIND5))
#define PCINT22_test    (PIND &  (1<<PIND6))
#define PCINT23_test    (PIND &  (1<<PIND7))

//PB0/PCINT0 	→ 	Menos Bomba A
ISR(PCINT0_vect)
{
	if(estado==s_timer || estado==s_enclavado)
	{
		desactivacion(); cli();
		estadoAnterior = estado;
		estado = s_pulsadores;
		PCICR &=~  (1<<PCIE0); //PCINT0_OFF;
	}
	//PCIFR |= (1<<PCIF0);
	//PCIFR |= (1<<PCIF2);
}

//PD2/PCINT18	→	Más Bomba B	  
//PD4/PCINT20	→ 	Menos Bomba B
//PD5/PCINT21	→ 	Más Bomba A 
//Simultáneo Más Bombas A y B
ISR(PCINT2_vect)
{ 
	if(estado==s_timer || estado==s_enclavado || estado == s_apagado)
	{
		cli();
		estadoAnterior = estado;
		estado = s_pulsadores;
		PCICR &=~ (1<<PCIE2);  //PCINT2_OFF;
	}
	//PCIFR |= (1<<PCIF0);
	//PCIFR |= (1<<PCIF2);
}

void habilita_Pin_X_PCINT_0_7(uint8_t PIN)
{
	switch(PIN)
	{
		case 0:
				PCINT0_in;
				//PCINT0_PU;
				PCMSK0 |= (1<<PCINT0);
				break;
		case 1:
				PCINT1_in;
				//PCINT1_PU;
				PCMSK0 |= (1<<PCINT1);
				break;
		case 2:
				PCINT2_in;
				//PCINT2_PU;
				PCMSK0 |= (1<<PCINT2);
				break;
		case 3:
				PCINT3_in;
				//PCINT3_PU;
				PCMSK0 |= (1<<PCINT3);
				break;
		case 4:
				PCINT4_in;
				//PCINT4_PU;
				PCMSK0 |= (1<<PCINT4);
				break;
		case 5:
				PCINT5_in;
				//PCINT5_PU;
				PCMSK0 |= (1<<PCINT5);
				break;
		case 6:
				PCINT6_in;
				//PCINT6_PU;
				PCMSK0 |= (1<<PCINT6);
				break;
		case 7:
				PCINT7_in;
				//PCINT7_PU;
				PCMSK0 |= (1<<PCINT7);
				break;
		default:
				//printf("Pin Invalido\r\n");
				break;
	}
}

void habilita_Pin_X_PCINT_8_14(uint8_t PIN)
{
	switch(PIN)
	{
		case 8:
				PCINT8_in;
				//PCINT8_PU;
				PCMSK1 |= (1<<PCINT8);
				break;
		case 9:
				PCINT9_in;
				//PCINT9_PU;
				PCMSK1 |= (1<<PCINT9);
				break;
		case 10:
				PCINT10_in;
				//PCINT10_PU;
				PCMSK1 |= (1<<PCINT10);
				break;
		case 11:
				PCINT11_in;
				//PCINT11_PU;
				PCMSK1 |= (1<<PCINT11);
				break;
		case 12:
				PCINT12_in;
				//PCINT12_PU;
				PCMSK1 |= (1<<PCINT12);
				break;
		case 13:
				PCINT13_in;
				//PCINT13_PU;
				PCMSK1 |= (1<<PCINT13);
				break;
		case 14:
				PCINT14_in;
				//PCINT14_PU;
				PCMSK1 |= (1<<PCINT14);
				break;
		default:
				//printf("Pin Invalido\r\n");
				break;
	}
}

void habilita_Pin_X_PCINT_16_23(uint8_t PIN)
{
	switch(PIN)
	{
		case 16:
				PCINT16_in;
				//PCINT16_PU;
				PCMSK2 |= (1<<PCINT16);
				break;
		case 17:
				PCINT17_in;
				//PCINT17_PU;
				PCMSK2 |= (1<<PCINT17);
				break;
		case 18:
				PCINT18_in;
				//PCINT18_PU;
				PCMSK2 |= (1<<PCINT18);
				break;
		case 19:
				PCINT19_in;
				//PCINT19_PU;
				PCMSK2 |= (1<<PCINT19);
				break;
		case 20:
				PCINT20_in;
				//PCINT20_PU;
				PCMSK2 |= (1<<PCINT20);
				break;
				
		case 21:
				PCINT21_in;
				//PCINT21_PU;
				PCMSK2 |= (1<<PCINT21);
				break;
		case 22:
				PCINT22_in;
				//PCINT22_PU;
				PCMSK2 |= (1<<PCINT22);
				break;
		case 23:
				PCINT23_in;
				//PCINT23_PU;
				PCMSK2 |= (1<<PCINT23);
				break;
		default:
				//printf("Pin Invalido\r\n");
				break;
	}
}

//PB0/PCINT0 	→ 	Menos Bomba A
void configuraPCINT_grupo0()
{
	PCMSK0 &=~ (0xFF<<PCINT0);
	habilita_Pin_X_PCINT_0_7(0);
	PCIFR |= (1<<PCIF0);  //flagPCINT0_OFF;
	PCICR |=  (1<<PCIE0); //PCINT0_ON;
}

//Pulsador P5 --> Reset Reloj      - PC2/PCINT10
/*void configuraPCINT_grupo1()
{
	PCMSK1 &=~ (0xFF<<PCINT8);
	habilita_Pin_X_PCINT_8_14(10);
	PCIFR |= (1<<PCIF1); //flagPCINT1_OFF;
	PCICR |=  (1<<PCIE1); //PCINT1_ON;
}*/

//Pulsador P2 --> más Dosis A   - PD4/PCINT20
//Pulsador P1 --> menos Dosis A - PD5/PCINT21
void configuraPCINT_grupo2()
{
	PCMSK2 &=~ (0xFF<<PCINT16);
	habilita_Pin_X_PCINT_16_23(18);
	habilita_Pin_X_PCINT_16_23(20);
	habilita_Pin_X_PCINT_16_23(21);
	PCIFR |= (1<<PCIF2);  //flagPCINT2_OFF;
	PCICR |= (1<<PCIE2);  //PCINT2_ON;
}

//PB0/PCINT0 	→ 	Menos Bomba A
//PD4/PCINT20	→ 	Menos Bomba B
//PD5/PCINT21	→ 	Más Bomba A
void configuraInterrupcionPorCambio()
{
	configuraPCINT_grupo0();
	//configuraPCINT_grupo1();
	configuraPCINT_grupo2();
}
#endif /* INTERRUPCIONPORCAMBIO_H_ */