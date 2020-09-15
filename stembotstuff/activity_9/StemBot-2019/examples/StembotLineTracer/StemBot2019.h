//****************** HCSR04 setting ******************//
#include "hcsr04.h"
#define TRIG_PIN 5
#define ECHO_PIN 7
//HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, min_distance(mm), max_distance(mm));
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000);
//****************** HCSR04 setting ******************//

//****************** OLED setting ******************//
#ifdef OLED
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif
//****************** OLED setting ******************//

//****************** PID Setting ******************//
//pid settings and gains
int Speed = 255, out_min = -Speed, out_max = Speed;
double IR_position = 0 , setPoint = 0, outputVal = 0, KP = 0, KI = 0, KD = 0;
//****************** PID Setting ******************//

//pins input
const int8_t IR1 = A7, IR2 = A6, IR3 = A3, IR4 = A2, IR5 = A1;
const int8_t battMon_pin = A0;

//pins output
const int8_t pwmL1 = 3, pwmL2 = 5, pwmR1 = 9, pwmR2 = 10, led_r = 13, led_y = 1, led_g = 0, battMon_en = 6;

void LED(bool r, bool y, bool g) {
  digitalWrite(led_r, r);
  digitalWrite(led_y, y);
  digitalWrite(led_g, g);
}

void oledDisplay(int8_t textSize, int8_t x, int8_t y) {
#ifdef OLED
  display.clearDisplay();
  display.display();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
#endif
}

int8_t offsetL = 0, offsetR = 0;
int8_t invert_L = 0, invert_R = 0;
void forward(int left, int right) {
  int outputL = left + offsetL;  if (outputL < 0) outputL = 0;
  int outputR = right + offsetR; if (outputR < 0) outputR = 0;
  if (invert_L == 0) {
    digitalWrite(pwmL2, 0);
    analogWrite(pwmL1, outputL);
  }
  else {
    digitalWrite(pwmL1, 0);
    analogWrite(pwmL2, outputL);
  }
  if (invert_R == 0) {
    digitalWrite(pwmR2, 0);
    analogWrite(pwmR1, outputR);
  }
  else {
    digitalWrite(pwmR1, 0);
    analogWrite(pwmR2, outputR);
  }
}
void backward(int left, int right) {
  int outputL = left + offsetL;  if (outputL < 0) outputL = 0;
  int outputR = right + offsetR; if (outputR < 0) outputR = 0;
  if (invert_L == 0) {
    digitalWrite(pwmL1, 0);
    analogWrite(pwmL2, outputL);
  }
  else {
    digitalWrite(pwmL2, 0);
    analogWrite(pwmL1, outputL);
  }
  if (invert_R == 0) {
    digitalWrite(pwmR1, 0);
    analogWrite(pwmR2, outputR);
  }
  else {
    digitalWrite(pwmR2, 0);
    analogWrite(pwmR1, outputR);
  }
}

void turnLeft(int left, int right) {
  int outputL = left + offsetL;  if (outputL < 0) outputL = 0;
  int outputR = right + offsetR; if (outputR < 0) outputR = 0;
  if (invert_L == 0) {
    digitalWrite(pwmL1, 0);
    analogWrite(pwmL2, outputL);
  }
  else {
    digitalWrite(pwmL2, 0);
    analogWrite(pwmL1, outputL);
  }
  if (invert_R == 0) {
    digitalWrite(pwmR2, 0);
    analogWrite(pwmR1, outputR);
  }
  else {
    digitalWrite(pwmR1, 0);
    analogWrite(pwmR2, outputR);
  }
}
void turnRight(int left, int right) {
  int outputL = left + offsetL;  if (outputL < 0) outputL = 0;
  int outputR = right + offsetR; if (outputR < 0) outputR = 0;
  if (invert_L == 0) {
    digitalWrite(pwmL2, 0);
    analogWrite(pwmL1, outputL);
  }
  else {
    digitalWrite(pwmL1, 0);
    analogWrite(pwmL2, outputL);
  }
  if (invert_R == 0) {
    digitalWrite(pwmR1, 0);
    analogWrite(pwmR2, outputR);
  }
  else {
    digitalWrite(pwmR2, 0);
    analogWrite(pwmR1, outputR);
  }
}

void Stop() {
  digitalWrite(pwmL1, 1); digitalWrite(pwmL2, 1);
  digitalWrite(pwmR1, 1); digitalWrite(pwmR2, 1);
}

void alignment(int alignL, int alignR, int _invert_L, int _invert_R, int align_test) {
  invert_L = _invert_L; invert_R = _invert_R;
  offsetL = alignL;     offsetR = alignR;
  if (align_test == 1) {
    forward(255, 255);
    delay(2000);
    Stop();
    while (1);
  }
}

int IR1_avg = 0,   IR2_avg = 0,   IR3_avg = 0,   IR4_avg = 0,   IR5_avg = 0;
void calibrateIR(int i, int auto_) {
  int IR1_min = 1023, IR2_min = 1023, IR3_min = 1023, IR4_min = 1023, IR5_min = 1023;
  int IR1_max = 0,   IR2_max = 0,   IR3_max = 0,   IR4_max = 0,   IR5_max = 0;
#ifdef OLED
  oledDisplay(2, 0, 0);
  display.println(" CALBRATE ");
  display.println("----------");
  display.println("  SENSOR  ");
  if (auto_ == 0) {
    display.println("--manual--");
    display.display();
  }
  else {
    display.println("---auto---");
    display.display();
  }
#endif
  int j = i * 100, k = 0, kk = 0, state = 0;
  do {
    k += 1; kk += 1;
    if (kk == 10) {
      kk = 0;
      state = !state;
      LED(state, state, state);
    }
    if (auto_ == 0) { //manual calibrate
      int IR1_cal = analogRead(IR1); delay(2);
      if (IR1_cal < IR1_min) {
        IR1_min = IR1_cal;
      }
      else if (IR1_cal > IR1_max) {
        IR1_max = IR1_cal;
      }
      int IR2_cal = analogRead(IR2); delay(2);
      if (IR2_cal < IR2_min) {
        IR2_min = IR2_cal;
      }
      else if (IR2_cal > IR2_max) {
        IR2_max = IR2_cal;
      }
      int IR3_cal = analogRead(IR3); delay(2);
      if (IR3_cal < IR3_min) {
        IR3_min = IR3_cal;
      }
      else if (IR3_cal > IR3_max) {
        IR3_max = IR3_cal;
      }
      int IR4_cal = analogRead(IR4); delay(2);
      if (IR4_cal < IR4_min) {
        IR4_min = IR4_cal;
      }
      else if (IR4_cal > IR4_max) {
        IR1_max = IR4_cal;
      }
      int IR5_cal = analogRead(IR5); delay(2);
      if (IR5_cal < IR5_min) {
        IR5_min = IR5_cal;
      }
      else if (IR5_cal > IR5_max) {
        IR5_max = IR5_cal;
      }
    }
    else { //auto calibrate
      int IR1_cal = analogRead(IR1); delay(2);
      if (IR1_cal < IR1_min) {
        IR1_min = IR1_cal;
      }
      else if (IR1_cal > IR1_max) {
        IR1_max = IR1_cal;
      }
      int IR2_cal = analogRead(IR2); delay(2);
      if (IR2_cal < IR1_min) {
        IR1_min = IR2_cal;
      }
      else if (IR2_cal > IR1_max) {
        IR1_max = IR2_cal;
      }
      int IR3_cal = analogRead(IR3); delay(2);
      if (IR3_cal < IR1_min) {
        IR1_min = IR3_cal;
      }
      else if (IR3_cal > IR1_max) {
        IR1_max = IR3_cal;
      }
      int IR4_cal = analogRead(IR4); delay(2);
      if (IR4_cal < IR1_min) {
        IR1_min = IR4_cal;
      }
      else if (IR4_cal > IR1_max) {
        IR1_max = IR4_cal;
      }
      int IR5_cal = analogRead(IR5); delay(2);
      if (IR5_cal < IR1_min) {
        IR1_min = IR5_cal;
      }
      else if (IR5_cal > IR1_max) {
        IR1_max = IR5_cal;
      }
    }
  }
  while (i != 0 && k != j);
  if (auto_ == 0) { //manual
    IR1_avg = (IR1_min + IR1_max) / 2; IR2_avg = (IR2_min + IR2_max) / 2; IR3_avg = (IR3_min + IR3_max) / 2;
    IR4_avg = (IR4_min + IR4_max) / 2; IR5_avg = (IR5_min + IR5_max) / 2;
  }
  else { //auto
    IR1_avg = (IR1_min + IR1_max) / 2;
    IR2_avg = IR1_avg, IR3_avg = IR1_avg, IR4_avg = IR1_avg, IR5_avg = IR1_avg;
  }
  LED(0, 0, 0);

}

const int8_t blackLine = 1, whiteLine = 0;
int val1 = 0, val2 = 0, val3 = 0, val4 = 0, val5 = 0;
void update_sensor(int8_t line, int offsetIR) {
  //    while (1) {
  if (analogRead(IR1) < IR1_avg - offsetIR) val1 = 1;
  else val1 = 0;
  if (analogRead(IR2) < IR2_avg - offsetIR) val2 = 1;
  else val2 = 0;
  if (analogRead(IR3) < IR3_avg - offsetIR) val3 = 1;
  else val3 = 0;
  if (analogRead(IR4) < IR4_avg - offsetIR) val4 = 1;
  else val4 = 0;
  if (analogRead(IR5) < IR5_avg - offsetIR) val5 = 1;
  else val5 = 0;
  if (line == blackLine) {
    val1 = !val1; val2 = !val2; val3 = !val3; val4 = !val4; val5 = !val5;
  }
  else if (line == whiteLine);

  //      oledDisplay(2, 0, 0);
  //      display.println("  " + String(val2) + " " + String(val3) + " " + String(val4) + "  ");
  //      display.println(String(val1) + "       " + String(val5));
  //      display.display();
  //    }
}

unsigned long previous_time = 0, current_time = 0, elapsed_time = 0;
double last_error = 0, _error = 0, int_error = 0, rate_error = 0;
double pid(double _setpoint, double _input, int min_out, int max_out) {
  current_time = millis();
  elapsed_time = current_time - previous_time;
  previous_time = current_time;

  _error = _setpoint - _input;
  int_error += _error * (elapsed_time);  if (int_error < -1000) int_error = -1000;
  if (int_error >  1000) int_error =  1000;
  rate_error = (_error - last_error) / (elapsed_time);
  last_error = _error;

  double _output = KP * _error + KI * int_error - KD * rate_error;
  if (_output < out_min) {
    _output = out_min;
  }
  else if (_output > out_max) {
    _output = out_max;
  }
  return (_output);
}

void followLine(int speedL, int speedR) {
  setPoint = 1000;
  outputVal = pid(setPoint, IR_position, out_min, out_max);

  if (outputVal > 0) { // line is on the LEFT of robot
    //LED(1, 1, 0);
    forward(speedL - outputVal, speedR);
  }
  else if (outputVal == 0) { // line is on the MIDDLE of robot
    //LED(0, 1, 0);
    forward(speedL, speedR);
  }
  else if (outputVal < 0) { // line is on the RIGHT of robot
    //LED(0, 1, 1);
    forward(speedL, speedR + outputVal);
  }
  LED(0, 1, 0);
  //      oledDisplay(2, 0, 0);
  //      display.println(IR_position);
  //      display.println(outputVal);
  //      display.display();
}

void junction(int speed_M, int trace_back, int TYPE, int action, int delay_b4_turn, int turn_speed, int turn_duration, int line, int offsetIR) {
  out_min = -speed_M;
  out_max = speed_M;
  int countIgnore = 0;
  do {
    update_sensor(line, offsetIR);
    int valSum = val1 + val2 + val3 + val4 + val5; // calculate line state value
    if (valSum > 0) { // robot is ON line
      IR_position = (val1 * 0 + val2 * 500 + val3 * 1000 + val4 * 1500 + val5 * 2000) / valSum;
      followLine(speed_M, speed_M);
      if (TYPE == 1 && action == 11 && val1 == 1 && (val3 == 1 || val4 == 1) && val5 == 0) { // Left junction, turn left
        delay(delay_b4_turn);
        turnLeft(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint - 100;
        break;
      }
      else if (TYPE == 1 && action == 22 && val1 == 1 && (val3 == 1 || val4 == 1) && val5 == 0) { // left junction, turn right
        delay(delay_b4_turn);
        turnRight(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint + 100;
        break;
      }
      else if (TYPE == 1 && action == 33 && val1 == 1 && (val3 == 1 || val4 == 1) && val5 == 0) { // left junction, ignore
        while (val1 == 1) {
          if (analogRead(IR1) < IR1_avg - offsetIR) val1 = 1;
          else val1 = 0;
          if (line == blackLine) val1 = !val1;
          else if (line == whiteLine);
        }
        break;
      }
      else if (TYPE == 3 && action == 11 && (val1 == 1 && val5 == 1)) { // middle junction, turn left
        delay(delay_b4_turn);
        turnLeft(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint - 100;
        break;
      }
      else if (TYPE == 3 && action == 22 && (val1 == 1 && val5 == 1)) { // middle junction, turn right
        delay(delay_b4_turn);
        turnRight(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint + 100;
        break;
      }
      else if (TYPE == 3 && action == 33 && (val1 == 1 && val5 == 1)) { // middle junction, ignore
        while (val1 == 1 && val5 == 1) { // make sure robot passed junction
          if (analogRead(IR1) < IR1_avg - offsetIR) val1 = 1;
          else val1 = 0;
          if (analogRead(IR5) < IR5_avg - offsetIR) val5 = 1;
          else val5 = 0;
          if (line == blackLine) {
            val1 = !val1; val5 = !val5;
          }
          else if (line == whiteLine);
        }
        break;
      }
      else if (TYPE == 2 && action == 11 && val1 == 0 && (val2 == 1 || val3 == 1) && val5 == 1) { // right junction, turn left
        delay(delay_b4_turn);
        turnLeft(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint - 100;
        break;
      }
      else if (TYPE == 2 && action == 22 && val1 == 0 && (val2 == 1 || val3 == 1) && val5 == 1) { // right junction, turn right
        delay(delay_b4_turn);
        turnRight(turn_speed, turn_speed);
        delay(turn_duration);
        IR_position = setPoint + 100;
        break;
      }
      else if (TYPE == 2 && action == 33 && val1 == 0 && (val2 == 1 || val3 == 1) && val5 == 1) { // right junction, ignore
        while (val5 == 1) { // make sure robot passed junction
          if (analogRead(IR5) < IR5_avg - offsetIR) val5 = 0;
          else val5 = 1;
          if (line == blackLine);
          else if (line == whiteLine) val5 = !val5;
        }
        break;
      }
      else if (TYPE == 4 && action == 11) { // dont care, turn left
        if (countIgnore == delay_b4_turn) {
          delay(delay_b4_turn);
          //Stop(); delay(50);
          turnLeft(turn_speed, turn_speed);
          delay(turn_duration);
          //Stop(); delay(50);
          IR_position = setPoint - 100;
          break;
        }
        countIgnore += 1;
      }

      else if (TYPE == 4 && action == 22) { // dont care, turn right
        if (countIgnore == delay_b4_turn) {
          delay(delay_b4_turn);
          //Stop(); delay(50);
          turnRight(turn_speed, turn_speed);
          delay(turn_duration);
          //Stop(); delay(50);
          IR_position = setPoint + 100;
          break;
        }
        countIgnore += 1;
      }

      else if (TYPE == 4 && action == 33 && delay_b4_turn != -1) { // dont care, ignore
        if (countIgnore == delay_b4_turn) {
          break;
        }
        countIgnore += 1;
      }
      else if (TYPE == 4 && action == 33 && delay_b4_turn == -1) { // dont care, loop forever
        // loop forever
      }
    }
    else { // robot is OFF line (trace back)
      if (IR_position < setPoint) { // last position is on the LEFT before lost
        LED(0, 0, 1);
        if (trace_back != 0)
          turnLeft(trace_back, trace_back);
        else
          forward(0, speed_M);
      }
      else {
        //else if (IR_position >= setPoint) { // last position is on the RIGHT before lost
        LED(1, 0, 0);
        if (trace_back != 0)
          turnRight(trace_back, trace_back);
        else
          forward(speed_M, 0);
      }
    }
  }
  while (1);
}

void obstacle(int speed_M, int trace_back, unsigned int distance, int action, int delay_b4_turn, int turn_speed, int turn_duration, int line, int offsetIR) {
  out_min = -speed_M;
  out_max = speed_M;
  do {
    update_sensor(line, offsetIR);    //int valSum = val1 + val2 + val3 + val4 + val5;
    int valSum = val1 + val2 + val3 + val4 + val5;
    if (valSum > 0) { // robot is ON line
      //IR_position = (val1 * 0 + val2 * 2000 + val3 * 2500 + val4 * 3000 + val5 * 5000) / valSum; // calculate line state value
      IR_position = (val1 * 0 + val2 * 2000 + val3 * 2500 + val4 * 3000 + val5 * 5000) / valSum;
      followLine(speed_M, speed_M);

      if (hcsr04.distanceInMillimeters() <= distance && action == 11) {
        Stop(); delay(delay_b4_turn);
        turnLeft(turn_speed, turn_speed);
        delay(turn_duration);
        Stop(); //delay(50);
        //break;
      }
      else if (hcsr04.distanceInMillimeters() <= distance && action == 22) {
        Stop(); delay(delay_b4_turn);
        turnRight(turn_speed, turn_speed);
        delay(turn_duration);
        Stop(); //delay(50);
        //break;
      }
    }
    else { // robot is OFF line (trace back)
      if (IR_position < setPoint) { // last position is on the LEFT before lost
        LED(0, 0, 1);
        if (trace_back != 0)
          turnLeft(trace_back, trace_back);
        else
          forward(0, speed_M);
      }
      else {
        //else if (IR_position >= setPoint) { // last position is on the RIGHT before lost
        LED(1, 0, 0);
        if (trace_back != 0)
          turnRight(trace_back, trace_back);
        else
          forward(speed_M, 0);
      }
    }
  }
  while (1);
}

void back_to_line (int speedL, int speedR, int line, int offsetIR) { // while going back to line
  while (1) {
    update_sensor(line, offsetIR);
    int valSum = val1 + val2 + val3 + val4 + val5;
    if (valSum > 0) { // back to line
      Stop(); delay(50);
      break;
    }
    else { // approaching line
      forward(speedL, speedR);
    }
  }
}

unsigned long start_ms = 0;
void bot_setup(int calibrate_time, int auto_calibrate) {
  //****************** OLED setup ******************//
#ifdef OLED
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
#endif
  //****************** OLED setup ******************//

  //****************** robot setup ******************//
  pinMode(pwmL1, OUTPUT);  digitalWrite(pwmL1, LOW);
  pinMode(pwmL2, OUTPUT);  digitalWrite(pwmL2, LOW);
  pinMode(pwmR1, OUTPUT);  digitalWrite(pwmR1, LOW);
  pinMode(pwmR2, OUTPUT);  digitalWrite(pwmR2, LOW);
  pinMode(led_r, OUTPUT); digitalWrite(led_r, LOW);
  pinMode(led_y, OUTPUT); digitalWrite(led_y, LOW);
  pinMode(led_g, OUTPUT); digitalWrite(led_g, LOW);
  pinMode(battMon_en, OUTPUT); digitalWrite(battMon_en, HIGH);
  //****************** robot setup ******************//

  //****************** check battery ******************//
#ifdef OLED
  oledDisplay(2, 0, 0);
  display.println("  Battery ");
  display.println("----------");
#endif
  float battVolt = 0;
  for (int i = 0; i < 100; i++) {
    battVolt += analogRead(battMon_pin) * 2.89 * 5.00 / 1023.00;
  }
  int8_t battPercent = (battVolt - 660) / (840 - 660) * 100;
  if (battPercent < 0)
    battPercent = 0;
  if (battPercent > 80) {
#ifdef OLED
    display.print(battPercent); display.println(" %");
    display.print("      Full");
    display.display();
#endif
    LED(0, 0, 1);
  }
  else if (battPercent <= 80 && battPercent > 30) {
#ifdef OLED
    display.print(battPercent); display.println(" %");
    display.print("    Normal");
    display.display();
#endif
    LED(0, 1, 0);
  }
  else if (battPercent <= 30 && battPercent > 0) {
#ifdef OLED
    display.print(battPercent); display.println(" %");
    display.print("       Low");
    display.display();
#endif
    LED(1, 0, 0);
  }
  else {
#ifdef OLED
    display.print(battPercent); display.println(" %");
    display.print("     Empty");
    display.display();
#endif
    while (1) {
      LED(1, 0, 0);
      delay(100);
      LED(0, 0, 0);
      delay(150);
    }
  }
  delay(1000);
  //****************** check battery ******************//

  calibrateIR(calibrate_time, auto_calibrate);

  //****************** tell user ready to go ******************//
#ifdef OLED
  oledDisplay(2, 0, 0);
  display.println("All system");
  display.println("");
  display.println("  READY!  ");
  display.display();
  delay(1000);
  oledDisplay(2, 0, 0);
  display.println("All system");
  display.println("");
  display.println("  ..GO..  ");
  display.display();
  delay(500);
#endif
  //****************** tell user ready to go ******************//

  start_ms = millis();
}

void display_finishTime() {
  //****************** display finish time ******************//
  //long stop_ms = millis();
  long ms = millis() - start_ms;
  int second = 0, minute = 0;
  while (ms >= 1000) {
    second += 1;
    ms -= 1000;
  }
  while (second >= 60) {
    minute += 1;
    second -= 60;
  }
#ifdef OLED
  oledDisplay(3, 0, 0);
  display.println("FINISH!");
  display.setTextSize(2);
  //display.println("\n  " + String(minute) + ":" + String(second) + ":" + String(ms / 10));
  display.print("\n ");
  if (minute < 10) {
    display.print("0"); display.print(minute); display.print(":");
  }
  else {
    display.print(minute); display.print(":");
  }
  if (second < 10) {
    display.print("0"); display.print(second); display.print(":");
  }
  else {
    display.print(second); display.print(":");
  }
  if (ms / 10 < 10) {
    display.print("0"); display.print(ms / 10); display.print("");
  }
  else {
    display.print(ms / 10); display.print("");
  }
  display.display();
#endif
  //****************** display finish time ******************//

  while (1);
}
