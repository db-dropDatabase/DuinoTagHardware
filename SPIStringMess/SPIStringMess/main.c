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

//setup pin change interrupt
#define SETUP_PIN_CHANGE PCMSK |= (1 << CS)
//enable pin change interrupt
#define ENABLE_PIN_INTR GIMSK |= (1 << PCIE)
#define DISABLE_PIN_INTR GIMSK &= ~(1 << PCIE)

volatile char *filename = '\0';
volatile bool newFile = false;

void init_spi(void);
void spi_slave(void);
void spi_master(void);

int main(void)
{	
	init_spi();
	spi_master();

	SETUP_PIN_CHANGE;
	ENABLE_PIN_INTR;

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	PORTB |= (1 << PB4);
	_delay_ms(200);
	PORTB &= ~(1 << PB4);
	_delay_ms(100);

    /* Replace with your application code */
    while (1) 
    {
		while(!newFile) sleep_cpu();
		_delay_ms(200);
		PORTB &= ~(1 << LED);
    }
}


ISR(PCINT0_vect){
	spi_slave();
	if(PINB & (1 << CS)){
		while(PINB & (1 << CS)) { //wait until CS is low, meaning transmission is done
			wdt_reset();
			if(USISR & (1 << USIOIF)){ //if USIDR is full
				const char *buff = USIDR; //read USIDR
				buff += '\0'; //add null term for strcat
				filename = strcat((char *)filename, buff); //append whatever character received to filename string
				USIDR = (char)buff[0]; //send it back for debugging
				USISR = (1 << USIOIF); //reset USI
			}
		}

		if(filename != '\0'){
			newFile = true;
			PORTB |= (1 << LED);
		}
	}
	spi_master();
}
