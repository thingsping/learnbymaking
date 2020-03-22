#define TILTPIN D5
#define BUZZERPIN D3

int count = LOW ; 
int oldstate = 0 ; 
void setup(){
    Serial.begin(9600);
    pinMode(TILTPIN, INPUT); 
    pinMode(BUZZERPIN, OUTPUT) ; 
    digitalWrite(BUZZERPIN, LOW); 
}

void loop() {
    int tilt = digitalRead(TILTPIN);
    if (tilt != oldstate) {
        Serial.println("Tilt detected - " + String(count++)); 
        digitalWrite(BUZZERPIN, HIGH ); 
        delay(200); // Debounce time
        oldstate = tilt ; 
    }
    else {
      digitalWrite(BUZZERPIN, LOW); 
    }
}
