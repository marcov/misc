#include	<pic.h>
#include	"LCD.h"
#include	"delay.h"

#define LCD_RS	RA5	// Register select
#define LCD_EN	RA4	// Enable
#define LCD_D4	RA3	// Data bits
#define LCD_D5	RA2	// Data bits
#define LCD_D6	RA1	// Data bits
#define LCD_D7	RA0	// Data bits

#define	LCD_STROBE	((LCD_EN = 1),(LCD_EN=0))



static unsigned char ddram_addr;



#define CGRAM_MODE		0x40
#define DDRAM_MODE		0x80


static void lcd_goto_CGRAM(unsigned char address)
{
	LCD_RS = 0;
	lcd_write(CGRAM_MODE | (address & 0x3f));
}

static void lcd_goto_DDRAM(void) {
	LCD_RS = 0;
	lcd_write(DDRAM_MODE | (ddram_addr & 0x7f));

}

void lcd_load_CGRAM_symbol(unsigned char cgaddr, unsigned char * symbolbuf){

	lcd_goto_CGRAM(cgaddr);
	
	LCD_RS = 1;
	for (unsigned char * ptr = symbolbuf; ptr < symbolbuf+8; ptr++) {
		lcd_write(*ptr);
	}
}


#pragma inline (test)
static void update_ddram_addr(void) {
	ddram_addr++;	
	
	if (ddram_addr > 0x4f) {			// We are at the end of 2nd line
		ddram_addr = 0x00;
	} else if (ddram_addr == 0x10) {	// We are at the end of 1st line.
		ddram_addr = 0x40;
	}
}

/* write a byte to the LCD in 4 bit mode */
void
lcd_write(unsigned char c)
{

	if(c & 0x80) LCD_D7=1; else LCD_D7=0;
	if(c & 0x40) LCD_D6=1; else LCD_D6=0;
	if(c & 0x20) LCD_D5=1; else LCD_D5=0;
	if(c & 0x10) LCD_D4=1; else LCD_D4=0;

	//PORTC = c >> 4;
	
	LCD_STROBE;

	if(c & 0x08) LCD_D7=1; else LCD_D7=0;
	if(c & 0x04) LCD_D6=1; else LCD_D6=0;
	if(c & 0x02) LCD_D5=1; else LCD_D5=0;
	if(c & 0x01) LCD_D4=1; else LCD_D4=0;

	//PORTC = c & 0x0F;
	
	LCD_STROBE;	
	DelayUs(40);
}

/*
 * 	Clear and home the LCD
 */
void
lcd_clear(void)
{
	LCD_RS = 0;

	lcd_write(0x1);
	DelayMs(2);
}

/* write a string of chars to the LCD */

void lcd_puts(char * const s)
{
	char * temp = s;
	
	lcd_goto_DDRAM();					// update position.
	
	LCD_RS = 1;	// write characters

	while( *temp != '\0' ) {
		lcd_write(*temp);
		temp++;
		
		update_ddram_addr();
	}
}

/* write one character to the LCD */

void
lcd_putch(unsigned char c)
{
	lcd_goto_DDRAM();					// update position.
	
	LCD_RS = 1;	// write characters
	lcd_write(c);
	
	update_ddram_addr();
}


/*
 * Go to the specified position
 */

void
lcd_goto(unsigned char x, unsigned char y)
{
	const unsigned char maxx = 16;
	
    // If the column is different than zero...
    if( x != 0u )
        // ... change the current column to it.
        x = x - 1;
    // If the line is different than zero...
    if( y != 0u )
        // ... change the current line to it.
        y = y - 1;
        

	ddram_addr = 0x7f & ((( y << 6 ) & 0x40 ) | ( x & 0x3f ) + ( y & 0x02 ? (maxx) : 0 )) ;
	LCD_RS = 0;
	lcd_write ( DDRAM_MODE | ddram_addr);

}
	
/* initialise the LCD - put into 4 bit mode */

void
lcd_init(void)
{
	LCD_RS = 0;	// write control bytes

	DelayMs(15);// power on delay

	LCD_D4 = 1;	// init!	
	LCD_D5 = 1; //
	LCD_STROBE;	
	DelayMs(5);

	LCD_STROBE;	// init!	
	DelayUs(100);

	LCD_STROBE;	// init!	
	DelayMs(5);

	LCD_D4 = 0;	// set 4 bit mode
	LCD_STROBE;	
	DelayUs(40);
	
	lcd_write(0x28);// 4 bit mode, 1/16 duty, 5x8 font, 2lines
	lcd_write(0x0C);// display on
	lcd_write(0x06);// entry mode advance cursor
	lcd_write(0x01);// clear display and reset cursor
	DelayMs(2);
}


void lcd_CGRAM_test(void)
{
//	unsigned char mvbuffer [] = {0x11,0x1b,0x15,0x11,0x0a,0x0a,0x04,0x00};
	unsigned char upbuffer [] = {0x0f,0x1f,0x0,0x0,0x0,0x0,0x0f,0x1f};
	unsigned char urbuffer [] = {0x1e,0x1f,0x3,0x3,0x3,0x3,0x1f,0x1f};
	unsigned char dobuffer [] = {0x1f,0x0f,0x0,0x0,0x0,0x0,0x1f,0x0f};
	unsigned char drbuffer [] = {0x1f,0x1f,0x3,0x3,0x3,0x03,0x1f,0x1e};
	
	
	lcd_goto_CGRAM(0);

	for (unsigned char i=0; i<sizeof(upbuffer); i++)
	{
		lcd_putch(upbuffer[i]);
	}
	for (unsigned char i=0; i<sizeof(urbuffer); i++)
	{
		lcd_putch(urbuffer[i]);
	}
	for (unsigned char i=0; i<sizeof(dobuffer); i++)
	{
		lcd_putch(dobuffer[i]);
	}
	for (unsigned char i=0; i<sizeof(drbuffer); i++)
	{
		lcd_putch(drbuffer[i]);
	}						


	lcd_goto(1,1);
	lcd_putch(0);
	lcd_putch(1);
	
	lcd_goto(1,2);
	lcd_putch(2);
	lcd_putch(3);

}