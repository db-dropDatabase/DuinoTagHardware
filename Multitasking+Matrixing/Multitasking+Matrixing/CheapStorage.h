/*
 * CheapStorage.h
 *
 * Created: 9/28/2016 8:41:00 PM
 *  Author: Noah
 *
 * Cheap way of creating a 3D bool array
 * without using 8 bits per bool
 */ 


#ifndef CHEAPSTORAGE_H_
#define CHEAPSTORAGE_H_

#include <stdint-gcc.h>

//define arrays length and width, area cannot be greater than storage type
//(e.g. 3x10 = 30 bits, which is less than the 32 a uint32_t has)
#define ARRAY_X 3
//the value below does not actually matter, but should still be adheared too
#define ARRAY_Y 10

//get an element out of a cheap array
//will only return a 1 or 0
extern uint8_t getElement(volatile const uint32_t *cont, uint8_t x, uint8_t y);

//set an element in a cheap array
extern void setElement(volatile uint32_t *cont, uint8_t x, uint8_t y, uint8_t set);

#endif /* CHEAPSTORAGE_H_ */