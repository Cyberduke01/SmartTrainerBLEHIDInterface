/**
 * This communicates with a BLE smart trainer to turn your cycle at home setup into a gamepad
 * author: Cyberduke01
 */
 //sudo chmod 666 /dev/ttyACM0

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
#include "USBHIDGamepad.h"
USBHIDGamepad Gamepad;


#include "BLEComm.h"
#include "SteeringAngle.h"
#include "Brakevalue.h"

#define STEERING_POT_PIN 4
#define STEERING_RESCALE_PIN 6
#define BRAKING_PIN 5 
  
int maxPower = 150; 

//Objects
BLEComm BLECommObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN,STEERING_RESCALE_PIN);
BrakeValue BrakeValueObj(BRAKING_PIN);

//Functions
int limitvalue(int input,int _min, int _max);

//varible such that bluetooth only gets val once a second
unsigned long old_millis = millis();


void setup() {
  Serial.begin(115200);
  BLECommObj.begin();
  SteeringAngleObj.begin();
  BrakeValueObj.begin();
  Gamepad.begin();
  USB.begin();

  old_millis = millis();     

} // End of setup.


// This is the Arduino main loop function.
void loop() {

  if ((millis() - old_millis) > 1000)
    {
      BLECommObj.loop();
      old_millis = millis();     
    }

  Serial.print("Cycling power: ");
  Serial.println(__cyclePower);

  Serial.print("Steering Angle: ");
  Serial.println(SteeringAngleObj.getSteeringAngle());
  

  Serial.println("%");

  int rescaledVal         = limitvalue((SteeringAngleObj.getSteeringAngle()) ,-127,127);
  int rescaledPower       = limitvalue((__cyclePower*1.0/maxPower)*127.0     ,0   ,127);  
  int rescaledbrakingperc = limitvalue((BrakeValueObj.getBrakingPerc()*1.27) ,0   ,127);

  Gamepad.rightStick(0,rescaledPower);  // Z Axis, Z Rotation
  Gamepad.leftStick(rescaledVal,rescaledbrakingperc);  


  //delay(1000); // Delay a second between loops.
} // End of loop

int limitvalue(int input,int _min, int _max)
{
  if  (input < _min)
    return _min;
   if  (input > _max)
    return _max;

   return input;
}

#endif /* ARDUINO_USB_MODE */
