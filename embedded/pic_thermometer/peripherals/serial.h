#ifndef SERIAL_H_
#define SERIAL_H_


//*****************
//function prototypes

void serial_setup(void);
void uart_putch(unsigned char c);
unsigned char getch(void);
void uart_puts(register const char * str);
unsigned char usart_timeout(void);
void uart_putu8(unsigned char c);
void putchhex(unsigned char c);
void putinthex(unsigned int c);

#define putlf putst("\n") //put line feed


#endif /*SERIAL_H_*/
