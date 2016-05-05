/*
 * mmc.h
 *
 * Created: 4/13/2016 8:39:25 AM
 *  Author: nkoontz0516
 */ 


#ifndef MMC_H_
#define MMC_H_

#define	_FLAGS	_SFR_IO_ADDR(GPIOR0)

#ifdef MODE
#if MODE >= 1
#define B_DO	5
#else
#define B_DO	1
#endif
#endif
#define B_CS	3

/*-----------------------------------------------------------------------*/
/* Low level disk control module                        (C)ChaN, 2009    */
/*-----------------------------------------------------------------------*/

#include "integer.h"
#include "diskio.h"

/* Low level SPI control functions */
void delay_ms(WORD ms);
void delay_us(WORD us);
void init_spi (void);
void spi_slave(void);
void spi_master(void);
void select (void);
void deselect (void);
void xmit_spi (BYTE);
BYTE rcv_spi (void);
void read_blk_part (void*, WORD, WORD);


/* Definitions of MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

BYTE CardType;

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

void release_spi (void);

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

BYTE send_cmd (BYTE cmd,	DWORD arg);

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void);

/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (void *dest,		/* Pointer to the destination object to put data */DWORD lba,		/* Start sector number (LBA) */WORD ofs,		/* Byte offset in the sector (0..511) */WORD cnt		/* Byte count (1..512), b15:destination flag */);

#endif /* MMC_H_ */