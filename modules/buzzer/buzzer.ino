#define BUZZER_PIN D3
#define PB_PIN 3

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600); 
  pinMode(BUZZER_PIN, OUTPUT) ; 
  pinMode(PB_PIN, INPUT_PULLUP) ; 
  digitalWrite(BUZZER_PIN, LOW) ; 

}

void loop() {
  // put your main code here, to run repeatedly:
  int pinStatus = digitalRead(PB_PIN) ; 
  //Serial.println("Pin status = " + String(pinStatus)); 
  if (pinStatus == 0) {
    digitalWrite(BUZZER_PIN, HIGH) ; 
  } else {
    digitalWrite(BUZZER_PIN, LOW) ; 
  }
}
