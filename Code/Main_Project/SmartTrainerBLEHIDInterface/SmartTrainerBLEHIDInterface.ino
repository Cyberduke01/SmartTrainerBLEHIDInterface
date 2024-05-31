/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */
 //sudo chmod 666 /dev/ttyACM0


#include <BLEComm.h>
#include <SteeringAngle.h>


#define STEERING_POT_PIN 4



BLEComm BLECommObj;
SteeringAngle SteeringAngleObj(STEERING_POT_PIN);


//varible such that bluetooth only gets val once a second
unsigned long old_millis = millis();


void setup() {
  Serial.begin(115200);
  BLECommObj.begin();
  SteeringAngleObj.begin();


  old_millis = millis();     

} // End of setup.


// This is the Arduino main loop function.
void loop() {

  if ((millis() - old_millis) > 1000)
    {
      BLECommObj.loop();
      old_millis = millis();     
    }

  Serial.print("Cycling power: ");
  Serial.println(__cyclePower);

  Serial.print("Steering Angle: ");
  Serial.println(SteeringAngleObj.getSteeringAngle());

    


  delay(10); // Delay a second between loops.
} // End of loop
