#include <TinyGPS++.h>

HardwareSerial Serial2(PA3, PA2);   // Satellite
HardwareSerial Serial1(PB7, PB6);   // GPS + affichage

TinyGPSPlus gps;

unsigned long lastSend = 0;
const unsigned long sendInterval = 180000;  // 3 minutes (180 000 ms)

char packet[20];

void readSatelliteResponse() {
  delay(3000);

  Serial1.println("---- Satellite Response ----");
  while (Serial2.available()) {
    Serial1.write(Serial2.read());
  }
  Serial1.println("\n----------------------------");
}

void setup() {

  Serial1.begin(38400);   // GPS + affichage
  Serial2.begin(115200);  // Satellite

  Serial1.println("=== START SYSTEM ===");

  Serial1.println("Sending AT+JOIN...");
  Serial2.println("AT+JOIN");

  delay(20000);          // Attente join
  readSatelliteResponse();

  // ❌ SUPPRESSION des 5 minutes d’attente
}

void loop() {

  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }

  if (gps.location.isValid()) {

    if (millis() - lastSend > sendInterval) {

      lastSend = millis();

      float latitude  = gps.location.lat();
      float longitude = gps.location.lng();

      Serial1.println("---- POSITION ----");
      Serial1.print("Latitude  : ");
      Serial1.println(latitude, 6);
      Serial1.print("Longitude : ");
      Serial1.println(longitude, 6);
      Serial1.println("------------------");

      sendGpsData(latitude, longitude);
    }
  }
}

void sendGpsData(float latitude, float longitude) {

  int32_t lat_enc = (int32_t)(latitude * 1e6);
  int32_t lon_enc = (int32_t)(longitude * 1e6);

  char hexFrameLong[9];
  char hexFrameLat[9];

  snprintf(hexFrameLong, sizeof(hexFrameLong), "%08X", lon_enc);
  snprintf(hexFrameLat, sizeof(hexFrameLat), "%08X", lat_enc);

  // Big Endian manuel
  for (int i = 0; i < 8; i++) {
    packet[i] = hexFrameLong[i];
  }

  for (int i = 0; i < 8; i++) {
    packet[i + 8] = hexFrameLat[i];
  }

  packet[16] = '\0';

  Serial1.println("Sending packet:");
  Serial1.println(packet);

  char packet_buffer[80];
  sprintf(packet_buffer, "AT+SEND=1,0,16,1,%s", packet);

  Serial2.println(packet_buffer);
  Serial1.println(packet_buffer);

  readSatelliteResponse();
}
