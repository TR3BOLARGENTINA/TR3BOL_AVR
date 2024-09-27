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

// Definir los pines de SPI
#define nCS     1  
#define CS_off  (PORTB |=  (1 << nCS))
#define CS_on	(PORTB &=~ (1 << nCS))
#define CS_out  (DDRB  |=  (1 << nCS))
#define MISO_PIN   4
#define SCK_PIN    5
#define ifMISO   ( PINB   & (1<<MISO_PIN))

// Definir la referencia de voltaje en voltios (5V)
#define VREF 5.0
#define CMAX 10411.99

char TxBuff[20];

void uart_init(uint32_t bps)
{
	UBRR0=F_CPU/16/bps-1;
	UCSR0B|=(1<<TXEN0)|(1<<RXEN0);
}

void mi_putc(char dato)
{
	while(!(UCSR0A&(1<<UDRE0)));
	{
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

// Inicialización de SPI en modo maestro
void SPI_init(void) {
	// Configurar SCK y CS como salida, MISO como entrada
	DDRB |= (1 << SCK_PIN);
	CS_out;
	DDRB &= ~(1 << MISO_PIN);  // MISO como entrada

	// Habilitar SPI, modo maestro, reloj fosc/16
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	//SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	CS_off;  // Desactivar CS al inicio
}
//
// Función para intercambiar datos por SPI
uint8_t SPI_transfer(uint8_t data) {
	SPDR = data;  // Enviar datos (aunque en este caso enviamos ceros para leer)
	while (!(SPSR & (1 << SPIF))){  // Esperar a que la transmisión termine
	_delay_loop_1(1);
	}
	return SPDR;  // Retornar el valor recibido por MISO
}


// Leer el ADC del MCP3550
uint32_t MCP3550_read(void) {
	uint32_t result = 0;

	// Activar CS (bajar el pin)
	CS_on;
	while (ifMISO){
		_delay_loop_1(1);
	}
	// Leer 3 bytes de datos del MCP3550
	result = SPI_transfer(0x00);  // Primer byte
	result = (result << 8) | SPI_transfer(0x00);  // Segundo byte
	result = (result << 8) | SPI_transfer(0x00);  // Tercer byte

	// Desactivar CS (subir el pin)
	CS_off;

	// Ignorar los dos bits menos significativos
	result >>= 2;  // Desplazamos los dos bits menos significativos

	// Convertir a un número positivo o negativo de 22 bits
	if (result & 0x200000) {  // Si el número es negativo en formato de 22 bits
		//result |= 0xFFC00000;  // Extender el signo a 32 bits
		result |= 0xFFF00000;  // IRI
	}

	return result;
}

// Función principal
int main(void) {
	_delay_ms(1000);
	uart_init(9600);
	mi_puts("Hola\0");
	SPI_init();  // Inicializar SPI
	_delay_ms(100);

	int32_t valADC;
	while (1) {
		valADC = MCP3550_read();  // Leer el valor del ADC

		sprintf(TxBuff,"AD:%ld\t volts:%.1f\r\n", valADC,((float)valADC)*VREF/CMAX);
		mi_puts(TxBuff);
		_delay_ms(100);
	}
}