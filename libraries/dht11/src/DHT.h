/* DHT library

MIT license
written by Adafruit Industries

@15th Feb. 2019 - Modified by Qantom software
1. Removed all parts of code pertaining to AVR - we will only use ESP
2. Removed all DEBUG related defines. We will use our logger. 
3. Removed the begin method. 
4. Removed the cTOf and fTOc method - they are trivial computations and 
   should have actually been static methods anyways. 
5. Converted computeHeatIndex into a static method - It is not an instance 
method of the DHT sensor class as such - it is a utility method and hence statid
makes more sense 
6. Made read() method into a private method. There was no need for this to be 
public
7. added a gracetime parameter to expectPulse() method. value defaults to zero. 
8. Removed out the InterruptLock class 

*/
#ifndef DHT_H
#define DHT_H

#include "Arduino.h"

// Define types of sensors.
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21


class DHT {
  public:
    DHT(uint8_t pin, uint8_t type);
    static float computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit=false);

    float readTemperature(bool S=false, bool force=false);
    float readHumidity(bool force=false);

 private:
  uint8_t data[5];
  uint8_t _pin, _type;
  uint32_t _lastreadtime, _maxcycles;
  bool _lastresult;
  uint32_t expectPulse(bool level, uint8_t gracetime=0);
  
  boolean read(bool force=false);

};

// We don't use this class anymore 
/*
class InterruptLock {
  public:
   InterruptLock() {
    noInterrupts();
   }
   ~InterruptLock() {
    interrupts();
   }

};
*/

#endif
