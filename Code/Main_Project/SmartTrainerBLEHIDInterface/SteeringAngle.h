#ifndef SteeringAngle_h
#define SteeringAngle_h



#include "Arduino.h"

#define  FILETERALPHA 0.99

class SteeringAngle
{

  public:


    SteeringAngle(int _steeringPotPin);
    void begin();
    void loop();

    float getSteeringAngle();
    
  private:

  int steeringPotPin;
  
  //Calibration values for steering angle
  uint16_t centreVal = 4096/2;//2259;
  float    DegPerVal = -0.021;//0.032787;
  float    steeringAngle = 0;
  float    steeringAngle_old = 0;
  void countdown(int seconds);
};


#endif
