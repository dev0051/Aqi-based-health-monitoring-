# ESP32 Hardware Integration

This folder contains ESP32 example code to send hardware sensor data to your dashboard server.

## 📋 ESP32 Example (`esp32_example.ino`)

For ESP32 microcontrollers with built-in WiFi capability.

**Setup:**
1. Install Arduino IDE
2. Install ESP32 board support: https://github.com/espressif/arduino-esp32
3. Install libraries:
   - WiFi (built-in)
   - HTTPClient (built-in)
   - ArduinoJson (via Library Manager: Tools → Manage Libraries → Search "ArduinoJson")

**Configuration:**
- Replace `YOUR_WIFI_SSID` with your WiFi network name
- Replace `YOUR_WIFI_PASSWORD` with your WiFi password
- Replace `YOUR_SERVER_IP` with your actual server IP (e.g., "192.168.0.111")
- Adjust sensor pins based on your hardware connections
- Modify sensor reading functions for your specific sensors

## 🔌 Common Sensors

### Air Quality (AQI)
- **MQ135** - Analog gas sensor
- **PMS5003** - Particulate matter sensor (UART)
- **SGP30** - I2C gas sensor

### SPO2 (Blood Oxygen)
- **MAX30102** - I2C pulse oximeter
- **MAX30100** - I2C pulse oximeter

### Heart Rate
- **MAX30102** - Also provides heart rate
- **Pulse sensor** - Analog sensor

### Body Temperature
- **DS18B20** - 1-Wire temperature sensor
- **MLX90614** - I2C infrared temperature sensor
- **LM35** - Analog temperature sensor

## 📤 Data Format

All examples send data in this JSON format:

```json
{
  "aqi": 82,
  "spo2": 97,
  "heart_rate": 78,
  "body_temp_c": 36.8,
  "timestamp": "2025-11-19T10:21:00Z"  // Optional
}
```

## 🔧 Customization

### For Your Specific Sensors:

1. **Identify your sensor type:**
   - Analog (0-5V or 0-3.3V)
   - I2C (uses SDA/SCL pins)
   - SPI (uses MOSI/MISO/SCK pins)
   - UART/Serial (uses TX/RX pins)
   - 1-Wire (uses single data pin)

2. **Find sensor library:**
   - Search Arduino Library Manager (Tools → Manage Libraries)
   - Check sensor manufacturer's GitHub
   - Look for ESP32-compatible libraries

3. **Implement reading function:**
   - Read raw sensor value
   - Convert to actual units (AQI, %, BPM, °C)
   - Return the value

4. **Test connection:**
   - Use serial monitor to verify sensor readings
   - Test HTTP POST with curl or Postman
   - Check server logs for received data

## 🧪 Testing

### Test with cURL:
```bash
curl -X POST http://YOUR_SERVER_IP:5000/api/telemetry \
  -H "Content-Type: application/json" \
  -d '{"aqi": 82, "spo2": 97, "heart_rate": 78, "body_temp_c": 36.8}'
```

### Test with Python:
```python
import requests
data = {"aqi": 82, "spo2": 97, "heart_rate": 78, "body_temp_c": 36.8}
requests.post("http://YOUR_SERVER_IP:5000/api/telemetry", json=data)
```

## 📝 Example Sensor Implementations

### MQ135 Air Quality Sensor (Analog)
```cpp
int readAQI() {
  int rawValue = analogRead(AQI_SENSOR_PIN);
  // Convert to AQI (simplified - adjust based on calibration)
  float voltage = (rawValue / 4095.0) * 3.3;
  int aqi = map(voltage * 100, 0, 330, 0, 500);
  return constrain(aqi, 0, 500);
}
```

### MAX30102 SPO2 & Heart Rate (I2C)
```cpp
#include "MAX30102.h"

MAX30102 sensor;

int readSPO2() {
  return sensor.getSpO2();
}

int readHeartRate() {
  return sensor.getHeartRate();
}
```

### DS18B20 Temperature Sensor (1-Wire)
```cpp
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

float readBodyTemp() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
```

## 🚀 Quick Start

1. Install Arduino IDE and ESP32 board support
2. Install ArduinoJson library
3. Open `esp32_example.ino` in Arduino IDE
4. Configure WiFi credentials and server IP
5. Connect your sensors to ESP32 GPIO pins
6. Implement sensor reading functions for your sensors
7. Upload code to ESP32
8. Open Serial Monitor (115200 baud) to verify connection
9. Check dashboard at `http://localhost:5000`

## ❓ Troubleshooting

**Hardware can't connect to server:**
- Verify both devices on same network
- Check server IP is correct
- Ensure firewall allows port 5000
- Test with ping/curl from hardware

**Sensor readings are wrong:**
- Check sensor wiring
- Verify power supply
- Calibrate sensor values
- Check sensor datasheet

**Data not appearing on dashboard:**
- Check server is running
- Verify JSON format is correct
- Check server console for errors
- Test with curl first

