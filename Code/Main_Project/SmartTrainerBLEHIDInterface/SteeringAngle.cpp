#include <Arduino.h>
#include "SteeringAngle.h"


SteeringAngle::SteeringAngle(int _steeringPotPin,int _steeringRescalePotPin)
{
  this->steeringPotPin        = _steeringPotPin;
  this->steeringRescalePotPin = _steeringRescalePotPin;
}


float SteeringAngle::getSteeringAngle()
{
  int analogValue = analogRead(this->steeringPotPin);//read value from pot
  
  this->steeringAngle_old   = this->steeringAngle;
  this->steeringAngle       = (analogValue - this->centreVal) * this->DegPerVal;

  this->steeringAngle = ANGLEFILETERALPHA * this->steeringAngle + (1-ANGLEFILETERALPHA)*this->steeringAngle_old;

  this->steeringRescaleFactor_old   = this->steeringRescaleFactor;
  int rescaleAnalogVal = analogRead(this->steeringRescalePotPin);//read value from pot

  this->steeringRescaleFactor = 0.8 +((1.0*rescaleAnalogVal) / 1100.0);
  this->steeringRescaleFactor = RESCALEFILETERALPHA * this->steeringRescaleFactor + (1-RESCALEFILETERALPHA)*this->steeringRescaleFactor_old;

  return this->steeringAngle*steeringRescaleFactor;
}

void SteeringAngle::countdown(int seconds,ScreenControl *screen)
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

void SteeringAngle::begin(bool calibtration,ScreenControl *screen)
{
  int countdowntime = 3;
  if (calibtration)
  {
    //DAC Setup
    analogReadResolution(12); // Read in 12 BIT
    //ask User if they want to calibrate steering
    screen->ClearScreen();
    screen->SetLine(0,"Calibrating Steering...",1);
    screen->SetLine(1,"Move to the zero degree position",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    this->centreVal = analogRead(this->steeringPotPin);//read value from pin pot
    screen->ClearScreen();
    screen->SetLine(0,"Value Captured!",1);
    screen->SetLine(1,"Move 45 Degrees to the left",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    uint16_t left45val= analogRead(this->steeringPotPin);
    screen->ClearScreen();
    screen->SetLine(0,"Value Captured!",1);
    screen->SetLine(1,"Move 45 Degrees to the right",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    uint16_t right45val= analogRead(this->steeringPotPin);
    this->DegPerVal = 90.0 / (right45val-left45val);

    screen->ClearScreen();
    screen->SetLine(0,"Steering  calibrated!");
    delay(2000);
    screen->ClearScreen();
    screen->SetLine(0,"Hardcode values in SteeringAngle.h!  Otherwise re-calibration will be needed on restart",1);
    delay(10000);

    char txtScreenBuffer1[50];
    sprintf(txtScreenBuffer1, "Centre: %d",centreVal);
    char txtScreenBuffer2[50];
    sprintf(txtScreenBuffer2, "DegPerVal: %1.6f",DegPerVal);
    screen->ClearScreen();
    screen->SetLine(0,txtScreenBuffer1,1);
    screen->SetLine(1,txtScreenBuffer2,1);

    delay(10000);
  }
 }
      
