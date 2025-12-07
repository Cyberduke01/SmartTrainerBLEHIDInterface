#include <Arduino.h>
#include "ScreenControl.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);//Screen width , Screen height,wire , reset pin

ScreenControl::ScreenControl(int _SCREEN_SDA, int _SCREEN_SCL)
{
  this->SCREEN_SDA = _SCREEN_SDA;
  this->SCREEN_SCL = _SCREEN_SCL;
}

void ScreenControl::DrawProgressBar(int _lineNo, float barHeight, int progress)//height is percentage of lineheight
{
  int x0 = 0;
  int y0 = this->lineHeight * _lineNo; 
  display.fillRect(x0, y0, 128, int(this->lineHeight * (barHeight*1.0)), BLACK);
  display.drawRect(x0, y0, 128, int(this->lineHeight * (barHeight*1.0)), WHITE);
  display.fillRect(x0, y0, int(progress/100.0*128.0), int(this->lineHeight * (barHeight*1.0)), WHITE);
  display.display();
  /*int x0 = 0;
  int y0 = this->lineHeight * _lineNo; 
  display.fillRect(x0, y0, 128, int(5), BLACK);
  display.drawRect(x0, y0, 128, int(5), WHITE);
  display.fillRect(x0, y0, int(newProgress/100.0*128.0), int(5), WHITE);
  display.display();*/
}

void ScreenControl::OverrideLine(int _lineNo,char* text)
{
  int x0 = 0;
  int y0 = this->lineHeight * _lineNo; 
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
  display.setCursor(0, lineNo * this->lineHeight);//x and y position to start writing
  display.println(text);
  display.display();
  //delay(5000);  // Show greeting for 5 seconds
}