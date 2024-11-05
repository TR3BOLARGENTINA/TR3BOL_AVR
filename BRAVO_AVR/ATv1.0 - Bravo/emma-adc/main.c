/*
 * emma-adc.c
 *
 * Created: 29/8/2024 22:03:38
 * Author : TR3BOL-ADC
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#define F_CPU 16000000
#include <util/delay.h>

#include "328p_interrupcionPorCambio.h"
#include "app_utils.h"

#define VREF 5.0
//#define CMAX 10411.99	//cuenta para 500 volts dividido 100. 


// Esto es con el circuito de proteus, que modela la impedancia del MCP3553, 
// típica 2.4 Mohm (depende de la frecuencia de funcionamiento). Para otro ADC, 
// o intercalando seguidores de tensión en Vin+ y Vin-, puede variar.



uint32_t leeAD_MCP3550(void)
{
	uint32_t adcval=0;
	CS_OFF;
	
	while (ifMISO){
		_delay_loop_1(1);
	}
 	//_delay_ms(100);

	for (uint8_t ck=1;ck<=24;ck++) // se puede hacer también por la interfaz SPI
	{
		SCK_OFF;
		//_delay_us(2);		
		SCK_ON;
		adcval=(adcval<<1);
		if (ifMISO) adcval|=1;
	}
	CS_ON;
	if(adcval&(0x200000)) // Si el número es negativo en formato de 22 bits
	{
		adcval |= 0xFFF00000;  // IRI
		//adcval |= 0xFFC00000;  // Chatgpt: Extender el signo a 32 bits
	}
	return adcval;
}
//cuenta para 500 volts dividido 100.
#define CMAX_n05	11300
#define CMAX_n1		10750
#define CMAX_n2		10538
#define CMAX_n5		10705
#define CMAX_n8		10638
#define CMAX_n13	10696
#define CMAX_n16	11659
#define CMAX_n20	10676
#define CMAX_n50	10671
#define CMAX_n100	10676
#define CMAX_n200	10713
#define CMAX_n300	10760
#define CMAX_n400	10835
#define CMAX_n500	10850


#define CMAX_p05	13200
#define CMAX_p1		11850
#define CMAX_p2		11100
#define CMAX_p5		10830
#define CMAX_p8		10731
#define CMAX_p13	10712
#define CMAX_p16	10734
#define CMAX_p20	10733
#define CMAX_p50	10675
#define CMAX_p100	10669
#define CMAX_p200	10711
#define CMAX_p300	10759
#define CMAX_p400	10839
#define CMAX_p500	10850

int32_t valADC = 0;
float Vread = 0.0;
uint16_t CMAX = 1;
void ADC_a_Volt()
{
	if(valADC>0) 
	{
		if     (valADC>CMAX_p400) CMAX = CMAX_p500;
		else if(valADC>CMAX_p300) CMAX = CMAX_p400;
		else if(valADC>CMAX_p200) CMAX = CMAX_p300;
		else if(valADC>CMAX_p100) CMAX = CMAX_p200;
		else if(valADC>CMAX_p50)  CMAX = CMAX_p100;
		else if(valADC>CMAX_p20)  CMAX = CMAX_p50;
		else if(valADC>CMAX_p16)  CMAX = CMAX_p20;
		else if(valADC>CMAX_p13)  CMAX = CMAX_p16;
		else if(valADC>CMAX_p8)   CMAX = CMAX_p13;
		else if(valADC>CMAX_p5)   CMAX = CMAX_p8;
		else if(valADC>CMAX_p2)   CMAX = CMAX_p5;
		else if(valADC>CMAX_p1)   CMAX = CMAX_p2;
		else if(valADC>CMAX_p05)  CMAX = CMAX_p1;
		else CMAX = CMAX_p05;
	}
	else         
	{
		if     (valADC>CMAX_n400) CMAX = CMAX_n500;
		else if(valADC>CMAX_n300) CMAX = CMAX_n400;
		else if(valADC>CMAX_n200) CMAX = CMAX_n300;
		else if(valADC>CMAX_n100) CMAX = CMAX_n200;
		else if(valADC>CMAX_n50)  CMAX = CMAX_n100;
		else if(valADC>CMAX_n20)  CMAX = CMAX_n50;
		else if(valADC>CMAX_n16)  CMAX = CMAX_n20;
		else if(valADC>CMAX_n13)  CMAX = CMAX_n16;
		else if(valADC>CMAX_n8)   CMAX = CMAX_n13;
		else if(valADC>CMAX_n5)   CMAX = CMAX_n8;
		else if(valADC>CMAX_n2)   CMAX = CMAX_n5;
		else if(valADC>CMAX_n1)   CMAX = CMAX_n2;
		else if(valADC>CMAX_n05)  CMAX = CMAX_n1;
		else CMAX = CMAX_n05;
	}
	Vread = ((float)valADC)*VREF/CMAX; //escala negativa
}

void calibracion ()
{
	while (1)
	{
		wdt_reset();
		valADC = leeAD_MCP3550();
		if(valADC!=-1)
		{
			//valADC = valADC - 5420; //ajuste a 0 (error amplitud 30,  +-15)
			sprintf(TxBuff,"AD: %ld\r\n", valADC);
			mi_puts(TxBuff);
		}
		_delay_ms(100);
	}
}

//uint16_t numRegistro = 0;
uint8_t numArchivo   = 0;
uint8_t numLinea     = 0;
uint8_t tryVolt = 0;
void lecturaVoltimetro()
{
	valADC = leeAD_MCP3550();
	tryVolt = 0;
	while(valADC==-1 && tryVolt<10)
	{
		wdt_reset();
		_delay_ms(100); //con 10 o 50 ms, fallaba
		valADC = leeAD_MCP3550();
		tryVolt++;
		sprintf(TxBuff," -"); //descubrí al agregar print por serial
		//mi_puts(TxBuff);
	}
	if(tryVolt>=10)
	{
		sprintf(TxBuff,"time out");
		mi_puts(TxBuff);
	}
	if(valADC!=-1)
	{
		// CADENA " : numArchivo , numLinea "
		//------ Registro -------//
		numArchivo=2;
		numLinea=6;
		//sprintf(TxBuff,":%d,%d,%d,", numRegistro++,numArchivo,numLinea);
		sprintf(TxBuff,":%d,%d,", numArchivo,numLinea);
		mi_puts(TxBuff);
		
		//------ RTC -------//
		// CADENA " : numArchivo , numLinea , AAMMDD, HH:MM " 
		leeRTC();

		//------ Voltimetro -------//
		// CADENA " : numArchivo , numLinea , AAMMDD, HH:MM , V " 
		ADC_a_Volt();
		if(Vread>=100)			sprintf(TxBuff,",%.1f\r\n", Vread);
		else if(Vread>=10 )		sprintf(TxBuff,",%.2f\r\n", Vread);
		else if(Vread>=0.5)		sprintf(TxBuff,",%.3f\r\n", Vread);
		if(Vread<=-100)			sprintf(TxBuff,",%.1f\r\n", Vread);
		else if(Vread<=-10 )	sprintf(TxBuff,",%.2f\r\n", Vread);
		else if(Vread<=-0.5)	sprintf(TxBuff,",%.3f\r\n", Vread);
		mi_puts(TxBuff);
	}
}

int main(void)
{
	/* Deshabilitación de Watchdog dentro de 15ms */
	_delay_ms(5);
	wdt_disable();
	
	/* Inicialización de Watchdog */
	wdt_init();				//sin sei()!!
	_delay_ms(500);
	
	/* Inicialización de PINES */
	iniciaPines();			wdt_reset();
	CS_OUT;
	DDRB |= (1<<pinSCK);
	
	
	/* Inicialización de VARIABLES */
	iniciaVariables();		wdt_reset();
	
	/* Configura PULSADORES */
	configuraInterrupcionPorCambio();	wdt_reset();	//PB0/PCINT0 	-->	P1
														//PD5/PCINT21	-->	P2
														//PD4/PCINT20	-->	P3
														//PD2/PCINT18	-->	P4
    
	
	
	/* Inicialización de UART */ 
	configuraUART(myBaudRate, 1, 0);
	printf("UART OK\n");
	//printf("\n");
	
	/* Inicialización de RTC */ 
	TWSR &=~ (3<<TWPS0);
	TWBR = 72;
	//configuraRTC(); //TIENE QUE ESTAR COMENTADO SI YA ESTÁ CONFIGURADA LA FECHA Y HORA

	_delay_ms(1000);
	
	
	//CALIBRACIÓN VOLTÍMTREO
	//calibracion();
	
	estado = s_reposo;
	sei();
		
    while (1) 
    {
		switch(estado)
		{
			case s_reposo:
							wdt_reset();
							//lecturaVoltimetro();
							valADC = leeAD_MCP3550(); //mantiene vivo al ADC
							miDelay_ms_reposo(500);
							break;
		    case s_pulsadores:
							wdt_reset();
							rutinaPulsador();
							if(estado == s_enviaVolt)
							{
								buzzer_N_times(2);
								lecturaVoltimetro();
							}
							else if (estado == s_Param)
							{
								buzzer_N_times(1);
								sprintf(TxBuff,":P%d\n", pulsadorNum);
								mi_puts(TxBuff);
							}
							estado = s_reposo;
							habilitaPulsadores();
							sei();
							break;
			default:
							wdt_reset();
							break;
		}
		wdt_reset();
    }
}

