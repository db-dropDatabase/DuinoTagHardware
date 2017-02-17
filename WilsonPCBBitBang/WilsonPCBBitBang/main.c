/*
 * WilsonPCBBitBang.c
 *
 * Created: 1/9/2017 12:51:31 PM
 * Author : Noah
 */ 
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#define SIN PB1
#define CLK PB2
#define LATCH PB3
#define IND PB0

int main(void)
{
	DDRB |= (1 << SIN) | (1 << CLK) | (1 << LATCH) | (1 << IND);

	PORTB |= (1 << IND);
	_delay_ms(1000);
	PORTB &= ~(1 << IND);
	_delay_ms(1000);

    /* Replace with your application code */
    while (1) 
    {
		
		//send some crap
		for(unsigned char i = 0; i < 0x08; i++){
			//BITBANG
			//one then zero, repeated eight times
			PORTB = (1 << SIN);
			_delay_ms(1);
			PORTB = (1 << SIN) | (1 << CLK);
			_delay_ms(1);
			PORTB = (1 << SIN);
			_delay_ms(1);
		}
		//latch
		PORTB = (1 << LATCH);
		_delay_ms(1);
		PORTB = 0;
		_delay_ms(100);
		
    }
}

