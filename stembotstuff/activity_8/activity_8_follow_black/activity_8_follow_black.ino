#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Declaration for an SSD1 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 is sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int ir1 = A0, ir3 = A2, ir5 = A7;
int pwmL1 = 3, pwmL2 = 5, pwmR1 = 9, pwmR2 = 10;

//the 0th element is forward/clockwise direction
//the 1st element is backward/anticlockwise direction
int pwmL[] = {pwmL1, pwmL2};
int pwmR[] = {pwmR1, pwmR2};

//auto-callibration vars
//they map to the IRs array, one-to-one
int irs[] = {ir1, ir3, ir5};
int irmins[] = {1023, 1023, 1023};
int irmaxs[] = {0, 0, 0};
int iravgs[] = {0, 0, 0};
int states[] = {0, 0, 0};

void setup(){
  for(int i=0; i<2; i++){
    pinMode(pwmL[i], OUTPUT); digitalWrite(pwmL[i], LOW);
    pinMode(pwmR[i], OUTPUT); digitalWrite(pwmR[i], LOW);
  }

  for(int i=0; i<3; i++){
    pinMode(irs[i], INPUT);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //Don't proceed, loop forever
  }
  display.display();

  callibrateSensors();
  delay(2000);
}

void loop(){
  for(int i=0; i<3; i++){
    states[i] = analogRead(irs[i]);  
  }

  /*ascii drawing of stembot's sensors:
   
   |   IR5   |   IR3   |   IR1   |
   | irs[2]  | irs[1]  | irs[0]  |
   |_____________________________|

   ***REMEMBER: larger value == Black***
  */

  //if line is on the left side
  if((states[2] > iravgs[2]) && (states[1] < iravgs[1])){
    turnLeft(50, 50);
    
    oledDisplay(2,0,0);
    display.println("   ");
    display.println("  Left  ");
    display.println("   ");
    display.display();
  }
  //if line is on the right side
  else if((states[0] > iravgs[0]) && (states[1] < iravgs[1])){
    turnRight(50, 50);
    
    oledDisplay(2,0,0);
    display.println("   ");
    display.println(" Right ");
    display.println("   ");
    display.display();
  }
  //if encounter right junction
  else if((states[0] > iravgs[0]) && (states[1] > iravgs[1])){
    goForward(100, 100);
    delay(10);
    turnRight(50, 50);
    delay(200);
    
    oledDisplay(2,0,0);
    display.println(" 90 ");
    display.println(" Right ");
    display.println("   ");
    display.display();
  }
  //if encounter left junction
  else if((states[2] > iravgs[2]) && (states[1] > iravgs[1])){
    goForward(100, 100);
    delay(10);
    turnLeft(50, 50);
    delay(200);

    oledDisplay(2,0,0);
    display.println(" 90 ");
    display.println("  Left  ");
    display.println("   ");
    display.display();
  }
  //if no black are detected
  else{
    goForward(100, 100);

    oledDisplay(2,0,0);
    display.println("   ");
    display.println(" Forward ");
    display.println("   ");
    display.display();
  }
}


void oledDisplay(int8_t textSize, int8_t x, int8_t y) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
}

void callibrateSensors(){
  //keep turning right to sample more areas
  turnRight(50, 50);
  
  //callibrate 300 times for accuracy
  for(int i=0; i<300; i++){
    
    //callibrate each sensor
    for(int j=0; j<3; j++){
      //get the reading from the current IR
      states[j] = analogRead(irs[j]);
      
      //we want (irmax = black) and (irmin = white)
      //is it now on a light or dark line?      
      if(irmins[j] > states[j]){
        //if on white, set the minimum to current reading
        irmins[j] = states[j];
      } else{
        //if on black, set the maximum to current reading
        irmaxs[j] = states[j];
      }
      //robot keep turning right, increase accuracy
    }
    delay(10);
  }
  goForward(0, 0); //stop turning
  //calculate avgs
  for(int k=0; k<3; k++){
    iravgs[k] = (irmins[k] + irmaxs[k])/2;
  }
}



/*------ all the inputted speeds must be in percentages ------*/

void goForward(float speed1, float speed2){
  float convertedSpeed1 = speed1/100*255;
  float convertedSpeed2 = speed2/100*255;
  
  //stop anticlockwise / backwards motion
  digitalWrite(pwmL[1], 0); digitalWrite(pwmR[1], 0); 
  //turn both motors clockwise / go forward
  analogWrite(pwmL[0], (int)convertedSpeed1);
  analogWrite(pwmR[0], (int)convertedSpeed2);
}

void goBackward(float speed1, float speed2){
  float convertedSpeed1 = speed1/100*255;
  float convertedSpeed2 = speed2/100*255;
  
  //stop clockwise / forwards motion
  digitalWrite(pwmL[0], 0); digitalWrite(pwmR[0], 0); 
  //turn both motors anticlockwise / go backward
  analogWrite(pwmL[1], (int)convertedSpeed1);
  analogWrite(pwmR[1], (int)convertedSpeed2);
}

void turnLeft(float speed1, float speed2){
  float convertedSpeed1 = speed1/100*255;
  float convertedSpeed2 = speed2/100*255;
  
  //initialize motors before turning
  digitalWrite(pwmL[0], 0); digitalWrite(pwmR[1], 0); 
  //to turn left,
  analogWrite(pwmL[1], (int)convertedSpeed1); //left motor goes anticlockwise
  analogWrite(pwmR[0], (int)convertedSpeed2); //right motor goes clockwise
}

void turnRight(float speed1, float speed2){
  float convertedSpeed1 = speed1/100*255;
  float convertedSpeed2 = speed2/100*255;
  
  //initialize motors before turning
  digitalWrite(pwmL[1], 0); digitalWrite(pwmR[0], 0); 
  //to turn right,
  analogWrite(pwmL[0], (int)convertedSpeed1); //left motor goes clockwise
  analogWrite(pwmR[1], (int)convertedSpeed2); //right motor goes anticlockwise
}
