/*
 * Multitasking+Matrixing.c
 *
 * Created: 9/25/2016 4:22:45 PM
 * Author : Noah
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "BasicPatterns.h"

const uint8_t TICK_LEN = 100;

void setLED(uint8_t LEDnum); //to be made later on
uint8_t returnRandom(); //also to be made later
bool returnRandomBool(); //same 

struct LEDState_t {
	uint16_t delaySetting,
	uint8_t powerSetting,
	uint8_t stepCounter,
};

void* animationQueue[] = {
	&sample,
	&flicker,
	&dim
};

LEDState_t LEDStates[sizeof(animationQueue)] = {};
uint8_t LEDqueue[sizeof(animationQueue) * 10] = {};
uint8_t nextLEDQueue[sizeof(animationQueue) * 10] = {};

int main(void)
{
	//initialize ISR here

	uint8_t queueCounter = 0;

    while (1) 
    {
		//update virtual state machine
		for(uint8_t i=0; i<sizeof(animationQueue); i++){
			//if the animation is not in the middle of delaying, process the state machine
			if(!LEDStates[i].delaySetting){
				switch(animationQueue[i]->[LEDStates[i].stepCounter]){ //very complicated way of looking at the current step of the current animation
				case L_SET_POWER:
					//if its a flicker random, set power to the random flicker
					if(animationQueue[i]->[LEDStates[i].stepCounter + 1] == L_RAND_FLICKER) LEDStates[i].powerSetting = returnRandomBool() * 10;
					//if its a regular random, set power to random
					else if(animationQueue[i]->[LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].powerSetting = returnRandom();
					//else set power to next item in animation
					else LEDStates[i].powerSetting = animationQueue[i]->[LEDStates[i].stepCounter + 1];
					//increment to next step
					LEDStates[i].stepCounter += 2;
					break;
				case L_DELAY:
					//if its a random delay, set delay to random * converstion factor
					if(animationQueue[i]->[LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].delaySetting = returnRandom() * (10000000 / TICK_LEN);
					//else set the delay to the next value times the conversion factor
					else LEDStates[i].delaySetting = animationQueue[i]->[LEDStates[i].stepCounter + 1] * (10000000 / TICK_LEN);
					//increment to next step
					LEDStates[i].stepCounter += 2;
					break;
				}

				//check to make sure the animation hasn't ended, and if it has reset it
				if(LEDStates[i].stepCounter >= sizeof(animationQueue[i])) LEDStates[i].stepCounter = 0;
			}
		}

		//now parse the state information into which LEDs to turn on when
		for(uint8_t i=0; i<10; i++){
			for(uint8_t j=0; j<sizeof(animationQueue); j++){
				//if there is a delay setting, decrement it accordingly
				if(LEDStates[i].delaySetting){
					 LEDStates[i].delaySetting--;
					 //make sure the LED doesn't change
					 nextLEDQueue[queueCounter] = LEDqueue[queueCounter];
				}
				//if not, then if the dimmable setting is greater than the sub state
				//set it to turn on
				else if(LEDStates[i].powerSetting > i) nextLEDQueue[queueCounter] = j;
				//else set it to not be on
				else nextLEDQueue[queueCounter] = 0;

				//increment queue
				queueCounter++;
			}
		}

		//if queue is full, wait until ISR empties it
		while(queueCounter >= sizeof(nextLEDQueue));
    }
}

