/*
 * ESP32 Example Code - Send Hardware Data to Dashboard Server
 * 
 * This code reads sensors and sends data to your Python server
 * Replace YOUR_SERVER_IP with your actual IP address (e.g., "192.168.0.111")
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server configuration
const char* serverURL = "http://YOUR_SERVER_IP:5000/api/telemetry";  // Replace with your IP

// Sensor pins (adjust based on your hardware)
#define AQI_SENSOR_PIN A0
#define SPO2_SENSOR_PIN A1
#define HEART_RATE_PIN A2
#define TEMP_SENSOR_PIN A3

// Update interval (milliseconds)
const unsigned long UPDATE_INTERVAL = 3000;  // Send data every 3 seconds
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize sensor pins
  pinMode(AQI_SENSOR_PIN, INPUT);
  pinMode(SPO2_SENSOR_PIN, INPUT);
  pinMode(HEART_RATE_PIN, INPUT);
  pinMode(TEMP_SENSOR_PIN, INPUT);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Sending data to: ");
  Serial.println(serverURL);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
      lastUpdate = currentMillis;
      
      // Read sensor values (adjust these functions based on your sensors)
      int aqi = readAQI();
      int spo2 = readSPO2();
      int heartRate = readHeartRate();
      float bodyTemp = readBodyTemp();
      
      // Send data to server
      sendDataToServer(aqi, spo2, heartRate, bodyTemp);
    }
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
  
  delay(100);
}

// Function to read AQI sensor
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
  
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  StaticJsonDocument<200> doc;
  doc["aqi"] = aqi;
  doc["spo2"] = spo2;
  doc["heart_rate"] = heartRate;
  doc["body_temp_c"] = bodyTemp;
  // Timestamp will be added by server if not provided
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  Serial.print("Sending data: ");
  Serial.println(jsonPayload);
  
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    
    String response = http.getString();
    Serial.println("Server response: " + response);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

