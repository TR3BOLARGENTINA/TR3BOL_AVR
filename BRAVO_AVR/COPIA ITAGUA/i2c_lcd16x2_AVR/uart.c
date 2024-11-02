/*
 * uart.c
 *
 * Created: 10/2/2020 12:45:45
 *  Author: Acer
 */ 

#include "uart.h"

int mi_putc(char c, FILE *stream)
{
	while(!(UCSR0A&(1<<UDRE0)))	// Espera mientras buffer de transmisión esté ocupado
	{
		_delay_us(1);
	}
	UDR0 = c;					 // UDR0 recibe el nuevo dato c a transmitir
	return 0;
}

int mi_getc(FILE *stream)
{
	while (!(UCSR0A&(1<<RXC0)))	// Espera mientras la recepción no esté completa
	{
		_delay_us(1); 
	}
	return UDR0;				 // Cuando se completa, se lee UDR0
}


void uart_send_string(uint8_t *c)
{
	for(uint8_t i=0; i<64; i++)
	{
		if(c[i]=='\n' || c[i]=='\0') 
		{
			i = 64;
			break;
		}
		else
		{
			printf("%c",c[i]); //uart_send_byte(c[i]);
		}
	}
	printf("\r\n");
}
// Declara un parámetro tipo stream de E/S para igualar los parámetros en stdio
FILE uart_io = FDEV_SETUP_STREAM(mi_putc, mi_getc, _FDEV_SETUP_RW);

void configuraUART(uint32_t BAUD, uint8_t intRx, uint8_t intTx)
{
	indcom = 0;
	cmd = 0;
	A = 0; B = 0; E = 0;
	//Parámetros de la comunicación
	UBRR0   =  F_CPU/16/BAUD-1;	// Configura baudrate
	UCSR0A &=~ (1<<U2X0);		// Velocidad simple
	//UCSR0A |=  (1<<U2X0);		// Velocidad doble
	
	UCSR0B |=  (1<<RXEN0);		// Habilita recepción
	UCSR0B |=  (1<<TXEN0);		// Habilita transmisión
	//UCSR0C |=  (1<<USBS0);		// 2 bits de STOP
	UCSR0C |=  (3<<UCSZ00);		// 8 bits de dato
	//El stream (FILE) uart_io es la E/S estandar, es decir para fputc y fgetc
	stdout = stdin = &uart_io;
	if(intRx)
	{
		UCSR0A |=  (1<<RXC0);	//Apaga flag de interrupción por Recepción Completa
		UCSR0B |=  (1<<RXCIE0);	//Habilita interrupcion RX
	}
	if(intTx)
	{
		UCSR0A |=  (1<<TXC0);	//Apaga flag de interrupción por Transmisión Completa
		UCSR0B |=  (1<<TXCIE0);	//Habilita interrupcion TX
	}
}

void buzzTest()
{
	buzzer_ON;
	_delay_ms(15);
	buzzer_OFF;
	_delay_ms(50);
}

void interpretaComando(void)
{	
	switch(comando[0])        // Analiza primer caracter del buffer.
	{
		case 'X':	//reinicio, pedido de toda la info	
					_delay_ms(700); wdt_reset(); //220
					//printf(":D%d\n", 0);			_delay_ms(700); wdt_reset();
					printf(":A%d\n", dosis_A);		_delay_ms(700); wdt_reset(); //con 500, se pierde A!!
					printf(":B%d\n", dosis_B);		_delay_ms(700); wdt_reset();
					printf(":C%d\n", modo);			_delay_ms(700); wdt_reset();
					printf(":X%d\n", 1);			_delay_ms(700); wdt_reset();
					buzzer_N_times(3);
					break;
					
		case 'A':
					if(comando[1])                // si es distinto de 0
					{
						A = atoi(&comando[1]);    // Convierte cadena decimal en entero
						if(A>=dosis_Min && A<=dosis_Max)
						{
							dosis_A = A;
							_delay_ms(500); wdt_reset();
							cambia_Dosis_A();
							//mem_Dosis_A();
							eeprom_write_byte(&eeprom_Dosis_A, dosis_A);	
							_delay_ms(500); wdt_reset();
						}
						else if(A==0 || A==1)
						{
							dosis_A = dosis_Min-1;
							bomba_A_APAGADA = 1;
							_delay_ms(100); wdt_reset();
							cambia_Dosis_A();
							//mem_Dosis_A();
							eeprom_write_byte(&eeprom_Dosis_A, dosis_A);   
							_delay_ms(500); wdt_reset();
							bomba_A_APAGADA = 0;
						}
					}
					break;
		case 'B':
					if(comando[1])                // si es distinto de 0
					{
						B = atoi(&comando[1]);    // Convierte cadena decimal en entero
						if(B>=dosis_Min && B<=dosis_Max)
						{
							dosis_B = B;
							_delay_ms(100); wdt_reset();
							cambia_Dosis_B();
							//mem_Dosis_B();
							eeprom_write_byte(&eeprom_Dosis_B,dosis_B);
							_delay_ms(500); wdt_reset();
						}
						else if(B==0 || B==1)
						{
							dosis_B = dosis_Min-1;
							bomba_B_APAGADA = 1;
							_delay_ms(100); wdt_reset();
							cambia_Dosis_B();
							//mem_Dosis_B();
							eeprom_write_byte(&eeprom_Dosis_B, dosis_B);
							_delay_ms(500); wdt_reset();
							bomba_B_APAGADA = 0;
						}
					}
					break;
					
		case 'C':
					if(comando[1])                // si es distinto de 0
					{
						if(C>=0 && C<=5)
						{
							C = atoi(&comando[1]);    // Convierte cadena decimal en entero
							modo = C;	
							//el estado es s_uart
							estado = s_cambioModo;
						}
					}
					break;
		case 'E':
					if(comando[1])                // si es distinto de 0
					{
						E = atoi(&comando[1]);    // Convierte cadena decimal en entero
						if(E==1)
						{
							buzzer_ON;
							display_gotoxy(20,0);				
							display_puts("Nivel Bajo Tanque A ");	wdt_reset();
							buzzer_OFF;
						}
						else if (E==2)
						{
							buzzer_ON;
							display_gotoxy(20,0);
							display_puts("Nivel Bajo Tanque B ");	wdt_reset();
							buzzer_OFF;
						}
					}
					break;
		default:
					printf("Consigna invalida\r\n");
					break;
	}
}

/*
	Rutina de servicio de interrupción por recepción de datos en UART
	Espera mensajes del tipo [:][comando][\r] y lo guarda en buffer comando[]
	limpiando delimitadores y agregando NULL al final del mismo para poder usar atoi 
*/

ISR(USART_RX_vect)
{            
	char dato;
	dato=fgetc();
	switch(dato)      
	{
		case ';':	// Delimitador de inicio
					indcom=0;   // Inicializa índice de buffer de recepción
					cmd = 1;	// Comando en curso
					break;
		case 8:		// Basckspace
					if(indcom>0) indcom--;
					break;
		case '\n':	// Delimitador de final
					if(cmd==1)				 // Si hay comando en curso
					{
						comando[indcom] = 0; // coloca NULL luego del último caracter recibido
						desactivacion(); cli();
						estadoAnterior = estado;
						estado = s_uart;
						//interpretaComando();
						cmd = 0;			 // fin de comando en curso
					}
					break;
		default:							// Todo lo que está entre delimitadores
					if(indcom<30) 
					{
						if(cmd==1)
						{
							comando[indcom++]=dato; // Guarda en elemento del buffer e incrementa indcom
						}
					}
						
					break;
	}
	UCSR0A |= (1<<RXC0);	//Apaga el flag de interrupción por RX
}
