const express = require('express');
const cors = require('cors');
const db = require('./db_config');

const app = express();
app.use(cors());
app.use(express.json());

const PORT = 8080;

// 1. INGEST: Traffic Density (from CCTV/IR Sensors)
app.post('/api/sensors/traffic/ingest', async (req, res) => {
    const { junctionId, count, congestionPct } = req.body;
    try {
        const query = 'INSERT INTO traffic_density (junction_id, vehicle_count, congestion_pct) VALUES (?, ?, ?)';
        await db.execute(query, [junctionId, count, congestionPct || 0]);
        res.status(201).json({ success: true, message: 'Traffic data stored.' });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 2. INGEST: Emergency RFID Detection
app.post('/api/sensors/rfid/ingest', async (req, res) => {
    const { scannerId, tagId, type } = req.body;
    try {
        const query = 'INSERT INTO emergency_events (scanner_id, rfid_tag, vehicle_type) VALUES (?, ?, ?)';
        await db.execute(query, [scannerId, tagId, type]);
        // Here you would also emit a WebSocket event to the dashboard to turn lights green
        res.status(201).json({ success: true, message: 'Emergency event triggered.' });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 3. INGEST: Parking Ultrasonic Sensor
app.post('/api/sensors/ultrasonic/ingest', async (req, res) => {
    const { slotId, status } = req.body;
    try {
        const query = 'UPDATE parking_slots SET status = ? WHERE slot_id = ?';
        const [result] = await db.execute(query, [status, slotId]);
        if (result.affectedRows === 0) {
            return res.status(404).json({ error: 'Parking slot not found.' });
        }
        res.status(200).json({ success: true, message: `Slot ${slotId} updated to ${status}.` });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// 4. FETCH: Get Latest Data for Dashboard
app.get('/api/dashboard/summary', async (req, res) => {
    try {
        const [activeEmergencies] = await db.execute('SELECT * FROM emergency_events WHERE status = "ACTIVE"');
        const [parkingSlots] = await db.execute('SELECT * FROM parking_slots');
        
        res.json({
            emergencies: activeEmergencies,
            parking: parkingSlots
        });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.listen(PORT, () => {
    console.log(`🚀 NEXUS API Server running on http://localhost:${PORT}`);
});
