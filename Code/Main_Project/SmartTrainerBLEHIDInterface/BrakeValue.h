#ifndef BrakeValue_h
#define BrakeValue_h

#include "Arduino.h"

#define  FILETERALPHA 0.97

class BrakeValue
{
  public:

    BrakeValue(int _Braking_pin);
    void begin();
    void loop();

    float getBrakingPerc();
    
  private:

  int BrakingPin;
  
  //Calibration values for breaking value in percantage
  uint16_t min_val = 1010;
  uint16_t max_val = 3165;
  float    brakingperc = 0;
  float    brakingperc_old = 0;
  void countdown(int seconds);
};


#endif
