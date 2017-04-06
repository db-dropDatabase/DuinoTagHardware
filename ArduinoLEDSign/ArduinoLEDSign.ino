#include <string.h>
#include "Adafruit_GFX.h"
#include "Adafruit_NeoMatrix.h"
#include "Adafruit_NeoPixel.h"
#include "pff.h"

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(30, 10, 10, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

FATFS fs;
DIR dir;
FILINFO inf;

UINT offset, width, height;
uint8_t padding;

uint8_t dirName = 1;
uint8_t fileName = 0; //both start at 1, fileName increments on start

void setup()
{
  matrix.begin();
  matrix.show();

  Serial.begin(9600);
  Serial.println(F("Here"));
  Serial.flush();

  //wait for sdcard to be ready
  while(pf_mount(&fs));

  nextFile();

}

void loop()
{
  BYTE buff[width*24 + padding];
  UINT fr;
 
  //seek to pixel array
  fr = pf_lseek(offset);
  if(fr) die(fr, "Seeking");
 
  do 
  {
	UINT read;
	
	fr = pf_read(buff, sizeof(buff), &read);
  } while (read == sizeof(buff));
 
 

}

void nextFile(){
	BYTE buff[26];
	UINT fr, read;
	
	//increment file
	fileName++;
	
	//open bitmap file (must be 24 bit)
	do{
	  char filenameStr[16];
	  
	  sprintf(filenameStr, "/%u/%u.BMP", dirName, fileName);
	  fr = pf_open(filenameStr);
	  Serial.print("File: ");
	  Serial.println(filenameStr);
	  
	  //if no file, search next directory
	  if(fr == FR_NO_FILE){
		  if(fileName == 1) die(fr, F("Could not find first file in nextFile()"));
		  nextDir();
		  fileName = 1;
	  }
	  else if(fr) die(fr, F("nextFile()"));
	}while(!fr);
	
	
	//read header of bitmap file
	pf_read(buff, sizeof(buff), read);
	
	//grab offset, width, height
	
	offset = buff[10] | (buff[11] << 8) | (buff[12] << 16) | (buff[13] << 24);
	width = buff[18] | (buff[19] << 8) | (buff[20] << 16) | (buff[21] << 24);
	height = buff[22] | (buff[23] << 8) | (buff[24] << 16) | (buff[25] << 24);
	
	Serial.print("Offset: ");
	Serial.print(offset);
	Serial.print(" Width: ");
	Serial.print(width);
	Serial.print(" Height: ");
	Serial.println(height);
	
	//calculate padding
	padding = ((width * 24 + 31)/32)*4;
	
	Serial.print("Padding: ");
	Serial.print(padding);
	
	Serial.flush();
}

void nextDir(){
	UINT fr;
	
	//increment dir
	dirName++;
	
	//search for dir
	do 
	{
		char dirStr[4];
		
		sprintf(dirStr, "/%u", dirName);
		fr = pf_opendir(&dir, dirName);
		
		Serial.print("Dir: ");
		Serial.println(dirName);
		
		if(fr == FR_NO_FILE) dirName = 1;
		else if(fr) die(fr, F("NextFile()"));
	} while (!fr);
}

void die(const UINT &error, const __FlashStringHelper * place){
	sei();

	Serial.println(place);

	switch(error){
		case FR_NO_FILE:
		  Serial.println(F("Unable to find file"));
		  break;
		case FR_NO_FILESYSTEM:
		  Serial.println(F("Unable to find filesystem"));
		  break;
		case FR_NOT_OPENED:
		  Serial.println(F("File not open"));
		  break;
		case FR_NOT_READY:
		  Serial.println(F("SDCard not ready"));
		  break;
		case FR_DISK_ERR:
		  Serial.println(F("SDCard not working"));
		  break;
		default:
		  Serial.println(F("Unknown Error"));
		  break;
	}

	Serial.flush();
	//die
	for(;;);
}

void die(const UINT &error, const char * place){
	sei();

	Serial.println(place);

	switch(error){
		case FR_NO_FILE:
		Serial.println(F("Unable to find file"));
		break;
		case FR_NO_FILESYSTEM:
		Serial.println(F("Unable to find filesystem"));
		break;
		case FR_NOT_OPENED:
		Serial.println(F("File not open"));
		break;
		case FR_NOT_READY:
		Serial.println(F("SDCard not ready"));
		break;
		case FR_DISK_ERR:
		Serial.println(F("SDCard not working"));
		break;
		default:
		Serial.println(F("Unknown Error"));
		break;
	}

	Serial.flush();
	//die
	for(;;);
}