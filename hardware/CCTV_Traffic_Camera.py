import cv2
import requests
import time
import json
import numpy as np

# Mock endpoint for the NEXUS dashboard
API_URL = "http://nexus-dashboard.local/api/sensors/cctv/ingest"
CAMERA_ID = "JUNC-01-NORTH"

def analyze_frame(frame):
    # In a real scenario, run YOLOv8 or Haar Cascades here to count vehicles
    # For hackathon demonstration, we return a simulated vehicle count
    # based on random density or basic motion detection
    return np.random.randint(5, 45) # returns dummy vehicle count

def main():
    cap = cv2.VideoCapture(0) # Open standard webcam / CCTV feed
    print(f"Started CCTV Monitoring for {CAMERA_ID}")
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
            
        # Run AI detection
        vehicle_count = analyze_frame(frame)
        
        # Calculate congestion percentage (assuming max 50 vehicles is 100% capacity)
        congestion = min(100, int((vehicle_count / 50.0) * 100))
        
        # Prepare JSON payload
        payload = {
            "cameraId": CAMERA_ID,
            "density": vehicle_count,
            "congestionPct": congestion,
            "timestamp": time.time()
        }
        
        # Send to NEXUS backend
        try:
            res = requests.post(API_URL, json=payload, timeout=3)
            print(f"[CCTV] Sent data: {vehicle_count} vehicles | Response: {res.status_code}")
        except Exception as e:
            print(f"[CCTV] Connection Error: {e}")
            
        time.sleep(10) # Process and send every 10 seconds

if __name__ == '__main__':
    main()
