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

Your ESP32 should send JSON data to `/api/telemetry` endpoint:

```json
{
  "aqi": 82,
  "spo2": 97,
  "heart_rate": 78,
  "body_temp_c": 36.8,
  "timestamp": "2025-11-19T10:21:00Z"
}
```

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

