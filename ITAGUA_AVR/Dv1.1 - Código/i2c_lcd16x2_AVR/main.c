/*
			DIOXITAGUA - PROYECTO TEC LAB.
				    Diciembre de 2022
				Bomba A: S3
				Bomba B: S4
*/

#include "lcdpcf8574.h"
#include "328p_interrupcionExterna_2020.h"
#include "mi_328p_TIMER1.h"
#include "mi_328p_TIMER_0_2.h"
#include "app_utils.h"
#include "uart.h"

//#define PASO_X_CERO en app_utils.h !!!

int  main(void)
{	
	/* Deshabilitación de Watchdog dentro de 15ms */
	_delay_ms(5);
	wdt_disable(); 
		
	/* Inicialización de Watchdog */
	wdt_init();				//sin sei()!!		  
	_delay_ms(500);
		
	/* Inicialización de PINES */
	iniciaPines();			wdt_reset();
		
	/* Inicialización de VARIABLES */
	iniciaVariables();		wdt_reset();
	
	/* Inicialización de DISPLAY */
	display_init( 0 );	wdt_reset();
	_delay_ms(200);
	
	/* Configuración de RTC */
	//configuraRTC(); //TIENE QUE ESTAR COMENTADO SI YA ESTÁ CONFIGURADA LA FECHA Y HORA
	
	//sólo para configurar FECHA Y HORA por única vez:
	//1) Descomentar configuraRTC()
	//2) Llenar arreglo rtc_data, ver comentarios
	//3) Compilar (build, re-build solution)
	//4) AVRDUDESS: Pulsar GRABAR en el minuto MIN con 0 segundos exactos y en la fecha adecuada
	//5) Esperar a que el display muestre la fecha correcta
	//6) Sin desconectar USB, comentar configuraRTC(), volver a compilar y grabar.
		
	/* Configuración de ENTRADAS en Dv1.1 */		
	configuraInterrupcionExterna();		wdt_reset();		//PD3/INT1		-->	Paso por Cero
	configuraInterrupcionPorCambio();	wdt_reset();		//PB0/PCINT0 	-->	Menos Bomba A
															//PD5/PCINT21	-->	Más Bomba A	
															//PD4/PCINT20	-->	Menos Bomba B
															//PD2/PCINT18	-->	Más Bomba B	   							
	/* Inicialización de CONVERSOR A/D */
	//configuraAD();			wdt_reset();
	
	/* Saludo Inicial Display */
	saludoInicial();	wdt_reset();
	_delay_ms(50);		wdt_reset();	
	
	/*#ifdef CODIGO_PRUEBA
		codigoDePrueba();
		while(1) wdt_reset();
	#endif*/
	
	// CALIBRACIÓN INICIAL, PARÁMETROS DE BOMBAS
	if (!menos_Bomba_A_Test && !menos_Bomba_B_Test)
	{
		_delay_ms(10);
		if (!menos_Bomba_A_Test && !menos_Bomba_B_Test)
		{
			seleccione_anchos();
			seleccione_desfasaje();
			seleccione_correccion_curvas();
		}
	}
	
	iniciaLCD();		wdt_reset();
	_delay_ms(200);		wdt_reset();
		
	if(estado == s_timer || estado == s_enclavado)
	{
		cambia_Dosis_A();
		cambia_Dosis_B();
	}
	else
	{
		setDosisAB();
		dosis_a_tiempo_A(); //faltaba!!
		dosis_a_tiempo_B(); //faltaba!!
	}
		
	/* Configuración de TIMERs */
	configuraTIMER1();		wdt_reset();	//Control de tiempo entre pulsos bombas A y B
	configuraTIMER0();		wdt_reset();	//Control de duración de pulsos bomba A
	configuraTIMER2();		wdt_reset();	//Control de duración de pulsos bomba B
		
	_delay_ms(200); wdt_reset();
	
	/* Inicialización de UART */ //<--- LO TUVE QUE VOLVER A BAJAR HASTA ACÁ!
	#ifdef UART_ESP
		configuraUART(myBaudRate, 1, 0);
		printf("\n");
	#endif
	
	desactivacion(); //fT=0;
	activacion();
	sei();
		
	while(1)
	{
		switch (estado)
		{						
			case s_timer:
						rutinaTimer();
						wdt_reset();
						break;
											
			case s_uart:
						desactivacion();  cli();
						interpretaComando(); wdt_reset();
						if(estado!=s_cambioModo)	
						{
							estado = estadoAnterior;
							estadoAnterior = estado;
							desactivacion();
							activacion();	  sei();
						}
						break;
						
			case s_cambioModo:
						
						buzzer_N_times(3);
						desactivacion(); cli();
						display_clear3();
						cambia_Modo();
						eeprom_write_byte(&eeprom_Modo, modo); //mem_Modo();
						modoAnterior();
						if (estado == s_timer || estado == s_apagado || estado == s_enclavado) 
						{
							iniciaLCD(); wdt_reset();
							sei();
						}
						if (estado == s_timer || estado == s_enclavado) 
						{
							activacion(); sei();
						}
						break;
						
			case s_enclavado:
						
						rutinaEnclavado();
						wdt_reset();
						break;
				
			case s_apagado:

						desactivacion();
						rutinaApagado();
						wdt_reset();
						break;
						
			case s_pulsadores:
				
						desactivacion();	
						rutinaPulsador();	wdt_reset();
						_delay_ms(500);		wdt_reset();
						activacion();		sei();
						break;
												
			case s_menu:

						desactivacion(); cli();
						rutinaMenu();	 wdt_reset();
						modoAnterior(); //Acá ya hace mem modo. Sacado de rutina Menu
						if (estado == s_timer || estado == s_apagado || estado == s_enclavado) 
						{
							iniciaLCD(); wdt_reset();
							sei();
						}
						if (estado == s_timer || estado == s_enclavado) 
						{
							activacion(); sei();
						}
						habilitaPulsadores();
						break;
						
			case s_cambioParametro:
			
						desactivacion();  cli();
						rutinaCambioParametro();
						if(estadoAnterior==s_enclavado) _delay_ms(600); wdt_reset();
						estado = estadoAnterior;
						estadoAnterior = estado;
						activacion();	  sei();
						habilitaPulsadores();
						break;
			
			case s_purga:
			
						rutinaPurga();	wdt_reset();
						display_clear();
						if (estado != s_errorPx0 && estado != s_uart) 
						{
							estado = s_apagado;
							modoAnterior();
						}	
						if (estado == s_timer || estado == s_enclavado || estado == s_apagado)
						{
							iniciaLCD(); wdt_reset();
							sei();
						}
						if (estado == s_timer || estado == s_enclavado || s_cambioModo)
						{
							activacion();	sei();
						}
						habilitaPulsadores();
						break;
												
			case s_errorPx0:
						
						desactivacion();	
						while(recuperaPx0())
						{
							buzzer_ON;
							display_gotoxy(disp_Dia,1); display_puts("   Error Px0!");
							buzzer_OFF;
							_delay_ms(500); wdt_reset();
							display_gotoxy(disp_Dia,1); display_puts("             ");
							_delay_ms(200); wdt_reset();
						}
						estado = estadoAnterior;
						estadoAnterior = estado;
						if (estado == s_timer || estado == s_enclavado || estado == s_apagado)
						{
							iniciaLCD(); wdt_reset();
							sei();
						}
						if (estado == s_timer || estado == s_enclavado)
						{
							activacion();	sei();
						}
						habilitaPulsadores();
						break;
			
			default:
				
						_delay_ms(1);
						break;
		}
		_delay_ms(1);
	}
}

/*case s_continuo:
			
						rutinaContinuo();	wdt_reset();
						if (estado != s_errorPx0) 
						{
							estado = s_apagado;
							modoAnterior();
						}
						if (estado == s_timer || estado == s_enclavado || estado == s_apagado)
						{
							iniciaLCD(); wdt_reset();
							sei();
						}
						if (estado == s_timer || estado == s_enclavado)
						{
							activacion();	sei();
						}
						modoAnterior();
						habilitaPulsadores();
						break;*/

