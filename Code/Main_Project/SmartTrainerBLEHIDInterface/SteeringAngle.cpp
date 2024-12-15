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

  this->steeringRescaleFactor =0.8 +((1.0*rescaleAnalogVal) / 1100.0);
  this->steeringRescaleFactor = RESCALEFILETERALPHA * this->steeringRescaleFactor + (1-RESCALEFILETERALPHA)*this->steeringRescaleFactor_old;

  return this->steeringAngle*steeringRescaleFactor;
}

void SteeringAngle::countdown(int seconds)
{
  unsigned long start = millis();
  int i = 0;
        
  while (millis() - start < (seconds*1000))
  {
    if ((millis() - start) > (i * 1000))
    {
      Serial.println(seconds-i);
      i++;
    }
  }
}

void SteeringAngle::begin()
{
    
   //DAC Setup
  analogReadResolution(12); // Read in 12 BIT
  //ask User if they want to calibrate steering
    Serial.println("If steering calibration is required, input 'Y' in the next 5 Seconds");
    delay(6000); // Delay a second between loops.
  
    if (Serial.available())  {
      char c = Serial.read();  //gets one byte from serial buffer
      if ((c == 'y')||(c == 'Y')) {
          Serial.println("Starting Steering calibration");
          Serial.println("Move steering wheel to the zero degree position");
          Serial.println("value will be captured in 10 Seconds");
          countdown(10);
          this->centreVal = analogRead(this->steeringPotPin);//read value from pin pot
          Serial.println("Value Captured!");
          Serial.println("Now move steering wheel 45 Degrees to the left");
          Serial.println("value will be captured in 10 Seconds");
          countdown(10);
          uint16_t left45val= analogRead(this->steeringPotPin);
          Serial.println("Value Captured!");
          Serial.println("Now move steering wheel 45 Degrees to the right");
          Serial.println("value will be captured in 10 Seconds");
          countdown(10);
          uint16_t right45val= analogRead(this->steeringPotPin);
          Serial.println("Value Captured!");
          this->DegPerVal = 90.0 / (right45val-left45val);//read value from pin pot;
          Serial.println("=================================");
          Serial.println("Calibration values determined(To be hardcoded in SteeringAngle.h file otherwise re-calibration will be needed on restart):");
          Serial.print("Centre Value: ");
          Serial.println(centreVal);
          Serial.print("Degree per value determined: ");
          Serial.printf("%f\n",DegPerVal);
          Serial.println("=================================");
          delay(10000); 
  
        }
      }
      else
        Serial.println("No steering angle calibration --> using saved values");
 }
      
