#define PB_PIN 3 
#define RELAY_PIN1 D0
#define RELAY_PIN2 D4

void setup() {
  // put your setup code here, to run once:
  pinMode(PB_PIN, INPUT_PULLUP) ; 
  pinMode(RELAY_PIN1, OUTPUT) ; 
  pinMode(RELAY_PIN2, OUTPUT); 
  digitalWrite(RELAY_PIN1, HIGH) ; 
  digitalWrite(RELAY_PIN2, HIGH) ;  
}

void loop() {
  int pbState = digitalRead(PB_PIN) ; 
  if (pbState == 0) {
    digitalWrite(RELAY_PIN1, LOW) ; 
    delay(500); 
    digitalWrite(RELAY_PIN2, LOW) ; 
    delay(500) ; 
  } else {
    digitalWrite(RELAY_PIN1, HIGH) ; 
    digitalWrite(RELAY_PIN2, HIGH) ; 
  }
}
