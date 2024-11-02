/*
 * uart.h
 *
 * Created: 10/2/2020 12:46:00
 *  Author: Acer
 */ 


#ifndef UART_H_
#define UART_H_

#ifndef F_CPU
	#define F_CPU 16000000
#endif

#include "app_utils.h"

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>

#define  myBaudRate 9600

unsigned int indcom;	// indice para llenar el buffer de recepción
unsigned int cmd;		// flag "comando en curso"
char comando[30];			// buffer de recepcion
int A, B, C, E;

int mi_putc(char c, FILE *stream);
int mi_getc(FILE *stream);
void uart_send_string(uint8_t *c);

// Redefinimos las primitivas de E/S para recibir/transmitir caracteres por UART
#define fgetc()  mi_getc(&uart_io)
#define fputc(x) mi_putc(x,&uart_io)

void configuraUART(uint32_t BAUD, uint8_t intRx, uint8_t intTx);
void interpretaComando(void);

#endif /* UART_H_ */