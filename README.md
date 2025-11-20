# HealthSense Dashboard

Real-time dashboard for monitoring Air Quality Index (AQI), SPO2, Heart Rate, and Body Temperature from ESP32 hardware.

## 🚀 Quick Start

### Local Testing

1. Make sure your ESP32 is running and accessible
2. Update the endpoint in `aqi.html` (line 271) if needed:
   ```javascript
   const DATA_ENDPOINT = 'http://esp32.local/api/telemetry';
   ```
3. Start a local server:
   ```bash
   python3 -m http.server 8000
   ```
4. Open `http://localhost:8000/aqi.html` in your browser

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

