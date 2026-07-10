#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://nexus-dashboard.local/api/sensors/gps/ingest";

TinyGPSPlus gps;
HardwareSerial SerialGPS(1); // Use UART 1 for GPS

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17); // RX, TX pins
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      if (gps.location.isUpdated()) {
        sendGPSData(gps.location.lat(), gps.location.lng(), gps.speed.kmph());
      }
    }
  }
}

void sendGPSData(float lat, float lng, float speed) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"lat\": " + String(lat, 6) + 
                         ", \"lng\": " + String(lng, 6) + 
                         ", \"speed\": " + String(speed) + 
                         ", \"vehicleId\": \"BUS-402\"}";

    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully");
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  delay(5000); // Send data every 5 seconds
}
