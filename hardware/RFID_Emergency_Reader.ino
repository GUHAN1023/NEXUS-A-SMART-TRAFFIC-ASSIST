#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://nexus-dashboard.local/api/sensors/rfid/ingest";

#define RST_PIN   22
#define SS_PIN    21
MFRC522 mfrc522(SS_PIN, RST_PIN);

const String SCANNER_ID = "CORRIDOR-A";

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("RFID Reader Ready");
}

void loop() {
  // Look for new RFID tags (like Ambulance or Fire Truck tags)
  if ( ! mfrc522.PICC_IsNewCardPresent()) return;
  if ( ! mfrc522.PICC_ReadCardSerial()) return;

  String rfidTag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfidTag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    rfidTag += String(mfrc522.uid.uidByte[i], HEX);
  }
  rfidTag.toUpperCase();
  Serial.println("Emergency Tag Detected: " + rfidTag);
  
  sendEmergencyAlert(rfidTag);
  
  // Halt PICC
  mfrc522.PICC_HaltA();
  delay(3000); // Wait 3 seconds before next scan
}

void sendEmergencyAlert(String tag) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"scannerId\": \"" + SCANNER_ID + "\", \"tagId\": \"" + tag + "\", \"type\": \"EMERGENCY_VEHICLE\"}";
    
    int resp = http.POST(payload);
    Serial.println("Dashboard Response: " + String(resp));
    http.end();
  }
}
