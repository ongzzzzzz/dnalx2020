int pwmL1 = 3, pwmL2 = 5;
int pwmR1 = 9, pwmR2 = 10;

//the 0th element is forward/clockwise direction
//the 1st element is backward/anticlockwise direction
int pwmL[] = {pwmL1, pwmL2};
int pwmR[] = {pwmR1, pwmR2};

void setup(){
  for(int i=0; i<2; i++){
    pinMode(pwmL[i], OUTPUT); digitalWrite(pwmL[i], LOW);
    pinMode(pwmR[i], OUTPUT); digitalWrite(pwmR[i], LOW);
  }

/*-------------------------------------------------
 * experiment: calculate the speed of the robot
 * how far does it go in 1 second with 100% speed?
  goForward(100, 100);
  delay(1000);
  goForward(0,0);

 * result: 100% speed for 1s goes ~80cm 
 * hence 100% speed = 0.8m/s
-------------------------------------------------*/

// when turning, the wheels draw a circle
// the circle has diameter 9cm, radius 4.5cm
// so 90degree turn draws an arc of ~7cm
// hence to turn 90deg for 1s requires (7/80*100)% speed or ~8.75%
 
  for(int j=0; j<4; j++){
    goForward(100, 100);
    delay(1000);
    turnRight(70, 70);
    delay(375);
  }

  goForward(0,0);
  
}

void loop(){
  //do nothing lmao
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
