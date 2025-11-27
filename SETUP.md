# Setup Guide: Receiving Hardware Data via Python Server

This guide explains how to set up the Python server to receive hardware data from your friend and display it on your dashboard.

## Quick Start

### 1. Install Dependencies

```bash
pip install -r requirements.txt
```

Or install manually:
```bash
pip install Flask flask-cors
```

### 2. Start the Server

```bash
python server.py
```

The server will start on `http://localhost:5000`

### 3. Access the Dashboard

Open your browser and go to:
```
http://localhost:5000
```

The dashboard will automatically poll the server every 3 seconds for new data.

### Demo Mode (no hardware/server required)

If you only need to showcase the UI (e.g., on Vercel) without an ESP32 or Python server:

1. Deploy `aqi.html` anywhere (GitHub Pages, Vercel, Netlify, etc.)
2. Open the page with `?demo=1` appended to the URL, for example:<br>
   `https://your-dashboard.vercel.app/aqi.html?demo=1`
3. The dashboard will generate realistic random vitals/AQI data every 3 seconds.

You can also set `window.DEMO_MODE = true` before the script runs if you want to hard-code demo mode into a build.

## How Your Friend Can Send Data

### Option 1: Using Python Script (Recommended)

1. Share the `sender_example.py` file with your friend
2. Your friend should:
   - Install requests: `pip install requests`
   - Edit `sender_example.py` and replace `YOUR_SERVER_IP` with your actual IP address
   - Run: `python sender_example.py`

### Option 2: Using cURL (Command Line)

Your friend can send data using cURL:

```bash
curl -X POST http://YOUR_IP:5000/api/telemetry \
  -H "Content-Type: application/json" \
  -d '{
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8
  }'
```

### Option 3: Using HTTP Client (Postman, etc.)

- **Method**: POST
- **URL**: `http://YOUR_IP:5000/api/telemetry`
- **Headers**: `Content-Type: application/json`
- **Body** (JSON):
```json
{
  "aqi": 82,
  "spo2": 97,
  "heart_rate": 78,
  "body_temp_c": 36.8,
  "timestamp": "2025-11-19T10:21:00Z"
}
```

### Option 4: Built-in demo data sender

Need moving charts but no hardware handy? Use the included script:

```bash
python demo_data_sender.py
```

Flags:
- `--endpoint https://your-url/api/telemetry` – point to a remote server or tunnel
- `--interval 5` – send data every 5 seconds (default 3s)
- `--once` – send a single payload and exit

This is great for demos on Vercel/Netlify when only the static dashboard is deployed.

## Finding Your IP Address

### macOS/Linux:
```bash
ifconfig | grep "inet " | grep -v 127.0.0.1
```

Or:
```bash
ip addr show | grep "inet " | grep -v 127.0.0.1
```

### Windows:
```bash
ipconfig
```
Look for "IPv4 Address" under your active network adapter.

## Data Format

The server expects JSON data with these fields:

```json
{
  "aqi": 82,              // Air Quality Index (number)
  "spo2": 97,             // Blood oxygen % (number)
  "heart_rate": 78,       // Heart rate BPM (number)
  "body_temp_c": 36.8,    // Body temperature in Celsius (number)
  "timestamp": "2025-11-19T10:21:00Z"  // Optional, auto-generated if missing
}
```

## API Endpoints

- **GET `/`** - Serves the dashboard HTML
- **GET `/api/telemetry`** - Dashboard polls this for latest data
- **POST `/api/telemetry`** - Friend sends data here
- **GET `/api/status`** - Check server status

## Troubleshooting

### Friend can't connect to server

1. **Check firewall**: Make sure port 5000 is open
   - macOS: System Settings → Firewall
   - Linux: `sudo ufw allow 5000`
   - Windows: Windows Defender Firewall settings

2. **Check network**: Both devices must be on the same network (or use port forwarding/ngrok for remote access)

3. **Verify IP address**: Make sure you're using the correct IP address

### Dashboard shows "Offline"

- Check that the server is running
- Open browser console (F12) to see error messages
- Verify the endpoint URL in the dashboard matches your server

### Data not updating

- Check server console for incoming POST requests
- Verify JSON format matches expected structure
- Check browser console for fetch errors

## Running on Different Port

Edit `server.py` and change:
```python
app.run(host='0.0.0.0', port=5000, debug=True)
```

Change `5000` to your desired port.

## Security Note

This server is designed for local network use. For production or internet access:
- Add authentication
- Use HTTPS
- Implement rate limiting
- Add input validation

