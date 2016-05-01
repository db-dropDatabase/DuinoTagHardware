#include "IRremote.h"
#include "IRremoteInt.h"

//+=============================================================================
IRrecv::IRrecv (int recvpin)
{
	irparams.recvpin = recvpin;
}

IRrecv::IRrecv (int recvpin, int blinkpin)
{
	irparams.recvpin = recvpin;
}



//+=============================================================================
// initialization
//
void  IRrecv::enableIRIn ( )
{
	//cli();
	// Setup pulse clock timer interrupt
	// Prescale /8 (16M/8 = 0.5 microseconds per tick)
	// Therefore, the timer interval can range from 0.5 to 128 microseconds
	// Depending on the reset value (255 to 0)
	TIMER_CONFIG_NORMAL();

	// Timer2 Overflow Interrupt Enable
	TIMER_ENABLE_INTR;

	TIMER_RESET;

	//sei();  // enable interrupts

	// Initialize state machine variables
	irparams.rcvstate = STATE_IDLE;
	irparams.rawlen = 0;
	// Set pin modes
	//pinMode(irparams.recvpin, INPUT);
}

//+=============================================================================
// Return if receiving new IR signals
// 
bool  IRrecv::isIdle ( ) 
{
 return (irparams.rcvstate == STATE_IDLE || irparams.rcvstate == STATE_STOP) ? true : false;
}
//+=============================================================================
// Restart the ISR state machine
//
void  IRrecv::resume ( )
{
	irparams.rcvstate = STATE_IDLE;
	irparams.rawlen = 0;
}
//+=============================================================================
// Pass through finished decoded data to something that transmits it
//
void IRrecv::onFinish(void (*function)(uint32_t, uint8_t)){
	irparams.onFinishPtr = function;
}

