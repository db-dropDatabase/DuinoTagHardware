/*
 * SPI Debugger.c
 *
 * Created: 5/3/2016 5:20:38 PM
 * Author : Noah
 */ 

 #define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>

#define LED PB0
#define CS PB2
#define MOSI PB3
#define MISO PB4
#define SCK PB5

#define SONG1 'B'
#define SONG2 'E'
#define SONG3 'E'

int main(void)
{
    //outputs
	DDRB |= (1 << LED) | (1 << CS) | (1 << MOSI) | (1 << SCK);

	//Enable SPI, set master mode, f/64 speed
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1);

	//blink
	PORTB |= (1 << LED);
	_delay_ms(200);
	PORTB &= ~(1 << LED);
	_delay_ms(100);

    while (1) 
    {
		//blink LED and enable CS
		PORTB |= (1 << LED) | (1 << CS);
		//transmit
		SPDR = SONG1;
		//and wait until done
		while(!(SPSR & (1 << SPIF)));
		_delay_ms(1);
		//and do it again
		SPDR = SONG2;
		while(!(SPSR & (1 << SPIF)));
		_delay_ms(1);
		//and again
		SPDR = SONG3;
		while(!(SPSR & (1 << SPIF)));
		_delay_ms(1);
		//back to blinking
		PORTB &= ~(1 << LED) & ~(1 << CS);
		//wait for effect
		_delay_ms(5000);
    }
}

