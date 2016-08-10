/*
 * OneWire.c
 *
 * Created: 6/19/2016 9:49:09 AM
 *  Author: Noah
 */ 
#ifndef ONEWIRE_C_
#define ONEWIRE_C_

#define F_CPU 8000000L //this is incorrect, and all values in library are adjusted to correct for it
//please do not try to use this library in another project w/o changing this value to 2x the F_CPU
//honestly don't even bother with this library, it isn't worth it

#include "OneWire.h"

void OWSetup(bool receive){
	if(receive) {
		DREG &= ~(1 << PIN);
		REG |= (1 << PIN);
		//pin change stuff
		GIMSK |= (1 << PCIE);
		PCMSK |= (1 << PIN);
	}
	else{
		DREG |= (1 << PIN);
	}
}

bool OWConvert(uint8_t iTicks){
	if(iTicks >= HEAD-2) return 0; //head
	else if(iTicks >= ONE-2) return 1; //one
	else if(iTicks >= ZERO-2) return 0; //zero
	else return 0; //???
}

ISR(PCINT0_vect){
	if(!(IN_REG & (1 << PIN))){
		//transmission incoming

		uint8_t charPlace = 0;
		uint8_t stringPlace = 0;
		char stringBuf[4] = {'\0', '\0', '\0', '\0'};
		uint8_t ticks = 0;
		bool lastState = false;

		//wait until header is done, to sync timing
		//while(IN_REG & (1 << PIN));

		while(stringPlace < 4){
			
			if(!(IN_REG & (1 << PIN)) == lastState) ticks++;

			else {
				if(lastState){ //mark
					stringBuf[stringPlace] |= (OWConvert(ticks) << charPlace);
					charPlace++;
					if(charPlace >= 8){
						stringPlace++;
						charPlace = 0;
					}
				}
				if(ticks > HEAD) break; //EOF

				debugLog[debugPlace] = ticks;
				debugPlace++;
				if(debugPlace > sizeof(debugLog)) debugPlace = 0;

				ticks = 0;
				lastState = (IN_REG & (1 << PIN));
			}

			_delay_us(50); //one tick, corrected
		}

		if(stringPlace > 0) {
			strncpy(retString, stringBuf, 3);
			done = true;
		}
	}
}

#endif