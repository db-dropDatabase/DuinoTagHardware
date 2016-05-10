;---------------------------------------------------------------------------;
; USI controls and Audio output control
;---------------------------------------------------------------------------;

.nolist
#include <avr/io.h>	// Include device specific definitions.
.list

#define MODE 0
;#define F_CPU 8000000

#define	_FLAGS	_SFR_IO_ADDR(GPIOR0)

#ifdef MODE
#if MODE >= 1
#define B_DO	5
#else
#define B_DO	1
#endif
#endif

#define B_CS	3

;---------------------------------------------------------------------------;
; Initialize USI
;
; void init_spi (void);

.global init_spi
.func init_spi
init_spi:
	ldi	r24, 0b00001000			; Enable only SCK and DI. DO is controlled by software
	out	_SFR_IO_ADDR(USICR), r24
	sbi	_SFR_IO_ADDR(PORTB), B_DO	; MMC DI = H
	;sbi	_SFR_IO_ADDR(PORTB), B_CS	; MMC CS = H
	ret
.endfunc


;---------------------------------------------------------------------------;
; Set USI to slave mode
;
; void spi_slave (void);

.global spi_slave
.func spi_slave
	spi_slave:
	ldi r24, (1 << PB4) | (1 << PB1); Set SCK to input, MISO to output, MOSI to input, and keep rest of pins original
	out _SFR_IO_ADDR(DDRB), r24
	ldi r24, (1 << PB0) | (1 << PB2) | (1 << PB3) ;set up approprite pullups
	out _SFR_IO_ADDR(PORTB), r24
	ret
.endfunc

;---------------------------------------------------------------------------;
; Set USI to master mode
;
; void spi_master (void);

.global spi_master
.func spi_master
	spi_master:
	ldi r24, ~((1 << PB3) | (1 << PB1)) ;reset pin directions
	out _SFR_IO_ADDR(DDRB), r24
	ldi r24, (1 << PB3) | (1 << PB1) ;reset pullups
	out _SFR_IO_ADDR(PORTB), r24
	ret
.endfunc
