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


#define animationNum (sizeof(animationStore) / sizeof(animationStore[0]))

typedef const uint8_t animation_t;

//below are commands understood by the interpreter
enum lightCommands{
	L_SET_POWER = 1, //set light power, from 0 - 10
	L_DELAY = 2,  //add a delay in the animation, in tens of milliseconds
	L_RAND = 255, //replace with a random number between 1 and 10
};

animation_t animationStore[][150] = { //array which stores the animations
	{
		L_SET_POWER, 10,  //turn the LED on
		L_DELAY, 100,  //delay for 100 millis
		L_SET_POWER, L_RAND, //set the power to a random number between 0 and 1
		L_DELAY, 100, //delay for another 100 millis
	}, //and repeat
	{
		L_SET_POWER, L_RAND,
		L_DELAY, L_RAND,
	},
	{
		L_SET_POWER, 50,
		L_DELAY, 10,
		L_SET_POWER, 45,
		L_DELAY, 10,
		L_SET_POWER, 40,
		L_DELAY, 10,
		L_SET_POWER, 35,
		L_DELAY, 10,
		L_SET_POWER, 30,
		L_DELAY, 10,
		L_SET_POWER, 25,
		L_DELAY, 10,
		L_SET_POWER, 20,
		L_DELAY, 10,
		L_SET_POWER, 15,
		L_DELAY, 10,
		L_SET_POWER, 10,
		L_DELAY, 10,
		L_SET_POWER, 9,
		L_DELAY, 10,
		L_SET_POWER, 8,
		L_DELAY, 10,
		L_SET_POWER, 7,
		L_DELAY, 10,
		L_SET_POWER, 6,
		L_DELAY, 10,
		L_SET_POWER, 5,
		L_DELAY, 10,
		L_SET_POWER, 4,
		L_DELAY, 10,
		L_SET_POWER, 3,
		L_DELAY, 10,
		L_SET_POWER, 2,
		L_DELAY, 10,
		L_SET_POWER, 1,
		L_DELAY, 10,
		L_SET_POWER, 0,
		L_DELAY, 10,
		L_SET_POWER, 2,
		L_DELAY, 10,
		L_SET_POWER, 3,
		L_DELAY, 10,
		L_SET_POWER, 4,
		L_DELAY, 10,
		L_SET_POWER, 5,
		L_DELAY, 10,
		L_SET_POWER, 6,
		L_DELAY, 10,
		L_SET_POWER, 7,
		L_DELAY, 10,
		L_SET_POWER, 8,
		L_DELAY, 10,
		L_SET_POWER, 9,
		L_DELAY, 10,
	},
	//insert as many as you like here
};

#endif /* BASICPATTERNS_H_ */