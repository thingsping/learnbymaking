#define TRIGGERPIN D8 
#define ECHOPIN D6

void setup() {
  Serial.begin(9600); 
  pinMode(TRIGGERPIN, OUTPUT) ;
  pinMode(ECHOPIN, INPUT) ;
}

void loop() {
   // Find the distance
    double distance ;
    digitalWrite(TRIGGERPIN, LOW);
    delayMicroseconds(3);
    digitalWrite(TRIGGERPIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIGGERPIN, LOW);
    long duration = pulseIn(ECHOPIN, HIGH); 
      // pulseIn returns the time in microseconds for the
      // echoPin to become high
    // speed of sound = 348.83 m/s 
    // distance = speed * time  => 348.83 * t / 10^6
    //    = 0.348 * t/10^3 = 0.000348 * t in meters
    //  = 0.0348 * t cms. 
    distance = duration * 0.0348 ;
    distance = distance / 2; // half of roundtrip time

    Serial.println("Distance=" + String(distance)); 
    delay(1000);
}
