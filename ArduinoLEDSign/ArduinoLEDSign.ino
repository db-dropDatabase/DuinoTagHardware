#include <string.h>
#include "Adafruit_GFX.h"
#include "Adafruit_NeoMatrix.h"
#include "Adafruit_NeoPixel.h"
#include "pff.h"

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(30, 10, 10, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

FATFS fs;
DIR dir;
FILINFO inf;
UINT fr, read;

BYTE buff[16];


void setup()
{
  matrix.begin();
  matrix.show();

  Serial.begin(9600);
  Serial.println(F("Here"));
  Serial.flush();

  cli();

  //wait for sdcard to be ready
  while(pf_mount(&fs));

  //signal good stuff
  digitalWrite(13, HIGH);

  //open directory
  fr = pf_opendir(&dir, "/ANIM1");
  if(fr) die(fr, F("Opening /ANIM1 dir"));

  //scan directories
  for(;;){
	fr = pf_readdir(&dir, &inf);
	sei();
	if(fr || !inf.fname[0]) break;
	if(inf.fattrib & AM_DIR){
	  Serial.print("Directory: ");
	  Serial.println(inf.fname);
	} 
	else{
	  Serial.print("File: ");
	  Serial.print(inf.fsize);
	  Serial.print(", ");
	  Serial.println(inf.fname);
	}
	Serial.flush();
	cli();
  }
  if(fr) die(fr, F("Scanning directories"));

  //scan again, but this time for a bitmap file
  //reopen directory
  fr = pf_opendir(&dir, "/ANIM1");
  sei();
  if(fr) die(fr, F("Opening /ANIM1 dir"));
  cli();

  for(;;){

	fr = pf_readdir(&dir, &inf);
	sei();
	if(fr) die(fr, F("Searching for bitmap"));
	if(!inf.fname[0]) die(FR_NO_FILE, F("Searching for bitmap"));
	//if not directory
	if(!(inf.fattrib & AM_DIR)){
		//if it looks like a bitmap
		if(!strspn(inf.fname, ".BMP")){
		  //lets see if it talks like a bitmap
		  cli();
		  char filename[32];
		  sprintf(filename, "/ANIM1/%s", inf.fname);
		  fr = pf_open(filename);
		  sei();
		  if(fr) die(fr, strcat(filename, ": Unable to open"));
		  else Serial.println("Found file!");
		  cli();
		  fr = pf_read(buff, sizeof(buff), &read);
		  sei();
		  if(fr) die(fr, F(" Reading file"));
		  for(; read > 0; read--) Serial.println(buff[sizeof(buff) - read], HEX);
		  Serial.flush();
		}
	}
  }

  sei();

  for(; read > 0; read--) Serial.println(buff[sizeof(buff) - read]);
  
}

void loop()
{
 

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