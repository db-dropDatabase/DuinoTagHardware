/*
 * WilsonPCB.c
 *
 * Created: 1/6/2017 8:34:29 AM
 * Author : Noah
 */ 

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

inline void initUSI(){
	PRR &= ~(1 << PRUSI);
	
}

inline void disableUSI(){
	PRR |= (1 << PRUSI);
}

inline void setBrightness(const volatile char * brigntness){
	if(*brigntness > 0){
		OCIE0A = *brigntness;  //store brightness value
		TIMSK |= (1 << OCIE0A); //enable brightness interrupt
	}
	else TIMSK &= ~(1 << OCIE0A); //disable brightness interrupt
}

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
	PORTB |= (1 << PB4);
	
	//init port interrupts
	GIMSK |= (1 << PCIE);
	PCMSK |= (1 << PCINT4);
	
	//init timer0 interrupt
	GTCCR |= (1 << TSM); //pause timer
	TCCR0B |= (1 << CS00) | (1 << CS01);  //clk/64
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
}

ISR(TIMER0_OVF_vect){
	//cycle animation
}

ISR(PCINT0_vect){
	//check input, do stuff
}