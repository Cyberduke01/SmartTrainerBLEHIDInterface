//sudo chmod 666 /dev/ttyACM0


#if ARDUINO_USB_MODE

#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}

#else

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

USBHIDKeyboard Keyboard;


byte KEY_ARROW_LEFT         =byte(0x50);


void setup() {
  // open the serial port:
  Serial.begin(115200);
  // initialize control over the keyboard:
  Keyboard.begin();
  USB.begin();
}
void loop() {
    
  for (int i = 0; i < 120; i++)
  {
    Keyboard.write(i);
    delay(100);
    Serial.write(i);
  }
  Keyboard.write(KEY_ARROW_LEFT);    delay(100);

    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);
    delay(100);
  Keyboard.write(KEY_ARROW_LEFT);

}
#endif /* ARDUINO_USB_MODE */






  
