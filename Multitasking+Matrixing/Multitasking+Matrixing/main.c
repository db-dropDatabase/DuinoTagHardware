/*
 * Multitasking+Matrixing.c
 *
 * Created: 9/25/2016 4:22:45 PM
 * Author : Noah
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "CheapRandom.h"
#include "BasicPatterns.h"

#define F_CPU 8000000L

void setLED(uint8_t LEDnum); //to be made later on


typedef struct {
	uint16_t delaySetting;
	uint8_t powerSetting;
	uint8_t stepCounter;
}LEDState_t;

LEDState_t LEDStates[ANIMATION_NUM] = {};
uint8_t LEDqueue[2][ANIMATION_NUM] = {};
volatile uint8_t queueSwap = 0;
volatile uint8_t queuePointer = 0; 

int main(void)
{
	//Timer ISR stuff
	TCCR0A = (1 << WGM01);             //CTC mode
	TCCR0B = (1 << CS01);              //div8
	OCR0A = TICK_LEN;						   // 50us compare value
	TIMSK |= (1<<OCIE0A);			   // enable timer0 interrupt

	//set all pins to output
	DDRB = 0b011111;

    while (1) 
    {
		//cli();

		uint8_t tempQueueSwap = queueSwap;

		//reset queue
		 for(uint8_t i=0; i<ANIMATION_NUM; i++) LEDqueue[tempQueueSwap][i] = 0;

		//for every animation
		for(uint8_t i=0; i<ANIMATION_NUM; i++){
			//if the animation is not in the middle of delaying, process the state machine
			if(!LEDStates[i].delaySetting){
				while(animationStore[i][LEDStates[i].stepCounter] != L_DELAY){
					if(animationStore[i][LEDStates[i].stepCounter] == L_SET_POWER){
						//if its a flicker random, set power to the random flicker
						if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND_FLICKER) LEDStates[i].powerSetting = returnRandomFlicker();
						//if its a regular random, set power to random
						else if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].powerSetting = returnRandom();
						//else set power to next item in animation
						else LEDStates[i].powerSetting = animationStore[i][LEDStates[i].stepCounter + 1];
						//increment to next step
						LEDStates[i].stepCounter += 2;
					}

					//insert other instructions here

					//check to make sure the animation hasn't ended, and if it has reset it
					if(LEDStates[i].stepCounter >= sizeof(animationStore[i])) LEDStates[i].stepCounter = 0;
				}

				//the next step is now guaranteed to be a delay, so process that
				//if its a flicker random, set power to the random flicker
				if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND_FLICKER) LEDStates[i].delaySetting = returnRandomFlicker() * (100000 / (TICK_LEN * DIM_RES));
				//if its a random delay, set delay to random*converstion factor
				else if(animationStore[i][LEDStates[i].stepCounter + 1] == L_RAND) LEDStates[i].delaySetting = returnRandom() * (100000 / (TICK_LEN * DIM_RES));
				//else set the delay to the next value times the conversion factor
				else LEDStates[i].delaySetting = animationStore[i][LEDStates[i].stepCounter + 1] * (100000 / (TICK_LEN * DIM_RES));
				//increment to next step
				LEDStates[i].stepCounter += 2;
				//check to make sure the animation hasn't ended, and if it has reset it
				if(LEDStates[i].stepCounter >= sizeof(animationStore[i]) || animationStore[i][LEDStates[i].stepCounter] == 0) LEDStates[i].stepCounter = 0;
			}

			//set the next frames of the animation
			LEDqueue[tempQueueSwap][i] = LEDStates[i].powerSetting;
			//dec. the delay
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
	for(uint8_t i=0; i<ANIMATION_NUM; i++){
		if(LEDqueue[queueSwap][i] <= queuePointer) PORTB |= (1 << pinRay[i]);
	}

	queuePointer++;

	if(queuePointer >= DIM_RES){
		queuePointer = 0;
		queueSwap = !queueSwap;
	}
}