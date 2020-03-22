#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

#include "myconstants.h"

#define BUZZER_PIN D3
#define PIR_PIN D5
#define RELAY_PIN1 D0
#define RELAY_PIN2 D4
#define IR_PIN D7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZER_PIN,OUTPUT);  
  pinMode(RELAY_PIN1,OUTPUT);  
  pinMode(RELAY_PIN2,OUTPUT); 
  pinMode(IR_PIN, INPUT); 
  pinMode(PIR_PIN, INPUT); 
  digitalWrite(RELAY_PIN1, HIGH); // Low trigger relays
  digitalWrite(RELAY_PIN2, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  /*
  const char* WIFI_SSID = "yourssid";
  const char* WIFI_PASSWORD = "ssid_password";
  const char* FIREBASE_HOST = "your firebase project name";
  const char* FIREBASE_AUTH  = "your firebase auth code" ; 
  */
  Firebase.wiconnect(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
}

void loop() {
  int analogValue = analogRead(A0); 
  int lightLevel = map(analogValue, 0, 1023, 1, 3) ; 
  bool isMotion = digitalRead(PIR_PIN); 
  bool isObstacle ; 
  int ir_read = digitalRead(IR_PIN);
  if ( ir_read == LOW){
    //IR_PIN becomes LOW when obstacle is detected
    isObstacle = true; 
  } else {
    isObstacle = false;
  }

  /*
  Serial.println("Light = " + String(analogValue) + ";" 
       + String(lightLevel) + 
      "; Motion = " + String(isMotion) + 
      "; isObstacle = " + String(isObstacle)
      ); 
      */
  if (lightLevel == 1 ){ // means it is night
    if (isMotion || isObstacle){ 
      Serial.println("Night time activity; motion = " 
      + String(isMotion) + ";light=" +String(analogValue) 
      + ";" + String(lightLevel)) ; 
      Firebase.setString("lightstate", "intruder") ; 
      digitalWrite(RELAY_PIN1, LOW) ; // TURN ON THE LIGHT
      for (int i = 0 ; i < 3; i ++){
        digitalWrite(BUZZER_PIN, HIGH); 
        delay(500);
        digitalWrite(BUZZER_PIN, LOW); 
        delay(200); 
      }
      Firebase.setString("Warning", "Activity detected @ " + String(millis()) + "milliseconds"); 
    }
  }

  if (isObstacle){
    Serial.println("Ringing doorbell") ; 
    for (int i = 0 ; i < 3; i ++){
      digitalWrite(RELAY_PIN2, LOW);       
      delay(200); // Note : Calling bells will only require you to press the switch and release it immediately. 
      digitalWrite(RELAY_PIN2, HIGH);  
    }
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW); 
  }
  // Switching on or switching off the light can only be done based 
  // on the flag set in the firebase database. 
  String lightstate = Firebase.getString("lightstate"); 
  if (lightstate.equals("on")) {
    Serial.println("Cloud wants me to turn on light") ; 
    digitalWrite(RELAY_PIN1, LOW); 
  }else if (lightstate.equals("off")){
    digitalWrite(RELAY_PIN1, HIGH); 
  }



}
