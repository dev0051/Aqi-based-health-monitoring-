#!/usr/bin/env python3
"""
HealthSense Dashboard Server
Receives hardware data from friend and serves the dashboard
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import json
import os
from datetime import datetime
from threading import Lock

app = Flask(__name__)
CORS(app)  # Enable CORS for cross-origin requests

# Store the latest telemetry data
latest_data = {
    "aqi": None,
    "spo2": None,
    "heart_rate": None,
    "body_temp_c": None,
    "timestamp": None
}

# Thread-safe lock for data updates
data_lock = Lock()

# Get the directory where this script is located
BASE_DIR = os.path.dirname(os.path.abspath(__file__))


@app.route('/')
def index():
    """Serve the dashboard HTML file"""
    return send_from_directory(BASE_DIR, 'aqi.html')


@app.route('/api/telemetry', methods=['GET'])
def get_telemetry():
    """
    GET endpoint: Dashboard polls this to get latest hardware data
    """
    with data_lock:
        if latest_data["timestamp"] is None:
            return jsonify({"error": "No data received yet"}), 503
        
        return jsonify(latest_data)


@app.route('/api/telemetry', methods=['POST'])
def receive_telemetry():
    """
    POST endpoint: Friend sends hardware data here
    Expected JSON format:
    {
        "aqi": 82,
        "spo2": 97,
        "heart_rate": 78,
        "body_temp_c": 36.8,
        "timestamp": "2025-11-19T10:21:00Z"  (optional, auto-generated if missing)
    }
    """
    try:
        data = request.get_json()
        
        if not data:
            return jsonify({"error": "No JSON data received"}), 400
        
        # Update latest data with thread safety
        with data_lock:
            latest_data["aqi"] = data.get("aqi")
            latest_data["spo2"] = data.get("spo2")
            latest_data["heart_rate"] = data.get("heart_rate")
            latest_data["body_temp_c"] = data.get("body_temp_c")
            
            # Use provided timestamp or generate one
            if "timestamp" in data:
                latest_data["timestamp"] = data["timestamp"]
            else:
                latest_data["timestamp"] = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
        
        print(f"✓ Data received: AQI={latest_data['aqi']}, SPO2={latest_data['spo2']}, "
              f"HR={latest_data['heart_rate']}, Temp={latest_data['body_temp_c']}°C")
        
        return jsonify({
            "status": "success",
            "message": "Data received and stored",
            "received": latest_data
        }), 200
        
    except Exception as e:
        print(f"✗ Error receiving data: {e}")
        return jsonify({"error": str(e)}), 500


@app.route('/api/status', methods=['GET'])
def status():
    """Check server status and latest data info"""
    with data_lock:
        return jsonify({
            "status": "running",
            "has_data": latest_data["timestamp"] is not None,
            "latest_timestamp": latest_data["timestamp"]
        })


@app.route('/api/telemetry/latest', methods=['GET'])
def get_telemetry_latest():
    """
    Compact GET endpoint: returns the latest telemetry in a small JSON
    Useful for constrained clients (ESP32) that only need the latest values.
    """
    with data_lock:
        if latest_data["timestamp"] is None:
            return jsonify({"error": "No data received yet"}), 503

        # Return a compact payload tailored for lightweight clients
        return jsonify({
            "aqi": latest_data.get("aqi"),
            "spo2": latest_data.get("spo2"),
            "heart_rate": latest_data.get("heart_rate"),
            "body_temp_c": latest_data.get("body_temp_c"),
            "timestamp": latest_data.get("timestamp")
        })


if __name__ == '__main__':
    print("=" * 60)
    print("HealthSense Dashboard Server")
    print("=" * 60)
    print("\n📊 Dashboard URL: http://localhost:5000/")
    print("📥 Data endpoint: http://localhost:5000/api/telemetry")
    print("\nYour friend can send POST requests to:")
    print("   http://YOUR_IP:5000/api/telemetry")
    print("\nTo find your IP address:")
    print("   - macOS/Linux: ifconfig | grep 'inet '")
    print("   - Windows: ipconfig")
    print("\n" + "=" * 60)
    print("Server starting...\n")
    
    # Run on all interfaces (0.0.0.0) so friend can connect
    # For localhost only, use: app.run(host='127.0.0.1', port=5000, debug=True)
    app.run(host='0.0.0.0', port=5000, debug=True)

