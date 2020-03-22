#define BUZZER_PIN D3

int initialSmokeLevel ; 
void setup() {
  // put your setup code here, to run once:
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(9600);
  delay(60000); 
  initialSmokeLevel = analogRead(A0) ; 
}

void loop() {
  // put your main code here, to run repeatedly:
  int smokeLevel = analogRead(A0);  
  Serial.println("initial level = " + String(initialSmokeLevel) + "; current = " + String(smokeLevel));
  if(smokeLevel - initialSmokeLevel >= 75){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  }else{
    digitalWrite(BUZZER_PIN, LOW);
  }
  delay(500);
}
