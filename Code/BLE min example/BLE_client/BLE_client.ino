/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

#define STEERING_POT_PIN 4

//Calibration values for steering angle
uint16_t centreVal = 2023;
float    DegPerVal = -0.034695;
float    SteeringAngle = 0;

// The remote service we wish to connect to.
static BLEUUID serviceUUID("00001818-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("00002A63-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);

  //this prints every individual byte in hex and base 10
  /*  for (int i = 0; i < length; i++)
    {
      uint8_t temp = pData[i];

      Serial.print("i = ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(temp,HEX);
      Serial.print("<==>");
      Serial.print(temp);
      Serial.println(" ");
    }*/

 
    uint16_t power        =    (pData[3] << 8 | pData[2] );
    uint16_t power2       =  *(pData+2); //(pData[3] << 8 | pData[2] );

    uint16_t revolutions =  (pData[5] << 8 | pData[4]) ;

    Serial.print("Power: ");
    Serial.println(power);

    Serial.print("Power2: ");
    Serial.println(power2);

    Serial.print("Revolutions: ");
    Serial.println(revolutions);

    //Serial.write(pData, length);
    Serial.println();
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
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
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void countdown(int seconds)
{
  unsigned long start = millis();
  int i = 0;
        
  while (millis() - start < (seconds*1000))
  {
    if ((millis() - start) > (i * 1000))
    {
      Serial.println(seconds-i);
      i++;
    }
  }
}


void setup() {
  Serial.begin(115200);

 //ask User if they want to calibra steering
 // Serial.setTimeout(5000);
  Serial.println("If steering calibration is required, input 'Y' in the next 5 Seconds");
  delay(7000); // Delay a second between loops.

  if (Serial.available())  {
    char c = Serial.read();  //gets one byte from serial buffer
    if ((c == 'y')||(c == 'Y')) {
        Serial.println("Starting Steering calibration");
        Serial.println("Move steering wheel to the zero degree position");
        Serial.println("value will be captured in 10 Seconds");
        countdown(10);
        centreVal = analogRead(STEERING_POT_PIN);//read value from pin pot
        Serial.println("Value Captured!");
        Serial.println("Now move steering wheel 45 Degrees to the left");
        Serial.println("value will be captured in 10 Seconds");
        countdown(10);
        uint16_t left45val= analogRead(STEERING_POT_PIN);
        Serial.println("Value Captured!");
        Serial.println("Now move steering wheel 45 Degrees to the right");
        Serial.println("value will be captured in 10 Seconds");
        countdown(10);
        uint16_t right45val= analogRead(STEERING_POT_PIN);
        Serial.println("Value Captured!");
        DegPerVal = 90.0 / (right45val-left45val);//read value from pin pot;
        Serial.println("=================================");
        Serial.println("Calibration values determined:");
        Serial.print("Centre Value: ");
        Serial.println(centreVal);
        Serial.print("Degree per value determined: ");
        Serial.printf("%f\n",DegPerVal);
        Serial.println("=================================");
        delay(10000); 

      }
    }
    else
      Serial.println("No steering angle calibration --> using saved values");


  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  //End of bluetooth Setup



  //DAC Setup
  analogReadResolution(12); // Read in 12 BIT

} // End of setup.


// This is the Arduino main loop function.
void loop() {


  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  int analogValue = analogRead(STEERING_POT_PIN);//read value from pot
  float SteeringAngle = (analogValue - centreVal) * DegPerVal;
  
  // print out the values you read:
  Serial.printf("Raw pot Value = %i\n",analogValue);
  Serial.printf("SteeringAngle = %f\n",SteeringAngle);



  delay(1000); // Delay a second between loops.
} // End of loop
