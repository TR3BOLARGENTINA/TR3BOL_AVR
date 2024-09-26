/*
 * emma-adc.c
 *
 * Created: 29/8/2024 22:03:38
 * Author : Quique
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#define F_CPU 16000000
#include <util/delay.h>
#define VREF 5.0
//#define CMAX 10411.99	//cuenta para 500 volts dividido 100. 
//#define CMAX 8388.608 //con esto llego a +-22V con +-30V reales
#define CMAX 5592.41 //con esto llego a +-22V con +-30V reales
//#define CMAX 12582.91 

// Esto es con el circuito de proteus, que modela la impedancia del MCP3553, 
// típica 2.4 Mohm (depende de la frecuencia de funcionamiento). Para otro ADC, 
// o intercalando seguidores de tensión en Vin+ y Vin-, puede variar.

#define pinCS   3
#define pinMISO 4
#define pinSCK  5

#define CS_OFF  (PORTB &=~(1<<pinCS))
#define CS_ON   (PORTB |= (1<<pinCS))

#define ifMISO   ( PINB   & (1<<pinMISO))

#define SCK_OFF  ( PORTB &=~(1<<pinSCK) )
#define SCK_ON   ( PORTB |= (1<<pinSCK))

char TxBuff[20];

void uart_init(uint32_t bps)
{
	UBRR0=F_CPU/16/bps-1;
	UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
}

void mi_putc(char dato)
{
	while(!(UCSR0A&(1<<UDRE0))){
		_delay_us(1);
	}
	UDR0=dato;
}

void mi_puts(char *cadena)
{
	while(*cadena)
	{
		mi_putc(*cadena);
		cadena++;
	}
}

#define TOPE 24 //ERA 24
uint32_t leeAD_MCP3550(void)
{
	uint32_t adcval=0;
	CS_OFF;
	
	while (ifMISO){
		_delay_loop_1(1);
	}
 	//_delay_ms(100);

	for (uint8_t ck=1;ck<=24;ck++) // se puede hacer también por la interfaz SPI
	//for (uint8_t ck=0;ck<=23;ck++)
	{
		SCK_OFF;
		//_delay_us(2);		
		SCK_ON;
		adcval=(adcval<<1);
		if (ifMISO) adcval|=1;
	}
	CS_ON;
	if(adcval&(0x200000))
	{
		adcval|=0xFFF00000;
	}
	return adcval;
}


int main(void)
{
    int32_t valADC;
	//DDRB=0b00100001;
	//DDRB=0b00101000;
	DDRB |= (1<<pinCS);
	DDRB |= (1<<pinSCK);
	
	uart_init(9600);
	mi_puts("Hola\0");
    while (1) 
    {
		valADC = leeAD_MCP3550();
		valADC = valADC + 33000;
		sprintf(TxBuff,"AD:%ld\t volts:%.1f\r\n", valADC,((float)valADC)*VREF/CMAX);
		mi_puts(TxBuff);
		_delay_ms(100);
    }
}

