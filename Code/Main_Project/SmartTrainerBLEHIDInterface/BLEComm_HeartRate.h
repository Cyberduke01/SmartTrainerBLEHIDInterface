#ifndef BLEComm_HeartRate_h
#define BLEComm_HeartRate_h

/*
BLE Advertised Device found: Name: Forerunner, Address: c3:3e:6a:ef:23:78, serviceUUID: 0000180d-0000-1000-8000-00805f9b34fb, rssi: -63, serviceData:
BLE Advertised Device found: Name: , Address: f0:9a:cb:ce:7a:96, manufacturer data: 4c0012199067d538c5a60a72a2b4679629b185f047fbef780519d20120, rssi: -87
BLE Advertised Device found: Name: , Address: 51:68:c1:b1:ab:11, manufacturer data: 4c0010050c1c09fdda, txPower: 17, rssi: -92
BLE Advertised Device found: Name: , Address: 45:6d:ff:fb:93:a7, manufacturer data: 4c0010060f1dbb5d7258, txPower: 12, rssi: -83
BLE Advertised Device found: Name: , Address: 53:e7:57:01:89:55, manufacturer data: 4c00160800d0896bcf934535, rssi: -80
BLE Advertised Device found: Name: , Address: 07:58:4b:e9:d5:55, manufacturer data: 0600010920220e68fc58d4a87db4e78cf463ba62697ef21605df6e6d3d, rssi: -8
BLE Advertised Device found: Name: , Address: 4c:aa:d0:81:94:74, manufacturer data: 4c0010050e1cc2b8bf, txPower: 12, rssi: -66
BLE Advertised Device found: Name: Tacx Flux 37271, Address: eb:e1:14:76:b9:3b, manufacturer data: 89062900, serviceUUID: 00001826-0000-1000-8000-00805f9b34fb, serviceUUID: 00001818-0000-1000-8000-00805f9b34fb, serviceUUID: 6e40fec1-b5a3-f393-e0a9-e50e24dcca9e, rssi: -72, serviceData: 

*/

#include "Arduino.h"
#include "BLEDevice.h"

  
extern uint16_t __heartRate;


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
