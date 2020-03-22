#include <LiquidCrystal_I2C.h>
#include <qlcddisplay.h>

#define BUZZERPIN D3
#define PBPIN 3
#define SDAPIN D2
#define SCLPIN D1

int ctr =0; 
QLcdDisplay qdisp(SDAPIN, SCLPIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUZZERPIN, OUTPUT) ; 
  pinMode(PBPIN, INPUT) ; 
  digitalWrite(BUZZERPIN, LOW) ; 
  qdisp.backlight_off(); 
  qdisp.displayText("Press the", "Push Button") ;
}

void loop() {
    // put your main code here, to run repeatedly:
    int pbstate = digitalRead(PBPIN) ; 
    if (pbstate == LOW) {
      ctr = ctr + 1; 
      qdisp.displayText("Count:", String(ctr)) ;
      Serial.println("Setting display to " + String(ctr)); 
      if (ctr %2 == 0) {
        qdisp.backlight_off(); 
      }else {
        qdisp.backlight_on(); 

      }
      
      delay(1000); 
    }
    
}
