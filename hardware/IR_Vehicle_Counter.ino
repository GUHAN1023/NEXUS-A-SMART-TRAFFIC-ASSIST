#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://nexus-dashboard.local/api/sensors/ir/ingest";

const int IR_PIN = 4; // IR Sensor connected to digital pin 4
const String JUNCTION_ID = "JUNC-02";

int vehicleCount = 0;
bool isObstacle = false;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 10000; // Send batch to server every 10 sec

void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void loop() {
  // Read IR Sensor (LOW means obstacle detected in most IR modules)
  int val = digitalRead(IR_PIN);
  
  if (val == LOW && !isObstacle) {
    isObstacle = true;
    vehicleCount++;
    Serial.println("Vehicle detected!");
  } else if (val == HIGH) {
    isObstacle = false;
  }
  
  // Send data to NEXUS backend every UPDATE_INTERVAL
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    sendVehicleData();
    lastUpdate = millis();
    vehicleCount = 0; // Reset counter for the next batch
  }
}

void sendVehicleData() {
  if (WiFi.status() == WL_CONNECTED && vehicleCount > 0) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"junctionId\": \"" + JUNCTION_ID + "\", \"count\": " + String(vehicleCount) + "}";
    http.POST(payload);
    http.end();
  }
}
