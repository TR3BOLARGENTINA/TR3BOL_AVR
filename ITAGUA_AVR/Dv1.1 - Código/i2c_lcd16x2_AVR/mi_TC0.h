//Timer 0 de 8 bits de Atmega328P
uint16_t prescaler_TC0 = 0;

void configura_ModoSalidas_TC0(uint8_t outA, uint8_t outB)
{
	TCCR0A &=~ (3<<COM0A0); 
	TCCR0A &=~ (3<<COM0B0); 
	switch(outA)
	{
		case 0:
				//printf("OC0A modo Off\r\n");
				TCCR0A &=~ (3<<COM0A0);
				break;
		case 1:
				//printf("OC0A modo Toggle\r\n");
				TCCR0A |= (1<<COM0A0);
				break;
		case 2:
				//printf("OC0A modo Clear\r\n");
				TCCR0A |= (1<<COM0A1); 
				break;
		case 3:
				//printf("OC0A modo Set\r\n");
				TCCR0A |= (3<<COM0A0);
				break;
		default: 
				//printf("Salida OC0A invalida\r\n"); 
				break;
	}
	switch(outB)
	{
		case 0:
				//printf("OC0B modo Off\r\n");
				TCCR0A &=~ (3<<COM0B0);
				break;
		case 1: 
				//printf("OC0B modo Toggle\r\n");
				TCCR0A |= (1<<COM0B0);
				break;
		case 2:
				//printf("OC0B modo Clear\r\n");
				TCCR0A |= (1<<COM0B1);
				break;
		case 3:
				//printf("OC0B modo Set\r\n");
				TCCR0A |= (3<<COM0B0);
				break;
		default: 
				printf("Salida OC0B invalida\r\n"); 
				break;
	}
}

void configuraPrescaler_TC0(float T) //[ms]
{
	T = T/1000.0; //pasa a [seg]
	float aux = (float)(256.0/F_CPU);

	if     ( T<=  aux		)	 prescaler_TC0 = 1;
	else if( T<= (aux*8)    )	 prescaler_TC0 = 8;
	else if( T<= (aux*64)   )	 prescaler_TC0 = 64;
	else if( T<= (aux*256)  )    prescaler_TC0 = 256;
	else if( T<= (aux*1024) )    prescaler_TC0 = 1024;
	else 
	{
		prescaler_TC0 = 0;
		#ifdef UART_DEBUG_
			printf("Tiempo TC0 fuera de rango\r\n");
		#endif
	}
	#ifdef UART_DEBUG_
		printf("Prescaler TC0: %d\r\n",prescaler_TC0);
	#endif
	//estos bits de este registro son para selección del clock, no prescaler
	TCCR0B &=~ (7<<CS00); //Resetea a 0 bits de configuracion de prescaler (en 0 el timer está apagado)
	/*switch(prescaler_TC0)
	{
		case 0:
				TCCR0B &=~(7<<CS00);
				break;
		case 1:
				TCCR0B |= (1<<CS00);
				break;
		case 8:
				TCCR0B |= (2<<CS00);
				break;
		case 64:
				TCCR0B |= (3<<CS00);
				break;
		case 256:
				TCCR0B |= (4<<CS00);
				break;
		case 1024:
				TCCR0B |= (5<<CS00);
				break;
		default:
				printf("Prescaler TC0 invalido\r\n");
				break;
	}*/
}

void configura_Modo_TC0(uint8_t modo)
{
	//printf("Modo de uso %d en TC0\r\n",modo);
	TCCR0A &=~ (3<<WGM00); 
	TCCR0B &=~ (1<<WGM02); //Resetea a 0 bits de configuración de modo
	switch(modo)
	{
		case 0:  
				TCCR0A &=~ (3<<WGM00);
				TCCR0B &=~ (1<<WGM02);
				break;
		case 1:	 
				TCCR0A |= (1<<WGM00);
				break;
		case 2:  
				TCCR0A |= (1<<WGM01);
				break;
		case 3:  
				TCCR0A |= (3<<WGM00);
				break;
		case 4:  
				TCCR0B |= (1<<WGM02);
				break;
		case 5:  
				TCCR0A |= (1<<WGM00);
				TCCR0B |= (1<<WGM02);
				break;
		case 6:  
				TCCR0A |= (1<<WGM01);
				TCCR0B |= (1<<WGM02);
				break;
		case 7:  
				TCCR0A |= (3<<WGM00);
				TCCR0B |= (1<<WGM02);
				break;
		default: 
				printf("Modo TC0 invalido\r\n");
				break;
	}
}

void interrupciones_TC0(uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow)
{
	if(OutputCaptA)
	{
		TIFR0  |=  (1<<OCF0A); //apaga flag
		TIMSK0 |= (1<<OCIE0A); //habilita interrupción
	}
	if(OutputCaptB)
	{
		TIFR0  |=  (1<<OCF0B); //apaga flag
		TIMSK0 |= (1<<OCIE0B); //habilita interrupción
	}
	if(Overflow)
	{
		TIFR0  |=  (1<<TOV0); //apaga flag
		TIMSK0 |= (1<<TOIE0); //habilita interrupción
	}
}