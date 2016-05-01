/*
 * attiny85 IR.cpp
 *
 * Created: 2/23/2016 9:41:21 AM
 * Author : nkoontz0516
*/
#define F_CPU 8000000L
#define IR_PIN PB3
#define SCL PB0
#define SDA PB2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "IRremote.h"

//setup pin change interrupt
#define SETUP_PIN_CHANGE PCMSK |= ((1 << SCL) | (1 << IR_PIN))
//enable pin change interrupt
#define ENABLE_PIN_INTR GIMSK |= 0b00100000
#define DISABLE_PIN_INTR GIMSK &= 0b11011111

IRrecv ir(IR_PIN);

bool clockStretch = false;


int main(void)
{
	cli();
	ir.enableIRIn();
	TIMER_DISABLE_INTR; //start up the IR thing, then turn off the interrupt
	DDRB &= ~((1 << IR_PIN) | (1 << SCL));    //Configure clock and IR pin as input
	SETUP_PIN_CHANGE;
	ENABLE_PIN_INTR;
	sei();
	set_sleep_mode(2);
	sleep_enable();
    while (1) 
    {
		sleep_cpu();
    }
}

//ISR for checking IR or I2C
ISR(PCINT0_vect)
{
	sleep_disable();
	if(PINB & (1 << IR_PIN)){
		ir.enableIRIn();
	}
	else if(PINB & (1 << SCL)){
		if(!ir.isIdle()){
			//clock stretch
			clockStretch=true;
		}
		else{
			//start transmit
		}
	}
}
