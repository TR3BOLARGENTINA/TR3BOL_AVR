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
#define CMAX 10411.99	//cuenta para 500 volts dividido 100. 
// Esto es con el circuito de proteus, que modela la impedancia del MCP3553, 
// típica 2.4 Mohm (depende de la frecuencia de funcionamiento). Para otro ADC, 
// o intercalando seguidores de tensión en Vin+ y Vin-, puede variar.

#define nCS 0
#define SCK (1<<5)
#define MISO (PINB&(1<<4))
#define SCKlow (PORTB&=~SCK)
#define SCKhigh (PORTB|=SCK)
char TxBuff[20];

void uart_init(uint32_t bps)
{
	UBRR0=F_CPU/16/bps-1;
	UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
}

void mi_putc(char dato)
{
	while(!(UCSR0A&(1<<UDRE0)));
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

uint32_t leeAD_MCP3550(void)
{
	uint32_t adcval=0;
	PORTB&=~(1<<nCS);
	while (MISO);
//	_delay_ms(100);

	for (uint8_t ck=1;ck<=24;ck++) // se puede hacer también por la interfaz SPI
	{
		SCKlow;		
		SCKhigh;
		adcval=(adcval<<1);
		if (MISO) adcval|=1;
	}
	PORTB|=(1<<nCS);
	if(adcval&(0x200000))
	{
		adcval|=0xFFF00000;
	}
	return adcval;
}


int main(void)
{
    int32_t valADC;
	DDRB=0b00100001;
	uart_init(9600);
	mi_puts("Hola\0");
    while (1) 
    {
		valADC = leeAD_MCP3550();
		sprintf(TxBuff,"AD:%ld\t volts:%.3f\r\n", valADC,((float)valADC)*VREF/CMAX);
		mi_puts(TxBuff);
		_delay_ms(100);
    }
}

