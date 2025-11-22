#!/usr/bin/env python3
"""
Quick script to get your local IP address
Share this IP with your friend so they can send data to your server
"""

import socket

def get_local_ip():
    """Get the local IP address of this machine"""
    try:
        # Connect to a remote address (doesn't actually connect)
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "Unable to determine IP"

if __name__ == "__main__":
    ip = get_local_ip()
    print("=" * 60)
    print("Your Local IP Address")
    print("=" * 60)
    print(f"\n📍 IP Address: {ip}")
    print(f"\n📥 Your friend should send data to:")
    print(f"   http://{ip}:5000/api/telemetry")
    print(f"\n📊 You can access the dashboard at:")
    print(f"   http://localhost:5000")
    print("=" * 60)

