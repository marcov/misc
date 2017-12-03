#ifndef PWM_H_
#define PWM_H_

void testPWMleds(void);
void setupPWM(void);

extern unsigned char sin_lookup_table [];
extern unsigned char sin_lookup_table_idx;
extern const unsigned char sin_lookup_table_size;


#endif /*PWM_H_*/
