#define BUZZER_PIN D3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  pinMode(BUZZER_PIN, OUTPUT) ; 
  digitalWrite(BUZZER_PIN, LOW) ; 
}

void loop() {
  int lightLevel = analogRead(A0); 
  Serial.print("Light level = "); 
  Serial.println(lightLevel) ; 
  if (lightLevel > 512) {
    digitalWrite(BUZZER_PIN, HIGH) ; 
  } else {
    digitalWrite(BUZZER_PIN, LOW) ; 
  }
  delay(1000);

}