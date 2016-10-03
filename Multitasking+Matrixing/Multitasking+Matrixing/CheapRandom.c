/*
* CheapRandom.c
*
* Created: 9/28/2016 4:49:36 PM
*  Author: Noah
*/

#include "CheapRandom.h"

uint8_t randPoint = 0;

const uint8_t PROGMEM cheapRandom[] = {
	82,	88,	86,
	61,	27,	72,
	68,	94,	23,
	7,	78,	46,
	35,	45,	37,
	45,	57,	82,
	85,	11,	55,
	60,	31,	31,
	45,	29,	28,
	80,	57,	91,
	48,	29,	25,
	40,	11,	35,
	21,	28,	72,
	96,	51,65,
	11,	86,	67,
	64,	34,	100,
	92,	41,
};

uint8_t returnRandom(){
	randPoint++;
	if(randPoint == sizeof(cheapRandom)-1) randPoint = 0;
	return cheapRandom[randPoint] % CHEAP_RAND_MAX + 1; //good enough
}

uint8_t returnRandomFlicker(){
	randPoint++;
	if(randPoint == sizeof(cheapRandom)-1) randPoint = 0;
	return cheapRandom[randPoint] % (CHEAP_RAND_MAX / 2);
}