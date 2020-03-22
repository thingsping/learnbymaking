#include <FirebaseCloudMessaging.h>
#include <FirebaseArduino.h>
#include <Firebase.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <FirebaseError.h>
#include <DHT.h>

// Remove next line (myconstants) and uncomment the 
// Auth lines when submitting to github
#include "myconstants.h"
/*
const char* WIFI_SSID = "yourssid";
const char* WIFI_PASSWORD = "ssid_password";
const char* FIREBASE_HOST = "your firebase project name";
const char* FIREBASE_AUTH  = "your firebase auth code" ; 
*/

#define PB_PIN 3 // Rx
#define LED_PIN D4
#define BUZZER_PIN D3
#define PIR_PIN D5
#define DHT_PIN D6
#define RELAY_PIN1 D0
#define RELAY_PIN2 D4

boolean is_auto = false ; 
DHT dhtSensor(DHT_PIN, DHT11) ; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PB_PIN, INPUT_PULLUP) ; 
  //pinMode(LED_PIN, OUTPUT) ; // In actual tests we don't need onboard LED. 
  pinMode(BUZZER_PIN, OUTPUT) ; 
  pinMode(PIR_PIN, INPUT) ; 
  pinMode(RELAY_PIN1, OUTPUT) ; 
  pinMode(RELAY_PIN2, OUTPUT) ; 
  digitalWrite(RELAY_PIN1, HIGH) ; 
  digitalWrite(RELAY_PIN2, HIGH) ; 
  digitalWrite(BUZZER_PIN, LOW) ; 
  
  Firebase.wiconnect(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH) ;

  String modeString = Firebase.getString("mode") ; 
  if (modeString.equals("manual")){
    is_auto = false ; 
  } else {
    is_auto = true ; 
  }
  Serial.println("Auto?" + String(is_auto)) ; 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (is_auto) {
    int pirState = digitalRead(PIR_PIN) ; 
    if (pirState) {
      Serial.println("Motion detected! Now doing other checks") ; 
      int ldrValue = analogRead(A0) ; 
      int lightLevel = map(ldrValue, 0, 1023, 0, 5) ; 
      int temperature = dhtSensor.readTemperature() ; 

      Firebase.setInt("lightlevel", lightLevel); 
      Firebase.setInt("temperature", temperature) ; 
      if (lightLevel < 3) {
        digitalWrite(RELAY_PIN1, LOW) ; // Low Trigger Relay
        Firebase.setString("lightstate", "on") ;  
      } else {
        digitalWrite(RELAY_PIN1, HIGH) ; // Low Trigger Relay
        Firebase.setString("lightstate", "off") ;
      }
      if (temperature > 28) {
        digitalWrite(RELAY_PIN2, LOW) ; // Low Trigger Relay
        Firebase.setString("fanstate", "on") ;  
        if (temperature > 30) {
          for (int i = 0 ; i < 3; i++) {
            digitalWrite(BUZZER_PIN, HIGH) ; 
            delay(200); 
            digitalWrite(BUZZER_PIN, LOW) ; 
            delay(200) ; 
          }
        }
      } else {
        digitalWrite(RELAY_PIN2, HIGH) ; // Low Trigger Relay
        Firebase.setString("fanstate", "off") ;  
      }
      Serial.println("Light level = " + String(lightLevel) + "; Temperature = " + temperature) ; 
      
    }else {
      Serial.println("NO Motion detected! Switching off light and fan") ; 
      digitalWrite(RELAY_PIN1, HIGH) ;
      digitalWrite(RELAY_PIN2, HIGH) ;
      Firebase.setString("fanstate", "off") ;  
      Firebase.setString("lightstate", "off") ;  
      delay(500) ; 
    }
  }else { // If mode is set to manual. 
    String fanstate = Firebase.getString("fanstate"); 
    String lightstate = Firebase.getString("lightstate") ; 
    
    if (fanstate.equals("on")) {
      digitalWrite(RELAY_PIN2, LOW) ;
    } else {
      digitalWrite(RELAY_PIN2, HIGH) ;
    }
    
    if (lightstate.equals("on")){
      digitalWrite(RELAY_PIN1, LOW) ;
    }else {
      digitalWrite(RELAY_PIN1, HIGH) ;
    }
    
  }

}
