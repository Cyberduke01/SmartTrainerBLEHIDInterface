#ifndef CapTouchControl_h
#define CapTouchControl_h
//I know its stupid, but break is defined as the right touch pad

#include "Arduino.h"
#include "ScreenControl.h"

class CapTouchControl
{
  public:
    CapTouchControl(int _touchPinBrake, int _touchPinBoost);
    void begin(bool calibtration,ScreenControl *screen);
    void loop();
    bool isBrakePressed();
    bool isBoostPressed();
    
  private:
    int touchPinBrake;
    int touchPinBoost;

    int aveTouchValueBrake;
    int aveTouchValueBoost;


    
    int numToAverage = 2;

    int TouchValuesBrake[50];
    int TouchValuesBoost[50];


    int Loopcount = 0;
    //bool NumToAverageLoopsReached = false;

    
    void countdown(int seconds,ScreenControl *screen);
    int touchValueMin_Brake = 64528;//47922; // Change these values for brake calibration!!!!
    int touchValueMax_Brake = 166000;//179104;
    int touchThreshold_Brake = touchValueMax_Brake-touchValueMin_Brake/2.0;

    int touchValueMin_Boost = 76010;//46484; // Change these values for brake calibration!!!!
    int touchValueMax_Boost = 158094;//178912;
    int touchThreshold_Boost = touchValueMax_Boost-touchValueMin_Boost/2.0;

};


#endif
