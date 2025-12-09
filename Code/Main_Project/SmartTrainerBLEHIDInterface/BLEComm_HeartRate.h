#ifndef BLEComm_HeartRate_h
#define BLEComm_HeartRate_h

#include "Arduino.h"
#include "BLEDevice.h"

  
extern uint16_t __heartRate;
extern uint16_t __heartRate_old;

class BLEComm_HeartRate
{

  public:
    BLEComm_HeartRate();
    void begin();
    void loop();
    
  private:
    bool connectToServer();
    BLERemoteCharacteristic* pRemoteCharacteristic;


};


#endif
