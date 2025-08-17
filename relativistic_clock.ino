// ============================================================================
// Relativistic Clock - M5Stack Core S3 + GNSS Unit (NEO-M9N + BMP280 + BMI270)
//
// Calculates and displays relativistic time dilation in real time (ns/hour)
// using real sensors:
//
//   - Special Relativity (SR): time delay due to velocity relative to an
//     inertial frame.
//   - General Relativity (GR): time advance due to gravitational potential
//     difference (higher altitude → faster clock).
//
// Also shows latitude, longitude, altitude, ground speed, azimuth, local gravity,
// tangential Earth-rotation speed, total relative speed, and net time gain/loss
// relative to sea level.
// ============================================================================

#include <M5Unified.h>
#include <Adafruit_BMP280.h>  // BMP280 barometer
#include <TinyGPSPlus.h>      // GNSS NMEA decoder
#include "relativistic_clock_hud.h"
#include "relativistic_clock_utils.h"
#include "tinygps_hae_utils.h"

// ---- Canvas instances (must match externs declared in HUD header) ----
M5Canvas canvasBackground(&M5.Display);
M5Canvas canvasStaticVelocity(&M5.Display);
M5Canvas canvasStaticAltitude(&M5.Display);
M5Canvas canvasStaticLatitude(&M5.Display);
M5Canvas canvasStaticTotalVelocity(&M5.Display);
M5Canvas canvasStaticLocalGravity(&M5.Display);
M5Canvas canvasStaticTimeDilation(&M5.Display);
M5Canvas canvasStaticHeader(&M5.Display);
M5Canvas canvasStaticLineChart(&M5.Display);

M5Canvas canvasDynamicVelocity(&M5.Display);
M5Canvas canvasDynamicAltitude(&M5.Display);
M5Canvas canvasDynamicLatitude(&M5.Display);
M5Canvas canvasDynamicTotalVelocity(&M5.Display);
M5Canvas canvasDynamicLocalGravity(&M5.Display);
M5Canvas canvasDynamicTimeDilation(&M5.Display);
M5Canvas canvasDynamicHeader(&M5.Display);
M5Canvas canvasDynamicLineChart(&M5.Display);

// ---- Physical constants ----
const double SPEED_OF_LIGHT = 299792458.0;  // m/s
const bool HAE_MODE = false;

// ---- Sensor objects ----
Adafruit_BMP280 barometer(&Wire1);
TinyGPSPlus gps;
TinyGPSHaeHelper hae(gps);
HardwareSerial GNSSSerial(1);  // UART1 for GNSS



// ---- Earth rotation tangential speed (m/s) at given lat/alt ----
double calcEarthRotationSpeed(double latitude_deg, double altitude_m) {
  const double a = 6378137.0;  // WGS-84 semi-major axis
  const double b = 6356752.0;  // WGS-84 semi-minor axis
  const double phi = radians(latitude_deg);
  const double s = sin(phi), c = cos(phi);
  const double e2 = (a * a - b * b) / (a * a);
  const double N = a / sqrt(1.0 - e2 * s * s);
  const double r = (N + altitude_m) * c;
  return 7.292115e-5 * r;  // ω·r
}

double calcLocalGravity(double latitude_deg, double altitude_m) {
  // Clamp opcional para evitar entradas absurdas (ex.: leituras ruins)
  if (altitude_m < -500.0) altitude_m = -500.0;
  if (altitude_m > 20000.0) altitude_m = 20000.0;

  const double phi = radians(latitude_deg);
  const double s = sin(phi);
  const double sin2 = s * s;

  // Constantes WGS84 “clássicas”
  const double ge = 9.7803253359;  // m/s²
  const double k = 0.00193185265241;
  const double e2 = 0.00669437999013;

  const double g0 = ge * (1.0 + k * sin2) / sqrt(1.0 - e2 * sin2);

  // Correção de ar livre linear (~0.3086 mGal/m = 3.086e-6 m/s² por metro)
  return g0 - 3.086e-6 * altitude_m;
}


// ---- Time dilation (returns ns/s). Horizontal ground speed only. ----
double calcTimeDilation(double velocity_kmh, double azimuth_deg, double latitude_deg, double altitude_m,
                        double &out_gravity, double &out_earthRotationSpeed, double &out_relativeVelocity) {
  // Convert to m/s
  const double v = velocity_kmh / 3.6;

  // Horizontal components (0° = North, 90° = East)
  const double vE = v * sin(radians(azimuth_deg));  // East (+)
  const double vN = v * cos(radians(azimuth_deg));  // North (+)

  // Earth rotation at location (Eastward)
  const double vRot = calcEarthRotationSpeed(latitude_deg, altitude_m);
  out_earthRotationSpeed = vRot;

  // Total inertial-frame speed (rotation + own motion)
  const double vTot = sqrt((vRot + vE) * (vRot + vE) + vN * vN);
  out_relativeVelocity = vTot;

  // Local gravity
  const double g = calcLocalGravity(latitude_deg, altitude_m);
  out_gravity = g;

  // SR: slows clock (negative)
  const double deltaSR = -(vTot * vTot) / (2.0 * SPEED_OF_LIGHT * SPEED_OF_LIGHT);

  // GR: speeds clock (positive)
  const double deltaGR = (g * altitude_m) / (SPEED_OF_LIGHT * SPEED_OF_LIGHT);

  // Net (ns per second)
  return (deltaSR + deltaGR) * 1e9;
}

// ---- Raw (for physics; no smoothing) ----
double raw_lat = NAN, raw_lon = NAN;
double raw_vel_kmh = NAN, raw_az_deg = NAN;
double raw_alt_m = NAN;

// ---- UI-smoothed (for gauges only) ----
double ui_vel_kmh = 0.0, ui_az_deg = NAN;

// ---- Status ----
double g_hdop = NAN;
int g_sats = 0;
int g_batt = -1;

// ---- Header refresh throttle ----
static uint32_t tHeader = 0;

// ---- Sea Level Pressure (configurable) ----
static float slp_hPa = 1013.25f;



// ---------------------- Setup ----------------------
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Power.setExtPower(true);  // power Grove port

  Wire1.begin(12, 11, 400000);  // I2C for GNSS/BMP module

  // BMP280 (default I2C address 0x76)
  if (!barometer.begin(0x76)) {
    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(RED);
    M5.Display.println("ERRO: BMP280 NAO ENCONTRADO!");
    while (1) {}
  }

  // GNSS (NEO-M9N) on UART1 (GPIO 18 RX, 17 TX)
  GNSSSerial.begin(460800, SERIAL_8N1, 18, 17);
  delay(200);

  // 25 Hz navigation; GSA/GSV at 1 Hz
  initUblox25Hz_reduceGSV_GSA(GNSSSerial, 460800, false);

  // HUD static layers
  drawBackground();
  drawStaticHeader();
  drawStaticLineChart();
  drawStaticVelocity();
  drawStaticLocalGravity();
  drawStaticTimeDilation();
  drawStaticTotalVelocity();
  drawStaticAltitude();
  drawStaticLatitude();
  createDynamicCanvases();

  delay(500);
}

// ---------------------- Main loop ----------------------
void loop() {
  M5.update();

  // GNSS ingest (non-blocking)
  while (GNSSSerial.available() > 0) {
    gps.encode(GNSSSerial.read());
  }
  const bool gpsOK = gps.location.isUpdated() && gps.speed.isUpdated() && gps.course.isUpdated();

  // Freshness: invalidate stale readings
  const unsigned long ageLoc = gps.location.age();
  const unsigned long ageSpd = gps.speed.age();
  const unsigned long ageCrs = gps.course.age();

  if (ageLoc > 3000) {
    raw_lat = NAN;
    raw_lon = NAN;
  }  // >3 s
  if (ageSpd > 3000) { raw_vel_kmh = NAN; }
  if (ageCrs > 3000) { raw_az_deg = NAN; }

  g_sats = (gps.satellites.isValid() && gps.satellites.age() < 5000)
             ? (int)gps.satellites.value()
             : 0;

  g_hdop = (gps.hdop.isValid() && gps.hdop.age() < 5000)
             ? gps.hdop.hdop()
             : NAN;

  // Current GNSS values (may be invalid)
  const double lat_now = gps.location.isValid() ? gps.location.lat() : NAN;
  const double lon_now = gps.location.isValid() ? gps.location.lng() : NAN;
  const double vel_now = gps.speed.isValid() ? gps.speed.kmph() : NAN;
  const double az_now = gps.course.isValid() ? gps.course.deg() : NAN;

  // Raw for physics (keep last valid)
  raw_lat = keepOr(raw_lat, lat_now);
  raw_lon = keepOr(raw_lon, lon_now);
  raw_vel_kmh = keepOr(raw_vel_kmh, vel_now);
  raw_az_deg = keepOr(raw_az_deg, az_now);


  hae.update();  // Update N cache

//HAE Mode
  if (HAE_MODE) {
    double hae_m = hae.getHAE_m(5000);
    const double alt_now = hae_m;
    if (!isnan(alt_now) && isfinite(alt_now)) {
      raw_alt_m = alt_now;  // raw for physics
    }
  } else {
    // Barometric altitude (m) using current SLP
    const double alt_now = barometer.readAltitude(slp_hPa);
    if (!isnan(alt_now) && isfinite(alt_now)) {
      raw_alt_m = alt_now;  // raw for physics
    }
  }

  // UI smoothing (visual only)
  if (!isnan(raw_vel_kmh)) ui_vel_kmh = lpf(ui_vel_kmh, raw_vel_kmh, 0.12f);
  if (!isnan(raw_az_deg)) ui_az_deg = smooth_heading_deg(ui_az_deg, raw_az_deg, 0.10f);

  // Physics use raw values only (replace NaNs with zeros)
  const double lat_calc = isnan(raw_lat) ? 0.0 : raw_lat;
  const double alt_calc = isnan(raw_alt_m) ? 0.0 : raw_alt_m;
  const double vel_calc = isnan(raw_vel_kmh) ? 0.0 : raw_vel_kmh;
  const double az_calc = isnan(raw_az_deg) ? 0.0 : raw_az_deg;

  double local_gravity = 0.0;         // m/s² (output)
  double earth_rotation_speed = 0.0;  // m/s  (output)
  double relative_velocity = 0.0;     // m/s  (output)

  const double delta_ns_per_second = calcTimeDilation(
    vel_calc, az_calc, lat_calc, alt_calc,
    local_gravity, earth_rotation_speed, relative_velocity);

  const double delta_ns_per_hour = delta_ns_per_second * 3600.0;

  // HUD dynamic layers
  if (gpsOK) {
    drawDynamicAltitude(alt_calc, ui_az_deg);
  } else {
    drawDynamicAltitude(alt_calc, -1);
  }
  drawDynamicLatitude(lat_calc);
  drawDynamicVelocity(earth_rotation_speed * 3.6);    // km/h
  drawDynamicTotalVelocity(relative_velocity * 3.6);  // km/h
  drawDynamicLocalGravity(local_gravity);
  drawDynamicTimeDilation(delta_ns_per_hour);
  drawDynamicLineChart(delta_ns_per_hour);

  // Header refresh (~200 ms)
  if (millis() - tHeader >= 200) {
    g_batt = M5.Power.getBatteryLevel();
    drawDynamicHeader(isnan(g_hdop) ? -1.0 : g_hdop, g_batt, g_sats);
    tHeader = millis();
  }

  // UI cadence ~60 Hz
  delay(16);
}
