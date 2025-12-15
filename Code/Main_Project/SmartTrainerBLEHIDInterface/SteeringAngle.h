#ifndef SteeringAngle_h
#define SteeringAngle_h



#include "Arduino.h"
#include "ScreenControl.h"

#define  ANGLEFILETERALPHA 0.7
#define  RESCALEFILETERALPHA 0.01

class SteeringAngle
{

  public:
    SteeringAngle(int _steeringPotPin,int _steeringRescalePotPin);
    void begin(bool calibtration,ScreenControl *screen);
    //void loop();

    float getSteeringAngle_pot();
    float getSteeringAngle_magnet();
    
  private:

  ScreenControl* screen;

  void loop();
  int steeringPotPin;
  int steeringRescalePotPin;
  
  //Calibration values for steering angle
  uint16_t centreVal = 1767;//2259;
  float DegPerVal = 0.036765;//-0.021;//
  float steeringAngle = 0;
  float steeringAngle_old = 0;
  float steeringRescaleFactor = 0.0;
  float steeringRescaleFactor_old = 0;

  float degAngleMagnet0; //raw angle in degrees (360/4096 * [value between 0-4095])
  float degAngleMagnetmin1;

  void countdown(int seconds,ScreenControl *screen);
};


#endif
