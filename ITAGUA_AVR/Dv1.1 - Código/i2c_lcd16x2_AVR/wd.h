/*
 * wd.h
 *
 * The WDT is a timer counting cycles of a separate on-chip 128kHZ oscillator.
 * The WDT gives an interrupt or a system reset when the counter reaches a
 * given time-out value. In normal operation mode, it is required that the
 * system uses the Watchdog Timer Reset (WDR) instruction to restart the
 * counter before the time-out value is reached. If the system doesn't restart
 * the counter, an interrupt or system reset will be issued.
 *
 * FUSE WDTON:
 * The Watchdog always on (WDTON) fuse, if programmed, will force the Watchdog
 * Timer to System Reset mode.
 *
 * Created: 8/5/2020 10:59:19
 *  Author: Gonzalo G. Fernández
 */ 


#ifndef WD_H_
#define WD_H_

#include <avr/interrupt.h>
#include <avr/wdt.h>

#ifdef UART_DEBUG_
	#include "uart.h"
#endif // UART_DEBUG_

/*
 * In Stopped mode, there is no action when timer expires. 
 */
#define STOPPED 0
/*
 * In Interrupt mode, the WDT gives an interrupt when the timer expires. This
 * interrupt can be used to wake the device from sleep-modes, and also as
 * general system timer. One example is to limit the maximum time allowed for
 * certain operations, giving a reset when the operation has run longer than
 * expected.
 */
#define INTERRUPT_MODE 1
/*
 * In System Reset mode, the WDT gives a reset when the timer expires. This is
 * typically used to  prevent system hang-up in case of runaway code.
 */
#define SYSTEM_RESET_MODE 2
/*
 * Interrupt and System Reset mode, combines the other two modes by first
 * giving an interrupt and the switching to System Reset mode. This mode will
 * for instance allow a safe shutdown by saving critical parameters before a
 * system reset.
 */
#define INTERRUPT_SYSTEM_RESET_MODE 3

/*
 * Turn off the Watchdog Timer. 
 */
void wdt_off(void);

void wdt_init(void);

#endif /* WD_H_ */