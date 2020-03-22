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
#define PBPIN 3
#define SDAPIN D2
#define SCLPIN D1

QLcdDisplay qdisp(SDAPIN, SCLPIN);


String backlight;
String line1; 
String line2;

void setup(){
  Serial.begin(9600); 
  pinMode(BUZZERPIN, OUTPUT) ; 
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
  String curLine1 = Firebase.getString("line1"); 
  String curLine2 = Firebase.getString("line2"); 
  String backlightstr = Firebase.getString("backlight") ; 

  if (curLine1.equals(line1) && curLine2.equals(line2)) {
    Serial.println("No change in display. Moving on...") ; 
  }else {
    line1 = curLine1; 
    line2 = curLine2 ; 
    qdisp.displayText(line1, line2); 
    digitalWrite(BUZZERPIN, HIGH); 
    delay(500) ; 
    digitalWrite(BUZZERPIN, LOW);     
  }

  if (backlightstr.equals("on")){
    qdisp.backlight_on(); 
  }else if(backlightstr.equals("off")){
    qdisp.backlight_off(); 
  }else if(backlightstr.equals("auto")){
    int ar = analogRead(A0) ; 
    int lightlevel = map(ar, 0, 1023, 1, 4); 
    Serial.println("Light level = " + String(ar) + ";" + String(lightlevel)); 
    if (lightlevel <3){
      qdisp.backlight_on(); 
    } else {
      qdisp.backlight_off(); 
    }
  }
  delay(1000) ; 
}
