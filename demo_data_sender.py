#!/usr/bin/env python3
"""
Demo Telemetry Sender
---------------------
Pushes random-but-realistic vitals/AQI data to the dashboard API so you can
showcase the project without any ESP32 hardware online or on Vercel.

Usage examples:
    python demo_data_sender.py
    python demo_data_sender.py --endpoint https://<vercel-app>.vercel.app/api/telemetry
    python demo_data_sender.py --interval 10
    python demo_data_sender.py --once
"""

from __future__ import annotations

import argparse
import random
import sys
import time
from datetime import datetime, timezone

import requests

DEFAULT_ENDPOINT = "http://localhost:5000/api/telemetry"


def generate_payload() -> dict[str, float]:
    """Generate bounded pseudo-random telemetry values."""
    clamp = lambda value, low, high: max(low, min(high, value))

    return {
        "aqi": round(clamp(random.gauss(70, 25), 0, 400)),
        "spo2": round(clamp(random.gauss(97, 1), 85, 100)),
        "heart_rate": round(clamp(random.gauss(78, 10), 45, 160)),
        "body_temp_c": round(clamp(random.gauss(36.7, 0.4), 34, 41), 1),
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    }


def send_payload(endpoint: str, payload: dict[str, float]) -> None:
    """Send payload to the API and print the response status."""
    try:
        response = requests.post(endpoint, json=payload, timeout=10)
        response.raise_for_status()
        print(f"✓ Sent to {endpoint} :: {payload}")
    except requests.RequestException as exc:
        print(f"✗ Failed to send payload: {exc}", file=sys.stderr)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Send demo telemetry data to the dashboard.")
    parser.add_argument(
        "--endpoint",
        default=DEFAULT_ENDPOINT,
        help=f"Telemetry endpoint (default: {DEFAULT_ENDPOINT})",
    )
    parser.add_argument(
        "--interval",
        type=float,
        default=3.0,
        help="Seconds between payloads (default: 3). Ignored when --once is used.",
    )
    parser.add_argument(
        "--once",
        action="store_true",
        help="Send only one payload and exit.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    print("=" * 60)
    print("Demo Telemetry Sender")
    print("=" * 60)
    print(f"Endpoint : {args.endpoint}")
    print(f"Mode     : {'single payload' if args.once else f'every {args.interval}s'}")
    print("=" * 60)

    try:
        while True:
            payload = generate_payload()
            send_payload(args.endpoint, payload)
            if args.once:
                break
            time.sleep(args.interval)
    except KeyboardInterrupt:
        print("\nStopping sender…")


if __name__ == "__main__":
    main()

