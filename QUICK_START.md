# Quick Start Guide

## ✅ Setup Complete!

### Your Server Information:
- **Your IP Address**: `192.168.0.111`
- **Dashboard URL**: `http://localhost:5000`
- **Data Endpoint**: `http://192.168.0.111:5000/api/telemetry`

---

## 📋 Step-by-Step Instructions

### Step 1: ✅ Python Checked
- Python 3.9.6 is installed

### Step 2: ✅ Dependencies Installed
- Flask and flask-cors are installed

### Step 3: ✅ IP Address Found
- Your IP: **192.168.0.111**

### Step 4: Start the Server
Run this command:
```bash
cd "/Users/devkumawat/mini project"
python3 server.py
```

### Step 5: Open Dashboard
Once server is running, open your browser:
```
http://localhost:5000
```

### Step 6: Share with Your Friend
Send your friend:
1. **Your IP**: `192.168.0.111`
2. **The file**: `sender_example.py`

Your friend should:
1. Edit `sender_example.py` and replace `YOUR_SERVER_IP` with `192.168.0.111`
2. Install requests: `pip install requests`
3. Run: `python sender_example.py`

---

## 📤 How Your Friend Can Send Data

### Option A: Using Python (Recommended)
```python
import requests

data = {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8
}

response = requests.post("http://192.168.0.111:5000/api/telemetry", json=data)
print(response.json())
```

### Option B: Using cURL
```bash
curl -X POST http://192.168.0.111:5000/api/telemetry \
  -H "Content-Type: application/json" \
  -d '{"aqi": 82, "spo2": 97, "heart_rate": 78, "body_temp_c": 36.8}'
```

---

## 🔍 Troubleshooting

**Server won't start?**
- Make sure port 5000 is not in use
- Check firewall settings

**Friend can't connect?**
- Both devices must be on the same WiFi network
- Check firewall allows port 5000
- Verify IP address is correct

**Dashboard shows "Offline"?**
- Make sure server is running
- Check browser console (F12) for errors

