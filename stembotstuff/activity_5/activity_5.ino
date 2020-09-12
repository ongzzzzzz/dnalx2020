#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Declaration for an SSD1 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 is sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledDisplay(int8_t textSize, int8_t x, int8_t y) {
  display.clearDisplay();
  display.setTextSize (textSize);
  display.setTextColor (WHITE);
  display.setCursor(x,y);
 
}

void setup() {
  //**************** OLED Setup *********************//
    //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3v internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {//Address 0x36 for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); //Don't proceed, loop forever
}
//Show initial display buffer contesnts on the screen --
//The library intializes this with an Adafruit splash screen.
display.display();
//***************************************************//
}

void loop() {

  oledDisplay(2,0,0);
  display.println("   i am   ");
  display.println("");
  display.println(" zhizheng ");
  display.display();
  delay(1000);
  oledDisplay(2,0,0);
  display.println("   i am   ");
  display.println("");
  display.println("  ready :D  ");
  display.display();
  delay(1000);
}
