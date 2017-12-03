#include <htc.h>

/* Program device configuration word
 * Oscillator = HS
 * Watchdog Timer = Off
 * Power Up Timer = Off
 * Brown Out Detect = Off
 * Low Voltage Program = Disabled
 * Flash Program Write = Write Protection Off
 * Background Debug = Disabled
 * Data EE Read Protect = Off
 * Code Protect = Off
 */
__CONFIG(HS & WDTDIS & PWRTDIS & BORDIS & LVPDIS & WRTEN & DEBUGDIS & DUNPROT & UNPROTECT);

// Peripheral initialization function
void init(void){
	/***** Common Code ****
	 *  Global interrupt disabled during initialization
	 */
	INTCON	= 0b00000000;
	
}


void init_pinout(void)
{
	// init PORT A as digital output.
	PORTA = 0x00;
	TRISA = 0x00;
	ADCON1 = 0x06;	// this is needed!
	
	PORTC = 0x00;
	// .7 and .6 : UART
	// .0-.5 digital output.
	TRISC = 0xC0;
}