# Relativistic Clock

**See time bend in your hand:** 25 Hz GPS relativistic clock revealing Einstein’s theory live.

---

## General Description

This project presents a **portable Relativistic Clock**, based on the **M5Stack CoreS3** and equipped with the **M5Stack GNSS module**, which integrates the NEO-M9N (25 Hz GPS), BMP280 (barometer), BMI270 (IMU), and BMM150 (magnetometer) sensors.

The system calculates, in real time, **time dilation** according to the principles of **General Relativity** and **Special Relativity**, comparing the time measured by the device with that of a theoretical clock located at the same latitude and longitude on the **WGS84 ellipsoid**.

Measurements are based on the WGS84 ellipsoid as the main reference. When the GNSS module provides the **HAE (Height Above Ellipsoid)** value, the altitude is obtained directly and accurately; when the BMP280 barometer is used, the altitude is calculated from atmospheric pressure, still referenced to the ellipsoid but without considering geoid separation, which slightly reduces accuracy.

The goal is to show how time is passing for you — whether faster or slower — compared to other locations on Earth. The device is inspired by the experiment conducted in 1971 by physicists Joseph Hafele and Richard Keating, who used atomic clocks aboard aircraft to measure time dilation. After extensive research, this is apparently the first portable device of its kind, capable of being used in various educational experiments and intriguing demonstrations.

For those who watch the series *How the Universe Works*, there is an episode in which a presenter comments that time passes differently for a person who lives one floor above in the same building. With this clock, it is possible to observe this difference practically and in real time.

During the development of the project, I carried out several AI-based simulations, using my code, which led to interesting discoveries, such as identifying:

- The place on the planet with the highest speed due to Earth’s rotation.  
- The point of lowest gravity.  
- The city where time passes the slowest and the city where it passes the fastest.  
- Estimates of which people on Earth have “lagged” the most in time due to relativistic effects.  

It is worth noting that these results are theoretical estimates, based on the conditions and parameters defined in the project. Accurate determination of time dilation requires atomic clocks or high-precision equipment, as extremely precise measurements would depend on data such as Bouguer maps (which show local gravity variations measured by gravimeters), in addition to other geophysical factors.

---

## 2. Components Used

**Hardware**

- **M5Stack CoreS3** – ESP32-S3 dual-core microcontroller, touchscreen display, integrated Wi-Fi and Bluetooth.  
- **M5Stack GNSS Unit** – Module connected via M-Bus, containing:  
  - **u-blox NEO-M9N** – Multi-constellation GNSS receiver (GPS, Galileo, GLONASS, BeiDou), with an update rate of up to 25 Hz and the EGM96 geoid model integrated in the firmware, allowing altitudes relative to mean sea level to be obtained directly, without the need for external calculations.  
  - **BMP280** – Barometer and thermometer for altitude calculation and atmospheric compensation.  
  - **BMI270** – 6-axis accelerometer and gyroscope for motion and tilt detection.  
  - **BMM150** – Magnetometer for direction/azimuth calculation.  
  - Active GNSS antenna.  

**Software / Libraries**

- **M5Unified** – Official library for unified control of M5Stack hardware.  
- **TinyGPS++** – Decoding of NMEA sentences and extraction of GNSS data.  
- **Adafruit BMP280** – Barometric altitude calculation.  
- **Wire (I²C)** – Communication with integrated sensors.  
- **M5GFX / LovyanGFX** – Graphic rendering and user interface on the CoreS3 display.  

---

## 3. Technical Specifications

**Processing and Interface**

- Platform: M5Stack CoreS3 (ESP32-S3, dual-core Xtensa LX7, 240 MHz, 512 KB SRAM, 16 MB Flash).

**GNSS (u-blox NEO-M9N)**

- Supported constellations: GPS, Galileo, GLONASS, and BeiDou (simultaneous operation).  
- Update rate: up to 25 Hz (configured in the project).  
- Typical horizontal position accuracy: ±1.5 m (CEP, active GNSS).  
- Altitude accuracy: ±1.5 m (CEP) with proper correction.  

**Models and altitude calculation:**

- Geometric reference: all calculations use the WGS84 ellipsoid as the base to determine local radius and theoretical gravity.  
- **EGM96**: used to obtain geoid separation (difference between geoid and ellipsoid) when the calculation involves HAE.  

**Two available calculation methods:**

1. **BMP280 mode**  
   - Latitude provided by GNSS for local WGS84 radius calculation.  
   - Altitude obtained by the BMP280 (barometer), converted from atmospheric pressure.  
   - Ignores geoid separation, resulting in slight accuracy loss.  

2. **HAE mode (more accurate)**  
   - Latitude provided by GNSS for local WGS84 radius calculation.  
   - Altitude obtained via HAE (Height Above Ellipsoid), extracted from GNSS with TinyGPSCustom and adjusted according to WGS84, considering geoid separation from EGM96.  

**Time Dilation Calculation**

- Theoretical basis: Principles of Special Relativity (velocity) and General Relativity (gravitational potential).  
- Reference: Comparison with a theoretical clock on the WGS84 ellipsoid, at the same latitude/longitude and at rest relative to space.  

**Inputs used:**
- Relative velocity (GNSS).  
- Altitude via BMP280 + WGS84 or HAE + WGS84 method.  
- Combined geoid and ellipsoid models for greater accuracy.  

**Output:** Estimated time difference in nanoseconds/hour.

---

## 4. How It Works

**LOCAL ROTATIONAL VELOCITY**

This is the speed at which you are moving due to Earth’s rotation, determined solely by your latitude and altitude.  
- The closer to the Equator, the greater this speed — at the Equator, it is approximately 1674 km/h — and it decreases as you approach the poles.  
- At the exact North or South Pole, rotational speed is zero, and therefore there is no contribution from special relativity (due to velocity) at that point.  

**TOTAL VELOCITY**

This is your total speed in the inertial reference frame (space), resulting from the vector sum of Earth’s rotation and your displacement measured by GNSS.

**Examples:**
- If, at your latitude, Earth’s rotation is 1600 km/h (eastward) and you travel at 200 km/h eastward, your total speed will be 1800 km/h — increasing your clock’s delay due to special relativity (velocity effect).  
- If you travel 200 km/h westward, the total speed will be 1400 km/h, and your clock will run slightly faster than if you were stationary or moving eastward.  
- If you travel exactly at the same speed as the local rotation, but westward (e.g., 1600 km/h at this latitude), you will completely cancel out the effect of rotation and thus special relativity (velocity effect) will cease to exist, leaving only the gravitational effect from general relativity.  
- If you travel faster than the local rotation westward (e.g., 2000 km/h at this latitude), you will be moving at 400 km/h opposite to Earth’s rotation, again increasing your total speed and slowing time.  

This effect is easier to cancel or reverse near the poles, as Earth’s natural rotational speed is lower in these regions.

**The calculation considers not only east-west movement but also north-south.**  
The project’s algorithm uses vector decomposition:  
- The east-west component directly interacts with Earth’s rotation, which can add or subtract speed.  
- The north-south component always increases total speed relative to space, as it represents movement perpendicular to rotational motion.  

📌 **Example:**  
If you move northwest, part of your speed reduces the rotation effect (west component), while another part increases total speed (north component).  
This ensures that the calculation is accurate for any direction and speed measured by GNSS.

---

**LOCAL GRAVITY**

This is the local gravity at your position, calculated from latitude and altitude relative to the WGS84 ellipsoid.

The calculation uses **Somigliana’s formula** to account for Earth’s flattening and gravity variation with latitude, plus the **free-air correction** to adjust gravity with altitude.

In the project, this altitude can come in two ways:
1. **Default mode with barometer (BMP280 + WGS84):** altitude calculated from atmospheric pressure and GNSS latitude.  
2. **HAE mode (Height Above Ellipsoid + WGS84):** altitude obtained directly from GNSS via HAE and adjusted according to the WGS84 model.  

**General relativity (gravitational effect)** is calculated relative to the WGS84 ellipsoid level. This means:
- If you are exactly at ellipsoid level (altitude = 0 m in this reference), the general relativity effect will be null.  
- Being above the ellipsoid, time runs slightly faster.  
- Being below the ellipsoid, time runs slightly slower.  

📌 **Actual behavior inside Earth:**  
- In the first layers below the surface, gravity tends to increase slightly due to being closer to the core.  
- In deeper layers, gravity begins to decrease, as a significant portion of the planet’s mass is now above you, exerting force in the opposite direction.  
- Near the outer core, gravity reaches a maximum and then gradually falls to zero exactly at Earth’s center.  

A graph known as **Free-Fall Acceleration of Earth** shows this behavior in detail, taking into account the specific density of each layer.  
Even at the deepest point ever explored by humans — the Mariana Trench — gravity is still slightly greater than at the surface, as it is within the initial increase region.

---

**Time Dilation Calculation (TIME DILATION ns/h)**

This function calculates, in real time, how much your clock is gaining or losing compared to a reference clock at rest at the WGS84 ellipsoid level, at the same latitude/longitude.

The result represents how many nanoseconds per hour (ns/h) your clock “gains” or “loses” due to velocity and gravity effects.

The calculation combines two phenomena of the Theory of Relativity:
1. **Special Relativity (velocity)** – The greater the velocity relative to space, the slower time passes.  
2. **General Relativity (gravity)** – The higher you are (lower local gravity), the faster time passes.  

**How the function works:**
1. Reading input data:  
   - Velocity and movement direction (from GNSS).  
   - Latitude and altitude (from GNSS and/or barometer, depending on the configured mode).  
2. Separation of velocity into east-west and north-south components:  
   - Correctly considers Earth’s rotation effect, which can increase or decrease your total speed relative to space.  
3. Calculation of local rotational speed:  
   - Determined by latitude and altitude — maximum at the Equator and zero at the poles.  
4. Determination of total speed relative to space:  
   - Combines Earth’s rotation with your own motion, considering any direction (east, west, north, south, or diagonal).  
5. Calculation of local gravity:  
   - Using latitude and altitude, adjusted for small variations with height (free-air correction).  
6. Combination of relativistic effects:  
   - Velocity (Special Relativity) delays the clock.  
   - Altitude (General Relativity) advances the clock.  
   - The result can be positive or negative, depending on which effect predominates.  
7. Conversion of result:  
   - The combined value is converted to nanoseconds per hour (ns/h) and displayed on the screen and in a line chart in real time.  

---

## 5. Interesting Facts

During the development of the project, I conducted some research and discovered quite important information in my simulations. Of course, time dilation on Earth is practically irrelevant for human life, but with further analysis, these curiosities could be recognized titles, if indeed factual:

- **City where time runs slowest on the planet:** Pontianak, Indonesia.  
- **City where time runs fastest:** Longyearbyen (home to the Svalbard Global Seed Vault). This vault stores over 1.3 million varieties of agricultural seeds, kept at about −18 °C inside a tunnel carved into a permafrost mountain, serving as a “world insurance policy” against biodiversity loss.  
- **Tourist spot where time runs fastest:** Praia do Café, Ilhéu das Rolas, São Tomé and Príncipe.  
- **Place with the lowest gravity on Earth:** Summit of Mount Chimborazo (Ecuador). It is the farthest point from Earth’s center, about 2.1 km farther than Mount Everest’s summit. In addition, it spins faster than any other point on land, not only due to its altitude but also because it is practically on the Equator, where Earth’s rotational speed is maximum.  
- **Place where Earth “spins” fastest:** Also the summit of Mount Chimborazo, due to the combination of altitude and proximity to the Equator.  

---
