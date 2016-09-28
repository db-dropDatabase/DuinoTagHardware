/*
 * Multitasking+Matrixing.c
 *
 * Created: 9/25/2016 4:22:45 PM
 * Author : Noah
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "BasicPatterns.h"

#define F_CPU 8000000L

const uint8_t TICK_LEN = 100;

void setLED(uint8_t LEDnum); //to be made later on

//TODO: split random into it's own file

const uint8_t cheapRandom[] = {
	10,6,5,1,3,3,8,10,5,6,3,4,5,7,3,5,8,5,8,4
};

uint8_t randPoint = 0;

uint8_t returnRandom(){
	randPoint++;
	if(randPoint == sizeof(cheapRandom)-1) randPoint = 0;
	return cheapRandom[randPoint];
}

bool returnRandomBool(){
	randPoint++;
	if(randPoint == sizeof(cheapRandom)-1) randPoint = 0;
	return cheapRandom[randPoint]%2;
}

//TODO: Replace 3D array with uint32_t

typedef struct {
	uint16_t delaySetting;
	uint8_t powerSetting;
	uint8_t stepCounter;
}LEDState_t;

LEDState_t LEDStates[animationNum] = {};
volatile bool LEDQueue[2][animationNum][10] = {};
volatile bool queueSwap = false;
volatile uint8_t queuePointer = 0; 

int main(void)
{
	//internet code, to be modified
	TCCR0A = (1 << WGM01);             //CTC mode
	TCCR0B = (1 << CS01);              //div8
	OCR0A = 100;						   // 50us compare value
	TIMSK |= (1<<OCIE0A);			   // enable timer0 interrupt

	//set all pins to output
	DDRB = 0b011111;

    while (1) 
    {
		//cli();

		bool tempQueueSwap = queueSwap;

		//reset queue
		for(uint8_t i=0; i<animationNum; i++) for(uint8_t o=0; o<10; o++) LEDQueue[queueSwap][i][o] = false;

		//for every animation
		for(uint8_t i=0; i<animationNum; i++){
			//if the animation is not in the middle of delaying, process the state machine
			if(!LEDStates[i].delaySetting){
				while(animationStore[i][LEDStates[i].stepCounter] != L_DELAY){
					if(animationStore[i][LEDStates[i].stepCounter] == L_SET_POWER){
						//if its a flicker random, set power to the random flicker
						if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND_FLICKER) LEDStates[i].powerSetting = returnRandomBool() * 10;
						//if its a regular random, set power to random
						else if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].powerSetting = returnRandom();
						//else set power to next item in animation
						else LEDStates[i].powerSetting = animationStore[i][LEDStates[i].stepCounter + 1];
						//increment to next step
						LEDStates[i].stepCounter += 2;

						//set the next ten frames of the animation
						for(uint8_t o=0; o<10; o++){
							if(LEDStates[i].powerSetting > o) LEDQueue[tempQueueSwap][i][o] = true;
						}
					}

					//insert other instructions here

					//check to make sure the animation hasn't ended, and if it has reset it
					if(LEDStates[i].stepCounter >= sizeof(animationStore[i])) LEDStates[i].stepCounter = 0;
				}

				//the next step is now guaranteed to be a delay, so process that
				//if its a random delay, set delay to random*converstion factor
				if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].delaySetting = returnRandom() * (1000 / TICK_LEN);
				//else set the delay to the next value times the conversion factor
				else LEDStates[i].delaySetting = animationStore[i][LEDStates[i].stepCounter + 1] * (1000 / TICK_LEN);
				//increment to next step
				LEDStates[i].stepCounter += 2;
				//check to make sure the animation hasn't ended, and if it has reset it
				if(LEDStates[i].stepCounter >= sizeof(animationStore[i]) || animationStore[i][LEDStates[i].stepCounter] == 0) LEDStates[i].stepCounter = 0;
			}
			//else if it is, dec. delay and copy last values (so the LED stays the same)
			else{
				for(uint8_t o=0; o<sizeof(LEDQueue[queueSwap][i]); o++) LEDQueue[queueSwap][i][o] = LEDQueue[!queueSwap][i][o];
			}
			//this always happens anyway
			LEDStates[i].delaySetting--;
		}

		//enable the interrupt (on the first run)
		sei(); 

		//wait until ISR empties queue
		while(queueSwap == tempQueueSwap);
    }
}

ISR(TIMER0_COMPA_vect){
	PORTB = 0;
	if(LEDQueue[!queueSwap][0][queuePointer]) PORTB |= (1 << PB0);
	if(LEDQueue[!queueSwap][1][queuePointer]) PORTB |= (1 << PB1);
	if(LEDQueue[!queueSwap][2][queuePointer]) PORTB |= (1 << PB2);

	queuePointer++;

	if(queuePointer >= 10){
		queuePointer = 0;
		queueSwap = !queueSwap;
	}
}