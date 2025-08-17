#include "Arduino.h"
#pragma once


// ===== UBX helpers (u-blox configuration) =====
static void sendUBX(HardwareSerial &ser, uint8_t cls, uint8_t id, const uint8_t *payload, uint16_t len) {
  uint8_t ckA = 0, ckB = 0;
  uint8_t hdr[4] = { cls, id, (uint8_t)(len & 0xFF), (uint8_t)(len >> 8) };
  for (int i = 0; i < 4; i++) {
    ckA += hdr[i];
    ckB += ckA;
  }
  for (uint16_t i = 0; i < len; i++) {
    ckA += payload[i];
    ckB += ckA;
  }

  ser.write(0xB5);
  ser.write(0x62);
  ser.write(hdr, 4);
  if (len) ser.write(payload, len);
  ser.write(ckA);
  ser.write(ckB);
  ser.flush();
}

static void setNmeaMsgRate(HardwareSerial &ser, uint8_t nmeaId, uint8_t rateUART1) {
  // Configure output rate of a specific NMEA sentence on each interface
  uint8_t payload[8] = {
    0xF0, nmeaId,  // NMEA class and message ID
    0x00,          // I2C
    rateUART1,     // UART1
    0x00,          // UART2
    0x00,          // USB
    0x00           // SPI
  };
  sendUBX(ser, 0x06, 0x01, payload, sizeof(payload));  // CFG-MSG
  delay(10);
}

// High-rate navigation (25 Hz) with reduced GSA/GSV to 1 Hz
void initUblox25Hz_reduceGSV_GSA(HardwareSerial &ser,uint32_t targetBaud = 460800, bool savePermanent = false) {
  delay(200);

  // 1) Set UART1 baud rate
  uint8_t cfgPRT[20] = {
    0x01, 0x00, 0x00, 0x00,  // portID=1
    0xD0, 0x08, 0x00, 0x00,  // 8N1
    0, 0, 0, 0,              // baud (filled below)
    0x07, 0x00,              // inProtoMask  (UBX|NMEA|RTCM)
    0x03, 0x00,              // outProtoMask (UBX|NMEA)
    0x00, 0x00,
    0x00, 0x00
  };
  cfgPRT[8] = (uint8_t)(targetBaud & 0xFF);
  cfgPRT[9] = (uint8_t)((targetBaud >> 8) & 0xFF);
  cfgPRT[10] = (uint8_t)((targetBaud >> 16) & 0xFF);
  cfgPRT[11] = (uint8_t)((targetBaud >> 24) & 0xFF);
  sendUBX(ser, 0x06, 0x00, cfgPRT, sizeof(cfgPRT));  // CFG-PRT
  delay(150);

  // 2) Match ESP32 UART to the same baud
  ser.updateBaudRate(targetBaud);
  delay(100);

  // 3) Navigation rate: measRate=40 ms (25 Hz), navRate=1, timeRef=UTC
  uint8_t cfgRATE[6] = { 0x28, 0x00, 0x01, 0x00, 0x00, 0x00 };
  sendUBX(ser, 0x06, 0x08, cfgRATE, sizeof(cfgRATE));  // CFG-RATE
  delay(50);

  // 4) Lower GSA/GSV to 1 Hz (every 25 cycles at 25 Hz)
  setNmeaMsgRate(ser, 0x02, 25);  // GSA
  setNmeaMsgRate(ser, 0x03, 25);  // GSV

  // 5) Optional: persist settings
  if (savePermanent) {
    uint8_t cfgCFG[13] = {
      0x00, 0x00, 0x00, 0x00,  // clearMask
      0xFF, 0xFF, 0x00, 0x00,  // saveMask
      0x00, 0x00, 0x00, 0x00,  // loadMask
      0x0F                     // deviceMask (BBR + Flash)
    };
    sendUBX(ser, 0x06, 0x09, cfgCFG, sizeof(cfgCFG));  // CFG-CFG
    delay(200);
  }
}


// ---- Small utilities ----
static inline double keepOr(double last, double now) {
  return (isnan(now) ? last : now);
}
static inline double lpf(double prev, double x, float a) {
  if (isnan(prev)) return x;
  return prev + a * (x - prev);
}
// Circular smoothing for heading (0–360°), UI only
static inline double smooth_heading_deg(double prev, double now, float a) {
  if (isnan(prev)) return now;
  double diff = now - prev;
  if (diff > 180.0) diff -= 360.0;
  if (diff < -180.0) diff += 360.0;
  double out = fmod(prev + diff * a, 360.0);
  if (out < 0) out += 360.0;
  return out;
}