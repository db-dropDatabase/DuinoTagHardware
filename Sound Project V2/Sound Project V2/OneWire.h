/*
 * OneWire.h
 *
 * Created: 6/11/2016 5:20:31 PM
 *  Author: Noah
 */ 

//Single wire song selection transmission protocol for ultra-small music player

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

//#define F_CPU 8000000L

//#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint-gcc.h>
#include <string.h>

#define PIN PB3
#define DREG DDRB
#define REG PORTB
#define IN_REG PINB
#define TIM_INT_VECT TIMER1_COMPA_vect
#define PIN_INT_VECT PCINT0_vect

#define TICK_LEN 16

//# of ticks, so if a tick was 500us then a head trans. would be 8*500us long
#define HEAD 16
#define ONE 8
#define ZERO 4
#define SPACE 4
#define TICK 1

extern volatile bool finished;
extern volatile char charBuf;
extern volatile uint8_t charPlace;
extern volatile char * stringBuf;
extern volatile uint8_t ticks;
extern volatile bool timingMark;

extern void OWSetup(bool receive);
extern void OWSetTimer(bool on);
extern void OWSetPinChange(bool on);
extern uint8_t OWConvert(uint8_t ticks);
//extern void OWSend(const char * string);
extern bool OWCheckRecv(char * data);

#endif /* ONEWIRE_H_ */