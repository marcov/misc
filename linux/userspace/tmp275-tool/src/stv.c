#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C_ADAPTER_PATH	"/dev/i2c/0"   //"/dev/i2c/1"
#define STV_ADDRESS_WRITE	0x96
#define STV_ADDRESS_READ	0x97



#define TEMPERATURE_REGISTER	0x00
#define CONFIGURATION_REGISTER	0x01
#define TLOW_REGISTER		0x02
#define THIGH_REGISTER		0x03

int main(void)
{
int tmp275 = -1;
uint8_t i2cbuffer[4];
int ret;
struct i2c_rdwr_ioctl_data ioctl_data;
struct i2c_msg	msg[2];
unsigned char buf0[4], buf1[4];


	memset(i2cbuffer, 0x00, sizeof(i2cbuffer) / sizeof(uint8_t));

	if ( (tmp275 = open(I2C_ADAPTER_PATH, O_RDWR)) < 0) {
		puts("Cannot open i2c adapter\n");
		goto exitpoint;
	} else {
		puts("Correctly opened I2C adapter\n");
	}

/*************************************/
	
	msg[0].addr = STV_ADDRESS_WRITE;
	
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = buf0;

	ioctl_data.msgs = &msg[0];
	ioctl_data.nmsgs = 1;

	msg[0].buf[0] = 0x00;
	msg[0].buf[1] = 0x04;

	if (ioctl(tmp275, I2C_RDWR, &ioctl_data) < 0) {
	    printf("Error ioctl RDWR\n");
	} else {
	    printf("ioctl_data write STV 00h OK \n"
		  ); 
	}
	
/***************************************/
	
	msg[0].addr = STV_ADDRESS_READ;
	msg[1].addr = STV_ADDRESS_READ;
	
	msg[0].flags = 0;
	msg[1].flags = I2C_M_RD;
	msg[0].len = 1;
	msg[1].len = 1;

	msg[0].buf = buf0;
	msg[1].buf = buf1;

	ioctl_data.msgs = &msg[0];
	ioctl_data.nmsgs = 2;

		
	msg[0].buf[0] = 0x00;

	if (ioctl(tmp275, I2C_RDWR, &ioctl_data) < 0) {
	    printf("Error ioctl RDWR\n");
	} else {
	    printf("ioctl_data read STV 00h , value: %02X \n",
		   msg[1].buf[0]); 
	}
/*************************************/	
	
	while (1) {
	
		msg[0].addr = STV_ADDRESS_READ;
		msg[1].addr = STV_ADDRESS_READ;

		ioctl_data.msgs = &msg[0];
		ioctl_data.nmsgs = 2;

		msg[0].len = 1;   //Write value TEMPERATURE_REGISTER
		msg[0].flags = 0;
		
		msg[1].len = 2;  //Read the two bytes of temperature.
		msg[1].flags = I2C_M_RD;	//message is read.

		msg[0].buf[0] = 0x01;
		
		if (ioctl(tmp275, I2C_RDWR, &ioctl_data) < 0) {
	    		printf("Error ioctl RDWR\n");
		} else {
			printf("I2C register: %02X %02X \n", 
				msg[1].buf[0], 
				msg[1].buf[1]
				);

			printf("Temperature is %.2f  \n\n", 
				 (float)
				 (
				 (unsigned int)msg[1].buf[0] << 4 
				 |
				 (unsigned int)msg[1].buf[1] >> 4
				 ) / 16.0f
			
			);
		}
	
		sleep(1);
	}


exitpoint:
	return 0;
}

