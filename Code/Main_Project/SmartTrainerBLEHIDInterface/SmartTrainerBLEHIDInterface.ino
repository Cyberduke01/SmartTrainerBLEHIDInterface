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


#include <BLEComm.h>
#include <SteeringAngle.h>


#define STEERING_POT_PIN 4
 
  
int maxPower = 200; 



BLEComm BLECommObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN);


//varible such that bluetooth only gets val once a second
unsigned long old_millis = millis();


void setup() {
  Serial.begin(115200);
  BLECommObj.begin();
  SteeringAngleObj.begin();
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

  int rescaledVal = SteeringAngleObj.getSteeringAngle() * 2.7;
  int rescaledPower = (__cyclePower*1.0/maxPower)*127.0;
  if (rescaledPower>127) rescaledPower = 127;

  
   Gamepad.rightStick(rescaledVal,rescaledPower);  // Z Axis, Z Rotation


  //delay(10); // Delay a second between loops.
} // End of loop


#endif /* ARDUINO_USB_MODE */
