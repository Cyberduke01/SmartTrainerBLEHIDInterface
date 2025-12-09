#include <Arduino.h>
#include "BLEComm_HeartRate.h"
#include "BLEDevice.h"


uint16_t __heartRate = 0;
uint16_t __heartRate_old = 0;

//15:06:57.007 -> HR BLE Advertised Device found: Name: Forerunner, Address: c3:3e:6a:ef:23:78, serviceUUID: 0000180d-0000-1000-8000-00805f9b34fb, rssi: -75, serviceData: 

// The remote service we wish to connect to.
static BLEUUID serviceUUID_HR("0000180d-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID_HR("00002A37-0000-1000-8000-00805f9b34fb");

static boolean connected       = false;
static BLEAdvertisedDevice*    myDevice;
static boolean doConnect       = false;
static boolean doScan          = false;


class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};



 static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    /*Serial.print("Notify callback for HR characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);

  //this prints every individual byte in hex and base 10
    for (int i = 0; i < length; i++)
    {
      uint8_t temp = pData[i];

      Serial.print("i = ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(temp,HEX);
      Serial.print("<==>");
      Serial.print(temp);
      Serial.println(" ");
    }

      Serial.print("Heart rate: ");
      Serial.println( pData[1]);*/

    __heartRate = pData[1];
}


/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks_HR: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
   //Serial.println("checking DO connect HR");
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("HR BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID_HR)) {
      //Serial.println("This is the droid I was looking for!");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    } // Found our server


  } // onResult
}; // MyAdvertisedDeviceCallbacks


bool BLEComm_HeartRate::connectToServer() {
    Serial.print("Forming a connection to heart rate: ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID_HR);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID_HR.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID_HR);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID_HR.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      //std::string value = pRemoteCharacteristic->readValue();
      //Serial.print("The characteristic value was: ");
      //Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

BLEComm_HeartRate::BLEComm_HeartRate()
{

  Serial.println("Creating Class Heart Rate");
  //this->cyclePower = _cyclePower;

}


void BLEComm_HeartRate::begin()
{
  Serial.println("Starting Arduino BLE Client application for heart rate...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks_HR());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  //End of bluetooth Setup

}

void BLEComm_HeartRate::loop()
{
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  //Serial.println("Heart rate BL loop");
  if (doConnect == true) {
    
    Serial.println("Heart rate DoConnect!");
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE heart rate Server.");
    } else {
      Serial.println("We have failed to connect to the heart rate server; there is nothing more we will do.");
    }
     doConnect = false;
  }



  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    //Set the characteristic's value to be the array of bytes that is actually a string.
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

}
