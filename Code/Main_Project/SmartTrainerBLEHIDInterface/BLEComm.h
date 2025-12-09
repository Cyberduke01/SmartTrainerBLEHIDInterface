#ifndef BLEComm_h
#define BLEComm_h



#include "Arduino.h"
#include "BLEDevice.h"

  
extern uint16_t __cyclePower;
extern uint16_t __cyclePower_old;


class BLEComm
{

  public:


    BLEComm();
    void begin();
    void loop();
    
  private:
    
    bool connectToServer();
  
    BLERemoteCharacteristic* pRemoteCharacteristic;


};


#endif
