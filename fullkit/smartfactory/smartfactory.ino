#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <DHT.h>
#include "myconstants.h"


#define BUZZER_PIN D3
#define DHT_PIN D6
#define RELAY_PIN1 D0
#define HALL_PIN D7
#define TILT_PIN D5

DHT dhtSensor(DHT_PIN, DHT11);

int prod=0; 
int vibrations=0 ; 
long startTime; 
long lastVibrationTime ; 
long lastProductionTime ; 

void ICACHE_RAM_ATTR productionHandler(){
    if ((millis() - lastProductionTime) >= 200) {
      prod++; 
      lastProductionTime = millis() ; 
      //Serial.println("Production=" + String(prod));   // Don't use serial.println statements in interrupts. Use 
          // when need to debug. Comment back immediately after that. 
    } // else - ignore any interrupt before debounce time.       
}

void ICACHE_RAM_ATTR vibrationsHandler() {
  if ((millis() - lastVibrationTime) >= 200) {
    vibrations++ ; 
    lastVibrationTime = millis() ; 
    //Serial.println("Vibrations = " + String(vibrations)) ; // Don't use serial.println statements in interrupts. Use 
          // when need to debug. Comment back immediately after that. 
  }   // else - ignore any interrupt before debounce time.
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZER_PIN,OUTPUT);  
  pinMode(RELAY_PIN1,OUTPUT); 
  pinMode(HALL_PIN, INPUT); 
  pinMode(TILT_PIN, INPUT); 
  digitalWrite(RELAY_PIN1, LOW);  // The Relay should be ON all the time
  digitalWrite(BUZZER_PIN, LOW);

  /*
  const char* WIFI_SSID = "yourssid";
  const char* WIFI_PASSWORD = "ssid_password";
  const char* FIREBASE_HOST = "your firebase project name";
  const char* FIREBASE_AUTH  = "your firebase auth code" ; 
  */
  Firebase.wiconnect(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  startTime = millis(); 
  lastVibrationTime = millis() ; 
  lastProductionTime = millis(); 
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), productionHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(TILT_PIN), vibrationsHandler, FALLING); 
}

void loop() {
  // put your main code here, to run repeatedly:
  double temperature = dhtSensor.readTemperature();

  if ((millis() - startTime) >=60000){
    startTime = millis(); 
    Serial.println("a minute has elapsed. Now writing to firebase") ; 
    Firebase.setInt("temperature", temperature);
    Firebase.setInt("vibrations per minute" , vibrations) ; 
    vibrations = 0 ; 
    Firebase.setInt("production", prod) ; 
    
  }  

  if (temperature >= 30) {
    digitalWrite(RELAY_PIN1, HIGH) ;   // SWITCH OFF THE MACHINE
    digitalWrite(BUZZER_PIN, HIGH) ; 
    Serial.println("Temperature = " + String(temperature)) ; 
    Firebase.setInt("temperature", temperature);
    delay(1000) ;  // Don't retry for one second atleast
  }else {
    digitalWrite(RELAY_PIN1, LOW) ; 
    digitalWrite(BUZZER_PIN, LOW) ; 
  }
}
