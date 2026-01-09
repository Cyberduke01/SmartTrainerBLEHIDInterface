#include <WiFi.h>
#include <HTTPClient.h>

// 🔹 Replace with your WiFi credentials
const char* ssid = "FRITZ!Box 7530 XB";
const char* password = "30812995310572268788";

// 🔹 Server URL
const char* serverName = "https://iantools.co.za/uploadworkout.php";

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");

  // Make HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Data to send (same as Python dictionary)
    char* postData = "endtime=11:16:13&heartrates=50,51,52,55&powers=50,51,52,90";

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();

      Serial.println("Response:");
      Serial.println(response);
      Serial.print("Status Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  // Nothing needed here
}