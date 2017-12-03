#include <htc.h>
#include "peripherals/Timer1.h"
#include "init.h"
#include "peripherals/serial.h"
#include "config.h"
#include "LCD.h"
#include "delay.h"
#include "shared.h"
#include "mvbus.h"
#include "ds1820.h"
#include "PWM.h"
#include <string.h>

// variable incremented of 1 every time Timer1 overflows.
// Used to count the ticks in one second.
unsigned char timertick = 0;	

typedef struct {
	unsigned char seconds;
	unsigned char minutes;
	unsigned char hours;
} clock_t;

clock_t clock = { 0 };

unsigned char rxchar;			// Character received from UART.

typedef union{
	unsigned char	f7,
					f6,
					f5,
					f4,
					f3,
					f2,
					f1,
					framingcomplete;
					
	unsigned char byte;
} flags_t;

flags_t flags;

unsigned char mvbus_framingcomplete = 0;


typedef enum{
	UART_OUTPUT,
	LCD_OUTPUT
}output_t;

signed short temperature;


/******************************************************************************/

void uart_print_time(void);
void lcd_print_time(unsigned char row, unsigned char column);
void clock_advance(void);
void print_temperature(output_t output);

void initTimer0(void);

/******************************************************************************/



/******************************************************************************/

void uart_output_test(unsigned char interchar)
{
	for (unsigned char i = 0x00; ; i++){
		uart_putch(i);
		DelayMs(interchar);
	}
}

/******************************************************************************/

void main(void)
{
	// Since ticks in a seconds is a .5 number for 8Mhz clock, 
	// we have introduced a compensation factor.
	//unsigned char driftcompensation = 0;
	
	signed char tmpsec;
		
	di();	// disable global interrupts during startup.
	
	//testPWMleds();
	
	init();	// Function call inserted by C-Wiz
	
	init_pinout();
	
	setupPWM();
	
	initTimer0();
	
	init_Timer1();	//init the timer 1.
	
	lcd_init();		// init the lcd
	
	//LCD_CGRAM_test();	// LCD CGRAM TEST

	serial_setup();	
	//could use printf() here but putst saves 1k of rom space
	uart_puts("\nWelcome on the PIC!\n\n");


	RCIF = 0;			// Clear pending Interrupt on UART Rx.
	RCIE = 1;			// Enable interrupt on uart RX.
	
	ei();	//Enable global interrupt.	
		
	while (1){
		

		lcd_print_time(1,1);
		//uart_print_time();
		
		//This is the MVBus handling part.
		if (mvbus_framingcomplete) {
			//serial_putch('X');
			mvbus_framingcomplete = 0;
			MVBUS_LED = 0;
			mvbus_parser();

		}

		DS1820_get_temperature(&temperature);
		//print_temperature(UART_OUTPUT);
		print_temperature(LCD_OUTPUT);
	 	
		// Timertick algorithm if using TimerA with source clock Fosc/4.
		/*		
		while (timertick < TIMER1_1SECOND + driftcompensation ) {
			// Sleep disables CPU HS oscillator....
		}
		timertick = 0;
		driftcompensation ^= 1;
		*/
		
		// We use TimerA with exteran 32kHz Xtal
		tmpsec = clock.seconds;
		while (tmpsec == clock.seconds);	//wait for the next second.
		clock_advance();
	}	// while(1)
}


void print_temperature(output_t output)
{
	char output_str[]  = { '0', '0', '0', '0', '0', '0', '0', '\0' }; // sign + 3 integer digits + dot + 2 decimal digit
	if (temperature < 0 ) {
		output_str[0] = '-';
		//remove sign
		temperature = -temperature;
		output_str[0] = '-';
	} else {
		output_str[0] = '+';
	}
	
	u16_to_5digits_str((unsigned short)temperature, &output_str[1]);
		
	// Add the dot.
	output_str[6] = output_str[5];
	output_str[5] = output_str[4];
	output_str[4] = '.';
	
	if (output == UART_OUTPUT) {
		uart_puts("\nTemperature: ");
		uart_puts(output_str);
		uart_putch('C');
		uart_putch('\n');
	} else if (output == LCD_OUTPUT) {
		//TODO
		lcd_goto(1,2);
		lcd_puts((char * const)"T: ");
		lcd_puts(output_str);
		lcd_putch('C');
		lcd_putch('\n');	
	}
}

void uart_print_time(void)
{
	uart_puts("\nTime: ");
	
	// Output on serial port.
	if (clock.hours < 10){
		uart_putch('0'); //padding
	}
	uart_putu8(clock.hours);
	
	uart_putch(':');
	
	if (clock.minutes < 10){
		uart_putch('0'); //padding
	}
	uart_putu8(clock.minutes);
	
	uart_putch(':');
	
	if (clock.seconds < 10){
		uart_putch('0'); //padding
	}
	uart_putu8(clock.seconds);
}


void lcd_print_time(unsigned char row, unsigned char column)
{
	char two_digit_str[]  = { '0', '0', '\0' };
	
	//Output on LCD.
	lcd_goto(row, column);
	
	lcd_puts((char * const)"C: ");
		
	u8_to_2digits_str(clock.hours, two_digit_str);
	lcd_puts(two_digit_str);
	
	lcd_putch(':');
	
	u8_to_2digits_str(clock.minutes, two_digit_str);
	lcd_puts(two_digit_str);
	
	lcd_putch(':');
	
	u8_to_2digits_str(clock.seconds, two_digit_str);
	lcd_puts(two_digit_str);
	
	//LCD Progress bar for seconds
	/*
	unsigned char tmpsec;
	lcd_goto(8, 2);
	for (unsigned char i = 0; i<6; i++) lcd_putch(' ');
	
	lcd_goto(8, 2);
	tmpsec = clock.seconds;
	while ( (tmpsec-=10) >= 0 ) {
		lcd_puts((char * const)"\xFF");
	}
	*/
}


void clock_advance(void)
{
	//Very simple clock algo...
	if ( clock.seconds > 59) {
		clock.seconds = 0;
		clock.minutes++;
		
		if ( clock.minutes > 59) {
			clock.minutes = 0;
			clock.hours++;
			
			if (clock.hours > 23) {
				clock.hours = 0;
			}
		}
	}	
}

/******************************************************************************/




void initTimer0(void)
{
	T0CS = 0;	//Use internal instruction cycle clock (Fosc/4)
	T0SE = 0;	//Increment on low-to-high
	PSA = 0;	// Prescaler assigned to Timer 0
	PS2 = 1;	//Prescaler set to 256. Overflow at 8.192 with 8MHz XTAL. 
	PS1 = 1;
	PS0 = 1;
	
	TMR0IE = 1;	//Enable interrupt on Timer 0;
	
}

/*
 * 	Interrupt service routine.
 * 	Current serviced interrupts:
 * 
 * 	- Timer1
 * 
 */
void interrupt global_isr(void){

	if (TMR0IE && TMR0IF) {
		TMR0IF = 0;				// Clear pending interrupt.
		
		CCPR1L = sin_lookup_table[sin_lookup_table_idx];
		sin_lookup_table_idx++;
		if (sin_lookup_table_idx >= sin_lookup_table_size){
			sin_lookup_table_idx = 0;
		}
		return;
	}
/*
	// No interrupts generated
	if (TMR1IE && TMR1IF) {
		TMR1IF = 0;				// Clear pending interrupt.
		TIMER_LED ^= 1;				// toggle led on TIMER_LED.			
		timertick++;
		return;
	}
*/	
	//Compare module, interrupt every 1 sec for time keeping.
	if (CCP2IE && CCP2IF) {
		CCP2IF = 0;				// Clear pending interrupt.
		TIMER_LED ^= 1;				// toggle led on TIMER_LED.
		clock.seconds++;			
		return;
	}
	
	if (RCIE && RCIF) {
		//MVBUS_LED ^= 1;				// toggle led on TIMER_LED.
		if (OERR)													
		{															
			CREN=0;													
			CREN=1;													
		} else {
	
			rxchar = RCREG;
			mvbus_framer();			// Do the framing.
		}
	}
}
