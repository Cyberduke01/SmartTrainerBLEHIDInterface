#include <Arduino.h>
#include "SteeringAngle.h"
#include <Wire.h>

float SteeringAngle::getSteeringAngle_magnet()
{
 this->loop();
 int lowbyte; //raw angle 7:0
 word highbyte; //raw angle 7:0 and 11:8
 int rawAngle; //final raw angle 

  Wire.begin(); //start i2C  
  //7:0 - bits
  Wire.beginTransmission(0x36); //connect to the sensor
  Wire.write(0x0D); //figure 21 - register map: Raw angle (7:0)
  Wire.endTransmission(); //end transmission
  Wire.requestFrom(0x36, 1); //request from the sensor

  
  unsigned long millisStart = millis();
  while(Wire.available() == 0)
  {
    if ((millis() - millisStart) > 3000)
    {
      this->screen->ClearScreen();
      this->screen->OverrideLine(0,"Error! Magnetic sensor not reachable",2);
    }
  } //wait until it becomes available and notify user if no sucess after 3 seconds
  lowbyte = Wire.read(); //Reading the data after the request
 
  //11:8 - 4 bits
  Wire.beginTransmission(0x36);
  Wire.write(0x0C); //figure 21 - register map: Raw angle (11:8)
  Wire.endTransmission();
  Wire.requestFrom(0x36, 1);
  
  while(Wire.available() == 0);  
  highbyte = Wire.read();
  
  //4 bits have to be shifted to its proper place as we want to build a 12-bit number
  highbyte = highbyte << 8; //shifting to left
  rawAngle = highbyte | lowbyte; //int is 16 bits (as well as the word)

  //We need to calculate the angle:
  //12 bit -> 4096 different levels: 360° is divided into 4096 equal parts:
  //360/4096 = 0.087890625
  //Multiply the output of the encoder with 0.087890625
  this->degAngleMagnetmin1 = this->degAngleMagnet0;
  this->degAngleMagnet0 = rawAngle * 0.087890625;
  //return this->steeringRescaleFactor * (ANGLEFILETERALPHA * this->degAngleMagnet0 + (1-ANGLEFILETERALPHA)*this->degAngleMagnetmin1);
  return -1*this->steeringRescaleFactor * this->loopint((ANGLEFILETERALPHA * this->degAngleMagnet0 + (1-ANGLEFILETERALPHA)*this->degAngleMagnetmin1) - 180 +83.6,-180,180); // the middle HAS AN OFFSET
}


SteeringAngle::SteeringAngle(int _steeringPotPin,int _steeringRescalePotPin)
{
  this->steeringPotPin        = _steeringPotPin;
  this->steeringRescalePotPin = _steeringRescalePotPin;
}

void SteeringAngle::loop()
{
  //int analogValue = analogRead(this->steeringPotPin);//read value from pot
  
 // this->steeringAngle_old   = this->steeringAngle;
  //this->steeringAngle       = (analogValue - this->centreVal) * this->DegPerVal;

  //this->steeringAngle = ANGLEFILETERALPHA * this->steeringAngle + (1-ANGLEFILETERALPHA)*this->steeringAngle_old;

  this->steeringRescaleFactor_old   = this->steeringRescaleFactor;
  int rescaleAnalogVal = analogRead(this->steeringRescalePotPin);//read value from pot

  this->steeringRescaleFactor = 0.8 +((1.0*rescaleAnalogVal) / 1100.0);
  this->steeringRescaleFactor = RESCALEFILETERALPHA * this->steeringRescaleFactor + (1-RESCALEFILETERALPHA)*this->steeringRescaleFactor_old;

  //return steeringRescaleFactor;
}

float SteeringAngle::getSteeringAngle_pot()
{
  int analogValue = analogRead(this->steeringPotPin);//read value from pot
  
  this->steeringAngle_old   = this->steeringAngle;
  this->steeringAngle       = (analogValue - this->centreVal) * this->DegPerVal;

  this->steeringAngle = ANGLEFILETERALPHA * this->steeringAngle + (1-ANGLEFILETERALPHA)*this->steeringAngle_old;

  //this->steeringRescaleFactor_old   = this->steeringRescaleFactor;
  //int rescaleAnalogVal = analogRead(this->steeringRescalePotPin);//read value from pot

  //this->steeringRescaleFactor = 0.8 +((1.0*rescaleAnalogVal) / 1100.0);
  //this->steeringRescaleFactor = RESCALEFILETERALPHA * this->steeringRescaleFactor + (1-RESCALEFILETERALPHA)*this->steeringRescaleFactor_old;

  return this->steeringAngle*this->steeringRescaleFactor;
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

int SteeringAngle::loopint(float in, int min,int max)
{
  if (in > max)
    return (int)(min+in-max);
  if (in < min)
    return (int)(min-in);
  return (int)in;
}
void SteeringAngle::begin(bool calibtration,ScreenControl *screen)
{
  /*int countdowntime = 3;
  this->screen = screen;
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
    screen->SetLine(0,"Steering  calibrated!",1.0);
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
  }*/
 }
      
