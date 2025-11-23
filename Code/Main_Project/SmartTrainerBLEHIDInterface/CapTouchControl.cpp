#include <Arduino.h>
#include "CapTouchControl.h"

 
CapTouchControl::CapTouchControl(int _touchPin)
{
  this->touchPin = _touchPin;
}

void CapTouchControl::begin(bool calibtration,ScreenControl *screen)
{
  int countdowntime = 3;
  if (calibtration)
  {
    screen->ClearScreen();
    screen->SetLine(0,"Calibrating Brake...",1);
    screen->SetLine(1,"Do not press brake",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    touchValueMin = touchRead(touchPin);//read value when no brake is pressed
    screen->ClearScreen();
    screen->SetLine(0,"Value Captured!",1);
    screen->SetLine(1,"Fully press brake",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    touchValueMax = touchRead(touchPin);//read value when brake is fully pressed

    screen->ClearScreen();
    screen->SetLine(0,"Brake calibrated");
    delay(2000);
    screen->ClearScreen();
    screen->SetLine(0,"Hardcode values in CapTouchControl.h!  Otherwise re-calibration will be needed on restart",1);
    delay(10000);

    char txtScreenBuffer[50];
    sprintf(txtScreenBuffer, "Min: %d",touchValueMin);
    char txtScreenBuffer2[50];
    sprintf(txtScreenBuffer2, "Max: %d",touchValueMax);
    screen->ClearScreen();
    screen->SetLine(0,txtScreenBuffer);
    screen->SetLine(1,txtScreenBuffer2);

    touchThreshold = touchValueMax-touchValueMin/2.0;
    delay(10000);
  }
  //touchValue = touchRead(touchPin);
  //Serial.print("Touch: ");
  //Serial.println(touchValue);
}

void CapTouchControl::countdown(int seconds,ScreenControl *screen)
{
  unsigned long start = millis();
  int i = 0;
  char txtScreenBuffer[1];      
  while (millis() - start < (seconds*1000))
  {
    if ((millis() - start) > (i * 1000))
    {
      sprintf(txtScreenBuffer, "  %d",seconds-i);
      screen->ClearScreen();
      screen->SetLine(1,txtScreenBuffer,4);
      i++;
    }
  }
}

bool CapTouchControl::isBrakePressed()
{
  if (touchRead(touchPin) > touchThreshold)
    return true;

  return false;
}

int CapTouchControl::getTouchVal()
{
  touchValue = touchRead(touchPin);
  Serial.print("Touch: ");
  Serial.println(touchValue);
  return touchValue;
}