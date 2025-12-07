# 📊 Complete Guide: Send Hardware Data to Live Dashboard

## Overview
Your HealthSense dashboard is now live on Vercel. This guide explains how to get real-time data from hardware (ESP32, sensors, etc.) and display it live on your dashboard.

---

## 🔗 The Data Flow

```
HARDWARE (ESP32/Sensors)
         ↓
    sends data (JSON)
         ↓
    HTTP POST request
         ↓
VERCEL SERVER (/api/telemetry endpoint)
         ↓
   Stores latest data
         ↓
DASHBOARD (aqi.html)
         ↓
  Polls every 2 seconds
         ↓
   Displays live data
```

---

## ⚠️ IMPORTANT: Update Your Vercel URL

**Replace `YOUR_VERCEL_URL` with your actual Vercel deployment URL**
- Example: `https://aqi-health-dashboard.vercel.app`
- Find it in your Vercel dashboard

---

## 📱 Step 1: Prepare Your Hardware

### Option A: Using ESP32 (Recommended for Real Hardware)

**File:** `hardware_examples/esp32_example.ino`

**Step 1a: Edit the Configuration**
```cpp
// Line 12: Enter your WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Line 16: Enter your Vercel URL
const char* serverURL = "https://YOUR_VERCEL_URL/api/telemetry";
```

**Step 1b: Connect Sensors to ESP32**
```
AQI Sensor      → GPIO 36 (A0)
SPO2 Sensor     → GPIO 39 (A1)
Heart Rate      → GPIO 34 (A2)
Temperature     → GPIO 35 (A3)
```

**Step 1c: Upload to ESP32**
1. Open Arduino IDE
2. Select: Tools → Board → ESP32 Dev Module
3. Copy the code from `esp32_example.ino`
4. Click Upload

**Step 1d: Monitor**
- Open Serial Monitor (Tools → Serial Monitor, 115200 baud)
- You should see:
  ```
  Connected! IP address: 192.168.x.x
  Data sending to: https://YOUR_VERCEL_URL/api/telemetry
  ```

---

### Option B: Using Python Script (Testing/Easy Setup)

**File:** `sender_example.py`

**Step 2a: Edit Configuration**
```python
# Line 11: Update to your Vercel URL
SERVER_URL = "https://YOUR_VERCEL_URL/api/telemetry"
```

**Step 2b: Install Dependencies**
```bash
pip install requests
```

**Step 2c: Modify the Script**
At the bottom of `sender_example.py`, replace the example usage with:

```python
if __name__ == "__main__":
    print("Hardware Data Sender")
    print("=" * 60)
    print(f"Sending to: {SERVER_URL}")
    print("Press Ctrl+C to stop\n")
    
    # Send data every 3 seconds
    import random
    while True:
        # Example: Simulate sensor readings
        aqi = random.randint(30, 150)
        spo2 = random.randint(94, 100)
        heart_rate = random.randint(60, 100)
        body_temp_c = round(36.5 + random.uniform(-0.5, 0.5), 1)
        
        send_hardware_data(aqi, spo2, heart_rate, body_temp_c)
        time.sleep(3)  # Send every 3 seconds
```

**Step 2d: Run It**
```bash
python3 sender_example.py
```

You should see:
```
✓ Data sent successfully: {
  "aqi": 75,
  "spo2": 98,
  "heart_rate": 72,
  "body_temp_c": 36.8
}
```

---

### Option C: Using Demo Data (Quick Testing)

```bash
python3 demo_data_sender.py --endpoint https://YOUR_VERCEL_URL/api/telemetry
```

---

## 🌐 Step 2: Verify Your Vercel Deployment

**Test if data endpoint is working:**

```bash
# Using cURL (Mac/Linux)
curl -X POST https://YOUR_VERCEL_URL/api/telemetry \
  -H "Content-Type: application/json" \
  -d '{
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8
  }'
```

Expected response:
```json
{
  "status": "success",
  "message": "Data received and stored",
  "received": {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8,
    "timestamp": "2025-12-08T10:30:45Z"
  }
}
```

---

## 📊 Step 3: Check Your Live Dashboard

1. **Open in Browser:**
   ```
   https://YOUR_VERCEL_URL
   ```

2. **Verify Status:**
   - Look for green "LIVE" badge in top-right
   - Check if numbers update every 2-3 seconds

3. **Troubleshooting:**
   - Open Browser Console (F12 → Console)
   - If you see errors, check:
     - Vercel URL is correct
     - Server is running
     - Data endpoint is accessible

---

## 📝 Data Format

Your hardware must send JSON in this exact format:

```json
{
  "aqi": 82,
  "spo2": 97,
  "heart_rate": 78,
  "body_temp_c": 36.8
}
```

**Field Descriptions:**
- `aqi`: Air Quality Index (0-400, lower is better)
- `spo2`: Blood Oxygen Saturation (85-100 %)
- `heart_rate`: Beats Per Minute (40-200)
- `body_temp_c`: Body Temperature in Celsius (34-41°C)
- `timestamp`: Optional, auto-generated if omitted

---

## 🔄 Live Update Mechanism

Your dashboard uses **polling** to fetch fresh data:

```javascript
// In aqi.html (JavaScript section)
setInterval(() => {
  fetch('/api/telemetry')
    .then(res => res.json())
    .then(data => updateDashboard(data))
}, 2000)  // Fetches every 2 seconds
```

This means:
- ✅ Updates are **live** (every 2 seconds)
- ✅ Works from **anywhere** (cloud-hosted)
- ✅ No special setup needed

---

## 🚀 Production Checklist

- [ ] Vercel URL configured in hardware code
- [ ] WiFi credentials entered (for ESP32)
- [ ] Sensors connected to correct pins
- [ ] Hardware code uploaded to ESP32
- [ ] Test data sends successfully
- [ ] Dashboard shows "LIVE" status
- [ ] Numbers update every 2-3 seconds
- [ ] Check browser console for errors

---

## 📞 Common Issues & Solutions

### Problem: "No data received yet"
**Solution:**
- Make sure hardware is sending data
- Check Vercel URL is correct
- Verify network connectivity
- Check firewall isn't blocking requests

### Problem: Dashboard shows "OFFLINE"
**Solution:**
- Verify `/api/telemetry` endpoint is working (test with cURL)
- Check browser console for CORS errors
- Make sure Vercel deployment is active

### Problem: Data stops updating
**Solution:**
- Hardware might have disconnected WiFi
- Check ESP32 serial monitor for errors
- Restart ESP32 or Python script
- Check internet connection

### Problem: CORS Error in Browser
**Solution:**
- Already handled in `vercel.json` with CORS headers
- If still issues, clear browser cache (Ctrl+Shift+Delete)

---

## 💾 Next Steps

1. **Real Sensors:** Replace mock sensor readings with actual hardware readings
2. **Database:** Add historical data storage (MongoDB/Firebase)
3. **Alerts:** Set up email/SMS alerts for abnormal readings
4. **Mobile App:** Create mobile app that reads same dashboard data

---

## 📚 File Reference

| File | Purpose |
|------|---------|
| `aqi.html` | Live dashboard (displays data every 2 sec) |
| `server.py` | Backend that receives & stores data |
| `sender_example.py` | Python template for sending data |
| `demo_data_sender.py` | Demo data generator |
| `esp32_example.ino` | ESP32 firmware for sensors |
| `vercel.json` | Vercel configuration |

---

**Good luck! 🎉 Your live health monitoring dashboard is ready!**
