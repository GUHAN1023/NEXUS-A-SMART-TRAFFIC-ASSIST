#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://nexus-dashboard.local/api/sensors/ultrasonic/ingest";

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const String SLOT_ID = "ZONE-A-12";
const int THRESHOLD_CM = 50; // Distance less than 50cm means car is parked

bool wasOccupied = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  long duration, distance;
  
  // Trigger ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Convert to cm
  
  bool isOccupied = (distance > 0 && distance < THRESHOLD_CM);
  
  // Only send update if state changes to avoid spamming the API
  if (isOccupied != wasOccupied) {
    sendParkingStatus(isOccupied);
    wasOccupied = isOccupied;
  }
  
  delay(1000); // Check every second
}

void sendParkingStatus(bool occupied) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String status = occupied ? "taken" : "free";
    String payload = "{\"slotId\": \"" + SLOT_ID + "\", \"status\": \"" + status + "\"}";
    
    http.POST(payload);
    http.end();
  }
}
