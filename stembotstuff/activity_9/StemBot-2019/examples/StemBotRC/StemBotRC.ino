/* OLED screen is deactivate for this combined (RC/auto) program
   because Arduino is out of memmory
*/
//#define OLED //activate this line to use OLED screen

#include "StemBot2019.h"
#include "bluetooth.h"

/*
   INSTRUCTIONS Setup
   bot_setup(IR sensor calibration time in seconds);
   rename_bt("name as u like (max 10 char)", yes/no display on OLED screen);
   alignment(reduce left speed, reduce right speed, inverted left, inverted right, test);
   LED(green, yellow, red);
*/

void setup() {
  bot_setup(0);
  bt_setup();
  rename_bt("StemBot", 0);
  alignment(0, 0, 0, 0, 0);
  LED(1, 0, 1);
  KP = 0.15, KI = 0, KD = 5;
}

/*
   INSTRUCTIONS loop
   BT_receiver(yes/no display character received from BT);
*/

void loop() {
  if (BT_receiver(0) == 'F') {
    forward(200, 200);
  }
  if (BT_receiver(0) == 'S') {
    Stop();
  }
}
