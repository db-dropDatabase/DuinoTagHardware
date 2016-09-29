/*
* CheapStorage.c
*
* Created: 9/28/2016 8:47:20 PM
*  Author: Noah
*/
#include "CheapStorage.h"

inline uint8_t getElement(volatile const uint32_t *cont, uint8_t x, uint8_t y){
	return *cont & (1 << ((ARRAY_X * y) + x));
}

void setElement(volatile uint32_t *cont, uint8_t x, uint8_t y, uint8_t set){
	if(set) *cont |= (1 << ((ARRAY_X * y) + x));
	else *cont &= ~(1 << ((ARRAY_X * y) + x));
}