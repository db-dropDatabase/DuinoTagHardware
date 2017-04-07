#include <string.h>
#include <avr/wdt.h>
#include "Adafruit_NeoPixel.h"
#include "pff.h"

#define MATRIX_WIDTH 40
#define MATRIX_HEIGHT 5
#define MATRIX_PIN 19

#define soft_reset() wdt_enable(WDTO_15MS); for(;;)

#define FCC(c1,c2,c3,c4)	(((DWORD)c4<<24)+((DWORD)c3<<16)+((WORD)c2<<8)+(BYTE)c1)

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(MATRIX_WIDTH*MATRIX_HEIGHT, MATRIX_PIN);

FATFS fs;
DIR dir;
FILINFO inf;

UINT offset, width, height, fileSize;
uint8_t padding;

uint8_t dirName = 1;
uint8_t fileName = 0; //both start at 1, fileName increments on start

void setup()
{
  MCUSR = 0;
  wdt_disable();

  matrix.begin();
  matrix.show();

  Serial.begin(9600);
  Serial.println(F("Here"));
  Serial.flush();

  //wait for sdcard to be ready
  while(pf_mount(&fs));

  Serial.println(F("Sd Mounted!"));
  Serial.flush();

  nextFile();
  /*
  Serial.println("Start loop!");
  Serial.flush();
  */
}

void loop()
{
  BYTE buff[width*3 + padding];
  UINT fr, read;
 
  //seek to pixel array
  fr = pf_lseek(offset);
  if(fr) die(fr, "Seeking");
  
  //unsigned int lastTime = 0;
  //unsigned int endTime = 0;
 
  do{
	uint8_t pixelRow = 0;
	
	do{
	  //lastTime = micros();
	  fr = pf_read(buff, sizeof(buff), &read);
	  if(fr) die(fr, F("Reading row"));
	  //endTime = micros();
	  
	  /*	  
	  Serial.print("Read chunk: ");
	  Serial.print(read);
	  Serial.print(" Expected: ");
	  Serial.print(sizeof(buff));
	  Serial.print(" Time: ");
	  Serial.println(endTime - lastTime);
	  Serial.flush();
	  */
		  
      if(read != sizeof(buff)) die(read, F("Reading row: out of file"));

	  //lastTime = micros();
   	  for (uint8_t i = 0; i < width; i++){
		const uint8_t optimize = i*3;
	  	setPixel(i, pixelRow, buff[optimize+2], buff[optimize+1], buff[optimize]);
	  }
	  //endTime = micros();
	  
	  /*
	  Serial.print("Set LEDs in time: ");
	  Serial.println(endTime - lastTime);
	  Serial.flush();
	  */
		
	  //if run out of rows, finish drawing
	} while(++pixelRow < MATRIX_HEIGHT);
	
	//lastTime = micros();
	matrix.show();
	//endTime = micros();

	/*
	Serial.print("Show frame in time: ");
	Serial.println(endTime - lastTime);
	Serial.flush();
	*/
	delay(200);
	
  } while(fs.fptr < fileSize - 1);

  //iterate to next file
  //lastTime = micros();
  nextFile();
  //endTime = micros();

  //Serial.print("Next file time: ");
  //Serial.println(endTime - lastTime);
  //animation wait
  //delay(200);
}

//interpolate x/y coordinates to pixels on a zigzag matrix starting from the bottom left

inline void setPixel(const uint8_t &x, const uint8_t &y, const uint8_t &r, uint8_t &g, uint8_t &b){
	//if y is odd
	uint8_t pixnum;
	if(y & 1)
	  pixnum = (y * (MATRIX_WIDTH)) + x;
	else
	  pixnum = (y * (MATRIX_WIDTH)) + ((MATRIX_WIDTH - 1) - x);

	matrix.setPixelColor(pixnum, r, g, b);

	//debug
	
	/*
	Serial.print(F("Pixel: ("));
	Serial.print(x);
	Serial.print(F(", "));
	Serial.print(y);
	Serial.print(F(") C: "));
	Serial.print(color, HEX);
	Serial.print(" Pixel: ");
	Serial.println(pixnum);
	Serial.flush();
	*/
	
	
}

//seek to next bitmap file
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
	  /*
	  Serial.print("File: ");
	  Serial.println(filenameStr);
	  Serial.flush();
	  */
	  
	  //if no file, search next directory
	  if(fr == FR_NO_FILE){
		  if(fileName == 1) die(fr, F("Could not find first file in nextFile()"));
		  nextDir();
		  fileName = 1;
	  }
	  else if(fr) die(fr, F("nextFile()"));
	}while(fr);
	
	
	//read header of bitmap file
	pf_read(buff, sizeof(buff), &read);
	
	//grab offset, width, height, file size
	
	offset = FCC(buff[10], buff[11], buff[12], buff[13]);
	width = FCC(buff[18], buff[19], buff[20], buff[21]);
	height = FCC(buff[22], buff[23], buff[24], buff[25]);
	fileSize = FCC(buff[2], buff[3], buff[4], buff[5]);
	
	/*
	Serial.print("Offset: ");
	Serial.print(offset);
	Serial.print(" Width: ");
	Serial.print(width);
	Serial.print(" Height: ");
	Serial.print(height);
	Serial.print(" Size: ");
	Serial.println(fileSize);
	Serial.flush();
	*/
	
	
	//calculate padding
	padding = (width * 3) % 4; 
	
	/*
	Serial.print("Padding: ");
	Serial.println(padding);
	Serial.flush();
	*/
}

//seek to next animation dir
void nextDir(){
	UINT fr;
	
	//increment dir
	dirName++;
	
	//search for dir
	do 
	{
		char dirStr[4];
		
		sprintf(dirStr, "/%u", dirName);
		fr = pf_opendir(&dir, dirStr);
		
		/*
		Serial.print("Dir: ");
		Serial.println(dirName);
		Serial.flush(); 
		*/
		
		if(fr == FR_NO_FILE) dirName = 1;
		else if(fr) die(fr, F("NextFile()"));
	} while (!fr);
}

//debug on fails
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
		  Serial.print(F("Unknown Error: "));
		  Serial.println(error);
		  break;
	}

	Serial.flush();
	//die
	soft_reset();
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
		Serial.print(F("Unknown Error: "));
		Serial.println(error);
		break;
	}

	Serial.flush();
	//die
	soft_reset();
}