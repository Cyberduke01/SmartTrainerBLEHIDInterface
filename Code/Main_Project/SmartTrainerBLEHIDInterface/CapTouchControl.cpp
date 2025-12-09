#include <Arduino.h>
#include "CapTouchControl.h"

 
CapTouchControl::CapTouchControl(int _touchPinBrake, int _touchPinBoost)
{
  this->touchPinBrake = _touchPinBrake;
  this->touchPinBoost = _touchPinBoost;
}

void CapTouchControl::begin(bool calibtration,ScreenControl *screen)
{
  if (calibtration)
  {
    //  delay(1000000); // Delay a second between loops.

    int countdowntime = 3;
    char txtScreenBuffer[50];
    char txtScreenBuffer2[50];

    for (int t = 0;t<this->numToAverage+5;t++)
      this->loop();
    //Lets calibrate the touch for the Brake!
    screen->ClearScreen();
    screen->SetLine(0,"Calibrating Brake...",1);
    screen->SetLine(1,"Do not press brake",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    for (int t = 0;t<this->numToAverage+5;t++)//make sure the value we are reading is an average of "touched"
      this->loop();
    touchValueMin_Brake = aveTouchValueBrake;//read value when no brake is pressed
    screen->ClearScreen();
    screen->SetLine(0,"Value Captured!",1);
    screen->SetLine(1,"Fully press brake",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    for (int t = 0;t<this->numToAverage+5;t++)//make sure the value we are reading is an average of "touched"
      this->loop();
    touchValueMax_Brake = aveTouchValueBrake;//read value when brake is fully pressed
    screen->ClearScreen();
    screen->SetLine(0,"Brake calibrated",2);
    delay(2000);
    screen->ClearScreen();
    screen->SetLine(0,"Hardcode values in CapTouchControl.h!  Otherwise re-calibration will be needed on restart",1);
    delay(10000);
    sprintf(txtScreenBuffer, "Min: %d",touchValueMin_Brake);
    sprintf(txtScreenBuffer2, "Max: %d",touchValueMax_Brake);
    screen->ClearScreen();
    screen->SetLine(0,txtScreenBuffer,2);
    screen->SetLine(1,txtScreenBuffer2,2);
    touchThreshold_Brake = touchValueMax_Brake-touchValueMin_Brake/2.0;
    delay(10000);
    //Lets calibrate the Boost for the Brake!
    screen->ClearScreen();
    screen->SetLine(0,"Calibrating boost...",1);
    screen->SetLine(1,"Do not press boost",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    for (int t = 0;t<this->numToAverage+5;t++)//make sure the value we are reading is an average of "touched"
      this->loop();
    touchValueMin_Boost = aveTouchValueBoost;//read value when no brake is pressed
    screen->ClearScreen();
    screen->SetLine(0,"Value Captured!",1);
    screen->SetLine(1,"Fully press boost",1);
    screen->SetLine(2,"value will be captured soon",1);
    delay(5000);
    countdown(countdowntime,screen);
    for (int t = 0;t<this->numToAverage+5;t++)//make sure the value we are reading is an average of "touched"
      this->loop();
    touchValueMax_Boost = aveTouchValueBoost;//read value when brake is fully pressed
    screen->ClearScreen();
    screen->SetLine(0,"Boost calibrated",2);
    delay(2000);
    screen->ClearScreen();
    screen->SetLine(0,"Hardcode values in CapTouchControl.h!  Otherwise re-calibration will be needed on restart",1);
    delay(10000);
    sprintf(txtScreenBuffer, "Min: %d",touchValueMin_Boost);
    sprintf(txtScreenBuffer2, "Max: %d",touchValueMax_Boost);
    screen->ClearScreen();
    screen->SetLine(0,txtScreenBuffer,2);
    screen->SetLine(1,txtScreenBuffer2,2);
    touchThreshold_Boost = touchValueMax_Boost-touchValueMin_Boost/2.0;
    delay(10000);
  }
}

void CapTouchControl::loop()//need to measure a value every loop such that we are averaging values
{
  if (Loopcount<numToAverage)
  {
    TouchValuesBrake[Loopcount] = touchRead(touchPinBrake);
    TouchValuesBoost[Loopcount] = touchRead(touchPinBoost);
    Loopcount++; 

    aveTouchValueBrake = touchRead(touchPinBrake);
    aveTouchValueBoost = touchRead(touchPinBoost);
  }
  if (Loopcount==numToAverage)
  {
    aveTouchValueBrake = 0;
    for (int t = 0;t<numToAverage;t++)
      aveTouchValueBrake += TouchValuesBrake[t]/(numToAverage*1.0);

    aveTouchValueBoost = 0;
    for (int t = 0;t<numToAverage;t++)
      aveTouchValueBoost += TouchValuesBoost[t]/(numToAverage*1.0);
  }

  int TouchValBrake0Old = TouchValuesBrake[0];
  int TouchValBoost0Old = TouchValuesBoost[0];

  for (int t=0;t<numToAverage-1;t++)//move all one up
  {
    TouchValuesBrake[t] = TouchValuesBrake[t+1];
    TouchValuesBoost[t] = TouchValuesBoost[t+1];
  }
    
  TouchValuesBrake[numToAverage-1] = touchRead(touchPinBrake);//replace the last val in array
  TouchValuesBoost[numToAverage-1] = touchRead(touchPinBoost);//replace the last val in array

  aveTouchValueBrake = aveTouchValueBrake - TouchValBrake0Old/(numToAverage*1.0) + TouchValuesBrake[numToAverage-1]/(numToAverage*1.0);
  aveTouchValueBoost = aveTouchValueBoost - TouchValBoost0Old/(numToAverage*1.0) + TouchValuesBoost[numToAverage-1]/(numToAverage*1.0);


/*
  Serial.print("TouchCurr BOOST: ");
  Serial.println(TouchValuesBoost[numToAverage-1]);
  Serial.print("TouchAve BOOSET: ");
  Serial.println(aveTouchValueBoost);

  Serial.print("TouchCurr BRAKE: ");
  Serial.println(TouchValuesBrake[numToAverage-1]);
  Serial.print("TouchAve BRAKE: ");
  Serial.println(aveTouchValueBrake);
      //aveTouchValueBoost;
*/
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
  if (aveTouchValueBrake > touchThreshold_Brake)
    return true;

  return false;
}

bool CapTouchControl::isBoostPressed()
{
  if (aveTouchValueBoost > touchThreshold_Boost)
    return true;

  return false;
}
/*
int CapTouchControl::getTouchVal()
{
  touchValue = touchRead(touchPin);
  //Serial.print("Touch: ");
  //Serial.println(touchValue);
  return touchValue;
}*/