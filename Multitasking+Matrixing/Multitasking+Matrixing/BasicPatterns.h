/*
 * BasicPatterns.h
 *
 * Created: 9/25/2016 4:23:26 PM
 *  Author: Noah
 *
 * This file will detail the syntax for creating simple lighting patterns
 * And hopefully have a way to display them all at once
 */
#ifndef BASICPATTERNS_H_
#define BASICPATTERNS_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

//TODO: add complier based number checking
//or better comments

#define animationNum (sizeof(animationStore) / sizeof(animationStore[0]))

typedef const uint8_t animation_t;

//below are commands understood by the interpreter
enum lightCommands{
	//below are commands for odd slots
	L_SET_SCALE = 1, //set the scale for L_SET_POWER to run off of, do not use random (please)
	L_SET_POWER = 2, //set light power, from 0 - scale max
	L_SET_DIM = 3, //set dimming type, do not use random
	L_DELAY = 4,  //add a delay in the animation, in tens of milliseconds
	//below are commands for even slots
	N_RAND = 254, //replace with a random number above zero
	N_RAND_5 = 253, //replace witha random number between 1-10
	D_DIM_INC = 252, //have the atate machine inc the power every tick, creating a dimming effect
	D_DIM_DEC = 251, //same as L_DIM_INC, but in reverse
	D_DIM_CYCLE_INC = 250, //dim continiously starting up
	D_DIM_CYCLE_DEC = 249, //dim continiously, starting down
};

animation_t PROGMEM animationStore[][50] = { //array which stores the animations
	{
		L_SET_POWER, 50,  //turn the LED on
		L_DELAY, 100,  //delay for 100 millis
		L_SET_POWER, N_RAND, //set the power to a random number between 0 and 1
		L_DELAY, 100, //delay for another 100 millis
		L_SET_POWER, 0,  //turn the LED off
		L_DELAY, 100,  //delay for 100 millis
	}, //and repeat
	{
		L_SET_SCALE, 10,
		L_SET_POWER, N_RAND,
		L_DELAY, N_RAND_5,
		L_SET_POWER, N_RAND_5,
		L_DELAY, N_RAND_5,
	},
	{
		L_SET_DIM, D_DIM_CYCLE_INC,
		L_DELAY, 200,
	},
	//insert as many as you like here
};

#endif /* BASICPATTERNS_H_ */