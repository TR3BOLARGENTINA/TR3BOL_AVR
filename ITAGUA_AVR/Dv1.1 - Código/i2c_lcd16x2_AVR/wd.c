/*
 * wd.c
 *
 * Created: 8/5/2020 11:24:29
 *  Author: Gonzalo G. Fernández
 */ 

#include "wd.h"

void wdt_off(void)
{
	/* Disable interrupt */
	cli();
	wdt_reset();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
	/* Enable interrupt */
	sei();
}

void wdt_init(void)
{
	/* Disable interrupt */
	cli();
	//wdt_reset();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);
	/* Write logical one to WDCE and WDE */
	WDTCSR = (1 << WDCE) | (1 << WDE);
	/* Set watchdog timer configuration and prescaler (time-out) value */
	WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3) | (1 << WDP0);
	//WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
	//WDTCSR = (1 << WDE) | (1 << WDP3) | (1 << WDP0);
	/* Enable interrupt */
	//sei();
}

ISR(WDT_vect)
{
	cli();
	WDTCSR |= (1 << WDIF);
	#ifdef UART_DEBUG_
	printf("int:wdt\n");
	#endif // UART_DEBUG_
	sei();
}