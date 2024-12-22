#include <SPI.h>
#include "FS.h"
#include "SD.h"
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


#include "BLEComm.h"            //Communication with smart trainer
#include "BLEComm_HeartRate.h"  //Communication with heart rate device
#include "SteeringAngle.h"
#include "Brakevalue.h"

#define STEERING_POT_PIN 4
#define STEERING_RESCALE_PIN 6
#define BRAKING_PIN 5 

#define SD_MOSI     1
#define SD_MISO     2
#define SD_SCLK     7
#define SD_CS       15

int maxPower = 150; 

//Objects
BLEComm BLECommObj;
BLEComm_HeartRate BLEComm_HeartRateObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN,STEERING_RESCALE_PIN);
BrakeValue BrakeValueObj(BRAKING_PIN);

//Functions
int limitvalue(int input,int _min, int _max);

//varible such that bluetooth only gets val once a second
unsigned long old_millis = millis();


void setup() {
  Serial.begin(115200);

  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);  
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("SD Card MOUNT FAIL");
  }
  else
  {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE)
    {
      Serial.println("No SD card attached");
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } 
    else if(cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } 
    else if(cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } 
    else 
    {
        Serial.println("UNKNOWN");
    }
  }

  BLEComm_HeartRateObj.begin();
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
      BLEComm_HeartRateObj.loop();
      old_millis = millis();     
    }

  Serial.print("Cycling power: ");
  Serial.println(__cyclePower);

  Serial.print("Steering Angle: ");
  Serial.println(SteeringAngleObj.getSteeringAngle());
  
  Serial.print("Heart Rate: ");
  Serial.println(__heartRate);

  Serial.println("%");

  int rescaledVal         = limitvalue((SteeringAngleObj.getSteeringAngle()) ,-127,127);
  int rescaledPower       = limitvalue((__cyclePower*1.0/maxPower)*127.0     ,0   ,127);  
  int rescaledbrakingperc = limitvalue((BrakeValueObj.getBrakingPerc()*1.27) ,0   ,127);

  Gamepad.rightStick(0,rescaledPower);  // Z Axis, Z Rotation
  Gamepad.leftStick(rescaledVal,rescaledbrakingperc);  


  delay(1000); // Delay a second between loops.
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
