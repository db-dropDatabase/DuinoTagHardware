/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <ch.h>
#include <hal.h>
#include <CheapRandom.h>

static WORKING_AREA(waBlinker1, 32);
static THD_FUNCTION(Blinker1, arg) {

  (void)arg;
  
  while (true) {
    palTogglePad(IOPORT2, PORTB_LED1);
    chThdSleepMilliseconds(1000);
  }
}

static WORKING_AREA(waBlinker2, 48);
static WORKING_AREA(waBlinker3, 48);
static WORKING_AREA(waBlinker4, 48);
static WORKING_AREA(waBlinker5, 48);
static THD_FUNCTION(Blinker2, arg) {

	while(TRUE){
		uint8_t randNum;
		returnRandom(&randNum);
		
		while(randNum--){
			palSetPad(IOPORT2, *(uint8_t *)arg);
			
			chThdSleepMicroseconds(randNum * 2);
			
			palClearPad(IOPORT2, *(uint8_t *)arg);
			
			chThdSleepMicroseconds(255 - (randNum * 2));
		}
		
		returnRandom(&randNum);
		chThdSleepMicroseconds((randNum % 50) * 600);
	}
	
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  palClearPad(IOPORT2, PORTB_LED1);
  palClearPad(IOPORT2, PB3);

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /*
   * Starts the LED blinker thread.
   */
  const uint8_t pin1 = PB0;
  const uint8_t pin2 = PB1;
  const uint8_t pin3 = PB2;
  const uint8_t pin4 = PB3;
  
  chThdCreateStatic(waBlinker1, sizeof(waBlinker1), NORMALPRIO, Blinker1, NULL);
  chThdCreateStatic(waBlinker2, sizeof(waBlinker2), NORMALPRIO, Blinker2, &pin1);
  chThdCreateStatic(waBlinker3, sizeof(waBlinker3), NORMALPRIO, Blinker2, &pin2);
  chThdCreateStatic(waBlinker4, sizeof(waBlinker4), NORMALPRIO, Blinker2, &pin3);
  chThdCreateStatic(waBlinker5, sizeof(waBlinker5), NORMALPRIO, Blinker2, &pin4);
  

  chnWrite(&SD1, (const uint8_t *)"Hello World!\r\n", 14);
  while(TRUE) {
    chThdSleepMilliseconds(1000);
  }
}
