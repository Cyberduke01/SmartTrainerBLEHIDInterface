#include <Arduino.h>
#include "BrakeValue.h"


BrakeValue::BrakeValue(int _Braking_pin)
{
  this->BrakingPin = _Braking_pin;
}


float BrakeValue::getBrakingPerc()
{
  int analogValue = analogRead(this->BrakingPin);//read value from pin
  //Serial.print("Value from brake: ");
  //Serial.println(analogValue);
  
  this->brakingperc_old   = this->brakingperc;
  this->brakingperc       = (analogValue-this->min_val)*(100.0/(this->max_val-this->min_val));

  this->brakingperc = FILETERALPHA * this->brakingperc + (1-FILETERALPHA)*this->brakingperc_old;
  
  return this->brakingperc;
}

void BrakeValue::countdown(int seconds)
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

void BrakeValue::begin()
{
    
   //DAC Setup
  analogReadResolution(12); // Read in 12 BIT
  //ask User if they want to calibrate steering
    Serial.println("If braking calibration is required, input 'Y' in the next 5 Seconds");
    delay(6000); // Delay a second between loops.
  
    if (Serial.available())  {
      char c = Serial.read();  //gets one byte from serial buffer
      if ((c == 'y')||(c == 'Y')) {
          Serial.println("Starting braking calibration");
          Serial.println("Move brake to zero position");
          Serial.println("value will be captured in 10 Seconds");
          countdown(10);
          this->min_val = analogRead(this->BrakingPin);//read value from pin pot
          Serial.println("Value Captured!");
          Serial.println("Now apply 100% brake");
          Serial.println("value will be captured in 10 Seconds");
          countdown(10);
          this->max_val= analogRead(this->BrakingPin);
          Serial.println("Value Captured!");
          Serial.println("=================================");
          Serial.println("Calibration values determined(To be hardcoded in BrakeValue.h file otherwise re-calibration will be needed on restar):");
          Serial.print("Min Value: ");
          Serial.println(this->min_val);
          Serial.print("Max Value: ");
          Serial.println(this->max_val);
          Serial.println("=================================");
          delay(10000); 
  
        }
      }
      else
        Serial.println("No braking calibration --> using saved values");
 }
      
