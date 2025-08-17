#pragma once
/*
  tinygps_hae_utils.h  —  Header-only helper for TinyGPS++
  --------------------------------------------------------
  - Captures Geoid Separation (N, meters) from GGA field 11 via TinyGPSCustom
  - Works with both "GPGGA" and "GNGGA"
  - Provides HAE (Height Above Ellipsoid):  h = H (MSL) + N
  - Keeps a cached N with timestamp and simple freshness checks

  Usage:
    #include <TinyGPSPlus.h>
    #include "tinygps_hae_utils.h"

    TinyGPSPlus gps;
    TinyGPSHaeHelper hae(gps);   // must be constructed after 'gps' exists

    // in loop(), after feeding gps.encode():
    hae.update();                // updates N cache if new values arrived

    double h_m = hae.getHAE_m(5000); // meters, NAN if invalid/stale (>5s)
    if (isfinite(h_m)) { ... }

  Notes:
   - Requires GGA sentences enabled.
   - N (geoid separation) is positive when the geoid is above the ellipsoid.
   - Internally caches N in meters (TinyGPS++ altitude is provided in meters already).
*/

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <math.h>
#include <stdlib.h>

class TinyGPSHaeHelper {
public:
  explicit TinyGPSHaeHelper(TinyGPSPlus& gpsRef)
  : gps(gpsRef),
    n_gpgga(gpsRef, "GPGGA", 11),
    n_gngga(gpsRef, "GNGGA", 11),
    lastN_m(NAN),
    lastN_ms(0)
  {}

  // Call this frequently (e.g., once per loop after gps.encode())
  inline void update() {
    double n_m;
    // Prefer whichever talker produced an update this iteration
    if (n_gpgga.isUpdated() && parseDoubleSafe(n_gpgga.value(), n_m)) {
      lastN_m  = n_m;
      lastN_ms = millis();
    } else if (n_gngga.isUpdated() && parseDoubleSafe(n_gngga.value(), n_m)) {
      lastN_m  = n_m;
      lastN_ms = millis();
    }
    // If neither updated, keep previous cache until maxAge check in getters
  }

  // Returns HAE (meters). NAN if:
  //  - MSL altitude is invalid, or
  //  - no valid N ever seen, or
  //  - cached N is older than maxAgeMs
  inline double getHAE_m(uint32_t maxAgeMs = 5000) const {
    if (!gps.altitude.isValid()) return NAN;               // need valid MSL (H)
    if (!isfinite(lastN_m))        return NAN;             // never saw valid N
    if (millis() - lastN_ms > maxAgeMs) return NAN;        // N too old
    return gps.altitude.meters() + lastN_m;                // h = H + N
  }

  inline double getHAE_ft(uint32_t maxAgeMs = 5000) const {
    const double h_m = getHAE_m(maxAgeMs);
    return isfinite(h_m) ? (h_m * 3.280839895) : NAN;
  }

  // Accessors for geoid separation cache (N)
  inline bool   hasRecentN(uint32_t maxAgeMs = 5000) const {
    return isfinite(lastN_m) && (millis() - lastN_ms <= maxAgeMs);
  }
  inline double getN_m(uint32_t maxAgeMs = 5000) const {
    return hasRecentN(maxAgeMs) ? lastN_m : NAN;
  }
  inline uint32_t ageN_ms() const {
    return isfinite(lastN_m) ? (millis() - lastN_ms) : UINT32_MAX;
  }

private:
  TinyGPSPlus& gps;
  TinyGPSCustom n_gpgga;   // field 11 (meters)
  TinyGPSCustom n_gngga;   // field 11 (meters)
  double   lastN_m;
  uint32_t lastN_ms;

  static inline bool parseDoubleSafe(const char* s, double& out) {
    if (!s || !*s) return false;
    char* endp = nullptr;
    double v = strtod(s, &endp);
    if (endp == s) return false;
    out = v;
    return true;
  }
};
