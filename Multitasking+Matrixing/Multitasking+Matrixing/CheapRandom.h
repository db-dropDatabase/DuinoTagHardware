/*
 * CheapRandom.h
 *
 * Created: 9/28/2016 4:39:38 PM
 *  Author: Noah
 *
 * Random numbers, doom style!
 * If you don't know what I'm talking about please google it
 */ 

#ifndef CHEAPRANDOM_H_
#define CHEAPRANDOM_H_

#include <avr/pgmspace.h>

//define the maximum number in the generation
#define CHEAP_RAND_MAX 50

extern const uint8_t PROGMEM cheapRandom[];

extern uint8_t randPoint;

extern uint8_t returnRandom(uint8_t mod);
//returns between 1-CHEAP_RAND_MAX

#endif /* CHEAPRANDOM_H_ */