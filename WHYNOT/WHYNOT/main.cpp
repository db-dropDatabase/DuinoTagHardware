/*
 * attiny85 IR.cpp
 *
 * Created: 2/23/2016 9:41:21 AM
 * Author : nkoontz0516
*/
//TODO: Add blink LED for debug, figure out why response is always 0

#define F_CPU 8000000L

#define IR_PIN PB3

#define MOSI PB0
#define MISO PB1
#define CLK PB2
#define CS PB4

#define ERROR 0x01
#define NO_PACKET 0x0e

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdbool.h>
#include "IRRemote/IRremote.h"

//enable/disable pin change interrupts
#define ENABLE_GLOBAL_PIN_INTR GIMSK |= 0b00100000
#define DISABLE_GLOBAL_PIN_INTR GIMSK &= 0b11011111

#define ENABLE_IR_PIN_INTR PCMSK |= (1 << IR_PIN)
#define DISABLE_IR_PIN_INTR PCMSK &= ~(1 << IR_PIN)

#define ENABLE_CS_PIN_INTR PCMSK |= (1 << CS)
#define DISABLE_CS_PIN_INTR PCMSK &= ~(1 << CS)

//start/stop the USI bus
//starts with interrupt and output disabled
#define ENABLE_USI USICR |= (1 << USICS1) 
//enable/disable USI output
#define DISABLE_USI_OUT USICR &= ~(1 << USIWM0)
#define ENABLE_USI_OUT USICR |= (1 << USIWM0)
//enable/disable USI ovf. interrupt
#define ENABLE_USI_INTR USICR |= (1 << USIOIE)
#define DISABLE_USI_INTR USICR &= ~(1 << USIOIE)
//reset USI bus with both of these
#define CLEAR_USI_DR USIDR = 0
#define CLEAR_USI_STATUS USISR = (1 << USIOIF) 

IRrecv ir(IR_PIN);

void onFinishIR(uint32_t packet, uint8_t packetLength);

volatile uint32_t packetStore = 0;
volatile int16_t packetLen = 0;

volatile bool IRAwake = false;

//volatile uint8_t DRDump = 0;

enum usi_states{
	idle = 0,
	transmit,
	finish,
};

volatile usi_states usiState = idle;

int main(void)
{
	cli();

	//inputs
	DDRB |= ~((1 << CS) | (1 << CLK)) | (1 << IR_PIN); //(1 << MOSI) |
	//and pullups
	//PORTB |= (1 << MOSI) | (1 << CS) | (1 << CLK); 

	//set this to call the onFinish function when IR is done
	ir.onFinish(onFinishIR); 
	
	//start up the IR thing, then turn off the interrupt
	ir.enableIRIn();
	TIMER_DISABLE_INTR; 

	//set up all the pin change interrupts
	ENABLE_IR_PIN_INTR;
	ENABLE_CS_PIN_INTR;
	ENABLE_GLOBAL_PIN_INTR; 

	//Enable the USI Bus
	ENABLE_USI; 
	
	sei();
	
	//setup sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	//BlinkyMcBlinkFace
	PORTB |= (1 << MOSI);
	_delay_ms(250);
	PORTB &= ~(1 << MOSI);
	_delay_ms(100);

	for(;;){
		//while nothing is happening, sleep the cpu
		while(!IRAwake && usiState == idle)
			sleep_cpu();
	}
}

void onFinishIR(uint32_t packet, uint8_t packetLength){
	//disable IR recieving
	TIMER_DISABLE_INTR;
	//copy the packet data
	packetStore = packet;
	packetLen = packetLength;
	//and reset keepAwake
	IRAwake = false;
	//and of course, blink an LED
	PORTB |= (1 << MOSI);
}

//ISR sleep wakeup and IR/USI start
ISR(PCINT0_vect)
{
	if(PINB & (1 << CS) && usiState == idle){
		//Enable USI ovf. interrupt
		ENABLE_USI_INTR;
		//Reset USI bus
		CLEAR_USI_STATUS;
		//Enable USI output
		ENABLE_USI_OUT;
		//and start sending packetLen
		if(packetLen == 0) {
			USIDR = NO_PACKET;
			usiState = finish;
		}
		else {
			USIDR = (uint8_t)packetLen;
			usiState = transmit;
		}
	}
	else if((PINB & (1 << IR_PIN)) && !IRAwake){
		//disable IR start detection
		DISABLE_IR_PIN_INTR;
		//IR recieveing enable
		ir.enableIRIn();
		//and sleep disable
		IRAwake = true;
	}
}

//ISR to handle USI transmission
ISR(USI_OVF_vect){
	//Reset USI
	CLEAR_USI_STATUS;
	switch(usiState){
		case transmit: 
			//transmit the first 8 bits of the packet, MSB first
			USIDR = (uint8_t)(packetStore >> (packetLen -= 8)); 
			//set state machine for next 8 bits, if such bits exist, else finish up
			if(packetLen <= 0) usiState = finish;
			break;
		case finish: 
			//Reset USI flags for next signal
			DISABLE_USI_OUT;
			DISABLE_USI_INTR;
			ENABLE_CS_PIN_INTR;
			//reset state machine, enabling sleep
			usiState = idle;
			break;
	}
}
