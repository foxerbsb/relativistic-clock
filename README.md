# Relativistic Clock

**See time bend in your hand:** A 25 Hz GPS-based relativistic clock revealing Einstein’s theory live.

---

## Overview

This project presents a **portable Relativistic Clock**, built on the **M5Stack CoreS3** and equipped with the **M5Stack GNSS Unit**, which integrates:

- **u-blox NEO-M9N** – 25 Hz multi-constellation GNSS receiver  
- **BMP280** – Barometer and thermometer  
- **BMI270** – 6-axis accelerometer/gyroscope  
- **BMM150** – Magnetometer  

The system calculates **real-time time dilation** according to the principles of **General Relativity** and **Special Relativity**, comparing the measured time against a theoretical clock at the same latitude and longitude on the **WGS84 ellipsoid**.

When available, the GNSS module’s **HAE (Height Above Ellipsoid)** provides precise altitude directly. If using the BMP280, altitude is derived from atmospheric pressure (still referenced to WGS84, but without geoid separation, slightly reducing accuracy).

The goal is to show **how time is passing for you**—whether faster or slower—compared to other places on Earth.  
It is inspired by the **1971 Hafele–Keating experiment**, where atomic clocks aboard aircraft measured relativistic effects.  
After extensive research, this appears to be the **first portable device** of its kind, suitable for educational experiments and live demonstrations.

If you’ve seen the *How the Universe Works* series, there’s an episode mentioning that time runs differently for people living one floor apart.  
With this clock, you can observe that effect **practically and in real time**.

---

## Highlights from Simulations

Based on AI-assisted simulations using this project’s code, we identified interesting theoretical points:

- The **fastest-rotating location** on the planet due to Earth’s spin  
- The **lowest gravity point** on Earth  
- The **city where time runs slowest** and **city where it runs fastest**  
- Estimates of people who have “lost” the most time due to relativistic effects  

⚠ These are **theoretical estimates**. Accurate time dilation measurements require atomic clocks and high-precision geophysical data such as Bouguer gravity maps.

---

## Hardware

- **M5Stack CoreS3** — ESP32-S3 dual-core MCU, touchscreen display, Wi-Fi & Bluetooth
- **GNSS Unit M5Stack** — via M-Bus, including:  
  - **u-blox NEO-M9N** – GPS, Galileo, GLONASS, BeiDou, up to 25 Hz update rate, EGM96 geoid model built-in  
  - **BMP280** – Barometric altitude & temperature  
  - **BMI270** – Motion & tilt sensing  
  - **BMM150** – Azimuth/direction  
  - Active GNSS antenna

---

## Software & Libraries

- [M5Unified](https://github.com/m5stack/M5Unified) — Unified M5Stack hardware API  
- [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) — GNSS NMEA decoding  
- [Adafruit BMP280](https://github.com/adafruit/Adafruit_BMP280_Library) — Barometric altitude calculation  
- **Wire (I²C)** — Sensor communication  
- [M5GFX](https://github.com/m5stack/M5GFX) / [LovyanGFX](https://github.com/lovyan03/LovyanGFX) — Graphics rendering & UI

---

## Technical Specs

### Processing & Interface
- Platform: M5Stack CoreS3 (ESP32-S3, Xtensa LX7 dual-core @ 240 MHz, 512 KB SRAM, 16 MB Flash)

### GNSS (u-blox NEO-M9N)
- Multi-constellation: GPS, Galileo, GLONASS, BeiDou  
- Update rate: up to 25 Hz  
- Horizontal accuracy: ±1.5 m (CEP)  
- Altitude accuracy: ±1.5 m (CEP) with corrections  
- Reference model: WGS84 ellipsoid, EGM96 geoid separation

### Altitude Modes
1. **BMP280 mode** — altitude from barometer, ignores geoid separation (slightly less precise)  
2. **HAE mode** — altitude from GNSS HAE, adjusted with geoid separation for best accuracy

---

## Time Dilation Calculation

Based on:
- **Special Relativity** — Velocity slows time  
- **General Relativity** — Higher altitude (lower gravity) speeds time

Reference: A theoretical clock at rest on the WGS84 ellipsoid, at the same lat/long.

Inputs:
- Velocity & heading (GNSS)  
- Latitude & altitude (GNSS or barometer)  
- Local rotational velocity (latitude + altitude)  
- Local gravity (Somigliana formula + free-air correction)

Output:
- Estimated time difference in **nanoseconds per hour (ns/h)**

---

## Example Effects

- **Eastward travel** adds to Earth’s rotation, increasing relativistic delay  
- **Westward travel** subtracts from rotation, possibly reversing the effect  
- **North/South movement** always increases total space velocity (perpendicular to rotation)  
- **Altitude gain** increases clock rate via reduced gravitational potential

---

## Gravity Calculation

- Uses Somigliana formula for latitude variation  
- Free-air correction for altitude  
- Mode 1: BMP280 altitude + WGS84  
- Mode 2: HAE altitude + WGS84 (more precise)

---

## Interesting Facts from This Project

- **Slowest time**: Pontianak, Indonesia (near Equator)  
- **Fastest time**: Longyearbyen, Svalbard — also home to the [Svalbard Global Seed Vault](https://seedvault.no/) with 1.3M+ seed varieties stored at −18 °C in permafrost tunnels  
- **Fastest tourist spot**: Praia do Café, Ilhéu das Rolas, São Tomé and Príncipe  
- **Lowest gravity**: Summit of Mount Chimborazo (Ecuador) — ~2.1 km farther from Earth’s center than Everest’s peak, and spins fastest on land due to Equator proximity  
- **Fastest rotation point on land**: Also Mount Chimborazo

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---
