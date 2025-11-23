#ifndef SDControl_h
#define SDControl_h

#include "Arduino.h"

#include <SPI.h>
#include "FS.h"
#include "SD.h"

class SDControl
{
  public:
    SDControl(int _SD_MOSI, int _SD_MISO, int _SD_SCLK, int _SD_CS);
    void begin();
    
  private:
    int SD_MOSI;
    int SD_MISO;
    int SD_SCLK;
    int SD_CS;

};


#endif
