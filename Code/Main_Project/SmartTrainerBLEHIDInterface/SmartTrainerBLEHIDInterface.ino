
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
#include "ScreenControl.h"    //Raw Communication with screen
#include "CapTouchControl.h"
//#include "SDControl.h"    //Raw Communication with SD card

#define STEERING_POT_PIN 4
#define STEERING_RESCALE_PIN 6
#define BRAKING_PIN 5 

#define SD_MOSI     1
#define SD_MISO     2
#define SD_SCLK     7
#define SD_CS       15

#define SCREEN_SDA 18
#define SCREEN_SCL 46

#define TOUCHPIN 10

int maxPower = 150; 

//Objects
BLEComm BLECommObj;
BLEComm_HeartRate BLEComm_HeartRateObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN,STEERING_RESCALE_PIN);
BrakeValue BrakeValueObj(BRAKING_PIN);
ScreenControl ScreenObj(SCREEN_SDA,SCREEN_SCL);
CapTouchControl TouchObj(TOUCHPIN);
//SDControl SDcardObj(SD_MOSI, SD_MISO, SD_SCLK, SD_CS);
//Functions
int limitvalue(int input,int _min, int _max);

//varible such that bluetooth only gets val once a second
unsigned long old_millis = millis();

char txtScreenBuffer[20];
bool oldBrake = TouchObj.isBrakePressed();
bool oldBrake = TouchObj.isBrakePressed();
void setup() {
  Serial.begin(115200);

  bool CalibrationRequired = true;
  
  ScreenObj.begin();
  TouchObj.begin(CalibrationRequired,&ScreenObj);
  //SDcardObj.begin();
  BLEComm_HeartRateObj.begin();
  BLECommObj.begin();
  SteeringAngleObj.begin();
  BrakeValueObj.begin();
  Gamepad.begin();
  USB.begin();
  

  ScreenObj.ClearScreen();
  old_millis = millis();     

} // End of setup.


// This is the Arduino main loop function.
void loop() {

  if ((millis() - old_millis) > 1000)//once every second
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

  int rescaledSteeringVal = limitvalue((SteeringAngleObj.getSteeringAngle()) ,-127,127);
  int rescaledPower       = limitvalue((__cyclePower*1.0/maxPower)*127.0     ,0   ,127);  
  int rescaledbrakingperc = limitvalue((BrakeValueObj.getBrakingPerc()*1.27) ,0   ,127);

  Gamepad.rightStick(0,rescaledPower);  // Z Axis, Z Rotation
  Gamepad.leftStick(rescaledSteeringVal,rescaledbrakingperc);  

  //ScreenObj.ClearScreen();
  sprintf(txtScreenBuffer, "Power: %d w",rescaledPower );
  ScreenObj.SetLine(0,txtScreenBuffer);

  if (oldBrake != TouchObj.isBrakePressed())
  {
    sprintf(txtScreenBuffer, "Brake: %d ",TouchObj.isBrakePressed());
    ScreenObj.OverrideLine(1,txtScreenBuffer);
    oldBrake = TouchObj.isBrakePressed();
  }

 
  sprintf(txtScreenBuffer, "Steer: %d",rescaledSteeringVal );
  ScreenObj.OverrideLine(2,txtScreenBuffer);




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
