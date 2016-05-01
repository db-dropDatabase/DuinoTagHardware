/*
 * Blink85.c
 *
 * Created: 2/4/2016 8:48:52 AM
 * Author : nkoontz0516
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRB |= 1 << DDB3;
    /* Replace with your application code */
    while (1) 
    {
		PORTB ^= (1 << PB3);
		_delay_ms(100);
		PORTB ^= (1 << PB3);
		_delay_ms(100); 
    }
}

