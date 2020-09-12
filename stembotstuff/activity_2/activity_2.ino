int green = 0, yellow = 1, red = 13;

int LEDPins[] = {red, yellow, green};
int index = 0;

void setup( ) {

pinMode(green,OUTPUT);

pinMode(yellow,OUTPUT);

pinMode(red,OUTPUT);

}

void loop( ) {

  for(int i = 0; i<3; i++){
    for(int j=0;j<3;j++){
      digitalWrite(LEDPins[j],LOW);
    }
    digitalWrite(LEDPins[i],HIGH);
    delay(1000);

  }


}
