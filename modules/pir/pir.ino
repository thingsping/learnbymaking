#define BUZZER_PIN D3
#define  PIR_PIN D5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT) ; 
  pinMode(BUZZER_PIN, OUTPUT) ; 
  digitalWrite(BUZZER_PIN, LOW) ;  
}

void loop() {
  int pbState = digitalRead(PIR_PIN) ; 
  if (pbState == 1) {
    digitalWrite(BUZZER_PIN, HIGH) ; 
    Serial.println("Motion detected..."); 
    delay(500);
  } else {
    digitalWrite(BUZZER_PIN, LOW) ;  
  }
}
