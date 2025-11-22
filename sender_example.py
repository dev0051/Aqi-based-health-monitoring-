#!/usr/bin/env python3
"""
Example script for your friend to send hardware data
Replace YOUR_SERVER_IP with your actual IP address
"""

import requests
import json
import time
from datetime import datetime

# Configuration
SERVER_URL = "http://YOUR_SERVER_IP:5000/api/telemetry"  # Replace with your IP

def send_hardware_data(aqi, spo2, heart_rate, body_temp_c):
    """
    Send hardware data to the dashboard server
    
    Args:
        aqi: Air Quality Index
        spo2: Blood oxygen saturation percentage
        heart_rate: Heart rate in BPM
        body_temp_c: Body temperature in Celsius
    """
    data = {
        "aqi": aqi,
        "spo2": spo2,
        "heart_rate": heart_rate,
        "body_temp_c": body_temp_c,
        "timestamp": datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
    }
    
    try:
        response = requests.post(SERVER_URL, json=data, timeout=5)
        response.raise_for_status()
        print(f"✓ Data sent successfully: {json.dumps(data, indent=2)}")
        return True
    except requests.exceptions.RequestException as e:
        print(f"✗ Error sending data: {e}")
        return False


# Example usage - simulate sending data every 3 seconds
if __name__ == "__main__":
    print("Hardware Data Sender")
    print("=" * 60)
    print(f"Sending to: {SERVER_URL}")
    print("Press Ctrl+C to stop\n")
    
    # Example: Send sample data
    sample_data = {
        "aqi": 82,
        "spo2": 97,
        "heart_rate": 78,
        "body_temp_c": 36.8
    }
    
    # Uncomment to send continuously:
    # while True:
    #     send_hardware_data(**sample_data)
    #     time.sleep(3)
    
    # Or send once:
    send_hardware_data(**sample_data)

