/*
 * ADC_ATmega.h
 * Funciones de configuraci�n y uso del m�dulo ADC del ATmega328
 * Created: 31/05/2017 08:47:43 p.m.
 *  Author: MyEP 
 */
//#define F_CPU 4000000
#ifndef F_CPU
	#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define energizaAD			(PRR    &=~(1<<PRADC))  // Energiza m�dulo AD
#define flagAD_OFF			(ADCSRA &=~(1<<ADIF))	// Borra flag de interrupci�n por fin de conversi�n
#define verificaFlagAD		(ADCSRA  & (1<<ADIF))	// Verifica el flag de interrupci�n 
#define iniciaConversionAD	(ADCSRA |= (1<<ADSC))	// Inicia conversi�n

// Selecciona tensi�n de referencia (que corresponde al valor m�ximo 1023)
typedef enum 
{
	Aref		= 0,
	Avcc		= 1,
	Interna1v1	= 3
} tVRef;

// Alineaci�n del valor convertido en ADC. 
// Izquierda:MSB es b15 y LSB es b6. 
// Derecha: MSB es b9 y LSB es b0 (0 a 1023)
typedef enum	
{		
	Izquierda	= 1,
	Derecha		= 0
} tAlineacion;

// Para clock del sistema de aproximaciones sucesivas. 
// El Tconv es aproximadamente F_CPU/(prex x 15 ciclos)
typedef enum	
{		
	AD_prescaler_div2	= 0,
	AD_prescaler_div4	= 2,
	AD_prescaler_div8	= 3,
	AD_prescaler_div16	= 4,
	AD_prescaler_div32	= 5,
	AD_prescaler_div64	= 6,
	AD_prescaler_div128	= 7
} tPrescaler;

typedef enum    
{
	intSI	= 1,
	intNO	= 0
} tInterrupt;

// Si atSI, la conversi�n se inicia autom�ticamente por el evento indicado en trigSrc. 
// Si atNO, debe iniciarse manualmente
typedef enum 	
{	
	atSI	= 1,
	atNO	= 0
} tAutotrig;

// Fuente de inicio de conversion, solamente vale si autotrig es atSI.
typedef enum 	
{				
	free_running	= 0,	// free running, cuando termina de convertir comienza una nueva conversi�n
	comp_Analogico	= 1,	// Un cambio en comparador analogico inicia conversion (p ejemplo, cuando la se�al supera un umbral)
	intExt_INT0		= 2,	// un evento en INT0 (ver modos de el capitulo interrupciones)
	Timer0_CompA	= 3,	// cuando Timer0 alcanza el valor del registro OCR1A
	Timer0_Overflow	= 4,	// cuando Timer0 desborda
	Timer1_CompB	= 5,	// cuando Timer1 alcanza el valor del registro OCR1B
	Timer1_Overflow	= 6,	// cuando Timer1 desborda
	Timer1_Capture	= 7		// cuando se produce una captura de evento en pin ICP1 (es decir, el muestreo se puede sincronizar con pulso externo
} tTriggerSel;

typedef struct
{ 
	uint8_t		canal; 
	tVRef		tensionRef;
	tAlineacion	alineacion;
	tPrescaler	prescaler;
	tInterrupt  interrupcion;
	tAutotrig	autoTrigger;
	tTriggerSel	tipoTrigger;
	uint8_t		disDIs;			// m�scara para deshabilitar los buffers digitales de las entradas AN0 a AN5 que se utilicen como anal�gicas						
} tADConfig;

extern tADConfig myADConfig;
			
void configuraAD(void);	
void iniciarAD(tADConfig * ADConfig);
uint16_t leerAD_canal(uint8_t canal);
uint16_t leerAD(void);

