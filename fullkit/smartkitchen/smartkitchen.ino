 #include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <DHT.h>


const char* WIFI_SSID = "yourssid";
const char* WIFI_PASSWORD = "ssid_password";
const char* FIREBASE_HOST = "your firebase project name";
const char* FIREBASE_AUTH  = "your firebase auth code" ; 


#define BUZZER_PIN D3
#define DHT_PIN D6
#define MQ2_PIN A0 
#define PIR_PIN D5
#define RELAY_PIN1 D0
#define RELAY_PIN2 D4

DHT dhtSensor(DHT_PIN, DHT11);

boolean isAuto = false;
int initialSmokeLevel ; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN1,OUTPUT);
  pinMode(RELAY_PIN2,OUTPUT);  
  digitalWrite(RELAY_PIN1, HIGH);
  digitalWrite(RELAY_PIN2, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  Firebase.wiconnect(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  delay(60000); 
  initialSmokeLevel = analogRead(A0) ; 
}

void loop() {
  // put your main code here, to run repeatedly:
  int temperature = dhtSensor.readTemperature();
  int smokeValue = analogRead(MQ2_PIN);

  Firebase.setInt("smokelevel", smokeValue);
  Firebase.setInt("temperature", temperature);
  Serial.println("Smoke level = " + String(smokeValue) + "; Initial level = " + 
      String(initialSmokeLevel) + "; Temperature = " + temperature) ; 
      
  bool isSmoke = false; 
  bool isFan = false ; 
  
  if(smokeValue - initialSmokeLevel >= 75){
    isSmoke = true ;     
    isFan = true ; 
  }
  if(temperature > 28){
    isFan = true ; 
    if(temperature > 30){
      isSmoke = true ; 
    }
   }

  if (isSmoke) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  if (isFan) {
    digitalWrite(RELAY_PIN2, LOW);
    Firebase.setString("fanstate","on");
  } else {
    digitalWrite(RELAY_PIN2, HIGH);
    Firebase.setString("fanstate","off");
  }
  
  String modeString = Firebase.getString("mode");
  if (modeString.equals("manual")){
   isAuto = false;
  } else {
   isAuto = true;
  }
  if (isAuto){
    int isMotion = digitalRead(PIR_PIN);
    if(isMotion){
      Serial.println("Motion detected! Turning on Light.") ;
      digitalWrite(RELAY_PIN1, LOW);
      Firebase.setString("lightstate","on");
    }else{
      Serial.println("NO Motion detected! Switching off light") ;
      digitalWrite(RELAY_PIN1, HIGH);
      Firebase.setString("lightstate","off");
    } 
  } else {  // If manual mode
    // Motion sensor should only be used to turn on or turn off light. 
    // Exhaust fan is for safety also and hence should only be turned on or off based
    // on temperature / smoke
    String lightState = Firebase.getString("lightstate");
    if (lightState.equals("on")){
      digitalWrite(RELAY_PIN1, LOW);
    } else {
      digitalWrite(RELAY_PIN1, HIGH);
    }
  }
}
