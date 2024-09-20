//Timer 2 de 8 bits de Atmega328P
uint16_t prescaler_TC2 = 0;


void configura_ModoSalidas_TC2(uint8_t outA, uint8_t outB)
{
	TCCR2A &=~ (3<<COM2A0);
	TCCR2A &=~ (3<<COM2B0);
	switch(outA)
	{
		case 0:
				//printf("OC2A modo Off\r\n");
				TCCR2A &=~ (3<<COM2A0);
				break;
		case 1:
				//printf("OC2A modo Toggle\r\n");
				TCCR2A |= (1<<COM2A0);
				break;
		case 2:
				//printf("OC2A modo Clear\r\n");
				TCCR2A |= (1<<COM2A1);
				break;
		case 3:
				//printf("OC2A modo Set\r\n");
				TCCR2A |= (3<<COM2A0);
				break;
		default:
				//printf("Salida OC2A invalida\r\n");
				break;
	}
	switch(outB)
	{
		case 0:
				//printf("OC2B modo Off\r\n");
				TCCR2A &=~ (3<<COM2B0);
				break;
		case 1:
				//printf("OC2B modo Toggle\r\n");
				TCCR2A |= (1<<COM2B0);
				break;
		case 2:
				//printf("OC2B modo Clear\r\n");
				TCCR2A |= (1<<COM2B1);
				break;
		case 3:
				//printf("OC2B modo Set\r\n");
				TCCR2A |= (3<<COM2B0);
				break;
		default:
				//printf("Salida OC2B invalida\r\n");
				break;
	}
}

void configuraPrescaler_TC2(float T) //[ms]
{
	T = T/1000.0; //pasa a [seg]
	float aux = (float)(256.0/F_CPU);

	if     ( T<=  aux		)	 prescaler_TC2 = 1;
	else if( T<= (aux*8)    )	 prescaler_TC2 = 8;
	else if( T<= (aux*32)   )	 prescaler_TC2 = 32;
	else if( T<= (aux*64)   )	 prescaler_TC2 = 64;
	else if( T<= (aux*128)  )    prescaler_TC2 = 128;
	else if( T<= (aux*256)  )    prescaler_TC2 = 256;
	else if( T<= (aux*1024) )    prescaler_TC2 = 1024;
	else 
	{
		prescaler_TC2 = 0;
		#ifdef UART_DEBUG_
			printf("Tiempo TC2 fuera de rango\r\n");
		#endif
	}
	#ifdef UART_DEBUG_
		printf("Prescaler TC2: %d\r\n",prescaler_TC2);
	#endif
	//estos bits de este registro son para selección del clock, no prescaler
	TCCR2B &=~ (7<<CS20); //Resetea a 0 bits de configuracion de prescaler (en 0 el timer está apagado)
	/*switch(prescaler_TC2)
	{
		case 0:
			TCCR2B &=~(7<<CS20);
			break;
		case 1:
			TCCR2B |= (1<<CS20);
			break;
		case 8:
			TCCR2B |= (1<<CS21);
			break;
		case 32:
			TCCR2B |= (1<<CS20);
			TCCR2B |= (1<<CS21);
			break;
		case 64:
			TCCR2B |= (1<<CS22);
			break;
		case 128:
			TCCR2B |= (1<<CS22);
			TCCR2B |= (1<<CS20);
			break;
		case 256:
			TCCR2B |= (3<<CS21);
			break;
		case 1024:
			TCCR2B |= (7<<CS20);
			break;
		default:
			printf("Prescaler TC2 invalido\r\n");
		break;
	}*/
}

void configura_Modo_TC2(uint8_t modo)
{
	//printf("Modo de uso %d en TC2\r\n",modo);
	TCCR2A &=~ (3<<WGM20);
	TCCR2B &=~ (1<<WGM22); //Resetea a 0 bits de configuración de modo
	switch(modo)
	{
		case 0:
				TCCR2A &=~ (3<<WGM20);
				TCCR2B &=~ (1<<WGM22);
				break;
		case 1:
				TCCR2A |= (1<<WGM20);
				break;
		case 2:
				TCCR2A |= (1<<WGM21);
				break;
		case 3:
				TCCR2A |= (3<<WGM20);
				break;
		case 4:
				TCCR2B |= (1<<WGM22);
				break;
		case 5:
				TCCR2A |= (1<<WGM20);
				TCCR2B |= (1<<WGM22);
				break;
		case 6:
				TCCR2A |= (1<<WGM21);
				TCCR2B |= (1<<WGM22);
				break;
		case 7:
				TCCR2A |= (3<<WGM20);
				TCCR2B |= (1<<WGM22);
				break;
		default:
				printf("Modo TC2 invalido\r\n");
				break;
	}
}

void interrupciones_TC2(uint8_t OutputCaptA, uint8_t OutputCaptB, uint8_t Overflow)
{
	if(OutputCaptA)
	{
		TIFR2  |=  (1<<OCF2A); //apaga flag
		TIMSK2 |= (1<<OCIE2A); //habilita interrupción
	}
	if(OutputCaptB)
	{
		TIFR2  |=  (1<<OCF2B); //apaga flag
		TIMSK2 |= (1<<OCIE2B); //habilita interrupción
	}
	if(Overflow)
	{
		TIFR2  |=  (1<<TOV2); //apaga flag
		TIMSK2 |= (1<<TOIE2); //habilita interrupción
	}
}