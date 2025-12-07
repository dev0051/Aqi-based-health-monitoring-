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
WIFI_DATA_FILE = os.path.join(BASE_DIR, 'wifi_data.json')


def append_to_wifi_file(payload: dict) -> None:
    """Append a JSON object as a single line to the wifi_data.json file."""
    try:
        with open(WIFI_DATA_FILE, 'a', encoding='utf-8') as f:
            f.write(json.dumps(payload, ensure_ascii=False) + "\n")
    except Exception as e:
        print(f"✗ Failed to append to wifi_data.json: {e}")


def read_last_entries(n: int = 1) -> list:
    """Read the last n JSON objects from wifi_data.json (one JSON object per line).

    Returns a list of parsed JSON objects (newest first).
    """
    if not os.path.exists(WIFI_DATA_FILE):
        return []

    try:
        with open(WIFI_DATA_FILE, 'r', encoding='utf-8') as f:
            lines = [ln.strip() for ln in f if ln.strip()]

        if not lines:
            return []

        entries = []
        for ln in lines[-n:][::-1]:
            try:
                entries.append(json.loads(ln))
            except Exception:
                # Skip malformed lines
                continue

        return entries
    except Exception as e:
        print(f"✗ Error reading wifi_data.json: {e}")
        return []


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
        # Prefer the in-memory latest_data if available
        if latest_data["timestamp"] is not None:
            return jsonify(latest_data)

    # Fallback: try reading the last entry from wifi_data.json
    entries = read_last_entries(1)
    if entries:
        return jsonify(entries[0])

    return jsonify({"error": "No data received yet"}), 503


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
        
        # Ensure timestamp
        if "timestamp" not in data or not data.get("timestamp"):
            data["timestamp"] = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")

        # Update latest data with thread safety
        with data_lock:
            latest_data["aqi"] = data.get("aqi")
            latest_data["spo2"] = data.get("spo2")
            latest_data["heart_rate"] = data.get("heart_rate")
            latest_data["body_temp_c"] = data.get("body_temp_c")
            latest_data["timestamp"] = data.get("timestamp")

        # Persist to wifi_data.json for dashboard history / crash recovery
        append_to_wifi_file(latest_data)
        
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



@app.route('/data', methods=['POST'])
def data_compat():
    """Compatibility endpoint: accept posts to `/data` (from `app.py` / simple senders).

    This mirrors the behaviour of `/api/telemetry` but keeps a simple text response
    for devices that expect a plain `OK` reply.
    """
    try:
        # Try to get JSON body; be permissive (some clients send form-encoded)
        data = request.get_json(silent=True)

        if not data:
            # Try form or query parameters as fallback
            data = request.form.to_dict() or request.args.to_dict() or {}

        # Some clients may wrap payload under a `data` key
        payload = data.get('data') if isinstance(data, dict) and 'data' in data else data

        if not payload:
            return "No JSON data received", 400

        # Ensure timestamp
        if "timestamp" not in payload or not payload.get("timestamp"):
            payload["timestamp"] = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")

        # Update latest_data with thread safety (only known keys)
        with data_lock:
            latest_data["aqi"] = payload.get("aqi", latest_data.get("aqi"))
            latest_data["spo2"] = payload.get("spo2", latest_data.get("spo2"))
            latest_data["heart_rate"] = payload.get("heart_rate", latest_data.get("heart_rate"))
            latest_data["body_temp_c"] = payload.get("body_temp_c", latest_data.get("body_temp_c"))
            latest_data["timestamp"] = payload.get("timestamp")

        # Persist to wifi_data.json
        append_to_wifi_file(latest_data)

        print(f"✓ /data received: AQI={latest_data['aqi']}, SPO2={latest_data['spo2']}, "
              f"HR={latest_data['heart_rate']}, Temp={latest_data['body_temp_c']}°C")

        # Return plain OK for very simple clients
        return "OK", 200

    except Exception as e:
        print(f"✗ Error on /data: {e}")
        return str(e), 500


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


@app.route('/api/history', methods=['GET'])
def get_history():
    """Return recent telemetry entries from wifi_data.json.

    Query parameter `limit` controls how many recent entries to return (default 50).
    """
    limit = request.args.get('limit', default=50, type=int)
    entries = read_last_entries(limit)
    # Return newest-first for easy consumption by frontend
    return jsonify({
        "count": len(entries),
        "entries": entries
    })


if __name__ == '__main__':
    print("=" * 60)
    print("HealthSense Dashboard Server")
    print("=" * 60)
    print("\n📊 Dashboard URL: https://aqi2.vercel.app/")
    print("📥 Data endpoint: https://aqi2.vercel.app/api/telemetry")
    print("\nTo find your IP address:")
    print("   - macOS/Linux: ifconfig | grep 'inet '")
    print("   - Windows: ipconfig")
    print("\n" + "=" * 60)
    print("Server starting...\n")

    # Allow configuring host/port/debug via CLI args or environment variables
    import argparse

    parser = argparse.ArgumentParser(description='Run HealthSense Dashboard Server')
    parser.add_argument('--host', default=os.getenv('HOST', '0.0.0.0'),
                        help='Host/IP to bind the server (env: HOST)')
    parser.add_argument('--port', type=int, default=int(os.getenv('PORT', '5000')),
                        help='Port to bind the server (env: PORT)')
    parser.add_argument('--debug', action='store_true',
                        help='Enable Flask debug mode (or set env DEBUG=1)')

    args = parser.parse_args()

    host = args.host
    port = args.port
    debug_flag = args.debug or os.getenv('DEBUG', '').lower() in ('1', 'true', 'yes')

    print(f"Binding to {host}:{port} (debug={debug_flag})")
    print("Tip: if port 5000 is in use, run with `--port 5001` or set `PORT=5001`")

    app.run(host=host, port=port, debug=debug_flag)

