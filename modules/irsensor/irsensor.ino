#define IRPIN D7
#define BUZZERPIN D3

void setup(){
    Serial.begin(9600);
    pinMode(IRPIN, INPUT);
    pinMode(BUZZERPIN, OUTPUT) ;
    digitalWrite(BUZZERPIN, LOW) ;
}

void loop() {
    int obstacle = digitalRead(IRPIN);
    if (obstacle == LOW){
        Serial.println("Obstacle detected."); 
        digitalWrite(BUZZERPIN, HIGH) ; 
    }
    else {      
        digitalWrite(BUZZERPIN, LOW) ; 
    }
    delay(200);
}
