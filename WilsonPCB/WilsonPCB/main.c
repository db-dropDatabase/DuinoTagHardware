/*
 * WilsonPCB.c
 *
 * Created: 1/6/2017 8:34:29 AM
 * Author : Noah
 */ 

#define F_CPU 1000000UL
#define BUTTON PB4
#define SOUT PB1
#define CLK PB2
#define LATCH PB3
#define NOSE PB0

#define SHORTPRESS 0x05
#define LONGPRESS 0x30

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "debounce.h"

typedef struct _animation {
	unsigned char frameLength;
	unsigned char frameNum;
	unsigned char frames[]; //formatted as such {frame, frame, noseLED, noseLED}
}animation_t;

const volatile animation_t simpleBlink = {
	.frameLength = 24,
	.frameNum = 2,
	.frames = {0b10101010, 0b01010101,
	0, 1}
};

const volatile animation_t oregonSign = {
	.frameLength = 24,
	.frameNum = 4,
	.frames = {0b00000000, 0b00000000, 0b11110000, 0b11111111, 
	0, 1, 1, 1}
};

const volatile animation_t openSign = {
	.frameLength = 24,
	.frameNum = 19,
	.frames = {0b00000000, 0b00000000, 0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b11111111,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1}
};

const volatile animation_t * animRay[] = {&simpleBlink, &oregonSign, &openSign};

volatile unsigned char lastButtonState = 0;
volatile unsigned char pressTime = 0;

volatile unsigned char animationCycle = 0;
volatile unsigned char animationState = 0;
volatile unsigned char animationPoint = 0;

volatile unsigned char ledNum = 0;

volatile unsigned char clkDiv = 0;

static inline void sendLEDs(const unsigned char data){
	//pulse clock and data eight times
	for(uint8_t i = 0; i < 8; i++){
		//set data pin
		if(data & (1 << i)) PORTB |= (1 << SOUT);
		else PORTB &= ~(1 << SOUT);
		//clock high
		PORTB |= (1 << CLK);
		//clock low
		PORTB &= ~(1 << CLK);
	}
	//disable data pin
	PORTB &= ~(1 << SOUT);
	//pulse latch
	PORTB |= (1 << LATCH);
	PORTB &= ~(1 << LATCH);
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
	DDRB |= (1 << NOSE) | (1 << SOUT) | (1 << CLK) | (1 << LATCH);
	PORTB |= (1 << BUTTON); //button pin pullup enabled
	
	//init timer0 interrupt
	GTCCR |= (1 << TSM); //pause timer
	TCCR0B |= (1 << CS01);  //clk/8
	TIMSK |= (1 << TOIE0); //enable timer overflow interrupt
	GTCCR &= ~(1 << TSM); //unpause timer

	setBrightness(32);

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
	PORTB &= ~(1 << NOSE);
}

ISR(TIMER0_OVF_vect){
	if(++clkDiv >= 8){
		clkDiv = 0;
		if(!(PINB & (1 << BUTTON)) && pressTime < 0xff) pressTime++; //if button reads pressed, inc varible
		else pressTime = 0;

		//if button is simply long pressed
		if(pressTime == LONGPRESS){
			//long press code
			uint8_t temp = getBrightness() + 32;
			if(temp >= 128) temp = 0;
			setBrightness(temp);
		}
		//else if button is not pressed, but it was pressed before (can't repeat)
		else if(lastButtonState > SHORTPRESS && lastButtonState < LONGPRESS && !pressTime){
			//short press code
			if(++animationPoint >= 3)
			animationPoint = 0;
			animationState = 0;
			animationCycle = 254;
		}

		lastButtonState = pressTime;
		
		if(getBrightness()){
			//cycle animation wait
			//if time for next frame in animation
			if(++animationCycle >= animRay[animationPoint]->frameLength){
				animationCycle = 0;
				//increment frames
				if(++animationState >= animRay[animationPoint]->frameNum) animationState = 0;
			}
		}
	}
	
	//if brightness > 0
	if(getBrightness()){
		//write leds, but one at a time
		sendLEDs(animRay[animationPoint]->frames[animationState] & (1 << ledNum));
		if(++ledNum >= 8) ledNum = 0;
		//toggle nose led
		if(animRay[animationPoint]->frames[animationState + animRay[animationPoint]->frameNum]) PORTB |= (1 << NOSE);
		else PORTB &= ~(1 << NOSE);
	}
	
}