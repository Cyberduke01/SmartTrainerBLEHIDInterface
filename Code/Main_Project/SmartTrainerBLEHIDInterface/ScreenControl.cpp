#include <Arduino.h>
#include "ScreenControl.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);//Screen width , Screen height,wire , reset pin

ScreenControl::ScreenControl(int _SCREEN_SDA, int _SCREEN_SCL)
{
  this->SCREEN_SDA = _SCREEN_SDA;
  this->SCREEN_SCL = _SCREEN_SCL;
}

void ScreenControl::DrawProgressBar(float _lineNo, float barHeight, int progress, int dir)//height is percentage of lineheight
{
  int x_prog = int(progress/100.0*128.0);

  int y0 = int(this->lineHeight * _lineNo); 
  display.fillRect(0, y0, 128, int(this->lineHeight * (barHeight*1.0)), BLACK);//clears all
  display.drawRect(0, y0, 128, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws empty box
  if (dir == -1)
    display.fillRect(x_prog, y0, 128, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws progress
  else
    display.fillRect(0, y0, x_prog, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws progress
  display.display();
}

void ScreenControl::DrawDoubleDirProgressBar(float _lineNo, float barHeight, int progress)
{
  int x_prog = int((progress*(128.0/200.0))+64.0);

  int y0 = int(this->lineHeight * _lineNo); 
  display.fillRect(0, y0, 128, int(this->lineHeight * (barHeight*1.0)), BLACK);//clears all
  display.drawRect(0, y0, 128, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws empty box
  if (progress < 0)
    display.fillRect(x_prog, y0, 64-x_prog, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws progress
  else
    display.fillRect(64, y0, x_prog-64, int(this->lineHeight * (barHeight*1.0)), WHITE);//draws progress
  display.display();
}

void ScreenControl::OverrideLine(float _lineNo,char* text,int _size)
{
  int x0 = 0;
  int y0 = int(this->lineHeight * _lineNo); 
  display.fillRect(x0, y0, 128, 0.5* this->lineHeight * _size, BLACK);
  SetLine(_lineNo,text,_size);
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

void ScreenControl::ClearScreen()
{
  display.clearDisplay();
}

void ScreenControl::SetLine(float lineNo,char* text,int _size)
{
  display.setTextSize(_size);
  display.setCursor(0, int(lineNo * this->lineHeight));//x and y position to start writing
  display.println(text);
  display.display();
  //delay(5000);  // Show greeting for 5 seconds
}