/*
 * OneWire.c
 *
 * Created: 6/19/2016 9:49:09 AM
 *  Author: Noah
 */ 
#ifndef ONEWIRE_C_
#define ONEWIRE_C_

#include "OneWire.h"

volatile bool finished;
volatile char charBuf = '\0';
volatile uint8_t charPlace = 0;
volatile char * stringBuf = (volatile char *)"\0\0\0\0";
volatile uint8_t ticks = 0;
volatile bool timingMark;
volatile uint8_t tickStore[100] = {};
volatile uint8_t tickPlace = 0;

void OWSetup(bool receive){
	if(receive) {
		DREG &= ~(1 << PIN);
		REG |= (1 << PIN);
		//timer stuff
		GTCCR |= (1 << TSM); //temp. disable timer
		TCCR1 |= (1 << CS12) | (1 << CS11) | (1 << CTC1); // f/32 prescale
		OCR1C = (TICK_LEN/4)-1;
		OCR1A = (TICK_LEN/4)-1;
		GTCCR &= ~(1 << TSM); //reset timer
		//pin change stuff
		GIMSK |= (1 << PCIE);
		OWSetPinChange(true);
	}
	else{
		DREG |= (1 << PIN);
	}
}

void OWSetTimer(bool on){
	if(on) {
		GTCCR |= (1 << PSR0);
		TCNT1 = 0; //reset timer
		TIMSK |= (1 << OCIE1A); //enable intr.

	}
	else TIMSK &= ~(1 << OCIE1A);
}

void OWSetPinChange(bool on){
	if(on) PCMSK |= (1 << PIN);
	else PCMSK &= ~(1 << PIN);
}

uint8_t OWConvert(uint8_t iTicks){
	tickStore[tickPlace] = iTicks;
	tickPlace++;

	if(iTicks >= HEAD-2) return HEAD;
	else if(iTicks >= ONE-2) return ONE;
	else if(iTicks >= ZERO-2) return ZERO;
	else return 0;
}

void OWSend(const char * string){
    REG |= (1 << PIN);
	_delay_us(HEAD * TICK_LEN);
	REG &= ~(1 << PIN);
	_delay_us(SPACE * TICK_LEN);
	for(uint8_t i=0; i<strlen(string); i++){
		for(uint8_t o=0; o<8; o++){
			REG |= (1 << PIN);
			if(string[i] & (1 << o))
			_delay_us(ONE * TICK_LEN);
			else
			_delay_us(ZERO * TICK_LEN);
			REG &= !(1 << PIN);
			_delay_us(SPACE * TICK_LEN);
		}
	}
}

void OWCheckRecv(char * data){
   if(finished && strlen((const char *)stringBuf) > 0){
        strcpy(data, (const char *)stringBuf);
        stringBuf[0] = '\0';
	    charPlace = 0;
        charBuf = '\0';
		finished = false;
	}
	else data[0] = '\0';
}

ISR(PIN_INT_VECT){
	if(IN_REG & (1 << PIN) && !finished){
		OWSetPinChange(false);
		OWSetTimer(true);
	}
}

ISR(TIM_INT_VECT){
	if(!timingMark && IN_REG & (1 << PIN)){
		timingMark = true;
		ticks = 0;
	}
	else if(timingMark && IN_REG & (1 << PIN)){
		ticks++;
	}
	else if(timingMark && !(IN_REG & !(1 << PIN))){
		switch(OWConvert(ticks)){
			case ONE:
			charBuf |= (1 << charPlace);
			charPlace++;
			break;
			case ZERO:
			charBuf &= ~(1 << charPlace);
			charPlace++;
			break;
			default:
			break;
		}
		if(charPlace >= 8){
			const char tempString[2] = {charBuf, '\0'};
			stringBuf = (volatile char *)strcat((char *)stringBuf, (const char *)tempString);
			if(strlen((const char *)stringBuf) >= 3) finished = true;
			charBuf = '\0'; //cast much?
            charPlace = 0;
		}
		timingMark = false;
		ticks = 0;
	}
	else if(strlen((const char *)stringBuf) != 0 && !finished){
		if(ticks > HEAD) finished = true;
		else ticks++;
	}

	if(finished){
		OWSetTimer(false);
		OWSetPinChange(true);
	}
}

#endif