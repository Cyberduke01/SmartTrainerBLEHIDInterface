#include <Arduino.h>
#include "SDControl.h"


SDControl::SDControl(int _SD_MOSI, int _SD_MISO, int _SD_SCLK, int _SD_CS)
{
  this->SD_MOSI = _SD_MOSI;
  this->SD_MISO = _SD_MISO;
  this->SD_SCLK = _SD_SCLK;
  this->SD_CS   = _SD_CS;
}


void SDControl::begin()
{
  
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);  
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("SD Card MOUNT FAIL");
  }
  else
  {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE)
    {
      Serial.println("No SD card attached");
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } 
    else if(cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } 
    else if(cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } 
    else 
    {
        Serial.println("UNKNOWN");
    }
  }
}