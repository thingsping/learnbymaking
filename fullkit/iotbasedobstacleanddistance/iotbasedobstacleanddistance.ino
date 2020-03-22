#include <myconstants.h>

#include <FirebaseCloudMessaging.h>
#include <FirebaseArduino.h>
#include <Firebase.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <FirebaseError.h>

#include <LiquidCrystal_I2C.h>
#include <qlcddisplay.h>

#define BUZZERPIN D3
#define SDAPIN D2
#define SCLPIN D1
#define IRPIN D7
#define TRIGGERPIN D8 
#define ECHOPIN D6

QLcdDisplay qdisp(SDAPIN, SCLPIN);

void setup() {
  Serial.begin(9600); 
  pinMode(BUZZERPIN, OUTPUT); 
  pinMode(IRPIN, INPUT) ; 
  pinMode(TRIGGERPIN, OUTPUT) ; 
  pinMode(ECHOPIN, INPUT) ; 
  digitalWrite(BUZZERPIN, LOW) ; 

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
    int obst = digitalRead(D7) ; 
    bool isObst ; 
    if (obst == LOW) {
        isObst = true; 
    }else {
        isObst = false ; 
    }
   // Find the distance
    double distance ;
    digitalWrite(TRIGGERPIN, LOW);
    delayMicroseconds(3);
    digitalWrite(TRIGGERPIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(TRIGGERPIN, LOW);
    long duration = pulseIn(ECHOPIN, HIGH); 
      // pulseIn returns the time in microseconds for the
      // echoPin to become high
    // speed of sound = 348.83 m/s 
    // distance = speed * time  => 348.83 * t / 10^6
    //    = 0.348 * t/10^3 = 0.000348 * t in meters
    //  = 0.0348 * t cms. 
    distance = duration * 0.0348 ;
    distance = distance / 2; // half of roundtrip time

    Serial.println("Distance=" + String(distance) + "; obst = " + String(isObst)); 
    Firebase.setBool("Obstacle",  isObst); 
    Firebase.setFloat("Distance", distance) ; 

    String line1 = "Obstacle? " + String(isObst); 
    String line2 = "Distance = " + String(distance); 
    qdisp.displayText(line1, line2); 
   
}
