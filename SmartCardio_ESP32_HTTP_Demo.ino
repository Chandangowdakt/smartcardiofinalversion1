/*
  SmartCardio ESP32 – REAL/SIM TEST VERSION (HTTP POST)

  - Reads optional ECG (AD8232) on GPIO 34  (if connected)
  - Uses fake HR/SpO2/BP by default so that the web dashboard always works
  - Sends JSON to backend every ~1 sec:
        http://<YOUR_LAPTOP_IPV4>:5000/api/data/push

  To use real MAX30102 + AD8232 later, you can extend this sketch.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= WIFI SETTINGS =======================
const char* ssid     = "smartcardio";       // change if needed
const char* password = "smartcardio@123";   // change if needed

// ================= BACKEND BASE URL ====================
// Replace with your laptop IPv4 when running the backend.
// Example: "http://192.168.137.1:5000"
const char* serverBase = "http://192.168.137.1:5000";

// ================= OPTIONAL ECG PIN ====================
#define ECG_PIN 34   // AD8232 OUTPUT -> ESP32 GPIO 34

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 1000;

// -----------------------------------
void ensureWifi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("Reconnecting WiFi...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  Serial.println();
}

// -----------------------------------
void captureFakeECG(JsonArray arr) {
  const int samples = 128;
  for (int i = 0; i < samples; i++) {
    int value = 2048 + 500 * sin(0.1 * i); // fake ECG-like waveform
    arr.add(value);
    delay(2);
  }
}

// -----------------------------------
void sendData() {
  ensureWifi();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping send.");
    return;
  }

  int hr  = 72 + random(-5, 6);      // fake heart rate
  int spo2 = 97 + random(-2, 1);     // fake SpO2
  int sys = 118 + random(-6, 7);     // fake BP systolic
  int dia = 76 + random(-4, 5);      // fake BP diastolic

  StaticJsonDocument<4096> doc;
  doc["deviceId"] = "ESP32-DEMO";
  doc["hr"] = hr;
  doc["spo2"] = spo2;
  doc["sys"] = sys;
  doc["dia"] = dia;

  JsonArray ecgArr = doc.createNestedArray("ecg");
  captureFakeECG(ecgArr);

  JsonArray ppgArr = doc.createNestedArray("ppg");
  for (int i = 0; i < 64; i++) {
    int val = 2000 + 200 * sin(0.2 * i);
    ppgArr.add(val);
  }

  String body;
  serializeJson(doc, body);

  HTTPClient http;
  String url = String(serverBase) + "/api/data/push";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  Serial.print("POST -> ");
  Serial.println(url);
  Serial.println(body);

  int code = http.POST(body);
  Serial.print("HTTP Status = ");
  Serial.println(code);
  if (code > 0) {
    Serial.println(http.getString());
  }

  http.end();
}

// -----------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  pinMode(ECG_PIN, INPUT);

  lastSend = millis();
}

// -----------------------------------
void loop() {
  if (millis() - lastSend >= SEND_INTERVAL) {
    sendData();
    lastSend = millis();
  }
}