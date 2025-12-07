#ifndef ScreenControl_h
#define ScreenControl_h

#include "Arduino.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



class ScreenControl
{
  public:
    ScreenControl(int _SCREEN_SDA, int _SCREEN_SCL);
    void begin();

    void SetLine(int lineNo,char* text);
    void SetLine(int lineNo,char* text,int _size);
    void OverrideLine(int lineNo,char* text);
    void DrawProgressBar(int _lineNo, float barHeight, int progress);
    void ClearScreen();
    
  private:
    int SCREEN_SDA;
    int SCREEN_SCL;

    int textSize = 2;
    int lineHeight = 15;//for spacing of the lines

    static const int screenWidth = 128;
    static const int screenHeight= 64;
    static const int resetPin = -1;

    
};


#endif
