int LED3 = 13, LED2 = 1, LED1 = 0;
int ir1 = A1, ir2 = A2; 
int LEDs[] = {LED1, LED3, LED2}; 
int IRs[] = {ir1, ir2};

int valueOffset0 = 175;
int whiteState0 = 20 + valueOffset0;
int valueOffset1 = 300;
int whiteState1 = 20 + valueOffset1;

void setup(){
  for(int i=0; i<3; i++){
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], LOW);
  }
  for(int j=0; j<2; j++){
    pinMode(IRs[j], INPUT);
  }
  Serial.begin(9600);
}
void loop(){
  int state0 = analogRead(IRs[0]);
  int state1 = analogRead(IRs[1]);

  Serial.print("first ir -> " + String(state0));
  Serial.print("    ");
  Serial.print("second ir -> " + String(state1));
  Serial.println("");
  
  if(state0 < whiteState0){
    digitalWrite(LEDs[0], HIGH);
  } else { 
    digitalWrite(LEDs[0], LOW); 
  }
  
  if(state1 < whiteState1){
    digitalWrite(LEDs[1], LOW);
  } else{ 
    digitalWrite(LEDs[1], HIGH); 
  }
}
