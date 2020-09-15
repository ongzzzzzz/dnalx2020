/* 
  OLED screen is deactivate for this combined (RC/auto) program
  because Arduino is out of memmory
 */
//#define OLED //activate this line to use OLED screen

#include "StemBot2019.h"

/*
   INSTRUCTIONS Setup
   bot_setup(IR sensor calibration time in seconds);
   alignment(reduce left speed, reduce right speed, inverted left, inverted right, test);
   LED(green, yellow, red);
 */
void setup() {
  bot_setup(2, 1);
  alignment(0, 0, 0, 0, 0);
  LED(1, 0, 1);
  KP = 0.15, KI = 0, KD = 5;
}

/*
   INSTRUCTIONS loop
   junction(speed, trace back, junction, action, delay b4 turn, turn speed, turn duration, line color 1=black 0=white, IR offset);
  
   types of junctions   |
          robot         |       type 1         type 2                       type 3                      type 4
        direction       |       left           right                        middle                      follow line (dont care any junction) 
                        |
            H           |        HH            HH                HHHHHHHHHHHH        HH                   HH
          HHHHH         |        HH            HH                     HH             HH                   HH
        HHHHHHHHH       |   HHHHHHH            HHHHHHH                HH             HH                   HH
           HHH          |        HH            HH                     HH        HHHHHHHHHHHH              HH
           HHH          |        HH            HH                     HH             HH                   HH
           HHH          |        HH            HH                     HH             HH                   HH
           HHH          |        HH            HH                     HH             HH                   HH
 */
 
void loop() {
    junction(200, 100, 2, 22, 200, 100, 200, 1, 0);
    junction(200, 100, 3, 33, 200, 100, 200, 1, 0);
    junction(200, 100, 1, 11, 200, 100, 200, 1, 0);
    junction(200, 100, 4, 69, -1, 100, 200, 1, 0);
    Stop();
    display_finishTime();
}
