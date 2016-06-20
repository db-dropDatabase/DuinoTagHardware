/*
 * SPIStringMess.c
 *
 * Created: 5/9/2016 5:46:32 PM
 * Author : Noah
 */ 
 #define F_CPU 8000000L
 #define CS PB3
 #define LED PB4

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

volatile char filename[4] = "";
volatile bool newFile = false;

void init_spi(void);
void spi_slave(void);
void spi_master(void);

static inline void new_spi_slave(){
	DDRB &= ~(1 << PB2);
	USICR |= (1 << USIWM0);
	USISR = (1 << USIOIF);
	USIDR = 0;
}

static inline void new_spi_master(){
	DDRB |= (1 << PB2);
	USICR &= ~(1 << USIWM0);
	USISR = (1 << USIOIF);
	USIDR = 0;
}

static inline void new_init_spi(){
	USICR = (1 << USICS1);
}

int main(void)
{	
	cli();

	DDRB  = 0b111110 & ~(1 << CS);
	PORTB = 0b101001 | (1 << CS);		/* Initialize port: - - H L H L L P */

	new_init_spi();
	new_spi_master();

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	PORTB |= (1 << PB4);
	_delay_ms(200);
	PORTB &= ~(1 << PB4);
	_delay_ms(100);

	sei();

    /* Replace with your application code */
    while (1) 
    {
		//while(!newFile) sleep_cpu();
    }
}


ISR(PCINT0_vect){
	if(PINB & (1 << CS)){
		new_spi_slave();
		newFile = true;
		while(PINB & (1 << CS)) { //wait until CS is low, meaning transmission is done
			if(USISR & (1 << USIOIF)){ //if USIDR is full
				const char temp = USIDR;
				for(uint8_t i=0; i<4; i++){ //copy char into next slot
					if(filename[i] == '\0'){
						filename[i] = temp;
						filename[i+1] = '\0';
						break;
					}
				}

				USIDR = filename[0];
				USISR = (1 << USIOIF);
			}
		}

		const char *temp = (const char *)filename;

		if(strstr("BEE BEE MCBEEFACE (that should do it)", temp)){
			PORTB |= (1 << LED); //Woohoo!
		}

		newFile = false;
		new_spi_master();
	}
}
