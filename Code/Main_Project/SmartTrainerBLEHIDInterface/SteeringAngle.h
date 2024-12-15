#ifndef SteeringAngle_h
#define SteeringAngle_h



#include "Arduino.h"

#define  ANGLEFILETERALPHA 0.97
#define  RESCALEFILETERALPHA 0.01

class SteeringAngle
{

  public:


    SteeringAngle(int _steeringPotPin,int _steeringRescalePotPin);
    void begin();
    void loop();

    float getSteeringAngle();
    
  private:

  int steeringPotPin;
  int steeringRescalePotPin;
  
  //Calibration values for steering angle
  uint16_t centreVal = 4096/2;//2259;
  float    DegPerVal = 0.032787;//-0.021;//
  float    steeringAngle = 0;
  float    steeringAngle_old = 0;
  float    steeringRescaleFactor = 0.0;
  float    steeringRescaleFactor_old = 0;

  void countdown(int seconds);
};


#endif
