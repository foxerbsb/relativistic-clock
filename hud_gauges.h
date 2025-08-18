#pragma once
#include <M5Unified.h>
#include <vector>

// ========= Simple Battery Status =========
// 0–10%: red | 10–20%: yellow | >20%: dark green
// No charging check, no voltage, no blinking.
inline void drawBatteryStatus(M5Canvas &canvas, int x, int y, float scale = 1.0f,
                              bool showPercent = true, int batteryLevel = 0) {
  int percent = batteryLevel;
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  // Dimensions
  const int body_w = int(34 * scale);
  const int body_h = int(16 * scale);
  const int tip_w = int(3 * scale);
  const int tip_h = int(8 * scale);
  const int pad = int(2 * scale);

  // Colors
  const uint16_t col_frame = canvas.color565(80, 80, 80);
  const uint16_t col_bg = canvas.color565(0, 0, 0);
  const uint16_t col_txt = canvas.color565(255, 255, 255);
  const uint16_t col_red = canvas.color565(220, 40, 40);   // 0–10%
  const uint16_t col_yel = canvas.color565(255, 220, 40);  // 10–20%
  const uint16_t col_green = canvas.color565(0, 248, 81);  // >20%

  // Frame
  int bx = x, by = y;
  canvas.drawRect(bx, by, body_w, body_h, col_frame);
  int tx = bx + body_w;
  int ty = by + (body_h - tip_h) / 2;
  canvas.fillRect(tx, ty, tip_w, tip_h, col_frame);

  // Inner area
  int in_x = bx + 1, in_y = by + 1;
  int in_w = body_w - 2, in_h = body_h - 2;

  // Fill color by band
  uint16_t fill_col = col_green;
  if (percent <= 10) fill_col = col_red;
  else if (percent <= 20) fill_col = col_yel;

  // Fill width
  int filled_w = int(in_w * (percent / 100.0f) + 0.5f);
  if (filled_w > 0) canvas.fillRect(in_x, in_y, filled_w, in_h, fill_col);

  // Text: percent
  if (showPercent) {
    int text_x = x + body_w + tip_w + pad;
    int text_y = y + body_h / 2;
    canvas.setTextDatum(ML_DATUM);
    canvas.setTextColor(col_txt, col_bg);
    canvas.loadFont(RobotoBoldCondensed12);
    canvas.setTextSize(scale < 1.2f ? 1 : (scale < 2.0f ? 2 : 3));
    char line1[16];
    snprintf(line1, sizeof(line1), "%3d%%", percent);
    canvas.drawString(line1, text_x, text_y);
  }
}


// Arc gauge for altitude with green gradient (dark -> light).
// Fills a number of segments proportional to altitude.
// If altitude < 0, filled segments turn red (alert).
void drawAltitudeGauge(M5Canvas &canvas, int cx, int cy,
                       int outer_radius, int inner_radius,
                       int segments, int start_angle_deg, int total_angle_deg,
                       int gap_deg, float altitude) {
  // Gradient: dark green (min) -> light green (max)
  int r_start = 0, g_start = 60, b_start = 0;
  int r_end = 100, g_end = 255, b_end = 100;

  // Scale limits
  float alt_min = -500.0f;
  float alt_max = 9000.0f;

  // How many segments to fill (clamp 0..1)
  float p = (altitude - alt_min) / (alt_max - alt_min);
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  int filled_segments = int(segments * p + 0.5f);

  float angle_per_segment = float(total_angle_deg) / segments;
  bool red_alert = (altitude < 0);

  for (int i = 0; i < segments; i++) {
    // Linear RGB gradient along segments
    float t = (segments > 1) ? float(i) / float(segments - 1) : 1.0f;
    int r = int(r_start + (r_end - r_start) * t);
    int g = int(g_start + (g_end - g_start) * t);
    int b = int(b_start + (b_end - b_start) * t);
    uint16_t seg_color = red_alert ? TFT_RED : canvas.color565(r, g, b);

    int seg_start = int(start_angle_deg + i * angle_per_segment);
    int seg_end = int(seg_start + angle_per_segment - gap_deg);
    if (seg_end < seg_start) seg_end = seg_start;

    if (i < filled_segments) {
      // Filled segments: green gradient OR red if alert
      canvas.fillArc(cx, cy, outer_radius, inner_radius, seg_start, seg_end, seg_color);
    } else {
      // Unfilled segments: dark background
      canvas.fillArc(cx, cy, outer_radius, inner_radius, seg_start, seg_end,
                     canvas.color565(30, 40, 30));
    }
  }
}

static inline int norm360(int a) {
  a %= 360;
  if (a < 0) a += 360;
  return a;
}
static inline float toCanvasRad(int deg) {
  return (deg - 90) * DEG_TO_RAD;
}

static void drawFixedTriangle(M5Canvas &canvas,
                              int cx, int cy, int ring_outer_r,
                              int tip_len_px, int base_half_deg,
                              int direction_deg, uint16_t color) {
  float ang_c = toCanvasRad(direction_deg);
  float ang_l = toCanvasRad(norm360(direction_deg - base_half_deg));
  float ang_r = toCanvasRad(norm360(direction_deg + base_half_deg));

  int tip_x = cx + (int)lround(cosf(ang_c) * (ring_outer_r + tip_len_px));
  int tip_y = cy + (int)lround(sinf(ang_c) * (ring_outer_r + tip_len_px));

  int base_r = ring_outer_r - 1;
  int bl_x = cx + (int)lround(cosf(ang_l) * base_r);
  int bl_y = cy + (int)lround(sinf(ang_l) * base_r);
  int br_x = cx + (int)lround(cosf(ang_r) * base_r);
  int br_y = cy + (int)lround(sinf(ang_r) * base_r);

  canvas.fillTriangle(tip_x, tip_y, bl_x, bl_y, br_x, br_y, color);
}

/**
 * 360° ring + 360° TRACK (same thickness as arc) + small rotating arc (underneath)
 * + 4 fixed triangles (on top).
 *
 * If azimuth_deg == -1 (or < 0), the ARC is hidden (track and triangles remain).
 */
void drawAzimuthRingWithArcUnderTriangles(M5Canvas &canvas,
                                          int cx, int cy,
                                          int gauge_outer_r,
                                          float azimuth_deg,
                                          int ring_offset_px = 7,
                                          int ring_thickness = 4,
                                          int marker_arc_deg = 12,
                                          uint16_t color_ring = 0,
                                          uint16_t color_track = 0,
                                          uint16_t color_arc = 0,
                                          int tri_len_px = 2,
                                          int base_half_deg = 7,
                                          int arc_inset_outer_px = 1,
                                          int arc_inset_inner_px = 1,
                                          bool draw_track = true) {
  if (color_ring == 0) color_ring = canvas.color565(24, 32, 28);    // base
  if (color_track == 0) color_track = canvas.color565(36, 46, 42);  // subtle track
  if (color_arc == 0) color_arc = canvas.color565(230, 240, 255);   // bright marker

  // Cardinal colors
  uint16_t color_N = canvas.color565(255, 0, 0);
  uint16_t color_E = canvas.color565(255, 255, 0);
  uint16_t color_S = canvas.color565(0, 120, 255);
  uint16_t color_W = canvas.color565(0, 255, 0);

  const int ring_outer_r = gauge_outer_r + ring_offset_px;
  const int ring_inner_r = ring_outer_r - ring_thickness;

  // 1) Base ring 360°
  canvas.fillArc(cx, cy, ring_outer_r, ring_inner_r, 0, 360, color_ring);

  // Track/arc geometry (same thickness)
  int track_outer = ring_outer_r - arc_inset_outer_px;
  int track_inner = ring_inner_r + arc_inset_inner_px;
  if (track_outer < track_inner) {
    int mid = (track_outer + track_inner) / 2;
    track_outer = mid + 1;
    track_inner = mid - 1;
    if (track_inner < 0) track_inner = 0;
  }

  // 2) 360° TRACK (optional)
  if (draw_track) {
    canvas.fillArc(cx, cy, track_outer, track_inner, 0, 360, color_track);
  }

  // 3) Small rotating arc (underneath) — hidden if azimuth == -1
  bool hide_arc = (azimuth_deg < 0.0f);  // sentinel: -1 (or any negative)
  if (!hide_arc) {
    if (marker_arc_deg < 1) marker_arc_deg = 1;  // safety

    const int angle_offset_arc = 270;  // 0° (N) at top
    int center = norm360((int)lround(azimuth_deg) + angle_offset_arc);
    int half = marker_arc_deg / 2;
    int start = norm360(center - half);
    int end = norm360(center + half);

    if (start <= end) {
      canvas.fillArc(cx, cy, track_outer, track_inner, start, end, color_arc);
    } else {
      canvas.fillArc(cx, cy, track_outer, track_inner, start, 360, color_arc);
      canvas.fillArc(cx, cy, track_outer, track_inner, 0, end, color_arc);
    }
  }

  // 4) Fixed triangles on top
  drawFixedTriangle(canvas, cx, cy, ring_outer_r, tri_len_px, base_half_deg, 0, color_N);
  drawFixedTriangle(canvas, cx, cy, ring_outer_r, tri_len_px, base_half_deg, 90, color_E);
  drawFixedTriangle(canvas, cx, cy, ring_outer_r, tri_len_px, base_half_deg, 180, color_S);
  drawFixedTriangle(canvas, cx, cy, ring_outer_r, tri_len_px, base_half_deg, 270, color_W);
}


// Arc gauge for latitude with a fixed orange pointer.
// Background is dark; draws a small white mark at latitude 0.
void drawLatitudeGauge(M5Canvas &canvas, int cx, int cy,
                       int outer_radius, int inner_radius,
                       int segments, int start_angle_deg, int total_angle_deg,
                       int gap_deg, float latitude) {
  // Clamp latitude to [-90, +90]
  if (latitude > 90.0f) latitude = 90.0f;
  if (latitude < -90.0f) latitude = -90.0f;

  // Colors
  uint16_t col_bg = canvas.color565(30, 40, 30);  // dark background for empty segments
  uint16_t col_arrow = TFT_ORANGE;                // orange pointer

  // Draw all segments as background
  float angle_per_segment = float(total_angle_deg) / segments;
  for (int i = 0; i < segments; i++) {
    int seg_start = int(start_angle_deg + i * angle_per_segment);
    int seg_end = int(seg_start + angle_per_segment - gap_deg);
    if (seg_end < seg_start) seg_end = seg_start;
    canvas.fillArc(cx, cy, outer_radius, inner_radius, seg_start, seg_end, col_bg);
  }

  // --- Mark latitude 0 line ---
  float lat0_angle = start_angle_deg + total_angle_deg / 2.0f;
  int lat0_start = int(lat0_angle - 2);
  int lat0_end = int(lat0_angle + 2);
  canvas.fillArc(cx, cy, outer_radius, inner_radius, lat0_start, lat0_end, TFT_WHITE);

  // --- Orange pointer for current latitude ---
  float t = (latitude + 90.0f) / 180.0f;  // -90..+90 -> 0..1
  float angle = start_angle_deg + t * total_angle_deg;

  // Keep the pointer within the arc range
  int arc_start = start_angle_deg;
  int arc_end = start_angle_deg + total_angle_deg;
  int ptr_start = int(angle - angle_per_segment / 2.0f);
  int ptr_end = int(angle + angle_per_segment / 2.0f - gap_deg);
  if (ptr_start < arc_start) ptr_start = arc_start;
  if (ptr_end > arc_end) ptr_end = arc_end;

  canvas.fillArc(cx, cy, outer_radius, inner_radius, ptr_start, ptr_end, col_arrow);
}


// Horizontal speed bar with true gradient fill.
// Fills from left (min) to right (max).
void drawSpeedBarGauge(M5Canvas &canvas, int x, int y, int width, int height,
                       float speed, float speed_min, float speed_max,
                       int r_start, int g_start, int b_start,  // gradient start color
                       int r_end, int g_end, int b_end)        // gradient end color
{
  uint16_t col_bg = canvas.color565(30, 40, 30);

  // Clamp value
  if (speed < speed_min) speed = speed_min;
  if (speed > speed_max) speed = speed_max;

  // Filled length (0..1)
  float p = (speed - speed_min) / (speed_max - speed_min);
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  int filled_w = int(width * p + 0.5f);

  // Background
  canvas.fillRect(x, y, width, height, col_bg);

  // Gradient fill (left -> right)
  int denom = (width > 1) ? (width - 1) : 1;
  for (int i = 0; i < filled_w; i++) {
    float t = float(i) / float(denom);
    int r = int(r_start + (r_end - r_start) * t);
    int g = int(g_start + (g_end - g_start) * t);
    int b = int(b_start + (b_end - b_start) * t);
    uint16_t col = canvas.color565(r, g, b);
    canvas.drawFastVLine(x + i, y, height, col);
  }

  // Optional border
  // canvas.drawRect(x, y, width, height, canvas.color565(80, 80, 80));
}

// Horizontal bar with gradient and optional scale (min, mid, max).
// - Fills from left (min) to right (max)
// - Optional ticks and labels below the bar
void drawHorizontalBarGauge(
  M5Canvas &canvas,
  int x, int y, int width, int height,
  float value, float val_min, float val_max,
  int r_start, int g_start, int b_start,  // gradient start color
  int r_end, int g_end, int b_end,        // gradient end color
  bool draw_ticks = true, int ticks = 5,
  bool draw_border = false,
  // --- labels:
  bool draw_labels = true,
  int decimals = 3,
  const char *unit = " m/s^2",
  int text_margin = 4,
  int text_size = 1) {

  // Colors
  uint16_t col_bg = canvas.color565(30, 40, 30);
  uint16_t col_tick = canvas.color565(70, 80, 70);
  uint16_t col_marker = canvas.color565(230, 230, 230);
  uint16_t col_text = canvas.color565(0, 0, 0);

  // Sanity checks
  if (val_max == val_min) val_max = val_min + 1e-6f;
  if (width < 1) width = 1;
  if (height < 1) height = 1;

  // Clamp value
  if (value < val_min) value = val_min;
  if (value > val_max) value = val_max;

  // Filled proportion
  float p = (value - val_min) / (val_max - val_min);
  if (p < 0) p = 0;
  if (p > 1) p = 1;
  int filled_w = int(width * p + 0.5f);

  // Background
  canvas.fillRect(x, y, width, height, col_bg);

  // Gradient fill (left -> right)
  int denom = (width > 1) ? (width - 1) : 1;
  for (int i = 0; i < filled_w; i++) {
    float t = float(i) / float(denom);
    int r = int(r_start + (r_end - r_start) * t);
    int g = int(g_start + (g_end - g_start) * t);
    int b = int(b_start + (b_end - b_start) * t);
    uint16_t col = canvas.color565(r, g, b);
    canvas.drawFastVLine(x + i, y, height, col);
  }

  // Ticks (top/bottom)
  if (draw_ticks && ticks > 0) {
    for (int k = 0; k <= ticks; k++) {
      float frac = float(k) / float(ticks);
      int x_tick = x + int(frac * (width - 1) + 0.5f);
      int tick_len = (height >= 10) ? (height / 3) : 2;
      canvas.drawFastVLine(x_tick, y, tick_len, col_tick);                      // top
      canvas.drawFastVLine(x_tick, y + height - tick_len, tick_len, col_tick);  // bottom
    }
  }

  // Current value marker (thin vertical line at the end of the fill)
  if (filled_w > 0) {
    int x_marker = x + (filled_w - 1);
    canvas.drawFastVLine(x_marker, y, height, col_marker);
  }

  // Optional border
  if (draw_border) {
    canvas.drawRect(x, y, width, height, canvas.color565(60, 60, 60));
  }

  // Value labels (min, mid, max) below the bar
  if (draw_labels) {
    int y_text = y + height + text_margin;
    float v_mid = (val_min + val_max) * 0.5f;

    auto fmt = [&](float v) {
      char buf[24];
      dtostrf(v, 0, decimals, buf);
      int len = strlen(buf);
      strlcpy(buf + len, unit, sizeof(buf) - len);
      return String(buf);
    };

    String s_min = fmt(val_min);
    String s_mid = fmt(v_mid);
    String s_max = fmt(val_max);

    // canvas.setTextSize(text_size);
    canvas.loadFont(RobotoBold9);
    canvas.setTextColor(col_text, canvas.color565(253, 47, 43));  // keep logic as-is
    canvas.setTextDatum(TC_DATUM);                                // Top-Center

    // min (left)
    canvas.drawString(s_min, x, y_text);
    // mid (center)
    canvas.drawString(s_mid, x + width / 2, y_text);
    // max (right)
    canvas.drawString(s_max, x + width, y_text);

    canvas.setTextDatum(TL_DATUM);  // restore default
  }
}


// === HDOP -> bars / level (unchanged logic) ===
inline int gpsSignalBarsFromHDOP(float hdop) {
  if (!(hdop > 0)) return 0;
  if (hdop <= 0.9f) return 5;
  if (hdop <= 1.5f) return 4;
  if (hdop <= 3.0f) return 3;
  if (hdop <= 6.0f) return 2;
  if (hdop <= 10.0f) return 1;
  return 0;
}

inline float gpsSignalLevelFromHDOP(float hdop) {
  if (!(hdop > 0)) return 0.0f;
  struct Cut {
    float hdop, lvl;
  } cuts[] = {
    { 10.0f, 1.0f }, { 6.0f, 2.0f }, { 3.0f, 3.0f }, { 1.5f, 4.0f }, { 0.8f, 5.0f }
  };
  if (hdop <= cuts[4].hdop) return 5.0f;
  float prev_hd = 1e9f, prev_lvl = 0.0f;
  for (int i = 0; i < 5; ++i) {
    if (hdop <= cuts[i].hdop) {
      float t = (prev_hd - hdop) / (prev_hd - cuts[i].hdop);
      if (t < 0) t = 0;
      if (t > 1) t = 1;
      return prev_lvl + t * (cuts[i].lvl - prev_lvl);
    }
    prev_hd = cuts[i].hdop;
    prev_lvl = cuts[i].lvl;
  }
  return 0.0f;
}

inline uint16_t gpsColorForBars(M5Canvas &c, int bars) {
  switch (bars) {
    case 5: return c.color565(40, 170, 80);     // strong green
    case 4: return c.color565(90, 200, 80);     // green
    case 3: return c.color565(255, 220, 40);    // yellow
    case 2: return c.color565(245, 170, 40);    // orange
    case 1: return c.color565(220, 60, 60);     // red
    default: return c.color565(120, 120, 120);  // gray
  }
}

// === 5-Bar GPS Gauge — Single Color (Flat) ===
// 'bars' = 0..5. All filled bars use the same color for the current level.
inline void drawGpsSignalGauge5SameColor(M5Canvas &c, int x, int y, int h,
                                         int bar_w, int gap, int bars) {
  if (h < 8) h = 8;
  if (bar_w < 3) bar_w = 3;
  if (gap < 1) gap = 1;
  if (bars < 0) bars = 0;
  if (bars > 5) bars = 5;

  const uint16_t base_dark = c.color565(30, 40, 30);  // background for empty bars
  const float frac[5] = { 0.52f, 0.66f, 0.78f, 0.90f, 1.00f };

  uint16_t fill_col = gpsColorForBars(c, bars);

  int cx = x;
  for (int i = 0; i < 5; ++i) {
    int bh = int(h * frac[i] + 0.5f);
    int by = y + (h - bh);

    // empty base (flat)
    c.fillRect(cx, by, bar_w, bh, base_dark);

    // filled?
    if (i < bars) c.fillRect(cx, by, bar_w, bh, fill_col);

    cx += bar_w + gap;
  }
}

// === 5-Bar GPS Gauge — Smooth/Partial (Flat) ===
// 'level' in [0..5]. Fully fills floor(level) and partially fills the next bar.
inline void drawGpsSignalGauge5Smooth(M5Canvas &c, int x, int y, int h,
                                      int bar_w, int gap, float level) {
  if (h < 8) h = 8;
  if (bar_w < 3) bar_w = 3;
  if (gap < 1) gap = 1;
  if (level < 0) level = 0;
  if (level > 5) level = 5;

  const uint16_t base_dark = c.color565(30, 40, 30);
  const float frac[5] = { 0.52f, 0.66f, 0.78f, 0.90f, 1.00f };

  int full = int(level);
  float part = level - full;
  if (full > 5) {
    full = 5;
    part = 0;
  }

  // Color based on the target level (ceil for a slightly optimistic transition)
  int bars_for_color = int(ceilf(level));
  if (bars_for_color < 0) bars_for_color = 0;
  if (bars_for_color > 5) bars_for_color = 5;
  uint16_t fill_col = gpsColorForBars(c, bars_for_color);

  int cx = x;
  for (int i = 0; i < 5; ++i) {
    int bh = int(h * frac[i] + 0.5f);
    int by = y + (h - bh);

    // empty base
    c.fillRect(cx, by, bar_w, bh, base_dark);

    // full bars
    if (i < full) {
      c.fillRect(cx, by, bar_w, bh, fill_col);
    }
    // partial bar (fill from bottom to top)
    else if (i == full && part > 0.01f) {
      int ph = int(bh * part + 0.5f);
      int pby = by + (bh - ph);
      c.fillRect(cx, pby, bar_w, ph, fill_col);
    }

    cx += bar_w + gap;
  }
}


// Sliding ECG with horizontal grid per UNIT and dense vertical grid (parametrizable)
void drawTimeDilationChart(M5Canvas &canvas, double dilation_value,
                           double minVal = -6.0, double maxVal = 3.0,
                           uint16_t colBg = 0, uint16_t colLine = 0,
                           uint16_t colZero = 0, uint16_t colGrid = 0,
                           int dashLen = 4, int gapLen = 3,
                           int vStepPx = 6, int vMajorEvery = 4, uint16_t colGridMajor = 0) {
  // Default colors
  if (colBg == 0) colBg = 0;
  if (colLine == 0) colLine = canvas.color565(80, 255, 80);           // waveform
  if (colZero == 0) colZero = canvas.color565(220, 170, 40);          // zero (amber)
  if (colGrid == 0) colGrid = canvas.color565(18, 36, 18);            // fine grid
  if (colGridMajor == 0) colGridMajor = canvas.color565(32, 64, 32);  // “major” grid

  const int W = canvas.width();
  const int H = canvas.height();
  if (W < 3 || H < 3) return;

  auto mapf_clamp = [](double v, double inMin, double inMax, int outMin, int outMax) {
    if (v < inMin) v = inMin;
    if (v > inMax) v = inMax;
    double p = (v - inMin) / (inMax - inMin);
    return int(outMin + p * (outMax - outMin) + 0.5);
  };

  // State: circular buffer
  struct State {
    bool prim = true;
    int head = -1;
    int lastW = 0, lastH = 0;
    std::vector<int> ybuf;
  };
  static State st;

  if (st.prim || st.lastW != W || st.lastH != H) {
    st.ybuf.assign(W, mapf_clamp(dilation_value, minVal, maxVal, H - 1, 0));
    st.head = W - 1;
    st.lastW = W;
    st.lastH = H;
    st.prim = false;
  } else {
    st.head = (st.head + 1) % W;
    st.ybuf[st.head] = mapf_clamp(dilation_value, minVal, maxVal, H - 1, 0);
  }

  // Background
  canvas.fillScreen(colBg);

  // DENSE VERTICAL GRID (major lines every vMajorEvery * vStepPx)
  if (vStepPx < 1) vStepPx = 1;
  for (int x = 0; x < W; x += vStepPx) {
    bool isMajor = (vMajorEvery > 0) && (x % (vStepPx * vMajorEvery) == 0);
    canvas.drawFastVLine(x, 0, H, isMajor ? colGridMajor : colGrid);
  }

  // HORIZONTAL GRID per unit (except zero)
  int uStart = (int)ceil(minVal);
  int uEnd = (int)floor(maxVal);
  for (int u = uStart; u <= uEnd; ++u) {
    if (u == 0) continue;
    int yU = mapf_clamp((double)u, minVal, maxVal, H - 1, 0);
    canvas.drawFastHLine(0, yU, W, colGrid);
  }

  // Zero as dashed line (if inside range)
  if (minVal <= 0.0 && 0.0 <= maxVal) {
    int yZero = mapf_clamp(0.0, minVal, maxVal, H - 1, 0);
    for (int x = 0; x < W; x += (dashLen + gapLen)) {
      int len = dashLen;
      if (x + len > W) len = W - x;
      if (len > 0) canvas.drawFastHLine(x, yZero, len, colZero);
    }
  }

  // Sliding waveform: newest at the right
  auto idx_from_x = [&](int x) -> int {
    int k = st.head - (W - 1 - x);
    k %= W;
    if (k < 0) k += W;
    return k;
  };
  int x0 = 0, y0 = st.ybuf[idx_from_x(0)];
  for (int x = 1; x < W; ++x) {
    int y1 = st.ybuf[idx_from_x(x)];
    canvas.drawLine(x0, y0, x, y1, colLine);
    x0 = x;
    y0 = y1;
  }
}
