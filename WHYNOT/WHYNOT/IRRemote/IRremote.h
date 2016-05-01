
//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
// Edited by Mitra to add new controller SANYO
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
//******************************************************************************

#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
// The ISR header contains several useful macros the user may wish to use
//
#include <stdint-gcc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "IRremoteInt.h"

//------------------------------------------------------------------------------
// Set DEBUG to 1 for lots of lovely debug output
//
#define DEBUG  0

//------------------------------------------------------------------------------
// Debug directives
//
#if DEBUG
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int  MATCH       (int measured, int desired) ;
int  MATCH_MARK  (int measured_ticks, int desired_us) ;
int  MATCH_SPACE (int measured_ticks, int desired_us) ;

//------------------------------------------------------------------------------
// Results returned from the decoder
//
class decode_results
{
	public:
		uint32_t				   value;        // Decoded value [max 32-bits]
		uint8_t                    bits;         // Number of bits in decoded value
};

//------------------------------------------------------------------------------
// Decoded value for NEC when a repeat code is received
//
#define REPEAT 0xFFFFFFFF

//------------------------------------------------------------------------------
// Main class for receiving IR
//
class IRrecv
{
	public:
		IRrecv (int recvpin) ;
		IRrecv (int recvpin, int blinkpin);

		void  enableIRIn ( ) ;
		bool  isIdle     ( ) ;
		void  resume     ( ) ;
		void  onFinish(void (*function)(uint32_t, uint8_t));
} ;

//------------------------------------------------------------------------------
// Main class for sending IR
//
class IRsend
{
	public:
		IRsend () { }

		void  custom_delay_usec (unsigned long uSecs);
		void  enableIROut 		(int khz) ;
		void  mark        		(unsigned int usec) ;
		void  space       		(unsigned int usec) ;
		void  sendRaw     		(const unsigned int buf[],  unsigned int len,  unsigned int hz) ;

		//......................................................................
#		if SEND_RC5
			void  sendRC5        (unsigned long data,  int nbits) ;
#		endif
#		if SEND_RC6
			void  sendRC6        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_NEC
			void  sendNEC        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SONY
			void  sendSony       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_PANASONIC
			void  sendPanasonic  (unsigned int address,  unsigned long data) ;
#		endif
		//......................................................................
#		if SEND_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
			void  sendJVC        (unsigned long data,  int nbits,  bool repeat) ;
#		endif
		//......................................................................
#		if SEND_SAMSUNG
			void  sendSAMSUNG    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_WHYNTER
			void  sendWhynter    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_AIWA_RC_T501
			void  sendAiwaRCT501 (int code) ;
#		endif
		//......................................................................
#		if SEND_LG
			void  sendLG         (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SANYO
			void  sendSanyo      ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_MISUBISHI
			void  sendMitsubishi ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_DISH
			void  sendDISH       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SHARP
			void  sendSharpRaw   (unsigned long data,  int nbits) ;
			void  sendSharp      (unsigned int address,  unsigned int command) ;
#		endif
		//......................................................................
#		if SEND_DENON
			void  sendDenon      (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_PRONTO
			void  sendPronto     (char* code,  bool repeat,  bool fallback) ;
#		endif
} ;

//YEAH MODIFYING CODE
uint8_t decodePulse(unsigned int pulseLength);

#endif
