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

const uint16_t TICK_LEN = 500;
const uint8_t DIM_RES = 50;

void setLED(uint8_t LEDnum); //to be made later on

typedef struct {
	uint16_t delaySetting;
	uint8_t powerSetting;
	uint8_t scaleSetting;
	uint8_t dimSetting;
	uint8_t dimTime;
	uint8_t dimCurrentTime;
	uint8_t stepCounter;
}LEDState_t;

LEDState_t LEDStates[animationNum] = {};
volatile uint8_t LEDPower[2][animationNum] = {};
volatile uint8_t swap = 0;
volatile uint8_t queuePointer = 0; 

int main(void)
{
	//internet code, to be modified
	TCCR0A = (1 << WGM01);             //CTC mode
	TCCR0B = (1 << CS01);              //div8
	OCR0A = TICK_LEN;						   // 50us compare value
	TIMSK |= (1<<OCIE0A);			   // enable timer0 interrupt

	//set all pins to output
	DDRB = 0b011111;

	//set default LED States
	for(uint8_t i=0; i<animationNum; i++) LEDStates[i].scaleSetting = DIM_RES;

    while (1) 
    {
		//cli();

		uint8_t tempSwap = swap;

		//reset queue
		for (uint8_t i=0; i<animationNum; i++) LEDPower[tempSwap][i] = 0;

		//for every animation
		for(uint8_t i=0; i<animationNum; i++){
			//keep processing state machine until there is delay
			while(!LEDStates[i].delaySetting){
				//cache progmem values
				const uint8_t currentStep = pgm_read_byte(&animationStore[i][LEDStates[i].stepCounter]);
				const uint8_t nextStep = pgm_read_byte(&animationStore[i][LEDStates[i].stepCounter + 1]);

				if(currentStep == L_SET_POWER){
					//parse special cases for numbers
					if(nextStep == N_RAND) LEDStates[i].powerSetting = returnRandom(LEDStates[i].scaleSetting);
					else if(nextStep == N_RAND_5) LEDStates[i].powerSetting = returnRandom(5);
					else LEDStates[i].powerSetting = nextStep;
				}

				else if(currentStep == L_SET_DIM){
					//set the dim setting
					if(nextStep >= D_DIM_OFF) LEDStates[i].dimSetting = nextStep;
					else LEDStates[i].dimTime = nextStep;
				}

				else if(currentStep == L_SET_SCALE){
					//set the scale
					LEDStates[i].scaleSetting = nextStep;
				}

				else if(currentStep == L_DELAY){
					//set the delay w/ special number cases
					//if its a random delay, set delay to random*converstion factor
					if(nextStep == N_RAND) LEDStates[i].delaySetting = returnRandom(DIM_RES) * (1000 / TICK_LEN);
					else if(nextStep == N_RAND_5) LEDStates[i].delaySetting = returnRandom(5) * 2; //fix this
					//else set the delay to the next value times the conversion factor
					else LEDStates[i].delaySetting = nextStep * (1000 / TICK_LEN);
				}

				//increment to next step
				LEDStates[i].stepCounter += 2;
				//check to make sure the animation hasn't ended, and if it has reset it
				if(LEDStates[i].stepCounter >= sizeof(animationStore[i])) LEDStates[i].stepCounter = 0;
			}

			//process dimming
			//if dimming timer is up
			if(!LEDStates[i].dimCurrentTime && LEDStates[i].dimSetting){
				//TODO: swap to if:else
				switch(LEDStates[i].dimSetting){
					case D_DIM_DEC:
						//don't dec if under zero
						if(LEDStates[i].powerSetting > 0) LEDStates[i].powerSetting--;
						else LEDStates[i].dimSetting = 0;
						break;
					case D_DIM_INC:
						//dont inc if over max val
						if(LEDStates[i].powerSetting < LEDStates[i].scaleSetting) LEDStates[i].powerSetting++;
						else LEDStates[i].dimSetting = 0;
						break;
					case D_DIM_CYCLE_DEC:
					//if can't dec, set to zero, then swap to inc
						if(LEDStates[i].powerSetting > 0) LEDStates[i].powerSetting--;
						else LEDStates[i].dimSetting = D_DIM_CYCLE_INC;
						break;
					case D_DIM_CYCLE_INC:
						//if can't inc, set to max, then swap to dec
						if(LEDStates[i].powerSetting < LEDStates[i].scaleSetting) LEDStates[i].powerSetting++;
						else LEDStates[i].dimSetting = D_DIM_CYCLE_DEC;
						break;
					default:
						LEDStates[i].dimSetting = 0;
						LEDStates[i].dimTime = 0;
						break;
					
				}

				LEDStates[i].dimCurrentTime = LEDStates[i].dimTime;
			}
			//else decrement dimming timer
			else if(LEDStates[i].dimTime) LEDStates[i].dimCurrentTime--;

			//set the next frames of the animation
			LEDPower[tempSwap][i] = LEDStates[i].powerSetting * (DIM_RES / LEDStates[i].scaleSetting);

			//dec. the delay
			LEDStates[i].delaySetting--;
		}

		//enable the interrupt (on the first run)
		sei(); 

		//wait until ISR empties queue
		while(tempSwap == swap);
    }
}

ISR(TIMER0_COMPA_vect){
	PORTB = 0;
	if(LEDPower[!swap][0] > queuePointer) PORTB |= (1 << PB0);
	if(LEDPower[!swap][1] > queuePointer) PORTB |= (1 << PB1);
	if(LEDPower[!swap][2] > queuePointer) PORTB |= (1 << PB2);

	queuePointer++;

	if(queuePointer >= DIM_RES){
		queuePointer = 0;
		swap = !swap;
	}
}