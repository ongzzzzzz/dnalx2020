const int trigPin = 2;
const int echoPin = 4;
const int LED = 13;

long duration;
int distance;
float ultrasonicSpeed = 0.034; 
//speed of sound = 340m/s = 0.034cm/microseconds

void setup(){
  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*ultrasonicSpeed/2;
  
  //divide by 2, because the wave travels a round trip, to and from the object
  Serial.println("Distance: " + String(distance));

  if(distance <= 5){
    digitalWrite(LED, HIGH);
  } else{
    digitalWrite(LED, LOW);
  }
}
