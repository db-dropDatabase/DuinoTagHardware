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
volatile char stringBuf[4] = {'\0', '\0', '\0', '\0'};
volatile uint8_t ticks = 0;
volatile bool timingMark;

void OWSetup(bool receive){
	if(receive) {
		DREG &= ~(1 << PIN);
		REG |= (1 << PIN);
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
		GTCCR = (1 << TSM); //temp. disable timer
		PLLCSR = 0;
		OCR1B = 0;
		TCCR1 = (1 << CS12) | (1 << CS11) | (1 << CTC1); // f/32 prescale
		OCR1C = (TICK_LEN/4)-1;
		OCR1A = (TICK_LEN/4)-1;
		TIMSK = (1 << OCIE1A); //enable intr.
		GTCCR &= ~(1 << TSM); //reset timer
	}
	else {
		TIMSK = 0;
		TCCR1 = 0;
		OCR1A = 0;
		OCR1C = 0;
	}
}

void OWSetPinChange(bool on){
	if(on) PCMSK |= (1 << PIN);
	else PCMSK &= ~(1 << PIN);
}

uint8_t OWConvert(uint8_t iTicks){
	if(iTicks >= HEAD-2) return HEAD;
	else if(iTicks >= ONE-2) return ONE;
	else if(iTicks >= ZERO-2) return ZERO;
	else return 0;
}

bool OWCheckRecv(char * data){
	if(data[0] != '\0') return true;
	if(finished && strlen((const char *)stringBuf) > 0){
        strncpy(data, (const char *)stringBuf, 4);
        stringBuf[0] = '\0';
	    charPlace = 0;
        charBuf = '\0';
		finished = false;
		return true;
	}
	else return false;
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
			uint8_t i = 0;
			while(i < 3 && stringBuf[i] != '\0') i++;
			stringBuf[i] = charBuf;
			stringBuf[i+1] = '\0';
			if(i == 3) finished = true;

			charBuf = '\0'; 
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