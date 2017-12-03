#include <htc.h>
/*
void init_Timer1 (void)
{
	
	TMR1ON = 0;			//Stop timer before configuring it.
	
	T1CON = 0;			// Clear all the control register.
	
	// TMR1CS  = 0 : Clock source is Fosc/4
	// T1SYNC  = 0 : Dont care in Timer mode.
	// T1OSCEN = 0 : Dont use external oscillator.
	// T1CKPS0 = 0 :
	// T1CKPS1 = 0 :  Prescaler value 
	T1CKPS0 = 0;
	T1CKPS1 = 0;
	
	TMR1IF = 0;		// Clear pending interrupt.
	
	PEIE = 1;		// Enable all peripherals interrrupts (needed!).
	TMR1IE = 1; 	// Enable interrupt on Timer 1.
	
	TMR1ON = 1;			//Start timer 1.
}
*/

/*
 *	External clock source (RC0-1) + compare mode + software interrupt
 *
 */

void init_Timer1 (void)
{
	
	TMR1ON = 0;			//Stop timer before configuring it.
	
	T1CON = 0;			// Clear all the control register.
	
	// TMR1CS  = 1 : Clock source is External XTAL
	TMR1CS = 1;
	
	// /T1SYNC  = 0 : synch Timer 1 clock with CPU clock.
	// This is needed for capture/compare operation.
	T1SYNC  = 0;
	
	// T1OSCEN = 0 : use external oscillator.
	// In this mode TRISC0-1 is ignored and pins RC0-1 become peripheral.
	T1OSCEN = 1;

	// T1CKPS0 = 0 :
	// T1CKPS1 = 0 :  Prescaler value 
	T1CKPS0 = 0;
	T1CKPS1 = 0;
	
	CCP2CON = 0x0B;	//compare mode, pwm off, for CCP2 resets Timer1.
	CCPR2H = 0x80;	//compare value high
	CCPR2L = 0x00;	//compare value low
	
	CCP2IE = 1;
	
	TMR1IF = 0;		// Clear pending interrupt.
	
	PEIE = 1;		// Enable all peripherals interrrupts (needed!).
	TMR1IE = 1; 	// Enable interrupt on Timer 1.
	
	TMR1ON = 1;			//Start timer 1.
}
