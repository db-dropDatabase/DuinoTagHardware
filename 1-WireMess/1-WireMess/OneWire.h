/*
 * OneWire.h
 *
 * Created: 6/11/2016 5:20:31 PM
 *  Author: Noah
 */ 

//Single wire song selection transmission protocol for ultra-small music player

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint-gcc.h>
#include <string.h>

#define PIN PB3
#define DREG DDRB
#define REG PORTB
#define IN_REG PINB

#define TICK_LEN 16

//# of ticks, so if a tick was 500us then a head trans. would be 8*500us long
#define HEAD 16
#define ONE 8
#define ZERO 4
#define SPACE 4
#define TICK 1

extern char retString[4]; //declare this in your main
extern volatile bool done; //this too
extern volatile uint8_t debugLog[(4*8)+10];
extern volatile uint8_t debugPlace;

extern void OWSetup(bool receive);
extern bool OWConvert(uint8_t ticks);

#endif /* ONEWIRE_H_ */