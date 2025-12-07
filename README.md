# HealthSense Dashboard

Real-time dashboard for monitoring Air Quality Index (AQI), SPO2, Heart Rate, and Body Temperature from ESP32 hardware.

## 🚀 Quick Start

### Local Testing

**Option A – Live data via Python server**
1. Install dependencies: `pip install -r requirements.txt`
2. Start the Flask receiver: `python server.py` (serves `aqi.html` at `http://localhost:5000`)
3. Point your ESP32 (or any sender) to `http://<your-ip>:5000/api/telemetry`
4. Open `http://localhost:5000` to view live updates

**Option B – Demo mode (no hardware/server)**
1. Serve `aqi.html` from any static host (local file, Netlify, Vercel, etc.)
2. Append `?demo=1` to the URL (e.g., `http://localhost:8000/aqi.html?demo=1`)
3. The dashboard will generate realistic random vitals for presentations

**Option C – Fully offline build**
1. Copy both `aqi_offline.html` and `chart.umd.min.js` to the same folder/USB
2. Open `aqi_offline.html` directly from disk (double-click works)
3. Everything runs client-side with synthetic data, no servers or query params required
4. Use this version for USB sharing or environments without internet access

### Deploy to Production (Get Your Own URL)

**Easiest Method - Netlify:**
1. Go to [netlify.com](https://netlify.com) (free signup)
2. Drag and drop this entire folder onto Netlify
3. Done! You'll get a URL like `https://yoursite.netlify.app`
4. Add a custom domain in Netlify settings if you want

**See `DEPLOYMENT.md` for detailed deployment instructions and other options.**

## 📱 Features

- ✅ Real-time data updates every 3 seconds
- ✅ Responsive design - works on mobile, tablet, desktop
- ✅ Visual gauges and charts for all metrics
- ✅ Health status indicators (Normal/Warning/Danger)
- ✅ Live connection status
- ✅ Works on all modern browsers

## 🔌 ESP32 Integration

Two integration modes are supported:

- **ESP32 as sender (POST)**: the ESP32 posts its sensor readings to the server at `/api/telemetry`.

  Example payload (POST to `http://<your-ip>:5000/api/telemetry`):

  ```json
  {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8,
    "timestamp": "2025-11-19T10:21:00Z"
  }
  ```

- **ESP32 as client (GET / polling)**: constrained devices can poll the compact endpoint at
  `/api/telemetry/latest` to receive the most recent telemetry as a lightweight JSON object.

  Example (GET `http://<your-ip>:5000/api/telemetry/latest`) returns:

  ```json
  {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8,
    "timestamp": "2025-11-19T10:21:00Z"
  }
  ```

There is an example Arduino sketch that demonstrates both sending and fetching data in `hardware_examples/`:

- `hardware_examples/esp32_example.ino` — ESP32 that sends sensor values via POST (existing example).
- `hardware_examples/esp32_fetch_example.ino` — ESP32 example that polls `/api/telemetry/latest` and parses JSON.

## 📋 Requirements

- ESP32 hardware with REST API endpoint
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Internet connection (for deployment) or local network (for testing)

## 📖 Documentation

See `DEPLOYMENT.md` for:
- Detailed deployment options (Netlify, Vercel, GitHub Pages, etc.)
- ESP32 public access setup (ngrok, Cloudflare Tunnel)
- Custom domain configuration
- Troubleshooting guide

## 🎨 Customization

Edit `aqi.html` to customize:
- Colors and themes
- Update intervals
- Chart configurations
- Health thresholds

