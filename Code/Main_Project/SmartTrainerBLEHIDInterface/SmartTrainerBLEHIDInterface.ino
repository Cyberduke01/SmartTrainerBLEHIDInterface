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

#include <Arduino.h>

#include <IRremote.h>

#include "BLEComm.h"            //Communication with smart trainer
#include "BLEComm_HeartRate.h"  //Communication with heart rate device
#include "SteeringAngle.h"
//#include "Brakevalue.h"
#include "ScreenControl.h"    //Raw Communication with screen
#include "CapTouchControl.h"
//#include "SDControl.h"    //Raw Communication with SD card
#include "Workout.h"

#define STEERING_POT_PIN 4
#define STEERING_RESCALE_PIN 6
#define SELECT_CALIBRATION_PIN 42 
#define IR_PIN 35

#define BUZZER_PIN 48
//#define BUZZER_PWM_CHANNEL 0
//#define BUZZER_PWM_FREQ    5000
//#define BUZZER_PWM_RES     8

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

//define the signals from the remote, adjust them according to your remote
#define REMOTE_RIGHT_ARROW 0x43
#define REMOTE_LEFT_ARROW 0x44
#define REMOTE_UP_ARROW 0x46
#define REMOTE_DOWN_ARROW 0x15
#define REMOTE_OK 0x40
#define REMOTE_DEBOUNCE_THRESHOLD_MILLIS 400

#define TIME_TO_START_OK_BUTTON_LIFTOFF_MILLIS 500

bool buzzer_on = false;

float remote_debounce_count = 0;
float ok_press_count_time = 0;
int ok_press_count = 0;

int maxPower = 150; 
float totalEnergy_cal = 0;
float old_totalEnergy_cal = -1;
int screenUpdateRateMillis = 33;
bool power_screen_update = false;

float start_millis;
int elapsed_workout_hours = 0;
int elapsed_workout_minutes = 0;
int elapsed_workout_seconds = 0;

bool seconds_updated = true;

//Objects
BLEComm BLECommObj;
BLEComm_HeartRate BLEComm_HeartRateObj;


SteeringAngle SteeringAngleObj(STEERING_POT_PIN,STEERING_RESCALE_PIN);
//BrakeValue BrakeValueObj(BRAKING_PIN);
ScreenControl ScreenObj(SCREEN_SDA,SCREEN_SCL);
CapTouchControl TouchObj(TOUCHPIN_BRAKE,TOUCHPIN_BOOST);
//SDControl SDcardObj(SD_MOSI, SD_MISO, SD_SCLK, SD_CS);
Workout WorkoutObj(&ScreenObj);
//Functions
int limitvalue(int input,int _min, int _max);
int loopint(int in, int max);
void IRAM_ATTR onTimer();

//varible such that bluetooth only gets val once a second
unsigned long old_millisBLE = millis();
unsigned long old_millisScreen = millis();
unsigned millisBLCheck;

//to calculate the energy burnt during play
unsigned long millisEnergy = millis();
unsigned long old_millisEnergy = millis();

char txtScreenBuffer[20];
bool oldBrake_screen  = TouchObj.isBrakePressed();
bool oldBrake_control = TouchObj.isBrakePressed();
bool oldBoost_control = TouchObj.isBoostPressed();
bool oldBoost_screen  = TouchObj.isBoostPressed();
int touch_control_case = 5;

int rescaledSteeringVal = 0;
int rescaledSteeringVal_old = -1;

typedef enum {HR_ENERGY,HR_TIME,ENERGY_TIME} DataFields;
DataFields dataFielsToShow = HR_ENERGY;
DataFields DataFields_vals[] = {HR_ENERGY,HR_TIME,ENERGY_TIME};

hw_timer_t *Second_timer = NULL;

typedef enum {IDLE,PLAYING,PAUSED} SystemStates;
SystemStates currentSystemState = IDLE;


void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(SELECT_CALIBRATION_PIN, INPUT);//check if we want to calibrate the values
  bool CalibrationRequired = !digitalRead(SELECT_CALIBRATION_PIN);
  
  ScreenObj.begin();
  TouchObj.begin(CalibrationRequired,&ScreenObj);
//  SDcardObj.begin();
  BLEComm_HeartRateObj.begin();
  BLECommObj.begin();
  SteeringAngleObj.begin(CalibrationRequired,&ScreenObj);
  //BrakeValueObj.begin();
  Gamepad.begin();
  USB.begin();
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  ScreenObj.ClearScreen();

  ScreenObj.SetLine(ENERGY_LINE-0.5," HR      Energy[Cal]",1);

  old_millisBLE = millis();     
  old_millisScreen = millis(); 

  millisEnergy = millis();
  old_millisEnergy = millis();

  millisBLCheck = millis();
  ok_press_count_time = millis();

  Second_timer = timerBegin(1000000);
  timerAttachInterrupt(Second_timer,&onTimer);
  timerAlarm(Second_timer, 1000000, true, 0);

} // End of setup.


// This is the Arduino main loop function.
void loop() {
  
  //use this code to see what the IR remote gives to the receiver
  /*if (IrReceiver.decode()) {
    Serial.print("Received code: ");
    Serial.println(IrReceiver.decodedIRData.command, HEX); // Print the HEX ID
    IrReceiver.resume(); // Receive the next value
  }*/



if (elapsed_workout_seconds > 59)
{
  elapsed_workout_seconds = 0;
  elapsed_workout_minutes ++;
  if (elapsed_workout_minutes > 59)
  {
    elapsed_workout_minutes = 0;
    elapsed_workout_hours ++;
  }
}

if ((seconds_updated) && (currentSystemState == PLAYING))//save new data to workout if we are currently having a workout
{
  //if (currentSystemState == PLAYING)
  elapsed_workout_seconds++;
  if (!WorkoutObj.AddDataPoint(__heartRate,__cyclePower))
  {
    analogWrite(BUZZER_PIN, 0);
    WorkoutObj.SaveWorkout(int(totalEnergy_cal));
  }


  seconds_updated = false;
}


if (IrReceiver.decode()) {//if the user pressed a button on the remote
   //remote_debounce_count = millis();
    if((millis() - remote_debounce_count) > REMOTE_DEBOUNCE_THRESHOLD_MILLIS)
    {
      //Serial.println(IrReceiver.decodedIRData.command, HEX); // Print the HEX ID
      remote_debounce_count = millis();
      
      if (IrReceiver.decodedIRData.command == REMOTE_RIGHT_ARROW)
            dataFielsToShow = DataFields_vals[loopint(((int)dataFielsToShow + 1),2)];
      if (IrReceiver.decodedIRData.command == REMOTE_LEFT_ARROW)
            dataFielsToShow = DataFields_vals[loopint(((int)dataFielsToShow - 1),2)];
      if (IrReceiver.decodedIRData.command == REMOTE_UP_ARROW)
      {
        maxPower += 5;
        power_screen_update = true;
      }
      if (IrReceiver.decodedIRData.command == REMOTE_DOWN_ARROW)
      {
        maxPower -= 5;
        power_screen_update = true;
      }
      if ((IrReceiver.decodedIRData.command == REMOTE_OK) && (millis() - start_millis)>4000)//the first 5 seconds after starting we cannot pause
      {
        ok_press_count++;
        Serial.println(ok_press_count);
        ok_press_count_time = millis();
        if ((ok_press_count > 5) && (currentSystemState == IDLE))//if we held ok button for around 3 seconds
        {
          Serial.println("START");//If we started a workout
          currentSystemState = PLAYING;
          WorkoutObj.BeginWorkout();
          power_screen_update = true;
          elapsed_workout_hours = 0;
          elapsed_workout_minutes = 0;
          elapsed_workout_seconds = 0;
          ok_press_count = 0;
          start_millis = millis();
        }
        if ((ok_press_count == 1) && (currentSystemState == PAUSED))//if we pressed ok once
        {
          Serial.println("resuming");
          currentSystemState = PLAYING;
          ok_press_count = 0 ;
        }
        if ((ok_press_count == 1) && (currentSystemState == PLAYING))//if we pressed ok once
        {
          Serial.println("PAUSING");
          currentSystemState = PAUSED;
        }
        if ((ok_press_count > 5) && ((currentSystemState == PLAYING)||(currentSystemState == PAUSED)))//if we held ok button for around 3 seconds
        {
          Serial.println("ENDING");
          currentSystemState = IDLE;
          ok_press_count = 0;
          power_screen_update = true;
          analogWrite(BUZZER_PIN, 0);
          WorkoutObj.SaveWorkout(int(totalEnergy_cal));
        }
      }

      switch (dataFielsToShow) //also need to update the screen if the user changed datafiels
      {
        case HR_ENERGY:
          ScreenObj.OverrideLine(ENERGY_LINE-0.5," HR      Energy[Cal]",1);
          sprintf(txtScreenBuffer, "%d - %d",__heartRate,int(totalEnergy_cal));
          ScreenObj.OverrideLine(ENERGY_LINE,txtScreenBuffer,2);//update the screen
          break;
        case HR_TIME:
          ScreenObj.OverrideLine(ENERGY_LINE-0.5," Time        HR",1);
          sprintf(txtScreenBuffer, "%d:%02d:%02d - %d",elapsed_workout_hours,elapsed_workout_minutes,elapsed_workout_seconds,__heartRate);
          ScreenObj.OverrideLine_smallerFont(ENERGY_LINE,txtScreenBuffer,1);//update the screen
          break;
        case ENERGY_TIME:
          ScreenObj.OverrideLine(ENERGY_LINE-0.5," Time    Energy[Cal]",1);
          sprintf(txtScreenBuffer, "%d:%02d:%02d - %d",elapsed_workout_hours,elapsed_workout_minutes,elapsed_workout_seconds,int(totalEnergy_cal));
          ScreenObj.OverrideLine_smallerFont(ENERGY_LINE,txtScreenBuffer,1);//update the screen
          break;
      
      }
    }
    IrReceiver.resume(); // Receive the next value
  }
  if ((millis() - ok_press_count_time) > TIME_TO_START_OK_BUTTON_LIFTOFF_MILLIS) //if the user hasnt been pressing OK for more than a certain amount of time
    ok_press_count = 0;

  rescaledSteeringVal = limitvalue((SteeringAngleObj.getSteeringAngle_magnet()) ,-127,127);
  int rescaledPower   = limitvalue((__cyclePower*1.0/maxPower)*127.0     ,0   ,127);  
  //int rescaledbrakingperc = limitvalue((BrakeValueObj.getBrakingPerc()*1.27) ,0   ,127);

  if ((millis() - old_millisBLE) > 1000)//once every second we get data from bluetooth
  {
    BLECommObj.loop();//let the power update
    BLEComm_HeartRateObj.loop();//let the heartrate update

     

    //Serial.println(buzzer_on);
      if ((__cyclePower<maxPower) && (!buzzer_on) && (currentSystemState == PLAYING))
      {
        analogWrite(BUZZER_PIN, 4);
        buzzer_on = true;
      }
      else
      {
        analogWrite(BUZZER_PIN, 0);
        buzzer_on = false;
      }

    
    if ((__cyclePower != __cyclePower_old) || power_screen_update)
    {
            //it only makes sense to update power on screen if we actually have a new power 
      sprintf(txtScreenBuffer, "%dw /%dw",__cyclePower,maxPower);
      ScreenObj.OverrideLine(0,txtScreenBuffer,2); 
      ScreenObj.DrawProgressBar(THROTTLE_LINE,THROTTLE_THICK,(rescaledPower*(100.0/127.0)),1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight
      __cyclePower_old = __cyclePower;
      power_screen_update = false;
    }

    millisEnergy = millis();
    totalEnergy_cal += (4.0*(((millisEnergy - old_millisEnergy)/1000.0) * __cyclePower * 0.000239006));
    old_millisEnergy = millisEnergy;

    if ((dataFielsToShow == HR_ENERGY) && ((int(old_totalEnergy_cal) != int(totalEnergy_cal)) || (__heartRate != __heartRate_old)))//so if we have the right screen, and both the HR and energy updated  or the user changed data screen
    {
      sprintf(txtScreenBuffer, "%d - %d",__heartRate,int(totalEnergy_cal));
      old_totalEnergy_cal = totalEnergy_cal;
      __heartRate_old = __heartRate;
      ScreenObj.OverrideLine(ENERGY_LINE,txtScreenBuffer,2);//update the screen
    }
    if ((dataFielsToShow == HR_TIME))//so if we have the right screen, we know every second time screen needs to be updated amyway, so no need to check
    {
        sprintf(txtScreenBuffer, "%d:%02d:%02d - %d",elapsed_workout_hours,elapsed_workout_minutes,elapsed_workout_seconds,__heartRate);
        //sprintf(txtScreenBuffer, "%d:%02d:%02d - %d",9,99,99,999);
        ScreenObj.OverrideLine_smallerFont(ENERGY_LINE,txtScreenBuffer,1);//update the screen
    }
    if ((dataFielsToShow == ENERGY_TIME))//so if we have the right screen, we know every second time screen needs to be updated amyway, so no need to check
    {
      sprintf(txtScreenBuffer, "%d:%02d:%02d - %d",elapsed_workout_hours,elapsed_workout_minutes,elapsed_workout_seconds,int(totalEnergy_cal));
      ScreenObj.OverrideLine_smallerFont(ENERGY_LINE,txtScreenBuffer,1);//update the screen
    }
    old_millisBLE = millis();
  }

  if ((millis() - old_millisScreen) > screenUpdateRateMillis)//check to update screen also only once an interval
  {
    bool isBrakePressed = TouchObj.isBrakePressed();
    bool isBoostPressed = TouchObj.isBoostPressed();

    if (isBrakePressed && !isBoostPressed && (touch_control_case != 0))//DrawDoubleDirProgressBar(int _lineNo, float barHeight, int progress)
    {
      ScreenObj.DrawDoubleDirProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,100);
      touch_control_case = 0; 
    }      
    if (isBoostPressed && !isBrakePressed && (touch_control_case != 1))
    {
      //Serial.println("showleft");
      ScreenObj.DrawDoubleDirProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,-100);
      touch_control_case = 1;
    }
    if (isBrakePressed && isBoostPressed && (touch_control_case != 2))
    {
      ScreenObj.DrawProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,100,1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight
      touch_control_case = 2;
    }     
    if (!isBoostPressed && !isBrakePressed && (touch_control_case != 3))    
    {
      ScreenObj.DrawProgressBar(CAP_TOUCH_LINE,CAP_TOUCH_THICK,1,1);//(int _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight
      touch_control_case = 3;
    } 

    if (rescaledSteeringVal != rescaledSteeringVal_old)
    {
      ScreenObj.DrawDoubleDirProgressBar(STEER_LINE,STEER_THICK,rescaledSteeringVal*(-100.0/127.0));
      rescaledSteeringVal_old = rescaledSteeringVal;
    }
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


  Gamepad.rightStick(0,rescaledPower);  // Z Axis, Z Rotation
  Gamepad.leftStick(rescaledSteeringVal,0);  

  TouchObj.loop();

  if ((millis() - millisBLCheck) > 10000)//try to reconnect if BL is disconnected, needs debugging!
  {
      if (!BLEComm_HeartRateObj.isConnected())
      {
        BLEComm_HeartRateObj.begin();
      }
      if (!BLECommObj.isConnected())
      {
        BLECommObj.begin();
      }
      millisBLCheck = millis();
  }


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

int loopint(int in, int max)
{
  if (in < 0)
    return max;
  if (in > max)
    return 0;
  return in;
}

void IRAM_ATTR onTimer(){
  seconds_updated = true;
}

#endif /* ARDUINO_USB_MODE */
