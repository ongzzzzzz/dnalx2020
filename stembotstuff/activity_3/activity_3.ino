int ir1 = A7, ir2 = A6, ir3 = A3, ir4 = A2, ir5 = A1;

int irs[] = {ir1, ir2, ir3, ir4, ir5};

void setup(){
  for(int i=0; i<5; i++){
    pinMode(irs[i], INPUT);
  }
  Serial.begin(9600);
}

void loop(){
  for(int i=0;i<5;i++){
    String message = "  Sensor " + String(i+1) + ": ";
    Serial.print(message);
    Serial.print(analogRead(irs[i]));
    
    
  }
  Serial.println("");
  delay(100);
}
