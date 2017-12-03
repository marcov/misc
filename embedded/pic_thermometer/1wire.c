// (C) copyright 2003 j.d.sandoz / jds-pic !at! losdos.dyndns.org

// released under the GNU GENERAL PUBLIC LICENSE (GPL)
// refer to http://www.gnu.org/licenses/gpl.txt

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

/***********************1Wire Class***********************/
/*Description: This class handles all communication */
/* between the processor and the 1wire */
/* sensors.
/*********************************************************/
#include <pic.h>
#include "1wire.h"
#include "delay.h"



/*-------1-wire definitions-------*/
#define ONE_WIRE_PORT_PIN	B1
// Dont change the following!
#define ONE_WIRE_PIN 		R##ONE_WIRE_PORT_PIN
#define ONE_WIRE_PIN_TRIS	TRIS##ONE_WIRE_PORT_PIN

#define ONEWIRE_OUTPUT_LOW()	{\
								ONE_WIRE_PIN = 0; \
								ONE_WIRE_PIN_TRIS = 0; \
							}
							
							
#define ONEWIRE_HIGH_IMPEDACE()	{\
									ONE_WIRE_PIN_TRIS = 1; \
								}

// Remeber to set to high impedance before reading!!
#define ONEWIRE_READ_PIN()		ONE_WIRE_PIN

/*******************1-wire communication functions********************/

/************onewire_reset*************************************************/
/*This function initiates the 1wire bus */
/* */
/*PARAMETERS: */
/*RETURNS: */
/*********************************************************************/

void onewire_reset()  // OK if just using a single permanently connected device
{
	ONEWIRE_OUTPUT_LOW();
	DelayBigUs( 500 ); // pull 1-wire low for reset pulse
	ONEWIRE_HIGH_IMPEDACE(); // float 1-wire high
	DelayBigUs( 500 ); // wait-out remaining initialisation window.
	ONEWIRE_HIGH_IMPEDACE();
}

/*********************** onewire_write() ********************************/
/*This function writes a byte to the sensor.*/
/* */
/*Parameters: byte - the byte to be written to the 1-wire */
/*Returns: */
/*********************************************************************/

void onewire_write(unsigned char data)
{
	for (unsigned char count=0; count<8; ++count) {
		ONEWIRE_OUTPUT_LOW();
		DelayUs( 2 ); // pull 1-wire low to initiate write time-slot.
		if ((data & 0x01) == 1) {
			ONEWIRE_HIGH_IMPEDACE();
		} else {
			ONEWIRE_OUTPUT_LOW();
		}
		data >>=1;
		DelayUs( 60 ); // wait until end of write slot.
		ONEWIRE_HIGH_IMPEDACE(); // set 1-wire high again,
		DelayUs( 2 ); // for more than 1us minimum.
	}
}

/*********************** read1wire() *********************************/
/*This function reads the 8 -bit data via the 1-wire sensor. */
/* */
/*Parameters: */
/*Returns: 8-bit (1-byte) data from sensor */
/*********************************************************************/

unsigned char onewire_read()
{
	unsigned char data = 0x00;

	for (unsigned char count=0; count<8; count++)	{
		ONEWIRE_OUTPUT_LOW();
		DelayUs( 2 ); // pull 1-wire low to initiate read time-slot.
		ONEWIRE_HIGH_IMPEDACE(); // now let 1-wire float high,
		DelayUs( 8 ); // let device state stabilise,
		data |= (ONEWIRE_READ_PIN() << count);
		
		//shift_right(&data,1,input(ONE_WIRE_PIN)); // and load result.
		DelayUs( 120 ); // wait until end of read slot.
	}

	return data;
}