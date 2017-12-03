#include "PWM.h"
#include <pic.h>
#include "delay.h"


/*
 * Sampling rate [Hz] = number of points * wave frequency[Hz]
 * 
 * sinus wave: amplitude*1/2*(1+sin(2*pi*1/n*x)
 * with x 0..n-1
 */
/*
octave-3.2.4.exe:48> diary on
octave-3.2.4.exe:49> x=[0:1:63];
octave-3.2.4.exe:50> y=127*1/2*(1+sin(2*pi*1/64*x));
octave-3.2.4.exe:51> plot(x,y)
octave-3.2.4.exe:52> y=int32(y);
octave-3.2.4.exe:53> save c:\pippo.txt y
octave-3.2.4.exe:54> diary off
*/
/*
// This is the standard sinus wave
unsigned char sin_lookup_table []= {
64, 70, 76, 82, 88, 93, 99, 104, 108, 113, 116, 120, 122, 124, 126, 127,
127, 127, 126, 124, 122, 120, 116, 113, 108, 104, 99, 93, 88, 82, 76, 70, 64, 57,
 51, 45, 39, 34, 28, 23, 19, 14, 11, 7, 5, 3, 1, 0, 0, 0, 1, 3, 5, 7, 11, 14, 19,
  23, 28, 34, 39, 45, 51, 57};
*/

// This is the standard sinus wave with added longer minimum
unsigned char sin_lookup_table []= {
64, 70, 76, 82, 88, 93, 99, 104, 108, 113, 116, 120, 122, 124, 126, 127, 127, 127,
126, 124, 122, 120, 116, 113, 108, 104, 99, 93, 88, 82, 76, 70, 64, 57, 51, 45, 39,
34, 28, 23, 19, 14, 11, 7, 5, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 
3, 5, 7, 11, 14, 19, 23, 28, 34, 39, 45, 51, 57};
  
unsigned char sin_lookup_table_idx;
const unsigned char sin_lookup_table_size = sizeof(sin_lookup_table);

void setupPWM(void)
{
	/*
	 * configure CCP module as 4000 Hz PWM output
	 */
	PR2 = 0b01111100 ;
	T2CON = 0b00000101 ;
	CCP1CON = 0b00001100 | 0b00111100 ;
/*	
//1. Establish the PWM period by writing to the PR2 register.
	PR2 = 0xFF;
	
//2. Establish the PWM duty cycle by writing to the DCxB9:DCxB0 bits.
	CCP1RL = 0x0F;
	CCP1CON = 0x30;
	
	
//3. Make the CCPx pin an output by clearing the appropriate TRIS bit.
	TRISC &= ~(0x01<<2);
	
//4. Establish the TMR2 prescale value and enable Timer2 by writing to T2CON.

	TMR2ON = 0;			//Stop timer before configuring it.
	T2CON = 0;			// Clear all the control register.

	T2CKPS0 = 0;
	T2CKPS1 = 0;
	TMR2ON = 1;			//Stop timer before configuring it.
	
	

//5. Configure the CCP module for PWM operation.
	CCPCON1 |= 0x0C;
	
	*/
}



/******************************************************************************/

/*
 * 	Test PWM led on RC2 / CCP1
 */
void testPWMleds(void)
{
unsigned char dc ;
	
	TRISC = 0 ;                     // set PORTC as output
	PORTC = 0 ;                     // clear PORTC
	
	/*
	 * configure CCP module as 4000 Hz PWM output
	 */
	PR2 = 0b01111100 ;
	T2CON = 0b00000101 ;
	CCP1CON = 0b00001100 ;
	//CCP2CON = 0b00111100 ;
	
	for(;;) {
	    /*
	     * PWM resolution is 10 bits
	     * don't use last 2 less significant bits CCPxCON,
	     * so only CCPRxL have to be touched to change duty cycle
	     */
	    for(dc = 0 ; dc < 0x80 ; dc++) {
	            CCPR1L = dc ;
	            //CCPR2L = 128 - dc ;
	            DelayMs(10) ;
	    }
	    
	    DelayMs(200);
	           
	    for(dc = 0x7F ; (signed char)dc >= 0 ; dc--) {
	            CCPR1L = dc ;
	            //CCPR2L = 128 - dc ;
	            DelayMs(10) ;
	    }
	    DelayMs(200);
	}
}