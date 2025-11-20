# HealthSense Dashboard - Deployment Guide

This guide will help you deploy your HealthSense dashboard so it can be accessed from any mobile device or browser with a custom URL.

## 📱 Quick Deploy Options

### Option 1: Netlify (Recommended - Easiest)

**Steps:**
1. Go to [netlify.com](https://netlify.com) and sign up (free)
2. Drag and drop your `mini project` folder onto Netlify's dashboard
3. Your site will be live with a URL like: `https://random-name-123456.netlify.app`
4. To get a custom domain:
   - Click "Domain settings" → "Add custom domain"
   - Enter your domain (e.g., `healthsense.yourname.com`)
   - Follow DNS setup instructions

**Important:** Update the ESP32 endpoint in `aqi.html` line 271 to your ESP32's public URL (see ESP32 Setup below)

---

### Option 2: Vercel (Fast & Free)

**Steps:**
1. Install Vercel CLI: `npm i -g vercel`
2. Open terminal in your project folder
3. Run: `vercel`
4. Follow the prompts
5. Your site will be live immediately!

**Or use web interface:**
1. Go to [vercel.com](https://vercel.com) and sign up
2. Click "Import Project"
3. Upload your folder or connect to GitHub
4. Deploy!

---

### Option 3: GitHub Pages (Free)

**Steps:**
1. Create a GitHub account if you don't have one
2. Create a new repository (e.g., `healthsense-dashboard`)
3. Upload your files to the repository
4. Go to Settings → Pages
5. Select branch: `main` and folder: `/root`
6. Your site will be at: `https://yourusername.github.io/healthsense-dashboard/`

---

### Option 4: Cloudflare Pages (Free)

**Steps:**
1. Go to [pages.cloudflare.com](https://pages.cloudflare.com)
2. Sign up for free
3. Click "Create a project" → "Upload assets"
4. Upload your `mini project` folder
5. Deploy!

---

## 🔧 ESP32 Setup for Public Access

Your ESP32 needs to be accessible from the internet. Here are options:

### Option A: Using ngrok (Easiest for Testing)

1. Install ngrok: `brew install ngrok` (Mac) or download from [ngrok.com](https://ngrok.com)
2. Sign up for free ngrok account
3. Get your authtoken from ngrok dashboard
4. Run: `ngrok authtoken YOUR_TOKEN`
5. Start tunnel: `ngrok http 80` (or your ESP32 port)
6. Copy the HTTPS URL (e.g., `https://abc123.ngrok.io`)
7. Update `aqi.html` line 271: 
   ```javascript
   const DATA_ENDPOINT = 'https://abc123.ngrok.io/api/telemetry';
   ```
8. Redeploy your dashboard

**Note:** Free ngrok URLs change on restart. For permanent URL, upgrade or use Option B.

---

### Option B: Cloudflare Tunnel (Free & Permanent)

1. Install cloudflared: `brew install cloudflared` or download from [cloudflare.com](https://developers.cloudflare.com/cloudflare-one/connections/connect-apps/)
2. Authenticate: `cloudflared tunnel login`
3. Create tunnel: `cloudflared tunnel create healthsense`
4. Configure tunnel (create config file)
5. Run: `cloudflared tunnel run healthsense`
6. You'll get a permanent URL like: `https://healthsense-xyz.trycloudflare.com`
7. Update `aqi.html` with this URL
8. Redeploy

---

### Option C: Port Forwarding (If you have router access)

1. Get your ESP32's local IP (e.g., `192.168.1.100`)
2. In your router settings, forward port 80 (or your ESP32 port) to that IP
3. Find your public IP: visit [whatismyip.com](https://whatismyip.com)
4. Update `aqi.html`: 
   ```javascript
   const DATA_ENDPOINT = 'http://YOUR_PUBLIC_IP/api/telemetry';
   ```
5. Redeploy

**Note:** Your public IP may change unless you have static IP. Use dynamic DNS service if needed.

---

## 📝 Configuration Steps

### Step 1: Update ESP32 Endpoint

Before deploying, update the endpoint in `aqi.html`:

```javascript
// Line 271 - Change this to your ESP32's public URL
const DATA_ENDPOINT = 'https://your-esp32-url.com/api/telemetry';
```

### Step 2: Test Locally First

1. Start a local server:
   ```bash
   cd "/Users/devkumawat/mini project"
   python3 -m http.server 8000
   ```
2. Open `http://localhost:8000/aqi.html` in browser
3. Verify it connects to your ESP32

### Step 3: Deploy

Choose one of the deployment options above and deploy!

---

## 🎯 Custom Domain Setup

### For Netlify:
1. Click "Domain settings" in your Netlify dashboard
2. Click "Add custom domain"
3. Enter your domain (you need to own it)
4. Follow DNS configuration instructions
5. Netlify will provide DNS records to add to your domain registrar

### For Vercel:
1. Go to Project Settings → Domains
2. Add your domain
3. Follow DNS setup instructions

### For Cloudflare Pages:
1. Go to your project → Custom domains
2. Add domain
3. Follow setup instructions

---

## 📱 Mobile Browser Testing

After deployment:
1. Open your deployed URL on mobile
2. Test on:
   - Safari (iOS)
   - Chrome (Android)
   - Firefox Mobile
   - Samsung Internet

---

## 🔒 Security Notes

- If using HTTP (not HTTPS), modern browsers may block requests
- Use HTTPS for production (all deployment platforms provide this)
- Consider adding authentication if needed
- CORS headers may need adjustment in ESP32 firmware

---

## 🐛 Troubleshooting

**Issue: "Failed to fetch" error**
- Check ESP32 endpoint is correct
- Verify ESP32 is accessible from internet
- Check CORS headers on ESP32
- Try accessing ESP32 URL directly in browser

**Issue: Data not updating**
- Check browser console for errors
- Verify ESP32 is sending correct JSON format
- Check network tab in browser dev tools

**Issue: Site not loading on mobile**
- Clear browser cache
- Try different browser
- Check if URL is correct
- Verify deployment was successful

---

## 📞 Need Help?

- Check ESP32 firmware is sending data to `/api/telemetry` endpoint
- Verify JSON format matches expected structure:
  ```json
  {
    "aqi": 82,
    "spo2": 97,
    "heart_rate": 78,
    "body_temp_c": 36.8,
    "timestamp": "2025-11-19T10:21:00Z"
  }
  ```

