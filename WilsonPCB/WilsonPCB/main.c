/*
 * WilsonPCB.c
 *
 * Created: 1/6/2017 8:34:29 AM
 * Author : Noah
 */ 
#define BUTTON PB4
#define SHORTPRESS 0x05
#define LONGPRESS 0x30
#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "debounce.h"

volatile unsigned char lastButtonState = 0;
volatile unsigned char pressTime = 0;

static inline void sendLEDs(const unsigned char data){
	//turn USI back on
	PRR &= ~(1 << PRUSI);
	USICR |= (1 << USIWM0) | (USICS0);
	USISR = 0;
	//fill USI data register
	USIDR = data;
	//aaaaand pulse clock for awhile
	while (!(USISR & (1 << USIOIE))){
		USICR |= (1 << USITC);
		USICR |= (1 << USITC) | (USICLK);
	}
	//pulse latch
	PORTB |= (1 << PB3);
	PORTB &= ~(1 << PB3);
	//now go back to bed
	PRR |= (1 << PRUSI);
}

static inline void setBrightness(const volatile unsigned char brigntness){
	if(brigntness == 0){
		TIMSK &= ~(1 << OCIE0A); //disable brightness interrupt
		OCR0A = 0; //erase brightness value
		sendLEDs(0b00000000); //turn off leds
	}
	else if(brigntness == 0xff) {
		TIMSK &= ~(1 << OCIE0A); //disable brightness interrupt
		OCR0A = 0xFF; //erase brightness value
	}
	else{
		OCR0A = brigntness;  //store brightness value
		TIMSK |= (1 << OCIE0A); //enable brightness interrupt
	}
	
}

static inline unsigned char getBrightness(){ return OCR0A; }

int main(void)
{
	//disable interrupts during setup
	cli();
	
	//init sleep stuff
	MCUSR |= (1 << BODS) |  (1 << SE) | (1 << BODSE); //disable brownn out detection
	PRR |= (1 << PRTIM1) | (1 << PRUSI) | (1 << PRADC);  //disable timer1, USI, and ADC
	ACSR |= (1 << ACD); //power off the ADC some more
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	//init ports
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	PORTB |= (1 << BUTTON); //button pin pullup enabled
	
	//init timer0 interrupt
	GTCCR |= (1 << TSM); //pause timer
	TCCR0B |= (1 << CS01) | (1 << CS00);  //clk/64
	TIMSK |= (1 << TOIE0); //enable timer overflow interrupt
	GTCCR &= ~(1 << TSM); //unpause timer

	//reenable interrupts
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		//sleep, cuz
		sleep_cpu();
    }
}


ISR(TIMER0_COMPA_vect){
	//turn off LEDs for brightness
	sendLEDs(0b00000000);
}

ISR(TIMER0_OVF_vect){
	if(!(PINB & (1 << BUTTON)) && pressTime < 0xff) pressTime++; //if button reads pressed, inc varible
	else pressTime = 0;

	//if button is simply long pressed
	if(pressTime == LONGPRESS){
		//long press code
		setBrightness(0);
	}
	//else if button is not pressed, but it was pressed before (can't repeat)
	else if(lastButtonState > SHORTPRESS && lastButtonState < LONGPRESS && !pressTime){
		//short press code
		setBrightness(getBrightness() + 0x20);
	}

	lastButtonState = pressTime;

	//cycle animation
	if(getBrightness()){
		sendLEDs(0b10101010);
	}
}