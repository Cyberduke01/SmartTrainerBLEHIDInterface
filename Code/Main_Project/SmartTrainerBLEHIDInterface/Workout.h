#ifndef Workout_h
#define Workout_h

#include "Arduino.h"
#include "ScreenControl.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define MAX_SIZE_SECONDS 7200  //the amount of seconds that we can save, 60 x 60 = 3600  for every hour
#define STR_BUFFER_SIZE 7200*4  //How long can the string for the data realistically be, assume 3 digits per point, then plus comma is x4 the MAX_SIZE
#define STR_BUFFER_SIZE_TOTAL 7200*4*2+100  //How long can the string that we post to the server realistically be, get the first part from STR_BUFFER_SIZE then times 2 for the 2 arrays plus 100 for the rest like time etc

static char strPbuffer[STR_BUFFER_SIZE] = "";//here power will be stored seperately
static char strPostData[STR_BUFFER_SIZE_TOTAL] = "heartrates=";//will be populated with HR data as we go along, then power and actuall time will be added before we push


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

    int current_size_seconds;
    int retrycount = 0;

    ScreenControl *screen;  
};


#endif
