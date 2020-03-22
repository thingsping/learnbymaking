 #include <DHT.h>

//https://thingsping.in/libraries/thingspingdht.zip
#define BUZZER_PIN D3 
#define DHT_PIN D6

DHT dhtSensor(DHT_PIN, DHT11) ; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 

  pinMode(BUZZER_PIN, OUTPUT) ; 
  digitalWrite(BUZZER_PIN, LOW) ;  
}

void loop() {
  float temperature;
  temperature = dhtSensor.readTemperature() ; 
  Serial.print("Temperature :") ; 
  Serial.println(temperature) ; 
  if (temperature >= 28) {
    digitalWrite(BUZZER_PIN, HIGH) ; 
  } else {
    digitalWrite(BUZZER_PIN, LOW) ;  
  }
  delay(2000) ; 
}
