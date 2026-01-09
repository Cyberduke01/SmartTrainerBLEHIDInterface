#ifndef Workout_h
#define Workout_h

#include "Arduino.h"
#include "ScreenControl.h"
#include "ArrayList.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define STR_BUFFER_SIZE_TOTAL 4500*4*2+100  //How long can the string realistically be, get the first part from STR_BUFFER_SIZE in arraylist.h then times 2 for the 2 arrays plus 100 for the rest

//static char buffer_heartrates[STR_BUFFER_SIZE];
//static char buffer_powers[STR_BUFFER_SIZE];
static char postData[STR_BUFFER_SIZE_TOTAL] = "endtime=";


class Workout
{

  public:
    Workout(ScreenControl *screen);
    void BeginWorkout();
    void SaveWorkout(int energy_cal);
    bool AddDataPoint(int heart_rate,int power);

  private:

    void post_workout_to_server();
    void shutdown_bluetooth();
    void getTime(char* time);
    
    void countdown(int seconds);

    const char* ssid = "FRITZ!Box 7530 XB";
    const char* password = "30812995310572268788";

    const char* ntpServer = "pool.ntp.org";
    const char* UploadServerName = "https://iantools.co.za/uploadworkout.php";
    const long gmtOffset_sec = 3600;      // UTC +1
    const int  daylightOffset_sec = 3600; // DST

    ArrayList list_heart_rates;
    ArrayList list_powers;

    ScreenControl *screen;  
};


#endif
