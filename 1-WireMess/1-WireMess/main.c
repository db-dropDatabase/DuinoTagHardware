/*
 * 1-WireMess.c
 *
 * Created: 6/11/2016 5:20:12 PM
 * Author : Noah
 */

 #define F_CPU 8000000L //this is incorrect, and all values in library are adjusted to correct for it
 //please do not try to use this library in another project w/o changing this value to 2x the F_CPU
 //honestly don't even bother with this library, it isn't worth it

#include <avr/io.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <util/delay.h>
#include "OneWire.h"
#include "BasicSerial.h"

#define CS PB1

char retString[4] = {'\0', '\0', '\0', '\0'}; //also used in OneWire
volatile uint8_t debugLog[(4*8)+10] = {};
volatile uint8_t debugPlace = 0;
volatile bool done = false;

static inline void SPIInit(){
	DDRB &= ~(1 << PB2);
	USICR = (1 << USICS1) | (1 << USIWM0) | (1 << USIOIE);
	USISR = (1 << USIOIF);
	USIDR = 0;
}

void serOut(const char* str)
{
	while (*str) TxByte(*str++);
}

char* itoa(int i, char b[]){
	char const digit[] = "0123456789";
	char* p = b;
	if(i<0){
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

int main(void)
{
	cli();

    DDRB  = 0b111110 & ~(1 << CS);
    PORTB = 0b101001 & ~(1 << CS);

	OWSetup(true);

	sei();

	while(true){
		while(!done);
		//serOut((const char *) retString);
		//serOut("\n");
		for(uint8_t i=0; i<debugPlace; i++){
			char temp[3] = {};
			char* tempString = itoa(debugLog[i], temp);
			serOut(tempString);
			serOut(", ");
			debugLog[i] = 0;
		}
		serOut("\n");

		debugPlace = 0;
		
		done = false;
	}	
}
