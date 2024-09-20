//Timer 1 (16 bits) de Atmega328p
#ifndef F_CPU
	#define F_CPU 16000000
#endif

uint16_t prescaler_TC1 = 0; 


//volatile uint16_t cont_Tiempo_A = 0;
//volatile uint16_t cont_Tiempo_B = 0;

void configura_ModoSalidas_TC1(uint8_t outA, uint8_t outB)
{
	TCCR1A &=~ (3<<COM1A0); 
	TCCR1A &=~ (3<<COM1B0); 
	switch(outA)
	{
		case 0:
				#ifdef UART_DEBUG_
					printf("OC1A modo Off\r\n");
				#endif // UART_DEBUG_
				TCCR1A &=~ (3<<COM1A0);
				break;
		case 1:
				#ifdef UART_DEBUG_
					printf("OC1A modo Toggle\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (1<<COM1A0);
				break;
		case 2:
				#ifdef UART_DEBUG_
					printf("OC1A modo Clear\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (1<<COM1A1); 
				break;
		case 3:
				#ifdef UART_DEBUG_
					printf("OC1A modo Set\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (3<<COM1A0);
				break;
		default:
				#ifdef UART_DEBUG_
				printf("Salida OC1A invalida\r\n");
				#endif // UART_DEBUG_
				break;
	}
	switch(outB)
	{
		case 0:
				#ifdef UART_DEBUG_
					printf("OC1B modo Off\r\n");
				#endif // UART_DEBUG_
				TCCR1A &=~ (3<<COM1B0);
				break;
		case 1:
				#ifdef UART_DEBUG_
					printf("OC1B modo Toogle\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (1<<COM1B0);
				break;
		case 2:
				#ifdef UART_DEBUG_
					printf("OC1B modo Clear\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (1<<COM1B1);
				break;
		case 3:
				#ifdef UART_DEBUG_
					printf("OC1B modo Set\r\n");
				#endif // UART_DEBUG_
				TCCR1A |= (3<<COM1B0);
				break;
		default: 
				#ifdef UART_DEBUG_
					printf("Salida OC1B invalida\r\n"); 
				#endif // UART_DEBUG_
				break;
	}
}

void configura_Prescaler_TC1(float T) //[ms]
{
	T = T/1000.0;						//pasa a [seg]
	float aux = (float)(pow(2,16)/F_CPU);		//máximo valor con prescaler=1
	if     (T<=     aux)	prescaler_TC1 = 1;
	else if(T<=   8*aux)    prescaler_TC1 = 8;
	else if(T<=  64*aux)	prescaler_TC1 = 64;
	else if(T<= 256*aux)    prescaler_TC1 = 256;
	else if(T<=1024*aux)    prescaler_TC1 = 1024;
	else {
		#ifdef UART_DEBUG_
			printf("Tiempo fuera de rango\r\n");
		#endif // UART_DEBUG_
	}
	#ifdef UART_DEBUG_
		printf("Prescaler Timer 1: %d\r\n", prescaler_TC1);
	#endif // UART_DEBUG_
	
	TCCR1B &=~ (7<<CS10); //Resetea a 0 bits de configuración de prescaler 
	/*switch(prescaler_TC1)
	{
		case 0:
				#ifdef UART_DEBUG_
					printf("Timer1 apagado\r\n");
				#endif // UART_DEBUG_
				TCCR1B &=~(7<<CS10);
				break;
		case 1:
				TCCR1B |= (1<<CS10);
				break;
		case 8:
				TCCR1B |= (2<<CS10);
				break;
		case 64:
				TCCR1B |= (3<<CS10);
				break;
		case 256:
				TCCR1B |= (4<<CS10);
				break;
		case 1024:
				TCCR1B |= (5<<CS10);
				break;
		default:
				#ifdef UART_DEBUG_
					printf("Prescaler invalido\r\n");
				#endif // UART_DEBUG_
				break;
	}*/
}

void configura_Modo_TC1(uint8_t modo)
{
	//printf("Modo de uso %d en timer 1\r\n",modo);
	TCCR1A &=~ (3<<WGM10); 
	TCCR1B &=~ (3<<WGM12); //Resetea a 0 bits de configuración de modo
	switch(modo)
	{
		case 0:  
					TCCR1A &=~ (3<<WGM10);
					TCCR1B &=~ (3<<WGM12);
					break;
		case 1:	 
					TCCR1A |= (1<<WGM10);
					break;
		case 2:  
					TCCR1A |= (1<<WGM11);
					break;
		case 3:  
					TCCR1A |= (3<<WGM10);
					break;
		case 4:  
					TCCR1B |= (1<<WGM12);
					break;
		case 5:  
					TCCR1A |= (1<<WGM10);
					TCCR1B |= (1<<WGM12);
					break;
		case 6:  
					TCCR1A |= (1<<WGM11);
					TCCR1B |= (1<<WGM12);
					break;
		case 7:  
					TCCR1A |= (3<<WGM10);
					TCCR1B |= (1<<WGM12);
					break;
		case 8:  
					TCCR1B |= (1<<WGM13);
					break;
		case 9:  
					TCCR1A |= (1<<WGM10);
					TCCR1B |= (1<<WGM13);
					break;
		case 10: 
					TCCR1A |= (1<<WGM11);
					TCCR1B |= (1<<WGM13);
					break;
		case 11: 
					TCCR1A |= (3<<WGM10);
					TCCR1B |= (1<<WGM13);
					break;
		case 12: 
					TCCR1B |= (3<<WGM12);
					break;
		case 13: 
					TCCR1A |= (1<<WGM10);
					TCCR1B |= (3<<WGM12);
					break;
		case 14: 
					TCCR1A |= (1<<WGM11);
					TCCR1B |= (3<<WGM12);
					break;
		case 15:
					TCCR1A |= (3<<WGM10);
					TCCR1B |= (3<<WGM12);
					break;
		default:
					#ifdef UART_DEBUG_
					printf("Modo invalido\r\n");
					#endif // UART_DEBUG_
					break;
	}
}

void interrupciones_TC1(uint8_t InputCapt, uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow)
{
	if(InputCapt)
	{
		TIFR1  |= (1<<ICF1); //apaga flag
		TIMSK1 |= (1<<ICIE1); //habilita interrupción
	}
	if(OutputCaptA)
	{
		TIFR1  |= (1<<OCF1A); //apaga flag
		TIMSK1 |= (1<<OCIE1A); //habilita interrupción
	}
	if(OutputCaptB)
	{
		TIFR1  |= (1<<OCF1B); //apaga flag
		TIMSK1 |= (1<<OCIE1B); //habilita interrupción
	}
	if(Overflow)
	{
		TIFR1  |= (1<<TOV1); //apaga flag
		TIMSK1 |= (1<<TOIE1); //habilita interrupción
	}
}

