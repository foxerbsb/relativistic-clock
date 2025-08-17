#pragma once
#include <M5Unified.h>  // Main library for M5Stack Core S3
#include "assets/fonts/BebasNeueRegular20.h"
#include "assets/fonts/BebasNeueRegular35.h"
#include "assets/fonts/RobotoBlack8.h"
#include "assets/fonts/RobotoBold9.h"
#include "assets/fonts/RobotoRegular9.h"
#include "assets/fonts/RobotoBoldCondensed12.h"
#include "assets/fonts/RobotoBoldCondensed10.h"
#include <hud_gauges.h>

extern M5Canvas canvasBackground;
extern M5Canvas canvasStaticVelocity;
extern M5Canvas canvasStaticAltitude;
extern M5Canvas canvasStaticLatitude;
extern M5Canvas canvasStaticTotalVelocity;
extern M5Canvas canvasStaticLocalGravity;
extern M5Canvas canvasStaticTimeDilation;
extern M5Canvas canvasStaticHeader;
extern M5Canvas canvasStaticLineChart;

extern M5Canvas canvasDynamicVelocity;
extern M5Canvas canvasDynamicAltitude;
extern M5Canvas canvasDynamicLatitude;
extern M5Canvas canvasDynamicTotalVelocity;
extern M5Canvas canvasDynamicLocalGravity;
extern M5Canvas canvasDynamicTimeDilation;
extern M5Canvas canvasDynamicHeader;
extern M5Canvas canvasDynamicLineChart;


// ---- Background (static) ----
// Draws the main background, header strip, bottom highlight band, and separators.
inline void drawBackground() {
  canvasBackground.setColorDepth(32);
  canvasBackground.createSprite(320, 240);
  canvasBackground.fillScreen(canvasBackground.color888(20, 21, 39));
  canvasBackground.fillRoundRect(0, 0, 320, 25, 0, canvasBackground.color888(0, 0, 0));
  canvasBackground.fillRoundRect(0, 160, 320, 90, 0, canvasBackground.color888(208, 247, 32));
  canvasBackground.drawGradientHLine(5, 63, 310, canvasBackground.color565(50, 50, 50), canvasBackground.color565(20, 21, 39));
  canvasBackground.drawGradientHLine(0, 154, 320, canvasBackground.color565(208, 247, 32), canvasBackground.color565(50, 50, 50));
  canvasBackground.drawGradientHLine(0, 156, 320, canvasBackground.color565(208, 247, 32), canvasBackground.color565(50, 50, 50));
  canvasBackground.drawGradientHLine(0, 158, 320, canvasBackground.color565(208, 247, 32), canvasBackground.color565(50, 50, 50));
  canvasBackground.loadFont(BebasNeueRegular20);
  canvasBackground.setTextColor(WHITE, BLACK);
  canvasBackground.drawString("RELATIVISTIC CLOCK", 5, 0);
  canvasBackground.unloadFont();
  canvasBackground.pushSprite(0, 0);
}

// ---- Header (static) ----
// Prepares the static header container; dynamic content is drawn elsewhere.
inline void drawStaticHeader() {
  canvasStaticHeader.setColorDepth(32);
  canvasStaticHeader.createSprite(180, 25);
  canvasStaticHeader.fillScreen(canvasStaticHeader.color888(0, 0, 0));
  canvasStaticHeader.fillRoundRect(0, 0, 180, 25, 0, canvasStaticHeader.color888(0, 0, 0));
  canvasStaticHeader.pushSprite(140, 0);
}

// ---- Line Chart (static) ----
// Creates the static area for the line chart; data is rendered dynamically.
inline void drawStaticLineChart() {
  canvasStaticLineChart.setColorDepth(32);
  canvasStaticLineChart.createSprite(160, 40);
  canvasStaticLineChart.pushSprite(155, 195);
}

// ---- Altitude gauge (static frame) ----
// Draws static decorations and units for the altitude gauge.
inline void drawStaticAltitude() {
  canvasStaticAltitude.setColorDepth(32);
  canvasStaticAltitude.createSprite(90, 90);
  canvasStaticAltitude.fillCircle(45, 45, 44, canvasStaticAltitude.color888(20, 21, 39));
  canvasStaticAltitude.fillCircle(45, 45, 40, canvasStaticAltitude.color888(20, 21, 39));
  canvasStaticAltitude.setTextColor(canvasStaticAltitude.color888(224, 106, 34), canvasStaticAltitude.color888(20, 21, 39));
  canvasStaticAltitude.loadFont(RobotoBoldCondensed12);
  canvasStaticAltitude.drawString("ALT.", 35, 22);
  canvasStaticAltitude.setTextColor(canvasStaticAltitude.color888(0, 0, 0), canvasStaticAltitude.color888(27, 228, 234));
  canvasStaticAltitude.fillCircle(46, 59, 8, canvasStaticAltitude.color888(27, 228, 234));
  canvasStaticAltitude.drawString("m", 42, 52);
  canvasStaticAltitude.unloadFont();
  canvasStaticAltitude.pushSprite(215, 100, TFT_TRANSPARENT);
}

// ---- Latitude gauge (static frame) ----
// Draws static decorations and label for the latitude gauge.
inline void drawStaticLatitude() {
  canvasStaticLatitude.setColorDepth(32);
  canvasStaticLatitude.createSprite(90, 90);
  canvasStaticLatitude.fillCircle(45, 45, 44, canvasStaticLatitude.color888(20, 21, 39));
  canvasStaticLatitude.fillCircle(45, 45, 40, canvasStaticLatitude.color888(20, 21, 39));
  canvasStaticLatitude.setTextColor(canvasStaticLatitude.color888(115, 228, 163), canvasStaticLatitude.color888(20, 21, 39));
  canvasStaticLatitude.loadFont(RobotoBoldCondensed10);
  canvasStaticLatitude.drawString("LAT.", 35, 22);
  canvasStaticLatitude.unloadFont();
  canvasStaticLatitude.pushSprite(150, 100, TFT_TRANSPARENT);
}

// ---- Local rotational velocity (static frame) ----
// Draws the static frame and labels for the local rotational velocity bar.
inline void drawStaticVelocity() {
  canvasStaticVelocity.setColorDepth(32);
  canvasStaticVelocity.createSprite(310, 30);
  canvasStaticVelocity.fillScreen(canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.fillRoundRect(0, 0, 60, 15, 0, canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.fillRoundRect(0, 15, 60, 15, 0, canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.fillRoundRect(60, 15, 250, 15, 0, canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.setTextColor(canvasStaticVelocity.color888(150, 150, 150), canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.loadFont(RobotoBoldCondensed10);
  canvasStaticVelocity.drawString("km/h", 19, 15);
  canvasStaticVelocity.loadFont(RobotoBoldCondensed10);
  canvasStaticVelocity.setTextColor(canvasStaticVelocity.color888(255, 255, 255), canvasStaticVelocity.color888(20, 21, 39));
  canvasStaticVelocity.drawString("LOCAL ROTATIONAL VELOCITY", 120, 15);
  canvasStaticVelocity.unloadFont();
  canvasStaticVelocity.loadFont(RobotoRegular9);
  canvasStaticVelocity.drawString("0", 60, 16);
  canvasStaticVelocity.drawString("1700", 289, 16);
  canvasStaticVelocity.unloadFont();
  canvasStaticVelocity.pushSprite(5, 30, TFT_TRANSPARENT);
}

// ---- Total velocity (static frame) ----
// Draws the static frame and labels for the total velocity bar.
inline void drawStaticTotalVelocity() {
  canvasStaticTotalVelocity.setColorDepth(32);
  canvasStaticTotalVelocity.createSprite(310, 30);
  canvasStaticTotalVelocity.fillScreen(canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.fillRoundRect(0, 0, 60, 15, 0, canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.fillRoundRect(0, 15, 60, 15, 0, canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.fillRoundRect(60, 15, 250, 15, 0, canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.setTextColor(canvasStaticTotalVelocity.color888(150, 150, 150), canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.loadFont(RobotoBoldCondensed10);
  canvasStaticTotalVelocity.drawString("km/h", 19, 15);
  canvasStaticTotalVelocity.loadFont(RobotoBoldCondensed10);
  canvasStaticTotalVelocity.setTextColor(canvasStaticTotalVelocity.color888(255, 255, 255), canvasStaticTotalVelocity.color888(20, 21, 39));
  canvasStaticTotalVelocity.drawString("TOTAL VELOCITY", 148, 15);
  canvasStaticTotalVelocity.unloadFont();
  canvasStaticTotalVelocity.loadFont(RobotoRegular9);
  canvasStaticTotalVelocity.drawString("0", 60, 16);
  canvasStaticTotalVelocity.drawString("2700", 289, 16);
  canvasStaticTotalVelocity.unloadFont();
  canvasStaticTotalVelocity.pushSprite(5, 67, TFT_TRANSPARENT);
}

// ---- Local gravity (static frame) ----
// Draws the pill-shaped static container and label for local gravity.
inline void drawStaticLocalGravity() {
  canvasStaticLocalGravity.setColorDepth(32);
  canvasStaticLocalGravity.createSprite(200, 45);
  // Good color options (kept as reference)
  // canvasStaticLocalGravity.fillRoundRect(0, 0, 200, 45, 7, canvasStaticLocalGravity.color888(7, 84, 76));
  // canvasStaticLocalGravity.fillRoundRect(0, 0, 200, 45, 7, canvasStaticLocalGravity.color888(35, 59, 66));
  // canvasStaticLocalGravity.fillRoundRect(0, 0, 200, 45, 7, canvasStaticLocalGravity.color888(50, 26, 50));
  canvasStaticLocalGravity.fillRoundRect(0, 0, 200, 45, 7, canvasStaticLocalGravity.color888(253, 47, 43));
  canvasStaticLocalGravity.loadFont(RobotoBold9);
  canvasStaticLocalGravity.setTextColor(canvasStaticLocalGravity.color888(255, 255, 255), canvasStaticLocalGravity.color888(253, 47, 43));
  canvasStaticLocalGravity.drawString("LOCAL GRAVITY (m/s   )", 12, 3);
  canvasStaticLocalGravity.loadFont(RobotoBlack8);
  canvasStaticLocalGravity.drawString("2", 102, 1);
  canvasStaticLocalGravity.pushSprite(5, 102, TFT_TRANSPARENT);
}

// ---- Time dilation (static frame) ----
// Draws the static container and title for the time dilation panel.
inline void drawStaticTimeDilation() {
  canvasStaticTimeDilation.setColorDepth(32);
  canvasStaticTimeDilation.createSprite(145, 70);
  canvasStaticTimeDilation.fillRoundRect(0, 0, 145, 70, 7, canvasStaticTimeDilation.color888(35, 34, 68));

  canvasStaticTimeDilation.loadFont(RobotoBoldCondensed10);
  canvasStaticTimeDilation.setTextColor(canvasStaticTimeDilation.color888(255, 255, 255), canvasStaticTimeDilation.color888(35, 34, 68));
  canvasStaticTimeDilation.drawString("TIME DILATION (ns/h)", 28, 3);

  canvasStaticTimeDilation.unloadFont();
  canvasStaticTimeDilation.pushSprite(5, 165, TFT_TRANSPARENT);
}

// ---- Altitude (dynamic) ----
// Renders the altitude radial gauge and the azimuth ring overlay.
inline void drawDynamicAltitude(float altitude, float azimuth = -1) {
  int cx = 45, cy = 45;
  int outer_radius = 33;
  int inner_radius = 27;
  int segments = 160;
  int start_angle_deg = 180;
  int total_angle_deg = 360;
  int gap_deg = 0;

  drawAltitudeGauge(canvasDynamicAltitude, cx, cy,
                    outer_radius, inner_radius,
                    segments, start_angle_deg, total_angle_deg,
                    gap_deg, altitude);

  // Smoothed azimuth (0..360) 
 float smoothed_azimuth = azimuth;

  drawAzimuthRingWithArcUnderTriangles(canvasDynamicAltitude,
                                       cx, cy,
                                       outer_radius,
                                       smoothed_azimuth,
                                       /*ring_offset_px*/     7,
                                       /*ring_thickness*/     4,
                                       /*marker_arc_deg*/     3,
                                       /*color_ring*/         0,
                                       /*color_track*/        1,   // track color (subtle default)
                                       /*color_arc*/          0,   // arc color (light default)
                                       /*tri_len_px*/         2,
                                       /*base_half_deg*/      7,
                                       /*arc_inset_outer_px*/ 4,   // lowers arc/track visually
                                       /*arc_inset_inner_px*/ 2,
                                       /*draw_track*/         true);

  canvasDynamicAltitude.loadFont(RobotoBoldCondensed10);
  canvasDynamicAltitude.setTextColor(WHITE, canvasDynamicAltitude.color888(20, 21, 39));
  canvasDynamicAltitude.drawFloat(altitude, 1, 30, 36);
  canvasDynamicAltitude.pushSprite(215, 100, TFT_TRANSPARENT);
}

// ---- Header (dynamic) ----
// Draws dynamic header info: satellites, GPS signal (smoothed), and battery.
inline void drawDynamicHeader(float hdop, int batteryLevel, int satellites) {
  const int sats = satellites;
  canvasDynamicHeader.loadFont(RobotoBoldCondensed10);

  // Opaque header background (no transparency)
  canvasDynamicHeader.fillRect(0, 0,
                               canvasDynamicHeader.width(),
                               canvasDynamicHeader.height(),
                               canvasDynamicHeader.color888(0, 0, 0));

  // Battery (reserve space on the right)
  drawBatteryStatus(canvasDynamicHeader, canvasDynamicHeader.width() - 60, 6, 0.8f, true, batteryLevel);

  canvasDynamicHeader.setTextDatum(TL_DATUM);
  canvasDynamicHeader.setTextColor(canvasDynamicHeader.color888(239, 224, 0), canvasDynamicHeader.color888(0, 0, 0));
  canvasDynamicHeader.drawString("SAT:", 0, 6);

  // SAT value
  canvasDynamicHeader.setTextColor(canvasDynamicHeader.color888(9, 193, 175), canvasDynamicHeader.color888(0, 0, 0));
  canvasDynamicHeader.drawNumber(sats, 24, 6);

  // Signal gauge
  canvasDynamicHeader.setTextColor(canvasDynamicHeader.color888(239, 224, 0), canvasDynamicHeader.color888(0, 0, 0));
  canvasDynamicHeader.drawString("SIGNAL:", 40, 6);

  // Exponential smoothing for signal level (0..5) derived from HDOP
  static float levelSmooth = 0.0f;                // internal state
  float target = gpsSignalLevelFromHDOP(hdop);    // 0..5
  const float alpha = 0.25f;                      // 0..1 (higher = faster)
  levelSmooth += alpha * (target - levelSmooth);  // exponential smoothing

  drawGpsSignalGauge5Smooth(canvasDynamicHeader, 82, 5, 13, 5, 2, levelSmooth);

  // Push header (opaque)
  canvasDynamicHeader.pushSprite(140, 0);
}

// ---- Line Chart (dynamic) ----
// Draws the live time-dilation chart over the static container.
inline void drawDynamicLineChart(double dilation) {
  // canvasDynamicLineChart.fillRoundRect(0, 0, 160, 40, 10, canvasStaticLineChart.color888(20, 21, 39));
  drawTimeDilationChart(canvasDynamicLineChart, dilation,
                          -6.0, 3.0,
                          /*bg*/ 0,
                          /*line*/ canvasDynamicLineChart.color565(80, 255, 80),
                          /*zero*/ canvasDynamicLineChart.color565(220, 170, 40),
                          /*grid*/ canvasDynamicLineChart.color565(18, 36, 18),
                          /*dash*/ 4, /*gap*/ 3,
                          /*vStepPx*/ 3,      // vertical grid lines every 6 px
                          /*vMajorEvery*/ 0,  // kept for compatibility
                          /*colGridMajor*/ canvasDynamicLineChart.color565(50, 90, 50));

  canvasDynamicLineChart.pushSprite(155, 195);
}

// ---- Latitude (dynamic) ----
// Renders the latitude gauge, numeric value (5 decimals), and hemisphere badge.
inline void drawDynamicLatitude(float latitude) {
  int cx_lat = 45;
  int cy_lat = 45;
  int outer_radius_lat = 36;
  int inner_radius_lat = 28;
  int segments_lat = 60;
  float start_angle_lat = 113.4f;  // Earth's axial tilt reference (23.4°)
  float total_angle_lat = 180.0f;  // Half circle
  int gap_lat = 0;

  drawLatitudeGauge(
    canvasDynamicLatitude,               // Canvas
    cx_lat, cy_lat,                      // Gauge center
    outer_radius_lat, inner_radius_lat,  // Outer/inner radii
    segments_lat,                        // Number of segments
    start_angle_lat, total_angle_lat,    // Start and total angles
    gap_lat,                             // Segment gap
    latitude                             // Latitude (-90 to +90)
  );

  canvasDynamicLatitude.loadFont(RobotoBoldCondensed10);
  canvasDynamicLatitude.setTextColor(WHITE, canvasDynamicLatitude.color888(20, 21, 39));
  canvasDynamicLatitude.drawFloat(latitude, 5, 23, 36);

  if (latitude > 0) {
    canvasDynamicLatitude.setTextColor(canvasDynamicLatitude.color888(0, 0, 0), canvasDynamicLatitude.color888(235, 41, 67));
    canvasDynamicLatitude.fillCircle(45, 59, 8, canvasDynamicLatitude.color888(235, 41, 67));
    canvasDynamicLatitude.drawString("N", 42, 52);
  } else if (latitude < 0) {
    canvasDynamicLatitude.setTextColor(canvasDynamicLatitude.color888(0, 0, 0), canvasDynamicLatitude.color888(26, 122, 170));
    canvasDynamicLatitude.fillCircle(45, 59, 8, canvasDynamicLatitude.color888(26, 122, 170));
    canvasDynamicLatitude.drawString("S", 42, 52);
  } else {
    canvasDynamicLatitude.fillCircle(45, 59, 8, canvasDynamicLatitude.color888(20, 21, 39));
    canvasDynamicLatitude.drawString("--", 42, 52);
  }

  canvasDynamicLatitude.unloadFont();
  canvasDynamicLatitude.pushSprite(150, 100, TFT_TRANSPARENT);
}

// ---- Local rotational velocity (dynamic) ----
// Draws the live bar and prints the numeric value.
inline void drawDynamicVelocity(float velocity) {
  int x = 60, y = 3;
  int width = 250, height = 10;
  float vmin = 0.0f, vmax = 1700.0f;
  float value = velocity;

  drawSpeedBarGauge(canvasDynamicVelocity, x, y, width, height,
                    value, vmin, vmax,
                    255, 0, 0,       // gradient start (R,G,B)
                    100, 255, 100);  // gradient end   (R,G,B)

  canvasDynamicVelocity.setTextColor(canvasDynamicVelocity.color888(255, 255, 255), canvasDynamicVelocity.color888(20, 21, 39));
  canvasDynamicVelocity.loadFont(RobotoBoldCondensed12);
  canvasDynamicVelocity.setTextDatum(top_left);
  canvasDynamicVelocity.drawFloat(velocity, 1, 12, 0);
  canvasDynamicVelocity.pushSprite(5, 30, TFT_TRANSPARENT);
}

// ---- Total velocity (dynamic) ----
// Draws the live bar and prints the numeric value.
inline void drawDynamicTotalVelocity(float total_velocity) {
  int x = 60, y = 3;
  int width = 250, height = 10;
  float vmin = 0.0f, vmax = 2700.0f;
  float value = total_velocity;

  drawSpeedBarGauge(canvasDynamicTotalVelocity, x, y, width, height,
                    value, vmin, vmax,
                    28, 236, 221,   // gradient start
                    234, 20, 223);  // gradient end

  canvasDynamicTotalVelocity.setTextColor(canvasDynamicTotalVelocity.color888(255, 255, 255), canvasDynamicTotalVelocity.color888(20, 21, 39));
  canvasDynamicTotalVelocity.loadFont(RobotoBoldCondensed12);
  //canvasDynamicTotalVelocity.setTextDatum(top_left);
  canvasDynamicTotalVelocity.drawFloat(value, 1, 12, 0);
  canvasDynamicTotalVelocity.pushSprite(5, 67, TFT_TRANSPARENT);
}

// ---- Local gravity (dynamic) ----
// Draws the live horizontal gauge for local gravity and the numeric value.
inline void drawDynamicLocalGravity(float gravity) {
  drawHorizontalBarGauge(canvasDynamicLocalGravity, 20, 17, 110, 12,
                         gravity, 9.76f, 9.84f,
                         /*start*/ 0, 160, 0,
                         /*end*/ 218, 34, 57,
                         true, 5, true,
                         /*labels*/ true, 2, "", 1, 4);

  canvasDynamicLocalGravity.loadFont(RobotoBoldCondensed10);
  canvasDynamicLocalGravity.setTextColor(canvasDynamicLocalGravity.color888(0, 0, 0), canvasDynamicLocalGravity.color888(253, 47, 43));
  canvasDynamicLocalGravity.drawFloat(gravity, 5, 116, 2);
  canvasDynamicLocalGravity.pushSprite(5, 102, TFT_TRANSPARENT);
}

// ---- Time dilation (dynamic) ----
// Renders the numeric value and status text ("LAGGING"/"ADVANCING").
inline void drawDynamicTimeDilation(float time_dilation) {
  canvasDynamicTimeDilation.fillRoundRect(0, 18, 145, 46, 0, canvasDynamicTimeDilation.color888(35, 34, 68));
  canvasDynamicTimeDilation.loadFont(BebasNeueRegular35);
  canvasDynamicTimeDilation.setTextColor(canvasDynamicTimeDilation.color888(35, 242, 240), canvasDynamicTimeDilation.color888(35, 34, 68));
  canvasDynamicTimeDilation.drawFloat(time_dilation, 6, 15, 14);
  canvasDynamicTimeDilation.loadFont(RobotoBoldCondensed12);

  if (time_dilation < 0) {
    canvasDynamicTimeDilation.setTextColor(canvasDynamicTimeDilation.color888(127, 255, 27), canvasDynamicTimeDilation.color888(35, 34, 68));
    canvasDynamicTimeDilation.drawString("TIME SLOWER", 39, 50);
  } else if (time_dilation > 0) {
    canvasDynamicTimeDilation.setTextColor(canvasDynamicTimeDilation.color888(239, 196, 16), canvasDynamicTimeDilation.color888(35, 34, 68));
    canvasDynamicTimeDilation.drawString("TIME FASTER", 40, 50);
  }

  canvasDynamicTimeDilation.pushSprite(5, 165, TFT_TRANSPARENT);
}

// ---- Create dynamic canvases with the same size as their static counterparts ----
// Call once during setup to allocate all dynamic sprites.
inline void createDynamicCanvases() {
  canvasDynamicHeader.setColorDepth(32);
  canvasDynamicHeader.createSprite(180, 25);

  canvasDynamicLineChart.setColorDepth(32);
  canvasDynamicLineChart.createSprite(160, 40);

  canvasDynamicVelocity.setColorDepth(32);
  canvasDynamicVelocity.createSprite(310, 30);

  canvasDynamicTotalVelocity.setColorDepth(32);
  canvasDynamicTotalVelocity.createSprite(310, 30);

  canvasDynamicLocalGravity.setColorDepth(32);
  canvasDynamicLocalGravity.createSprite(200, 45);

  canvasDynamicTimeDilation.setColorDepth(32);
  canvasDynamicTimeDilation.createSprite(145, 70);

  canvasDynamicAltitude.setColorDepth(32);
  canvasDynamicAltitude.createSprite(90, 90);

  canvasDynamicLatitude.setColorDepth(32);
  canvasDynamicLatitude.createSprite(120, 120);
}
