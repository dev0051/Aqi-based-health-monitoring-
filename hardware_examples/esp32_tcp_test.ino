/*
 * Minimal ESP32 Network Diagnostic Sketch
 * Tests WiFi, DNS, and TCP connection to local server
 * Helps diagnose -1 errors from HTTPClient
 */

#include <WiFi.h>

// EDIT THESE
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverHost = "10.116.204.42";
const int serverPort = 5001;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== ESP32 TCP Diagnostic Test ===\n");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ WiFi Connected!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("✗ WiFi Connection Failed!");
    return;
  }
  
  // Test DNS resolution
  Serial.print("\nDNS Resolution test for ");
  Serial.println(serverHost);
  IPAddress resolvedIP = WiFi.hostByName(serverHost);
  if (resolvedIP == INADDR_NONE) {
    Serial.println("✗ DNS failed! Cannot resolve hostname.");
    Serial.println("Trying direct IP instead...");
  } else {
    Serial.print("✓ DNS resolved to: ");
    Serial.println(resolvedIP);
  }
  
  // Test raw TCP connection
  Serial.print("\nTesting TCP connection to ");
  Serial.print(serverHost);
  Serial.print(":");
  Serial.println(serverPort);
  
  WiFiClient client;
  client.setTimeout(5000);
  
  unsigned long startTime = millis();
  bool connected = client.connect(serverHost, serverPort);
  unsigned long elapsedTime = millis() - startTime;
  
  if (connected) {
    Serial.print("✓ TCP Connection successful! (");
    Serial.print(elapsedTime);
    Serial.println(" ms)");
    
    // Try to send a simple HTTP GET
    Serial.println("\nSending HTTP GET request...");
    String request = "GET /api/status HTTP/1.1\r\n";
    request += "Host: ";
    request += serverHost;
    request += ":";
    request += serverPort;
    request += "\r\nConnection: close\r\n\r\n";
    
    client.print(request);
    
    // Read response
    unsigned long timeout = millis() + 5000;
    while (client.connected() && millis() < timeout) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n✓ HTTP request completed!");
  } else {
    Serial.print("✗ TCP Connection failed! (");
    Serial.print(elapsedTime);
    Serial.println(" ms)");
    Serial.println("Possible causes:");
    Serial.println("  - ESP32 and laptop on different WiFi networks");
    Serial.println("  - Firewall blocking port 5001");
    Serial.println("  - Server not running or listening");
    Serial.println("  - Wrong IP address");
  }
  
  Serial.println("\n=== Test Complete ===\n");
}

void loop() {
  delay(1000);
}
