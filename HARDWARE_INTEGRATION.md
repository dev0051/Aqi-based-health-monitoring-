# Hardware Integration Guide

## 🔄 How It Works

```
┌─────────────┐         HTTP POST          ┌──────────────┐         HTTP GET          ┌─────────────┐
│   Hardware  │ ──────────────────────────> │ Python Server│ <─────────────────────── │  Dashboard  │
│  (ESP32/    │   JSON Data                 │  (server.py) │   Polls every 3 seconds  │  (aqi.html) │
│  Arduino/   │   {aqi, spo2, hr, temp}     │              │                          │             │
│  Raspberry) │                             │  Port 5000   │                          │  Browser    │
└─────────────┘                             └──────────────┘                          └─────────────┘
```

## 📋 Step-by-Step Process

### Step 1: Hardware Reads Sensors
Your hardware (ESP32, Arduino, Raspberry Pi) reads values from:
- **AQI Sensor** → Air Quality Index
- **SPO2 Sensor** → Blood Oxygen %
- **Heart Rate Sensor** → Beats per minute
- **Temperature Sensor** → Body temperature in °C

### Step 2: Hardware Sends Data via HTTP POST
Hardware sends JSON data to your server:
```json
POST http://YOUR_IP:5000/api/telemetry
Content-Type: application/json

{
  "aqi": 82,
  "spo2": 97,
  "heart_rate": 78,
  "body_temp_c": 36.8
}
```

### Step 3: Server Stores Data
Your Python server (`server.py`) receives and stores the latest data.

### Step 4: Dashboard Polls Server
The dashboard (`aqi.html`) automatically polls the server every 3 seconds:
```
GET http://localhost:5000/api/telemetry
```

### Step 5: Dashboard Updates
Dashboard displays the latest sensor values in real-time.

---

## 🔌 Hardware Setup Options

### Option 1: ESP32 (Recommended - Has WiFi Built-in)

**Advantages:**
- Built-in WiFi
- Powerful processor
- Many GPIO pins
- Low cost

**What You Need:**
- ESP32 development board
- Sensors (MQ135, MAX30102, DS18B20, etc.)
- Jumper wires
- Breadboard

**Code:** See `hardware_examples/esp32_example.ino`

**Steps:**
1. Install Arduino IDE
2. Add ESP32 board support
3. Install required libraries
4. Upload code to ESP32
5. Configure WiFi and server IP
6. Connect sensors

### Option 2: Arduino + Ethernet Shield

**Advantages:**
- Simple Arduino Uno/Nano
- Ethernet connectivity
- Many tutorials available

**What You Need:**
- Arduino Uno/Nano
- Ethernet Shield
- Sensors
- Ethernet cable

**Code:** See `hardware_examples/arduino_example.ino`

### Option 3: Raspberry Pi

**Advantages:**
- Full Linux OS
- Easy Python programming
- Many sensor libraries
- Can run server and hardware code on same device

**What You Need:**
- Raspberry Pi (any model)
- Sensors
- GPIO cables

**Code:** See `hardware_examples/raspberry_pi_example.py`

---

## 📡 Communication Methods

### Method 1: WiFi (ESP32, Raspberry Pi)
- Hardware connects to your WiFi network
- Sends HTTP POST requests directly
- **Best for:** ESP32, Raspberry Pi

### Method 2: Ethernet (Arduino + Shield)
- Hardware connects via Ethernet cable
- Sends HTTP POST requests
- **Best for:** Arduino with Ethernet Shield

### Method 3: Serial Bridge (Any Microcontroller)
- Hardware sends data via Serial/USB
- Computer/another device forwards to server
- **Best for:** Simple microcontrollers without network

---

## 🔧 Implementation Steps

### For ESP32:

1. **Install Arduino IDE**
   - Download from arduino.cc
   - Install ESP32 board support package

2. **Install Libraries**
   ```
   Tools → Manage Libraries → Search and install:
   - ArduinoJson
   - WiFi (built-in)
   - HTTPClient (built-in)
   ```

3. **Open Example Code**
   - Open `hardware_examples/esp32_example.ino`

4. **Configure Settings**
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* serverURL = "http://192.168.0.111:5000/api/telemetry";
   ```

5. **Connect Sensors**
   - Wire sensors to ESP32 GPIO pins
   - Update pin definitions in code

6. **Upload and Test**
   - Upload code to ESP32
   - Open Serial Monitor (115200 baud)
   - Check for "Connected!" message
   - Verify data is being sent

### For Raspberry Pi:

1. **Install Python Libraries**
   ```bash
   pip install requests
   pip install RPi.GPIO  # For GPIO sensors
   pip install smbus     # For I2C sensors
   ```

2. **Open Example Code**
   - Open `hardware_examples/raspberry_pi_example.py`

3. **Configure Server URL**
   ```python
   SERVER_URL = "http://192.168.0.111:5000/api/telemetry"
   ```

4. **Implement Sensor Functions**
   - Replace placeholder functions with actual sensor code
   - Install sensor-specific libraries

5. **Run Script**
   ```bash
   python3 raspberry_pi_example.py
   ```

---

## 🧪 Testing Your Hardware

### Test 1: Check Sensor Readings
Use Serial Monitor (Arduino) or print statements (Python) to verify sensors are reading correctly.

### Test 2: Test HTTP Connection
Send a test POST request from your hardware:
```cpp
// ESP32/Arduino
http.begin("http://192.168.0.111:5000/api/telemetry");
http.addHeader("Content-Type", "application/json");
int code = http.POST("{\"aqi\":82,\"spo2\":97,\"heart_rate\":78,\"body_temp_c\":36.8}");
```

### Test 3: Verify Server Receives Data
Check your server console - you should see:
```
✓ Data received: AQI=82, SPO2=97, HR=78, Temp=36.8°C
```

### Test 4: Check Dashboard
Open `http://localhost:5000` and verify data appears.

---

## 📊 Data Flow Example

### Hardware Side (ESP32):
```cpp
// Read sensors
int aqi = readAQI();           // e.g., 82
int spo2 = readSPO2();         // e.g., 97
int hr = readHeartRate();      // e.g., 78
float temp = readBodyTemp();   // e.g., 36.8

// Create JSON
{"aqi":82, "spo2":97, "heart_rate":78, "body_temp_c":36.8}

// Send via HTTP POST
POST http://192.168.0.111:5000/api/telemetry
```

### Server Side (Python):
```python
# Receives POST request
# Stores in latest_data dictionary
latest_data = {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8,
    "timestamp": "2025-11-19T10:21:00Z"
}
```

### Dashboard Side (JavaScript):
```javascript
// Polls every 3 seconds
fetch('http://localhost:5000/api/telemetry')
  .then(response => response.json())
  .then(data => {
    // Update display
    document.getElementById('aqi-value').textContent = data.aqi;
    document.getElementById('spo2-value').textContent = data.spo2;
    // ... etc
  });
```

---

## 🔍 Common Sensors & Libraries

### Air Quality Sensors:
- **MQ135** - `MQ135` library
- **PMS5003** - Serial/UART communication
- **SGP30** - `Adafruit_SGP30` library

### SPO2 Sensors:
- **MAX30102** - `SparkFun_MAX3010x` library
- **MAX30100** - `MAX30100lib` library

### Heart Rate:
- **MAX30102** - Same as SPO2 (provides both)
- **Pulse Sensor** - Analog reading

### Temperature:
- **DS18B20** - `OneWire` + `DallasTemperature` libraries
- **MLX90614** - `Adafruit_MLX90614` library
- **LM35** - Simple analog reading

---

## ❓ Troubleshooting

### Hardware can't connect to WiFi:
- Check SSID and password
- Verify WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check signal strength

### HTTP POST fails:
- Verify server IP is correct
- Check server is running
- Ensure both devices on same network
- Check firewall settings

### Sensor readings are 0 or wrong:
- Check wiring connections
- Verify power supply
- Check sensor is properly initialized
- Review sensor datasheet for correct reading method

### Dashboard shows "Offline":
- Server might not be running
- Check browser console (F12) for errors
- Verify endpoint URL is correct

---

## 📚 Next Steps

1. Choose your hardware platform
2. Select appropriate sensors
3. Use the example code as a starting point
4. Customize sensor reading functions
5. Test with your actual hardware
6. Deploy and monitor!

For detailed code examples, see the `hardware_examples/` folder.

