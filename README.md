# Satellite GPS Safety Beacon System

Autonomous GPS-to-satellite localization system designed for isolated areas without WiFi or GSM coverage.

---

## 1. Problem Context

In mountainous or forest environments:

- No WiFi coverage  
- Limited or no GSM signal  
- Risk scenarios: loss, fall, accident  
- Difficult and slow rescue localization  

This project provides an autonomous safety beacon capable of transmitting GPS coordinates via satellite communication.

---

## 2. System Overview

The system performs:

1. High-precision GPS acquisition (u-blox ZED-F9P)
2. Binary encoding of coordinates on Arduino
3. HEX payload transmission using AT commands
4. Satellite uplink
5. MQTT reception
6. Real-time decoding in Node-RED
7. Distance calculation (Haversine formula)
8. Data storage (InfluxDB)
9. Supervision dashboard (Grafana)

---

## 3. Hardware Components

- GPS RTK Module: u-blox ZED-F9P  
- Satellite Module: EchoStar Mobile EM2050  
- Antenna: Active L1/L2 antenna  
- Power Supply: 10,000 mAh power bank  

---

## 4. End-to-End Workflow

GPS → Encoding (Arduino) → Satellite → MQTT → Node-RED → InfluxDB → Grafana

---

## 5. GPS Encoding Strategy (Arduino)

Steps:

- Read latitude and longitude using TinyGPS++
- Multiply by 1e6 to preserve precision
- Convert to signed int32
- Encode using Big Endian format
- Build a 16-character HEX frame
- Send using:AT+SEND=1,0,16,1,<payload>

Transmission interval: 3 minutes.

---

## 6. Payload Decoding (Node-RED)

- Extract `frmPayload`
- Base64 → Buffer
- ASCII → HEX
- HEX → Binary buffer
- Extract:
  - 4 bytes longitude
  - 4 bytes latitude
- Divide by 1e6 to recover float values

---

## 7. Distance Calculation

The Haversine formula is used:

- Earth radius R = 6,371,000 meters
- Each new coordinate is compared with the previous one
- Segment distance is accumulated
- Total distance displayed on dashboard

---

## 8. Data Supervision

### InfluxDB

- Bucket: `gps`
- Measurement: `gps_tracking`
- Fields:
  - latitude
  - longitude
  - distance_total_m

### Grafana

- Real-time map visualization
- Auto-refresh every 10 seconds
- Trajectory reconstruction
- Distance evolution tracking

---

## 9. Power and Autonomy

- Battery: 10,000 mAh @ 3.7V ≈ 37 Wh
- Estimated autonomy: ~33 hours
- Average power consumption: ~1.1 W
- Autonomy increases with longer transmission intervals

---

## 10. Applications

- Off-piste skiing
- Forest hiking
- Mountaineering
- Outdoor extreme environments

---

## 11. Future Improvements

- SOS button
- Fall detection using accelerometer
- Emergency high-frequency transmission mode
- Waterproof casing
- Energy optimization

---

## 12. Conclusion

A fully autonomous safety solution combining:

- High-precision GPS
- Satellite communication
- IoT architecture (MQTT)
- Real-time monitoring
- Historical data supervision
