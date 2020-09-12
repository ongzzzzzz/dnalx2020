int LED1 = 0, LED2 = 1, LED3 = 13;

void setup( ) {

pinMode(LED1,OUTPUT);

pinMode(LED2,OUTPUT);

pinMode(LED3,OUTPUT);
}

void loop ( ) {

    digitalWrite(LED1,HIGH);

    digitalWrite(LED2,HIGH);

    digitalWrite(LED3,HIGH);
}
