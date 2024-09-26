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
#define CS_PIN PB3  
#define MISO_PIN PB4
#define SCK_PIN PB5

// Definir la referencia de voltaje en voltios (5V)
#define VREF 5.0

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
	DDRB = (1 << SCK_PIN) | (1 << CS_PIN);
	DDRB &= ~(1 << MISO_PIN);  // MISO como entrada

	// Habilitar SPI, modo maestro, reloj fosc/16
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	PORTB |= (1 << CS_PIN);  // Desactivar CS al inicio
}

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
	PORTB &= ~(1 << CS_PIN);

	// Leer 3 bytes de datos del MCP3550
	result = SPI_transfer(0x00);  // Primer byte
	result = (result << 8) | SPI_transfer(0x00);  // Segundo byte
	result = (result << 8) | SPI_transfer(0x00);  // Tercer byte

	// Desactivar CS (subir el pin)
	PORTB |= (1 << CS_PIN);

	// Ignorar los dos bits menos significativos
	result >>= 2;  // Desplazamos los dos bits menos significativos

	// Convertir a un número positivo o negativo de 22 bits
	if (result & 0x200000) {  // Si el número es negativo en formato de 22 bits
		result |= 0xFFC00000;  // Extender el signo a 32 bits
	}

	return result;
}

// Función principal
int main(void) {
	SPI_init();  // Inicializar SPI
	uart_init(9600);
	mi_puts("Hola\0");

	uint32_t adc_value;
	double voltage;

	while (1) {
		adc_value = MCP3550_read();  // Leer el valor del ADC

		// Convertir el valor leído a voltaje
		voltage = ((double)adc_value / 2097151.0) * VREF;

		// Formatear el voltaje a dos decimales y enviarlo por puerto serie
		sprintf(TxBuff,"AD:%ld\t volts:%.3f\r\n", adc_value,voltage);
		mi_puts(TxBuff);

		_delay_ms(100);  // Esperar 1 segundo antes de la siguiente lectura
	}
}