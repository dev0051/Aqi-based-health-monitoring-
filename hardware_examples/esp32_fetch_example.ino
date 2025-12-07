/*
 * ESP32 Example Code - Fetch Latest Telemetry from Dashboard Server
 *
 * This example shows how an ESP32 can poll the Python server's
 * GET endpoint to receive the latest telemetry values.
 * Replace YOUR_WIFI_SSID, YOUR_WIFI_PASSWORD and YOUR_SERVER_IP.
 *
 * Requires: ArduinoJson (https://arduinojson.org/)
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server configuration - replace with your server IP
const char* serverURL = "http://YOUR_SERVER_IP:5000/api/telemetry/latest"; // compact payload

// Poll interval (milliseconds)
const unsigned long POLL_INTERVAL = 3000;
unsigned long lastPoll = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Polling data from: ");
  Serial.println(serverURL);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastPoll >= POLL_INTERVAL) {
      lastPoll = now;
      fetchLatestTelemetry();
    }
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
  delay(10);
}

void fetchLatestTelemetry() {
  HTTPClient http;
  http.begin(serverURL);
  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.print("Received JSON: ");
      Serial.println(payload);

      // Parse JSON using ArduinoJson
      StaticJsonDocument<256> doc;
      DeserializationError err = deserializeJson(doc, payload);
      if (err) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(err.c_str());
      } else {
        // Extract values (guarded - fields might be null)
        if (doc.containsKey("aqi")) {
          int aqi = doc["aqi"].as<int>();
          Serial.print("AQI: "); Serial.println(aqi);
        }
        if (doc.containsKey("spo2")) {
          int spo2 = doc["spo2"].as<int>();
          Serial.print("SPO2: "); Serial.println(spo2);
        }
        if (doc.containsKey("heart_rate")) {
          int hr = doc["heart_rate"].as<int>();
          Serial.print("Heart Rate: "); Serial.println(hr);
        }
        if (doc.containsKey("body_temp_c")) {
          float t = doc["body_temp_c"].as<float>();
          Serial.print("Body Temp (C): "); Serial.println(t);
        }
        if (doc.containsKey("timestamp")) {
          const char* ts = doc["timestamp"];
          Serial.print("Timestamp: "); Serial.println(ts);
        }

        // Example action: If AQI > 150, toggle an LED or take action
        if (doc.containsKey("aqi")) {
          int aqi = doc["aqi"].as<int>();
          if (aqi > 150) {
            Serial.println("WARNING: AQI is high!");
            // e.g., digitalWrite(ALARM_PIN, HIGH);
          }
        }
      }

    } else {
      Serial.print("GET request failed, HTTP code: ");
      Serial.println(httpCode);
    }
  } else {
    Serial.print("Failed to connect or GET failed, error: ");
    Serial.println(httpCode);
  }

  http.end();
}
