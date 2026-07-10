-- NEXUS Smart City Database Schema

CREATE DATABASE IF NOT EXISTS nexus_db;
USE nexus_db;

-- 1. Store vehicle count data from IR/CCTV Sensors
CREATE TABLE traffic_density (
    id INT AUTO_INCREMENT PRIMARY KEY,
    junction_id VARCHAR(50) NOT NULL,
    vehicle_count INT NOT NULL,
    congestion_pct INT,
    recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. Store emergency vehicle RFID events
CREATE TABLE emergency_events (
    id INT AUTO_INCREMENT PRIMARY KEY,
    scanner_id VARCHAR(50) NOT NULL,
    rfid_tag VARCHAR(100) NOT NULL,
    vehicle_type VARCHAR(50) DEFAULT 'EMERGENCY',
    status VARCHAR(20) DEFAULT 'ACTIVE',
    detected_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    cleared_at TIMESTAMP NULL
);

-- 3. Store GPS tracking of City Buses/Cabs
CREATE TABLE gps_tracking (
    id INT AUTO_INCREMENT PRIMARY KEY,
    vehicle_id VARCHAR(50) NOT NULL,
    latitude DECIMAL(10, 6) NOT NULL,
    longitude DECIMAL(10, 6) NOT NULL,
    speed_kmh DECIMAL(5, 2),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 4. Store Smart Parking Slots State
CREATE TABLE parking_slots (
    slot_id VARCHAR(20) PRIMARY KEY,
    zone VARCHAR(20) NOT NULL,
    status ENUM('free', 'taken', 'reserved') DEFAULT 'free',
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Insert some initial parking slots
INSERT INTO parking_slots (slot_id, zone, status) VALUES 
('ZONE-A-01', 'A', 'free'),
('ZONE-A-02', 'A', 'taken'),
('ZONE-B-12', 'B', 'free');
