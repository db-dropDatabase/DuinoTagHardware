/*
 * OneWire.h
 *
 * Created: 6/11/2016 5:20:31 PM
 *  Author: Noah
 */ 

//Single wire transmission protocol for ultra-small music player

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#define PIN PB2
#define REG PORTB

#define HEAD iTime*4
#define ONE iTime*2
#define ZERO iTime
#define SPACE iTime

#include <util/delay.h>
#include <string.h>

const uint16_t iTime;

static void OWsetup(uint16_t speed){
	iTime = speed;
}

static void OWSend(const char * string){
	for(uint8_t i=0; i<strlen(string); i++){
		REG |= (1 << PIN);
		_delay_ms(HEAD);
		REG &= ~(1 << PIN);
		_delay_ms(SPACE)

		for(uint8_t o=0; o<8; o++){
			REG |= (1 << PIN);

			if(string[i] & (1 << o))
				_delay_ms(ONE);
			else
				_delay_ms(ZERO);

			REG &= !(1 << PIN);
			_delay_ms(SPACE)
		}
	}
}


#endif /* ONEWIRE_H_ */