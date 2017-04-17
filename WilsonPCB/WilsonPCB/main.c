/*
 * WilsonPCB.c
 *
 * Created: 1/6/2017 8:34:29 AM
 * Author : Noah
 */ 

#define F_CPU 1000000UL
#define BUTTON PB4
#define SOUT PB1
#define CLK PB2
#define LATCH PB3
#define NOSE PB0

#define SHORTPRESS 0x05
#define LONGPRESS 0x30

#define BRIGHT_INC 0x40 //should be a divisor of 255 if you want it to wrap around at 0x00

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

typedef struct _animation {
	unsigned char frameLength;
	unsigned char frameNum;
	unsigned char frames[]; //formatted as such {frame, frame, noseLED, noseLED}
}animation_t;

const volatile animation_t simpleBlink = {
	.frameLength = 24,
	.frameNum = 2,
	.frames = {0b10101010, 0b01010101,
	0, 1}
};

const volatile animation_t oregonSign = {
	.frameLength = 24,
	.frameNum = 4,
	.frames = {0b00000000, 0b00000000, 0b11110000, 0b11111111, 
	0, 1, 1, 1}
};

const volatile animation_t openSign = {
	.frameLength = 24,
	.frameNum = 19,
	.frames = {0b00000000, 0b00000000, 0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b11111111,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1}
};

const volatile animation_t alwaysOn = {
	.frameLength = 255,
	.frameNum = 1,
	.frames = {0b11111111, 1}
};

const volatile animation_t * animRay[] = {&simpleBlink, &oregonSign, &openSign, &alwaysOn};

volatile unsigned char lastButtonState = 0;
volatile unsigned char pressTime = 0;

volatile unsigned char animationCycle = 0;
volatile unsigned char animationState = 0;
volatile unsigned char animationPoint = 0;

volatile unsigned char clkDiv = 0;

static inline void sendLEDs(const unsigned char data){
	//Bitbang version, takes moar instructions
	//pulse clock and data eight times
	for(uint8_t i = 0; i < 8; i++){
		//set data pin
		if(data & (1 << i)) PORTB |= (1 << SOUT);
		else PORTB &= ~(1 << SOUT);
		//clock high
		PORTB |= (1 << CLK);
		//clock low
		PORTB &= ~(1 << CLK);
	}
	//disable data pin
	PORTB &= ~(1 << SOUT);

	//pulse latch
	PORTB |= (1 << LATCH);
	PORTB &= ~(1 << LATCH);
}

static inline void setBrightness(const volatile unsigned char brigntness){
	if(brigntness == 0){
		TIMSK &= ~(1 << OCIE0A); //disable brightness interrupt
		OCR0A = 0; //erase brightness value
		sendLEDs(0b00000000); //turn off leds
		PORTB &= ~(1 << NOSE);
	}
	else if(brigntness == 0xff) {
		TIMSK &= ~(1 << OCIE0A); //disable brightness interrupt
	}
	else{
		OCR0A = brigntness;  //store brightness value
		TIMSK |= (1 << OCIE0A); //enable brightness interrupt
	}
	
}

static inline unsigned char getBrightness(){ return OCR0A; }

int main(void)
{
	//disable WDT just in case
	if(MCUSR & (1 << WDRF)){
		MCUSR = 0;
		wdt_disable();
	}

	//disable interrupts during setup
	cli();
	
	//init sleep stuff
	MCUCR = (1 << SE);
	PRR = (1 << PRTIM1) | (1 << PRUSI) | (1 << PRADC);  //disable timer1, USI, and ADC
	ACSR = (1 << ACD); //power off the ADC some more

	//init ports
	DDRB = (1 << NOSE) | (1 << SOUT) | (1 << CLK) | (1 << LATCH);
	PORTB = (1 << BUTTON); //button pin pullup enabled
	
	//init timer0 interrupt
	GTCCR = (1 << TSM); //pause timer
	TCCR0B = (1 << CS01);  //clk/8
	TIMSK = (1 << TOIE0); //enable timer overflow interrupt
	GTCCR = 0; //unpause timer

	//init pin change interrupt
	PCMSK = (1 << BUTTON);

	//init brightness
	setBrightness(BRIGHT_INC);

	//reenable interrupts
	sei();
	
    while (1) 
    {
		//enable BOD disabling during sleep
		MCUCR |= (1 << BODS) | (1 << BODSE);
		MCUCR |= (1 << BODS);
		MCUCR &= ~(1 << BODSE);

		//sleep, cuz
		sleep_cpu();
    }
}

//isr to control brightness of LEDs
ISR(TIMER0_COMPA_vect){
	//turn off LEDs for brightness
	sendLEDs(0b00000000);
	PORTB &= ~(1 << NOSE);
}

ISR(TIMER0_OVF_vect){
	if(++clkDiv == 8){
		clkDiv = 0;

		//button stuff
		if(!(PINB & (1 << BUTTON))){
			if(pressTime <= LONGPRESS) pressTime++; //if button reads pressed, inc variable
		}
		else pressTime = 0;

		//if button is simply long pressed
		if(pressTime == LONGPRESS){
			//long press code
			setBrightness(getBrightness() + BRIGHT_INC);
		}
		//else if button is not pressed, but it was pressed before (can't repeat)
		else if(lastButtonState > SHORTPRESS && lastButtonState < LONGPRESS && !pressTime){
			//short press code
			if(++animationPoint == 4)
				animationPoint = 0;
			animationState = 0;
			animationCycle = 0;
		}
		else{
			//cycle animation wait
			//if time for next frame in animation
			if(++animationCycle >= animRay[animationPoint]->frameLength){
				animationCycle = 0;
				//increment frames
				if(++animationState >= animRay[animationPoint]->frameNum) animationState = 0;
			}
		}
		//remember button state
		lastButtonState = pressTime;
	}

	//if brightness is still on (it could be changed during loop)
	if(getBrightness()){
		//write leds, but one at a time
		sendLEDs(animRay[animationPoint]->frames[animationState] & (1 << clkDiv));
		//toggle nose led
		if(animRay[animationPoint]->frames[animationState + animRay[animationPoint]->frameNum] && !clkDiv) PORTB |= (1 << NOSE);
		else PORTB &= ~(1 << NOSE);
	}

	//if no brightness, activate sleeping and waiting for pin change (SUPER POWER SAVE)
	else{
		//reset animations
		animationState = 0;
		animationCycle = 0;
		clkDiv = 0;

		//disable timer0 for wdt poweroff
		PRR |= (1 << PRTIM0);

		//enable global pin change
		GIMSK = (1 << PCIE);

		//set sleep mode to PWR_DOWN
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	}
}

//initial super sleep button detect isr
//triggers watchdog isr, which debounces the input
//all to save all the power
ISR(PCINT0_vect){
	//if button down
	if(!(PINB & (1 << BUTTON))){
		//activate watchdog sleep debounce
		WDTCR = (1 << WDCE) | (1 << WDE);
		//set wdt to interrupt and prescale to 0.5s
		WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP0);
		//self disable pin change
		GIMSK = 0;
	}
}

//wdt super sleep debouching isr
//waits 0.5s before checking button input to see if still down
//in deep sleep of course
ISR(WDT_vect){
	//if button down
	if(!(PINB & (1 << BUTTON))){
		//turn on timer
		PRR &= ~(1 << PRTIM0);
		//turn on lights
		setBrightness(BRIGHT_INC);
		//reset sleep mode
		set_sleep_mode(SLEEP_MODE_IDLE);
	}
	//else go back to pin change
	else GIMSK = (1 << PCIE);

	//self disable
	wdt_disable();
}