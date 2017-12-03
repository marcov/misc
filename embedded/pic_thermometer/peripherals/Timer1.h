#ifndef TIMER1_H_
#define TIMER1_H_


// Number of ticks to reach 1 seconds with currently configured Timer1.
// Timer1 uses as source clock Fosc/4.
// Timer1 counter size is 16 bit.
#define TIMER1_1SECOND	(PIC_CLK/4/0x10000)


void init_Timer1 (void);


#endif /*TIMER1_H_*/
