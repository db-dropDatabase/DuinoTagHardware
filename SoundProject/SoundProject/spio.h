/*
 * SPIO.h
 *
 * Created: 4/9/2016 2:03:24 PM
 *  Author: Noah
 */ 
 #include <stdint.h>

#ifndef SPIO_H_
#define SPIO_H_

enum msgs{
	OK = 0x00,
	MOUNT_OK = 0x01,
	MOUNT_FAIL = 0x02,
	OPEN_WAV_DIR = 0x03,
	OPEN_ROOT_DIR = 0x04,
	OPEN_DIR_FAIL = 0x05,
	FILE_NOT_FOUND = 0x07,
	PLAY_SONG = 0x08,
	PLAY_FAIL = 0x09,
	FINISH = 0x0a,
	INTERRUPTED = 0x0b,
	DIR_ERROR = 0x0c,
	END_OF_DIR = 0x0d,
	FILENAME = 0x06,
	TIME_ELAPASED = 0x0e
};

void sendDiag(uint8_t msg);

void sendFilename(char * filename);

void sendTime(uint16_t timeElapsed);


#endif /* SPIO_H_ */