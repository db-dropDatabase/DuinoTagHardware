/*----------------------------------------------------------------------------/
/  8-pin SD audio player R0.05d                    (C)ChaN, 2011              /
/-----------------------------------------------------------------------------/
/ This project, program codes and circuit diagrams, is opened under license
/ policy of following trems.
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This project is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/
#define MODE 0
#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include "pff.h"
#include "xitoa.h"
#include "OneWire.h"

#define CS PB3

#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)	/* FourCC */

void delay_us (WORD);	/* Defined in asmfunc.S */

/*---------------------------------------------------------*/
/* Work Area                                               */
/*---------------------------------------------------------*/

volatile BYTE FifoRi, FifoWi, FifoCt;	/* FIFO controls */

BYTE Buff[256];		/* Wave output FIFO */

FATFS Fs;			/* File system object */
DIR Dir;			/* Directory object */
FILINFO Fno;		/* File information */

WORD rb;			/* Return value. Put this here to avoid avr-gcc's bug */

volatile bool quit = false; //declared in OneWire
bool loop = true;
volatile bool firstPlay = false;
volatile uint8_t debugCount = 0;
char filename[4] = {'\0', '\0', '\0', '\0'};


/*---------------------------------------------------------*/

void deselect (void);
BYTE rcv_spi (void);

static inline void new_init_spi(){
	USICR = (1 << USICS1);
}

static void release_spi (void)
{
	deselect();
	rcv_spi();
}

static
DWORD load_header (void)	/* 0:Invalid format, 1:I/O error, >=1024:Number of samples */
{
	DWORD sz, f;
	BYTE b, al = 0;


	if (pf_read(Buff, 12, &rb)) return 1;	/* Load file header (12 bytes) */

	if (rb != 12 || LD_DWORD(Buff+8) != FCC('W','A','V','E')) return 0;

	for (;;) {
		pf_read(Buff, 8, &rb);			/* Get Chunk ID and size */
		if (rb != 8) return 0;
		sz = LD_DWORD(&Buff[4]);		/* Chunk size */

		switch (LD_DWORD(&Buff[0])) {	/* Switch by chunk ID */
		case FCC('f','m','t',' ') :					/* 'fmt ' chunk */
			if (sz & 1) sz++;						/* Align chunk size */
			if (sz > 100 || sz < 16) return 0;		/* Check chunk size */
			pf_read(Buff, sz, &rb);					/* Get content */
			if (rb != sz) return 0;
			if (Buff[0] != 1) return 0;				/* Check coding type (LPCM) */
			b = Buff[2];
			if (b != 1 && b != 2) return 0;			/* Check channels (1/2) */
			GPIOR0 = al = b;						/* Save channel flag */
			b = Buff[14];
			if (b != 8 && b != 16) return 0;		/* Check resolution (8/16 bit) */
			GPIOR0 |= b;							/* Save resolution flag */
			if (b & 16) al <<= 1;
			f = LD_DWORD(&Buff[4]);					/* Check sampling freqency (8k-48k) */
			if (f < 8000 || f > 48000) return 4;
			OCR0A = (BYTE)(F_CPU / 8 / f) - 1;		/* Set sampling interval */
			break;

		case FCC('d','a','t','a') :		/* 'data' chunk */
			if (!al) return 0;							/* Check if format is valid */
			if (sz < 1024 || (sz & (al - 1))) return 0;	/* Check size */
			if (Fs.fptr & (al - 1)) return 0;			/* Check word alignment */
			return sz;									/* Start to play */

		case FCC('D','I','S','P') :		/* 'DISP' chunk */
		case FCC('L','I','S','T') :		/* 'LIST' chunk */
		case FCC('f','a','c','t') :		/* 'fact' chunk */
			if (sz & 1) sz++;				/* Align chunk size */
			pf_lseek(Fs.fptr + sz);			/* Skip this chunk */
			break;

		default :						/* Unknown chunk */
			return 0;
		}
	}

	return 0;
}



static
void ramp (
	int dir		/* 0:Ramp-down, 1:Ramp-up */
)
{
#if MODE != 3
	BYTE v, d, n;


	if (dir) {
		v = 0; d = 1;
	} else {
		v = 128; d = (BYTE)-1;
	}

	n = 128;
	do {
		v += d;
		OCR1A = v; OCR1B = v;
		delay_us(100);
	} while (--n);
#else
	dir = dir ? 128 : 0;
	OCR1A = (BYTE)dir; OCR1B = (BYTE)dir;
#endif
}



static
FRESULT play (
	const char *dir,	/* Directory */
	const char *fn		/* File */
)
{
	DWORD sz;
	FRESULT res;
	BYTE sw;
	WORD btr;

	xsprintf((char*)Buff, PSTR("%s/%s"), dir, fn);
	res = pf_open((char*)Buff);		/* Open sound file */
	if (res == FR_OK) {
		sz = load_header();			/* Check file format and ready to play */
		if (sz < 1024) return 255;	/* Cannot play this file */

		FifoCt = 0; FifoRi = 0; FifoWi = 0;	/* Reset audio FIFO */

		//if(!TCCR1){
			PLLCSR = 0b00000110;	/* Select PLL clock for TC1.ck */
			GTCCR =  0b01100000;	/* Enable OC1B as PWM */
			TCCR1 = MODE ? 0b01100001 : 0b00000001;	/* Start TC1 and enable OC1A as PWM if needed */
			TCCR0A = 0b00000010;	/* Start TC0 as interval timer at 2MHz */
			TCCR0B = 0b00000010;
			TIMSK = _BV(OCIE0A);
			ramp(1);
		//}
		
		pf_read(0, 512 - (Fs.fptr % 512), &rb);	/* Snip sector unaligned part */
		sz -= rb;
		sw = 1;	/* Button status flag */
		do {	/* Data transfer loop */
			btr = (sz > 1024) ? 1024 : (WORD)sz;/* A chunk of audio data */
			res = pf_read(0, btr, &rb);	/* Forward the data into audio FIFO */
			if (rb != 1024) break;		/* Break on error or end of data */
			sz -= rb;					/* Decrease data counter */

			sw <<= 1;					/* Break on button down */
		} while (((PINB & 1) || ++sw != 1) && !quit);
	}

	//while (FifoCt) ;			/* Wait for audio FIFO empty */
	
	OCR1A = 128; OCR1B = 128;	/* Return output to center level */

	return res;
}

/*-----------------------------------------------------------------------*/
/* Main                                                                  */

int main (void)
{
	FRESULT res;
	char *dir;
	BYTE org_osc = OSCCAL;

	cli();

	//OWSetup(true);

	DDRB  = 0b111110;
	PORTB = 0b101001;		/* Initialize port: - - H L H L L P */

	DDRB &= ~(1 << CS);
	PORTB |= (1 << CS);

	GIMSK |= (1 << PCIE);
	PCMSK |= (1 << CS);

	new_init_spi();

	MCUSR = 0;

	sei();

	for (;;) {
		//OWSetup(true);

		//while(!OWCheckRecv(filename) && filename[0] != '\0');
		//if(firstPlay){
		//	strncpy(filename, "PEW", 3);
		//	filename[3] = '\0';
		//}

		quit = false;
		loop = true;

		cli();
		if (pf_mount(&Fs) == FR_OK) {	/* Initialize FS */
			Buff[0] = 0;
			if (!pf_open("osccal")) pf_read(Buff, 1, &rb);	/* Adjust frequency */
			OSCCAL = org_osc + Buff[0];

			res = pf_opendir(&Dir, dir = "wav");	/* Open sound file directory */
			if (res == FR_NO_PATH)
				res = pf_opendir(&Dir, dir = "");	/* Open root directory */

			while(!quit);

			while (res == FR_OK && loop) {				/* Repeat in the dir */
				res = pf_readdir(&Dir, 0);			/* Rewind dir */

				while (res == FR_OK && loop) {				/* Play all wav files in the dir */
					res = pf_readdir(&Dir, &Fno);		/* Get a dir entry */
					if (res || !Fno.fname[0]) {
						loop = false;	/* Break on error or end of dir */
						break;
					}
					if (!(Fno.fattrib & (AM_DIR|AM_HID)) && strstr(Fno.fname, (const char *)filename)){
						quit = false;
						sei();
						res = play(dir, Fno.fname);		/* Play file */
						if(!res) firstPlay = true;
						loop = false; //break after correct file
					}
				}
			}
		}
	}
}

ISR(PIN_INT_VECT){
	if(PORTB & (1 << CS)){
		quit = true;
		if(!firstPlay){
			//filename[0] = 'B';
			//filename[1] = 'E';
			//filename[2] = 'E';
			//filename[3] = '\0';
			strncpy(filename, "BEE", 3);
		}
		else{
			//filename[0] = 'P';
			//filename[1] = 'E';
			//filename[2] = 'W';
			//filename[3] = '\0';
			strncpy(filename, "PEW", 3);

			uint8_t tempGTC = GTCCR;
			uint8_t tempTCR = TCCR1;
			uint8_t tempOCR1C = OCR1C;
			uint8_t tempOCR1A = OCR1A;
			uint8_t tempTIM = TIMSK;


			GTCCR = (1 << TSM); //temp. disable timer
			TCCR1 = (1 << CS12) | (1 << CS11) | (1 << CTC1); // f/32 prescale
			OCR1C = (TICK_LEN/4)-1;
			OCR1A = (TICK_LEN/4)-1;
			TIMSK = (1 << OCIE1A); //enable intr.
			GTCCR &= ~(1 << TSM); //reset timer

			//disable timer 0?
			
			sei();

			while(debugCount < 128);

			cli();
			
			GTCCR = tempGTC;
			TCCR1 = tempTCR;
			OCR1C = tempOCR1C;
			OCR1A = tempOCR1A;
			TIMSK = tempTIM;

			debugCount = 0;

			release_spi();
		}
	}
}

ISR(TIM_INT_VECT){
	debugCount++;
}