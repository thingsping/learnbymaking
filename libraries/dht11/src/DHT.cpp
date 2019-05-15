/* DHT library

MIT license
Initial code written by Adafruit Industries. 
Modified by Qantom Software

*/

#include "DHT.h"

#define MIN_INTERVAL 2000

DHT::DHT(uint8_t pin, uint8_t type) {
  _pin = pin;
  _type = type;
  _lastreadtime = millis()-MIN_INTERVAL; // The first call should always work!

  _maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for
                                                 // reading pulses from DHT sensor.
  // Note that count is now ignored as the DHT reading algorithm adjusts itself
  // basd on the speed of the processor.
}

//boolean isFahrenheit: True == Fahrenheit; False == Celcius
float DHT::computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit) {
  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  float hi;

  if (!isFahrenheit)
    temperature =  temperature * 1.8 + 32; 

  hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (percentHumidity * 0.094));

  if (hi > 79) {
    hi = -42.379 +
             2.04901523 * temperature +
            10.14333127 * percentHumidity +
            -0.22475541 * temperature*percentHumidity +
            -0.00683783 * pow(temperature, 2) +
            -0.05481717 * pow(percentHumidity, 2) +
             0.00122874 * pow(temperature, 2) * percentHumidity +
             0.00085282 * temperature*pow(percentHumidity, 2) +
            -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

    if((percentHumidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

    else if((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }

  return isFahrenheit ? hi :  (hi - 32) * 0.55555; 
}

//boolean S == Scale.  True == Fahrenheit; False == Celcius
// Default values of both are true. 
float DHT::readTemperature(bool S, bool force) {
  float f = NAN;

  if (read(force)) {
    switch (_type) {
    case DHT11:
      f = data[2];
      break;
    case DHT22:
    case DHT21:
      f = data[2] & 0x7F;
      f *= 256;
      f += data[3];
      f *= 0.1;
      if (data[2] & 0x80) {
        f *= -1;
      }
      break;
    }
  }
  return f;
}

float DHT::readHumidity(bool force) {
  float f = NAN;
  if (read()) {
    switch (_type) {
    case DHT11:
      f = data[0];
      break;
    case DHT22:
    case DHT21:
      f = data[0];
      f *= 256;
      f += data[1];
      f *= 0.1;
      break;
    }
  }
  return f;
}

boolean DHT::read(bool force) 
{
  // Check if sensor was read less than two seconds ago and return early
  // to use last reading.
  uint32_t currenttime = millis();
  if (!force && ((currenttime - _lastreadtime) < 2000)) {
    return _lastresult; // return last correct measurement
  }
  _lastreadtime = currenttime;

  // Reset 40 bits of received data to zero.
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // Send start signal.  See DHT datasheet for full signal diagram:
  //   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

  // Added by Raghu:
  pinMode(_pin, OUTPUT); // For whatever reason, this statement was not there.
          // and that might have been the reason for the intermittent failures. 
  // Go into high impedence state to let pull-up raise data line level and
  // start the reading process.
  digitalWrite(_pin, HIGH);
  delay(250);

  // First set data line low for 20 milliseconds.
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(20); // As per BITS notes, it is 1 to 10 milliseconds. But didn't work that 
        // For DHT-22 it is 1 to 10 ms. For DHT-11 it is a minimum of 18 millisecond

  uint32_t cycles[80];
  {  // Begin a block to indicate time critical section 
    //InterruptLock lock; 
    // By Raghu - It was a nice trick he had used - To create a new instance of the 
    // InterruptLock class. In the constructor of this class, he calls the method 
    // noInterrupts. In the destructor of this class, he calls interrupts(). Thus 
    // what will happen is that in the above, statement the contructor will be 
    // called and when this block goes out of scope, then the destructor is  called.
    // A neat trick but here's the issue - We are depending on the stack / heap manage
    // ment feature. Instead it is better to implicitly call these two methods 

    // Turn off interrupts temporarily because the next sections are timing critical
    // and we don't want any interruptions.
    noInterrupts() ; 

    // End the start signal by setting data line high for 40 microseconds.
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30); // Original was 40

    // Now start reading the data line to get the value from the DHT sensor.
    pinMode(_pin, INPUT_PULLUP);
    
    // First expect a low signal for ~80 microseconds followed by a high signal
    // for ~80 microseconds again. Give a grace time of 10 microseconds to settle down. 
    if (expectPulse(LOW, 10) == 0) {
      //DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
      _lastresult = false;
      return _lastresult;
    }
    if (expectPulse(HIGH) == 0) {
      //DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
      _lastresult = false;
      return _lastresult;
    }

    // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
    // microsecond low pulse followed by a variable length high pulse.  If the
    // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
    // then it's a 1.  We measure the cycle count of the initial 50us low pulse
    // and use that to compare to the cycle count of the high pulse to determine
    // if the bit is a 0 (high state cycle count < low state cycle count), or a
    // 1 (high state cycle count > low state cycle count). Note that for speed all
    // the pulses are read into a array and then examined in a later step.
    for (int i=0; i<80; i+=2) {
      cycles[i]   = expectPulse(LOW);
      cycles[i+1] = expectPulse(HIGH);
    }

    // Turn back interrupts again
    interrupts() ; 
  } // Timing critical code is now complete.

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (int i=0; i<40; ++i) {
    uint32_t lowCycles  = cycles[2*i];
    uint32_t highCycles = cycles[2*i+1];
    if ((lowCycles == 0) || (highCycles == 0)) {
      //DEBUG_PRINTLN(F("Timeout waiting for pulse."));
      _lastresult = false;
      return _lastresult;
    }
    data[i/8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i/8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
    // Note by Raghu - Here he is cleverly checking only if the high pulse duration
    // is greater than the low puse duration. The low pulse duration is invariably 
    // 54 microseceonds. If data is high, thenthe pulse duration is 70 microseconds
    // If data is low, then the pulse duration is 24-28 microseconds.  He is not 
    // actually checking for the exact time. But seems to work so won't change anything. 
  }

  // Check we read 40 bits and that the checksum matches.
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) 
  {
    _lastresult = true;
    return _lastresult;
  }
  else {
    
    _lastresult = false;
    return _lastresult;
  }
}

// Expect the signal line to be at the specified level for a period of time and
// return a count of loop cycles spent at that level (this cycle count can be
// used to compare the relative time of two pulses).  
// This is adapted from Arduino's pulseInLong function (which is only available
// in the very latest IDE versions):
//   https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_pulse.c
// Added by Qantom:
// Specify a grace time in microseconds. Even if the state is not the expected
// level, TRY for upto this gracetime. 
// After the state becomes equal to the level parameter, count the cycle only till 
// a maximum of one millisecond. If the pin stays in the level for more than 1 millsecond
// return 0 to indicate an error. 
uint32_t DHT::expectPulse(bool level, uint8_t gracetime) {
  uint32_t count = 0;
  if (gracetime != 0 ) 
  {
    long starttime = micros() ; 
    // If the pin level is not the expected level wait till the grace time in microseconds
    while (digitalRead(_pin) != level && (micros() - starttime) <= gracetime ) {}
  }
  while (digitalRead(_pin) == level) {
    if (count++ >= _maxcycles) {
      return 0; // Exceeded timeout, fail.
    }
  }
  return count;
}
