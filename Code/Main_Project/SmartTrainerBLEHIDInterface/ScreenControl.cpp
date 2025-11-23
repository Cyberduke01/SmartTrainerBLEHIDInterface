#include <Arduino.h>
#include "ScreenControl.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);//Screen width , Screen height,wire , reset pin
 
ScreenControl::ScreenControl(int _SCREEN_SDA, int _SCREEN_SCL)
{
  this->SCREEN_SDA = _SCREEN_SDA;
  this->SCREEN_SCL = _SCREEN_SCL;
}

void ScreenControl::OverrideLine(int _lineNo,char* text)
{
  int x0 = 0;
  int y0 = lineHeight * _lineNo; 
  display.fillRect(x0, y0, 128, lineHeight, BLACK);
  SetLine(_lineNo,text);
}
void ScreenControl::begin()
{  
  Wire.setPins(SCREEN_SDA,SCREEN_SCL);

//Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, resetPin);//Screen width , Screen height,wire , reset pin  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(textSize);
}

void ScreenControl::SetLine(int lineNo,char* text)
{
  display.setTextSize(textSize);
  display.setCursor(0, lineNo * lineHeight);//x and y position to start writing
  display.println(text);
  display.display();
  //delay(5000);  // Show greeting for 5 seconds
}

void ScreenControl::ClearScreen()
{
  display.clearDisplay();
}

void ScreenControl::SetLine(int lineNo,char* text,int _size)
{
  display.setTextSize(_size);
  display.setCursor(0, lineNo * lineHeight);//x and y position to start writing
  display.println(text);
  display.display();
  //delay(5000);  // Show greeting for 5 seconds
}