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
	;sbi	_SFR_IO_ADDR(PORTB), B_DO	; MMC DI = H
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
	cbi _SFR_IO_ADDR(DDRB), PB2 ;set clock pin input
	sbi _SFR_IO_ADDR(USICR), USIWM0 ;set USI for slave mode
	ldi r24, (1 << USIOIF)
	out _SFR_IO_ADDR(USISR), r24 ;reset USI bus
	clr r24
	out _SFR_IO_ADDR(USIDR), r24 ;clear resudual USI data
	ret
.endfunc

;---------------------------------------------------------------------------;
; Set USI to master mode
;
; void spi_master (void);

.global spi_master
.func spi_master
	spi_master:
	clr r24
	out _SFR_IO_ADDR(USIDR), r24 ;clear resudual USI data
	ldi r24, (1 << USIOIF)
	out _SFR_IO_ADDR(USISR), r24 ;and wipe status just in case
	sbi _SFR_IO_ADDR(DDRB), PB2 ; set clock pin output
	cbi _SFR_IO_ADDR(USICR), USIWM0; set USI back to original state
	ret
.endfunc
