#include <Arduino.h>
#include "Workout.h"

#include "esp_bt.h"


Workout::Workout(ScreenControl *_screen)
{
  this->screen = _screen;
}



void Workout::BeginWorkout()
{

 //this->countdown(3);
 this->screen->ClearScreen();
 this->screen->SetLine(1,"Start",3);
 unsigned long start = millis();
 while (millis() - start < (500));//wait half a second
 this->screen->ClearScreen();

 list_heart_rates.clear();
 list_powers.clear();


 //Save the time
}

void Workout::SaveWorkout(int energy_cal)
{
  //energy_cal = 6;
  //Serial.println(list_heart_rates.toStr()); 
  //Serial.println(list_powers.toStr()); 

  this->screen->ClearScreen();
  this->screen->SetLine(1,"Saving...",3);
  unsigned long start = millis();
  while (millis() - start < (500));//wait half a second
  //this->screen->ClearScreen();

  int httpResponseCode = 0;
  while (httpResponseCode != 200)
  {

   //Shutting down bluetooth, necceceriy for WiFi
    btStop();                      // Arduino wrapper
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
    esp_bt_mem_release(ESP_BT_MODE_BLE);


    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
      Serial.println("\nConnected to WiFi");
      delay(1000);
    // Make HTTP POST request
    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      http.begin(UploadServerName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Data to send (same as Python dictionary)
      //char postData[2000] = "endtime=14:16:13&heartrates=50,51,52,55&powers=50,51,52,90";

      //get time
      char time[32];
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo))
      {
        strftime(time,sizeof(time),"%H:%M:%S",&timeinfo);
      }

        
      //get energy
     char strcalories[12]; // enough for int (-2147483648)
     itoa(energy_cal, strcalories, 10);

      //list_heart_rates.toStr(buffer_heartrates);
      //list_powers.toStr(buffer_powers);
    //char* data = "&heartrates=50,51,52,55&powers=50,51,52,90";
      //char* time = "12:13:10";

      //char postData[2000] = "endtime=";//240*60*3+240*60*3+100 = 86500
      snprintf(postData, sizeof(postData), "%s", "endtime=");
      strcat(postData,time);
      strcat(postData,"&calories=");
      strcat(postData,strcalories);
      strcat(postData,"&heartrates=");

      strcat(postData,list_heart_rates.toStr(true));
      //strcat(postData,"50,51,52,55");

      strcat(postData,"&powers=");

      strcat(postData,list_powers.toStr(false));
      //strcat(postData,"50,51,52,90");

      Serial.println(postData);

      httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        String response = http.getString();

        Serial.println("Response:");
        Serial.println(response);
        Serial.print("Status Code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }

  delay(1000);
  this->screen->ClearScreen();
  this->screen->SetLine(1,"retrying...",2);
  
  //WiFi.disconnect(true);   // disconnect + erase config
  //WiFi.mode(WIFI_OFF);     // turn radio off

  }

  this->screen->ClearScreen();
  this->screen->SetLine(1,"Saved!!   please    restart",2);
  while(1==1);

}


bool Workout::AddDataPoint(int heart_rate,int power)
{//make a check for the max length of array, if to much, save workout
  if (!(list_heart_rates.add(heart_rate,true) && list_powers.add(power,false)))
  {
    return false; 
  }
  return true;
}


void Workout::countdown(int seconds)
{
  unsigned long start = millis();
  int i = 0;
  char txtScreenBuffer[1];      
  while (millis() - start < (seconds*1000))
  {
    if ((millis() - start) > (i * 1000))
    {
      sprintf(txtScreenBuffer, "  %d",seconds-i);
      this->screen->ClearScreen();
      this->screen->SetLine(1,txtScreenBuffer,4);
      i++;
    }
  }
}

void Workout::post_workout_to_server()
{

}

void Workout::shutdown_bluetooth()
{

}

void Workout::getTime(char* time)
{

}     
