#ifndef CapTouchControl_h
#define CapTouchControl_h

#include "Arduino.h"
#include "ScreenControl.h"

class CapTouchControl
{
  public:
    CapTouchControl(int _touchPin);
    void begin(bool calibtration,ScreenControl *screen);
    bool isBrakePressed();
    int getTouchVal();
    
  private:
    int touchPin;
    int touchValue;

    void countdown(int seconds,ScreenControl *screen);
    int touchValueMin = 57922; // Change these values for brake calibration!!!!
    int touchValueMax = 189104;
    int touchThreshold = touchValueMax-touchValueMin/2.0;

};


#endif
