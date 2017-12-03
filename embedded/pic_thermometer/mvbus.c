/*
 *  UART Loader 
 *	Originally designed to load data from UART to an LCD.
 *
 */

#include "LCD.h"
#include "mvbus.h"
#include "delay.h"
#include <htc.h>
#include "peripherals/serial.h"	

/* Protocol definition.
 * 
 *  Name:  MVBus
 * 
 *  Features:
 *    - master / slave communication
 *    - sessionless
 *    - half duplex only
 *    - master and slave assignment is fixed and pre-assigned
 *    - request and response are composed of a single frame only
 *    - timeout on reception
 *    - timeout on response
 *
 *  Frames format:
 * 
 *  Request / Response frame format:
 *  Delimiter - CMD - Bytecount - PAYLOAD
 *
 *  Delimiter = 0x7E
 *  CMD = 0x00 <-> 0x##
 *  Bytecount = # of bytes in payload
 *  Payload = n. bytes of data related to the command. Max size is of T.B.D. characters.
 * 
 *  Timeouts:
 *  Reception: T.B.D.
 *  Response: T.B.D.
 * 
 */


#define DELIMITER_VALUE		0x7E
#define MAX_PAYLAOD_SIZE	48
#define CMD0	0
#define CMD1	1
#define CMD2	2
#define CMD3	3
#define CMD4	4
#define CMD5	5
#define CMD6	6


/******************************************************************************/

// Type definition of state machine that does the framing of a message.
typedef enum{
	WAIT_DELIM,
	WAIT_CMD,
	WAIT_BC,
	WAIT_PAYLOAD
} framer_state_t;

// Type definition for a MVBus frame.
typedef struct{
	unsigned char command;
	unsigned char bytecount;
	unsigned char payload[MAX_PAYLAOD_SIZE];
} mvframe_t;


static framer_state_t framer_state;
mvframe_t mvframe;
extern unsigned char rxchar;
extern unsigned char mvbus_framingcomplete;

/******************************************************************************/

void mvbus_framer(void){	
static unsigned char rcvdpl;  // Variable holding received payload size.
	
	// Get the byte in some way. Byte is stored in rxchar.
	
	switch(framer_state) {
		
		case WAIT_DELIM:
			if (rxchar == DELIMITER_VALUE){
				framer_state = WAIT_CMD;
			}
		break;
		
		case WAIT_CMD:
			mvframe.command = rxchar;
			framer_state = WAIT_BC;
		break;
		
		case WAIT_BC:
			
			mvframe.bytecount = rxchar;
			
			if (mvframe.bytecount > 0) {
				framer_state = WAIT_PAYLOAD;
				rcvdpl = 0;
			} else {
				framer_state = WAIT_DELIM; // Reset the state machine.
				// Signal in some way that the reception of the frame has completed.
				mvbus_framingcomplete = 1;
			}
		break;
		
		case WAIT_PAYLOAD:
			mvframe.payload[rcvdpl] = rxchar;
			rcvdpl++;
			if (rcvdpl >= mvframe.bytecount){
				framer_state = WAIT_DELIM; // Reset the state machine.
				// Signal in some way that the reception of the frame has completed.	
				mvbus_framingcomplete = 1;		
			}
		break; 
	}
}


/******************************************************************************/

void mvbus_parser(void) 
{
	
	switch (mvframe.command) {

		case CMD0:
			// Clears LCD
			uart_puts("CMD 0 : Clear LCD.\n");
			lcd_clear();
		break;
						
		case CMD1:
			// Loads a symbol in the CGRAM of the HD44780
			// Payload:
			// Byte 0: CGRAM Address
			// Byte 1-8: symbol data.
			
			uart_puts("CMD 1 : Load a symbol in the CGRAM.\n");
			
			if (mvframe.bytecount >= 9) {
				
				lcd_load_CGRAM_symbol(mvframe.payload[0], &mvframe.payload[1]);
			} else {
				// T.B.D.				
			}
		break;

		case CMD2:
			// goto line identified by payload
			uart_puts("CMD 2 : Goto LCD x,y position.\n");
			
			if (  mvframe.bytecount >=2 
				&& 
				  mvframe.payload[0] <= 2			// x
				&&
				  mvframe.payload[1] <= 16 ){		// y

				lcd_goto(mvframe.payload[0],mvframe.payload[1]);
			} else {
				//TBD
			}
		break;
		
		case CMD3:
			// Write a string on LCD
			uart_puts("CMD 3 : Write a string on LCD.\n");
			
			if (mvframe.bytecount > 0) {
				for (unsigned char i = 0; i < mvframe.bytecount; i++ ) {
					lcd_putch(mvframe.payload[i]);
				}
			} else {
				// TBD
			}
			
		break;

		default:
			uart_puts("Invalid CMD.\n");
		break;
	}
}



