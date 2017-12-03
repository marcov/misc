#include "ds1820.h"
#include "1wire.h"
#include "delay.h"


#define START_T_CONVERSION	0x44
#define WRITE_SCRATCHPAD	0x4E
#define READ_SCRATCHPAD		0xBE
#define SKIP_ROM_CHECK		0xCC

/*
 *  Reads temperature from DS1820 and returns (see format depending on mode!).
 */
void DS1820_get_temperature(signed short * temperature)
{
	unsigned char busy=0;
	unsigned char sign;
	unsigned char scratchpad_register[9];
	
	onewire_reset();
	onewire_write(SKIP_ROM_CHECK);
	onewire_write(START_T_CONVERSION);

	DelayMs(100);
	
	while (busy == 0) {
		busy = onewire_read();
		//DelayMs(100);
		//serial_putch('.');
	}

	onewire_reset();
	onewire_write(SKIP_ROM_CHECK);
	onewire_write(READ_SCRATCHPAD);
	
	for (unsigned char i = 0; i<sizeof(scratchpad_register);i++) {
		scratchpad_register[i] = onewire_read();
	}
	
//1: Temperature with the standard resolution:
//Format:
//Abs(T) * 10.
//The MSB of temperature represent the sign!
/*
	*temperature = scratchpad_register[0];
	
	sign = scratchpad_register[1];
	
	// We want to store the absolute value.
 	if (sign > 0) {
 		*temperature = -(signed char)*temperature;
 	} 
 	
 	*temperature *=5;		// In this way we will get temperature x10 value.
	
	//Cowardly adding sign.
	if (sign > 0) {
 		*temperature |= 0x8000;
	}
*/
	
//2: Using float
// Format:
// float
/*
	float float_temp;
	float_temp = (scratchpad_register[0] | scratchpad_register[1]<<8)/2.0f - \
	           0.25f + \
	           ((float)(scratchpad_register[7] - scratchpad_register[6]) / (float)scratchpad_register[6]);
	*temperature = (signed short)float_temp;
*/

//3: Using integer with extended resolution.
// Format:
// T * 100 (signed short).
	signed short faketemp_int;
	*temperature = (scratchpad_register[0] | (scratchpad_register[1]<<8))*5*10 - \
	                25 + \
	               (100 * (scratchpad_register[7] - scratchpad_register[6]) / scratchpad_register[7]);
}

