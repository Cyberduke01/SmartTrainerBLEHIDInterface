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

const int buttonPin = 0;
int previousButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Gamepad.begin();
  USB.begin();
  Serial.begin(115200);
  Serial.println("\n==================\nUSB Gamepad Testing\n==================\n");
  Serial.println("Press BOOT Button to activate the USB gamepad.");
  Serial.println("Longer press will change the affected button and controls.");
  Serial.println("Shorter press/release just activates the button and controls.");
 
  int midpoint = 128;
  Gamepad.rightStick(-midpoint, midpoint);  // Z Axis, Z Rotation
  delay(1000);
}

void loop() {

  int midpoint = 128;
     
  for (int t =midpoint-1; t<midpoint;t++)
  {
    /*Gamepad.pressButton(t);
    delay(100);
    Gamepad.releaseButton(t);
    delay(100);*/
    Gamepad.rightStick(t, -t);  // Z Axis, Z Rotation
    delay(80);
   // Gamepad.rightStick(0,0);  // Z Axis, Z Rotation
    //delay(1000);
  }

/*
 for (int t =midpoint; t<midpoint*2;t++)
{

  Gamepad.rightStick(0, t);  // Z Axis, Z Rotation
  delay(50);
 // Gamepad.rightStick(0,0);  // Z Axis, Z Rotation
  //delay(1000);
} */
}
#endif /* ARDUINO_USB_MODE */
