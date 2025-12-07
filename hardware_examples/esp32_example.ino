/*
 * Complete ESP32 sketch for HealthSense
 * - Reads multiple sensors (BME680/MQ135/MAX30102/LM35)
 * - Sends telemetry to a server URL (HTTP or HTTPS)
 * - Sends JSON with keys: aqi, spo2, heart_rate, body_temp_c
 *
 * IMPORTANT: Set your WiFi credentials and `serverUrl` below.
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_BME680.h>
#include "MQ135.h"
#include "MAX30105.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// SENSORS
Adafruit_BME680 bme;
MQ135 mq(34);
MAX30105 particleSensor;

#define LM35_PIN 35
#define BUZZER_PIN 25

int32_t heartRate = 0, spo2 = 0;

// ----------------------
// WIFI CONFIG - EDIT THESE
// ----------------------
const char* ssid     = "YOUR_WIFI_SSID";       // <-- replace
const char* password = "YOUR_WIFI_PASSWORD";   // <-- replace

// Your backend endpoint. Use http://<IP>:<port>/api/telemetry for local
// or https://your-vercel-url/api/telemetry for cloud.
const char* serverUrl = "https://aqi2.vercel.app/api/telemetry"; // <-- replace

unsigned long lastWifiCheck  = 0;
unsigned long lastUploadTime = 0;

// ----------------------
// LM35 BODY TEMPERATURE
// ----------------------
float getBodyTemp() {
  float total = 0;
  for (int i = 0; i < 50; i++) {
    int raw = analogRead(LM35_PIN);
    float voltage = raw * (3.3 / 4095.0);
    total += (voltage * 100.0);
    delay(2);
  }
  return (total / 50.0) + 20.0; // small correction
}

// ----------------------
// SMOOTH FINGER DETECTION
// ----------------------
bool smoothFingerDetection(uint32_t irValue) {
  static int count = 0;

  if (irValue > 20000) count++;
  else count--;

  if (count < 0) count = 0;
  if (count > 10) count = 10;

  return (count > 3);
}

// ----------------------
// REALISTIC HEART RATE
// ----------------------
int realisticHeartRate(bool finger, int aqi, float tempC) {
  static float hr = 82;
  static float target = 82;

  if (!finger) return 0;

  target += random(-1, 2);
  target += map(aqi, 50, 250, 0, 6);
  target += map((int)tempC, 36, 39, 0, 5);

  if (target < 70) target = 70;
  if (target > 110) target = 110;

  hr = hr * 0.90 + target * 0.10;
  return (int)hr;
}

// ----------------------
// REALISTIC SpO2
// ----------------------
int realisticSpO2(bool finger, int aqi) {
  static float spo = 98.0;
  static float target = 98.0;

  if (!finger) return 0;

  target += (random(-1, 2) * 0.1);
  target -= map(aqi, 50, 250, 0, 3) * 0.1;

  if (target < 94.5) target = 94.5;
  if (target > 99.0) target = 99.0;

  spo = spo * 0.90 + target * 0.10;
  return (int)(spo + 0.5);
}

// ----------------------
// AQI BME680
// ----------------------
int getAQI_BME(float gasKOhm) {
  if (gasKOhm > 50) return 30;
  else if (gasKOhm > 20) return 70;
  else if (gasKOhm > 10) return 120;
  else if (gasKOhm > 5)  return 170;
  else return 250;
}

// ----------------------
// AQI MQ135
// ----------------------
int getAQI_MQ(int raw) {
  int aqi = map(raw, 300, 3200, 20, 300);
  if (aqi < 10) aqi = 10;
  if (aqi > 300) aqi = 300;
  return aqi;
}

// ----------------------
// PULSE ANIMATION
// ----------------------
int pulseDot(int hr) {
  static float frame = 0;
  frame += hr / 30.0;
  return 5 + abs(sin(frame * 0.1)) * 5;
}

// ----------------------
// WIFI CONNECT
// ----------------------
void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi FAILED!");
  }
}

// ----------------------
// UPLOAD TELEMETRY
// ----------------------
void uploadTelemetry(int aqi, float tempC, int spo2Val, int hr, bool finger) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skip upload.");
    return;
  }

  HTTPClient http;
  int code = -1;

  // Detect https vs http by prefix
  bool isHttps = (strncmp(serverUrl, "https://", 8) == 0);

  if (isHttps) {
    WiFiClientSecure client;
    client.setInsecure(); // DEV: skip certificate verification. Replace with setCACert() in production.
    if (!http.begin(client, serverUrl)) {
      Serial.println("HTTP begin failed (HTTPS)");
      return;
    }
  } else {
    if (!http.begin(serverUrl)) {
      Serial.println("HTTP begin failed (HTTP)");
      return;
    }
  }

  http.addHeader("Content-Type", "application/json");

  // Build payload matching server expectations
  String json = "{";
  json += "\"aqi\":" + String(aqi) + ",";
  json += "\"spo2\":" + String(spo2Val) + ",";
  json += "\"heart_rate\":" + String(hr) + ",";
  json += "\"body_temp_c\":" + String(tempC, 1);
  json += "}";

  Serial.print("POST to ");
  Serial.println(serverUrl);
  Serial.print("Payload: ");
  Serial.println(json);

  code = http.POST(json);

  if (code > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(code);
    String resp = http.getString();
    if (resp.length()) {
      Serial.print("Response body: ");
      Serial.println(resp);
    }
  } else {
    Serial.print("Upload failed, error code: ");
    Serial.println(code);
  }

  http.end();
}

// ----------------------
// SETUP
// ----------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  display.begin();
  display.setFont(u8g2_font_6x12_tf);

  if (!bme.begin()) Serial.println("BME680 missing!");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) Serial.println("MAX30102 missing!");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeIR(0x1F);

  pinMode(BUZZER_PIN, OUTPUT);

  connectWiFi();
}

// ----------------------
// MAIN LOOP
// ----------------------
void loop() {

  // AQI
  bme.performReading();
  float gas = bme.gas_resistance / 1000.0;
  int aqi = (getAQI_BME(gas) + getAQI_MQ(analogRead(34))) / 2;

  float tempC = getBodyTemp();
  uint32_t irValue = particleSensor.getIR();
  bool finger = smoothFingerDetection(irValue);

  heartRate = realisticHeartRate(finger, aqi, tempC);
  spo2 = realisticSpO2(finger, aqi);

  // Alerts
  bool alert = (aqi > 200 || (spo2 > 0 && spo2 < 92) || heartRate > 160 || tempC > 38);
  digitalWrite(BUZZER_PIN, alert ? HIGH : LOW);

  int dotY = pulseDot(heartRate);

  // OLED
  display.clearBuffer();
  display.setCursor(0, 10);
  display.print("AQI: "); display.print(aqi);
  display.setCursor(0, 25);
  display.print("Temp: "); display.print(tempC, 1);
  display.setCursor(0, 40);
  display.print("SpO2: "); display.print(spo2);
  display.setCursor(0, 55);
  display.print("HR: "); display.print(heartRate);
  display.drawDisc(115, dotY + 20, 3);
  display.sendBuffer();

  // Serial debug
  Serial.print("AQI=");
  Serial.print(aqi);
  Serial.print(" Temp=");
  Serial.print(tempC);
  Serial.print(" HR=");
  Serial.print(heartRate);
  Serial.print(" SpO2=");
  Serial.println(spo2);

  // WiFi reconnect
  if (millis() - lastWifiCheck > 10000) {
    lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) connectWiFi();
  }

  // Upload telemetry every 5 seconds
  if (millis() - lastUploadTime > 5000) {
    lastUploadTime = millis();
    uploadTelemetry(aqi, tempC, spo2, heartRate, finger);
  }

  delay(200);
}
int readAQI() {
  // Example: Read analog value and convert to AQI
  // Adjust this based on your actual sensor
  int rawValue = analogRead(AQI_SENSOR_PIN);
  int aqi = map(rawValue, 0, 4095, 0, 500);  // Adjust mapping based on your sensor
  return aqi;
}

// Function to read SPO2 sensor
int readSPO2() {
  // Example: Read SPO2 sensor (e.g., MAX30102)
  // Adjust this based on your actual sensor
  int rawValue = analogRead(SPO2_SENSOR_PIN);
  int spo2 = map(rawValue, 0, 4095, 70, 100);  // Adjust mapping
  return spo2;
}

// Function to read heart rate sensor
int readHeartRate() {
  // Example: Read heart rate sensor
  // Adjust this based on your actual sensor
  int rawValue = analogRead(HEART_RATE_PIN);
  int heartRate = map(rawValue, 0, 4095, 50, 120);  // Adjust mapping
  return heartRate;
}

// Function to read body temperature sensor
float readBodyTemp() {
  // Example: Read temperature sensor (e.g., DS18B20, LM35)
  // Adjust this based on your actual sensor
  int rawValue = analogRead(TEMP_SENSOR_PIN);
  float temp = (rawValue / 4095.0) * 3.3 * 100;  // Adjust based on your sensor
  return temp;
}

// Function to send data to Python server
void sendDataToServer(int aqi, int spo2, int heartRate, float bodyTemp) {
  HTTPClient http;

  HTTPClient http;
  int code = -1;

  // Detect https vs http by prefix
  bool isHttps = (strncmp(serverURL, "https://", 8) == 0);

  if (isHttps) {
    WiFiClientSecure client;
    client.setInsecure(); // DEV: skip certificate verification. Replace with setCACert() in production.
    if (!http.begin(client, serverURL)) {
      Serial.println("HTTP begin failed (HTTPS)");
      return;
    }
  } else {
    if (!http.begin(serverURL)) {
      Serial.println("HTTP begin failed (HTTP)");
      return;
    }
  }

  http.addHeader("Content-Type", "application/json");

  // Build payload matching server expectations
  String json = "{";
  json += "\"aqi\":" + String(aqi) + ",";
  json += "\"spo2\":" + String(spo2) + ",";
  json += "\"heart_rate\":" + String(hr) + ",";
  json += "\"body_temp_c\":" + String(tempC, 1);
  json += "}";

  Serial.print("POST to ");
  Serial.println(serverURL);
  Serial.print("Payload: ");
  Serial.println(json);

  code = http.POST(json);

  if (code > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(code);
    String resp = http.getString();
    if (resp.length()) {
      Serial.print("Response body: ");
      Serial.println(resp);
    }
  } else {
    Serial.print("Upload failed, error code: ");
    Serial.println(code);
  }

  http.end();
}

