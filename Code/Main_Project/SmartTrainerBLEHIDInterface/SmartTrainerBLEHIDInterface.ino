
/**
 * This communicates with a BLE smart trainer to turn your cycle at home setup into a gamepad
 * author: Cyberduke01
 */
 //sudo chmod 666 /dev/ttyACM0
 //http://wiki.fluidnc.com/en/hardware/ESP32-S3_Pin_Reference

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
#define SELECT_CALIBRATION_PIN 42 

#define SD_MOSI     1
#define SD_MISO     2
#define SD_SCLK     7
#define SD_CS       15

#define SCREEN_SDA 18
#define SCREEN_SCL 46

#define TOUCHPIN_BOOST 5
#define TOUCHPIN_BRAKE 10

#define BRAKE_BUTTON_ON_GAMEPAD 2
#define BOOST_BUTTON_ON_GAMEPAD 1

#define DEFAULT_TEXT_SIZE 2
#define CAP_TOUCH_LINE 3
#define STEER_LINE 3.5
#define THROTTLE_LINE 1
#define CAP_TOUCH_THICK 0.5
#define STEER_THICK 0.5
#define THROTTLE_THICK 0.5
#define ENERGY_LINE 2

int maxPower = 150; 
int totalEnergy = 0;
int screenUpdateRateMillis = 25;

//Objects
BLEComm BLECommObj;
BLEComm_HeartRate BLEComm_HeartRateObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN,STEERING_RESCALE_PIN);
BrakeValue BrakeValueObj(BRAKING_PIN);
ScreenControl ScreenObj(SCREEN_SDA,SCREEN_SCL);
CapTouchControl TouchObj(TOUCHPIN_BRAKE,TOUCHPIN_BOOST);
//SDControl SDcardObj(SD_MOSI, SD_MISO, SD_SCLK, SD_CS);
//Functions
int limitvalue(int input,int _min, int _max);

//varible such that bluetooth only gets val once a second
unsigned long old_millisBLE = millis();
unsigned long old_millisScreen = millis();

//to calculate the energy burnt during play
unsigned long millisEnergy = millis();
unsigned long old_millisEnergy = millis();


char txtScreenBuffer[20];
bool oldBrake_screen  = TouchObj.isBrakePressed();
bool oldBrake_control = TouchObj.isBrakePressed();
bool oldBoost_control = TouchObj.isBoostPressed();
bool oldBoost_screen  = TouchObj.isBoostPressed();

void setup() {
  Serial.begin(115200);

  pinMode(SELECT_CALIBRATION_PIN, INPUT);//check if we want to calibrate the values
  bool CalibrationRequired = !digitalRead(SELECT_CALIBRATION_PIN);
  
  ScreenObj.begin();
  TouchObj.begin(CalibrationRequired,&ScreenObj);
  //SDcardObj.begin();
  BLEComm_HeartRateObj.begin();
  BLECommObj.begin();
  SteeringAngleObj.begin(CalibrationRequired,&ScreenObj);
  //BrakeValueObj.begin();
  Gamepad.begin();
  USB.begin();

  ScreenObj.ClearScreen();

  

  old_millisBLE = millis();     
  old_millisScreen = millis(); 

  millisEnergy = millis();
  old_millisEnergy = millis();
} // End of setup.


// This is the Arduino main loop function.
void loop() {
  
  int rescaledSteeringVal = limitvalue((SteeringAngleObj.getSteeringAngle()) ,-127,127);
  int rescaledPower       = limitvalue((__cyclePower*1.0/maxPower)*127.0     ,0   ,127);  
  //int rescaledbrakingperc = limitvalue((BrakeValueObj.getBrakingPerc()*1.27) ,0   ,127);

  if ((millis() - old_millisBLE) > 1000)//once every second we get data from bluetooth
  {
    BLECommObj.loop();
    //BLEComm_HeartRateObj.loop();
    
    //it only makes sense to update power on screen if we actually have a new power 
    sprintf(txtScreenBuffer, "%dw /%dw",rescaledPower,maxPower);
    ScreenObj.OverrideLine(0,txtScreenBuffer,2); 
    ScreenObj.DrawProgressBar(THROTTLE_LINE,THROTTLE_THICK,(rescaledPower*(100.0/127.0)),1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight

    millisEnergy = millis();
    totalEnergy += ((millisEnergy - old_millisEnergy)/1000.0) * __cyclePower;
    old_millisEnergy = millisEnergy;
    sprintf(txtScreenBuffer, "E: %d Cal",int(totalEnergy*(0.000239006)));
    ScreenObj.OverrideLine(ENERGY_LINE,txtScreenBuffer,2); 

    old_millisBLE = millis();
  }

  if ((millis() - old_millisScreen) > screenUpdateRateMillis)//check to update screen also only once an interval
  {
    bool isBrakePressed = TouchObj.isBrakePressed();
    bool isBoostPressed = TouchObj.isBoostPressed();

    if (isBrakePressed && !isBoostPressed)//DrawDoubleDirProgressBar(int _lineNo, float barHeight, int progress)
      ScreenObj.DrawDoubleDirProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,100); 
    if (isBoostPressed && !isBrakePressed)
      ScreenObj.DrawDoubleDirProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,-100);
    if (isBrakePressed && isBoostPressed)     
      ScreenObj.DrawProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,100,1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight
    if (!isBoostPressed && !isBrakePressed)     
      ScreenObj.DrawProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,0,1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight

    ScreenObj.DrawDoubleDirProgressBar(STEER_LINE,STEER_THICK,rescaledSteeringVal*(-100.0/127.0));
    old_millisScreen = millis();  
  }

    if (oldBrake_control != TouchObj.isBrakePressed())//check if user brake status changed for controll reasons
    {
      if (TouchObj.isBrakePressed())
        Gamepad.pressButton(BRAKE_BUTTON_ON_GAMEPAD);
      else
        Gamepad.releaseButton(BRAKE_BUTTON_ON_GAMEPAD);
      oldBrake_control = TouchObj.isBrakePressed();
    }
    if (oldBoost_control != TouchObj.isBoostPressed())//check if user boost status changed for controll reasons
    {
      if (TouchObj.isBoostPressed())
        Gamepad.pressButton(BOOST_BUTTON_ON_GAMEPAD);
      else
        Gamepad.releaseButton(BOOST_BUTTON_ON_GAMEPAD);
      oldBoost_control = TouchObj.isBoostPressed();
    }

  Serial.print("Cycling power: ");
  Serial.println(__cyclePower);

  Serial.print("Steering Angle: ");
  Serial.println(SteeringAngleObj.getSteeringAngle());
  
  Serial.print("Heart Rate: ");
  Serial.println(__heartRate);

  Serial.println("%");

  Gamepad.rightStick(0,rescaledPower);  // Z Axis, Z Rotation
  Gamepad.leftStick(rescaledSteeringVal,0);  

  TouchObj.loop();
  //delay(800); // Delay a second between loops.
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
