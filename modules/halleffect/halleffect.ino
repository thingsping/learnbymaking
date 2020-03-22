#define HALLPIN D7
#define BUZZERPIN D3

void setup(){
    Serial.begin(9600);
    pinMode(HALLPIN, INPUT); 
    pinMode(BUZZERPIN, OUTPUT) ; 
    digitalWrite(BUZZERPIN, LOW) ; 
}

void loop() {
    int mgt = digitalRead(HALLPIN);
    if (mgt == LOW){
        Serial.println("Magnet detected."); 
        digitalWrite(BUZZERPIN, HIGH) ; 
        delay(200);
    }
    else {      
        digitalWrite(BUZZERPIN, LOW) ; 
    }
}
