/*
 * app_utils.c
 *
 * Created: 13/4/2020 16:26:27
 *  Author: Acer
 */ 

#include "app_utils.h"

//uint8_t EEMEM eeprom_HorasACT[cantidad_Horas] = {1,2,3,4,5,6,7,8,9,10,11,12};
//uint8_t EEMEM eeprom_anchoA;

void iniciaPines(void)
{
	/*-------------	Placa ATv1.1 ------------
		PB0/PCINT0 	<- 	entrada P1
		PB1			->	SPI
		PB2			->	salida S1 (no usada)
		PB3 		->	MOSI (no usado)
		PB4			->	SPI
		PB5			->	SPI
		PB6-7		->	Cristal externo 16MHz
		PC0-1		->	Entradas analógicas (no usadas)
		PC2			->	entrada P5 (no usada)
		PC3			->	Habilita Display (HIGH, no usado)
		PC4-5		->	SDA/SCL, i2c lcd (pull-Up externo)
		PC6			->	Reset
		PD0			->	Uart Rx (no usado)
		PD1 		->	Uart Tx
		PD2/PCINT18 ->	entrada P4		(INT0)
		PD3/INT1 	-> 	entrada B (no usado)
		PD4/PCINT20 -> 	entrada P3
		PD5/PCINT21 -> 	entrada P2	
		PD6 		-> 	buzzer (HIGH)
		PD7 		-> 	salida A (no usada)
	*/
	buzzer_OUT;		buzzer_OFF;
	
	//Pines no usados --> salidas apagadas
	DDRB |=  (1<<DDB2);   PORTB &=~ (1<<PORTB2);   // PB2
	DDRC |=  (15<<DDC0);  PORTC &=~ (15<<PORTC0);  // PC0-1-2-3
	DDRD |=  (1<<DDD0);  PORTD &=~ (1<<PORTD0);    // PD0
	DDRD |=  (1<<DDD3);  PORTD &=~ (1<<PORTD3);    // PD3
	DDRD |=  (1<<DDD7);  PORTD &=~ (1<<PORTD7);    // PD7
	
	buzzer_N_times(3);
}

void habilitaPulsadores ( void )
{
	PCIFR |= (1<<PCIF2);  //flagPCINT2_OFF;
	PCICR |= (1<<PCIE2);  //PCINT2_ON;
	PCIFR |= (1<<PCIF0);  //flagPCINT0_OFF;
	PCICR |= (1<<PCIE0);  //PCINT0_ON;
}

void iniciaVariables(void)
{
	/*tipoPCF8574 = eeprom_read_byte(&eeprom_PCF);
	if(tipoPCF8574  > 3)
	{
		tipoPCF8574  = 0;
		eeprom_write_byte(&eeprom_PCF,tipoPCF8574);
	}*/	
	centenas = 0;
	decenas  = 0;
	unidades = 0;
		
	contTimer0 = 0;
	contTimer1 = 0;
	contTimer2 = 0;
	
	flagTimer0 = 0;
	flagTimer1 = 0;
	flagTimer2 = 0;
}

void buzzer_N_times (uint8_t N_times)
{
	for(uint8_t i=0; i<2*N_times; i++)
	{
		buzzer_Toggle;
		_delay_ms(BUZZER_DELAY); wdt_reset();
	}
}

/*void mem_Modo ( void )
{
	eeprom_write_byte(&eeprom_Modo, modo);
	#ifdef UART_ESP
		printf(":C%d\n", modo);
	#endif
	_delay_ms(500); wdt_reset();
}*/

/*void setDosisAB(void)
{
	uint16_t dosis_A_Print = dosis_A ;
	uint16_t dosis_B_Print = dosis_B ;
	
	centenas = dosis_A_Print/100;
	decenas  = ( dosis_A_Print -100*centenas )/10;
	unidades = ( dosis_A_Print -100*centenas -10*decenas );
	if(centenas==0 ) cadena_Dosis_A[0]=' ';
	else cadena_Dosis_A[0]=centenas+48;
	if(decenas==0 && centenas==0 ) cadena_Dosis_A[1]=' ';
	else cadena_Dosis_A[1]=decenas+48;
	cadena_Dosis_A[2]=unidades+48;
	cadena_Dosis_A[3]='%';
	
	centenas = dosis_B_Print/100;
	decenas  = ( dosis_B_Print -100*centenas )/10;
	unidades = ( dosis_B_Print -100*centenas -10*decenas );
	if(centenas==0 ) cadena_Dosis_B[0]=' ';
	else cadena_Dosis_B[0]=centenas+48;
	if(decenas==0 && centenas==0 ) cadena_Dosis_B[1]=' ';
	else cadena_Dosis_B[1]=decenas+48;
	cadena_Dosis_B[2]=unidades+48;
	cadena_Dosis_B[3]='%';
}*/

void miDelay_ms_reposo(uint16_t kMax)
{
	for(uint16_t k=0; k<kMax; k++)
	{
		if(estado==s_reposo)
		{
			_delay_ms(1);	wdt_reset();
		}
		else
		{
			k=kMax;
			break;
		}
	}
}

//----------------------------------------------//
//--------------- Máquina de Estados ---------- //
//----------------------------------------------//
void rutinaPulsador (void)
{
	_delay_ms(10); //Más tiempo para ambos pulsadores en modo Configuración
	if(!P1_Test)
	{
		_delay_ms(2);
		if(!P1_Test)
		{
			printf("P1!");
			while(!P1_Test)
			{
				estado = s_enviaVolt;
				_delay_ms(10); wdt_reset();
			}
			printf("\n");
		}
	}
	else if(!P2_Test)
	{
		_delay_ms(2);
		if(!P2_Test)
		{
			printf("P2!");
			while(!P2_Test)
			{
				_delay_ms(10); wdt_reset();
			}
		}
	}
	else if(!P3_Test)
	{
		_delay_ms(2);
		if(!P3_Test)
		{
			printf("P3!");
			while(!P3_Test)
			{
				_delay_ms(10);	wdt_reset();
			}
		}
	}
	else if(!P4_Test)
	{
		_delay_ms(2);
		if(!P4_Test)
		{
			printf("P4!");
			while(!P4_Test)
			{
				_delay_ms(10);  wdt_reset();
			}
		}
	}
	_delay_ms(50);
}


