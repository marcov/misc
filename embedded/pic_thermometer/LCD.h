#ifndef __LCD_H
#define __LCD_H
/*
 *	LCD interface header file
 *	See lcd.c for more info
 *
 * Copywrite Craig Lee 1998
 */

/* write a byte to the LCD in 4 bit mode */

extern void lcd_write(unsigned char);

/* Clear and home the LCD */

extern void lcd_clear(void);

/* write a string of characters to the LCD */

void lcd_puts(char * const s);

/* Go to the specified position x, y where x is column and y is row*/

extern void lcd_goto(unsigned char x, unsigned char y);
	
/* intialize the LCD - call before anything else */

extern void lcd_init(void);

extern void lcd_putch(char);

/*	Set the cursor position */

#define	lcd_cursor(x)	lcd_write(((x)&0x7F)|0x80)
#define 	LINE1			0x00	/* position of line1 */
#define 	LINE2			0x40	/* position of line2 */


extern void lcd_load_CGRAM_symbol(unsigned char cgaddr, unsigned char * symbolbuf);

#endif
