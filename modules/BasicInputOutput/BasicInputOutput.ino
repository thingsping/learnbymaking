#define PB_PIN 3
#define LED_PIN D4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  pinMode(LED_PIN, OUTPUT); 
  pinMode(PB_PIN, INPUT) ; 
  digitalWrite(LED_PIN, HIGH) ; 

}

void loop() {
  // put your main code here, to run repeatedly:
  int pbState = digitalRead(PB_PIN); 
  if (pbState == 0) {
    digitalWrite(LED_PIN, LOW) ;   
    Serial.println("Push button pressed! Stop blinking...") ; 
  }
  else {
    digitalWrite(LED_PIN, HIGH) ; 
    delay(500); 
    digitalWrite(LED_PIN, LOW) ; 
    delay(500); 
  }
  delay(100) ; 
}
