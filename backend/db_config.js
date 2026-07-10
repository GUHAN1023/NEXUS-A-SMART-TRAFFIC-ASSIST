const mysql = require('mysql2/promise');

// NEXUS Database Connection Pool
const pool = mysql.createPool({
    host: 'localhost',       // Or AWS RDS / Cloud SQL endpoint
    user: 'nexus_admin',
    password: 'secure_password_123',
    database: 'nexus_db',
    waitForConnections: true,
    connectionLimit: 10,
    queueLimit: 0
});

// Helper function to test connection on startup
async function testConnection() {
    try {
        const connection = await pool.getConnection();
        console.log("✅ Successfully connected to NEXUS SQL Database.");
        connection.release();
    } catch (err) {
        console.error("❌ Database connection failed:", err.message);
    }
}

testConnection();

module.exports = pool;
