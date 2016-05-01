/*
 * SPIO.c
 *
 * Created: 4/9/2016 2:26:54 PM
 *  Author: Noah
 */ 

 #include "SPIO.h"

 #ifndef SPIO_C_
 #define SPIO_C_

 void sendDiag(MSG_LIST msg){
	 deselect(); //deselect the SD card, select the Ardunio
	 xmit_spi(msg); //send diagnostic message
	 select(); //go back to normal
 }

 void sendDiag(char[] filename){
	 deselect();
	 xmit_spi(TIME_ELAPASED)
	 for(uint8_t i = 0; i < sizeof(filename); i++){
		 xmit_spi(filename[i]);  //send the entire song filename
	 }
	 select();
 }

 void sendDiag(uint16_t timeElapsed){
	deselect();
	xmit_spi(TIME_ELAPASED);
	xmit_spi(timeElapsed);
	select();
 }

 #endif