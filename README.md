# Interview controller sketch for Waveshare ESP32-S3-LCD-1.47

## What this does
- Connects to Wi-Fi `M3_Devices` with password `Marzook7494.`
- Serves a local web page (`/`) with buttons: **A, B, C, D**
- Sends selection with `/set?part=A/B/C/D`
- Displays current part on the 1.47" LCD (ST7789, 172x320)
- Shows a **green dot** in top-right when Wi-Fi is connected, **red** when not connected

## Flash notes
- File: `interview_controller.ino`
- Pin mapping used (from Waveshare page):
  - MOSI: `GPIO45`
  - SCLK: `GPIO40`
  - CS: `GPIO42`
  - DC: `GPIO41`
  - RST: `GPIO39`
  - BL: `GPIO48`

## Files and libraries
- Install in Arduino IDE:
  - `Adafruit ST7735 and ST7789 library`
  - `Adafruit GFX Library`
  - `WebServer` and `WiFi` are built into ESP32 core
- ESP32 board package: `esp32 by Espressif Systems` (>= 3.0.2)

## Recommended flash flow (Arduino IDE)
1. Open `interview_controller.ino`
2. Select:
   - Board: `ESP32S3 Dev Module` (or your board variant)
   - Port: one of the `USB Serial Device` ports
3. Press Upload

## Current serial ports detected in this PC session
- `COM5` (`USB Serial Device`, VID_303A PID_1001)
- `COM8` (`USB Serial Device`, VID_0483 PID_5740)
- `COM7` (`USB Serial Device`, VID_0483 PID_5740)

Try flashing to each (if needed) and pick the one that resets into upload mode on upload.

## After flash
- Open serial monitor at 115200 baud
- Wait for IP address shown on display and in serial
- From phone/laptop on same Wi‑Fi: open `http://<esp32-ip>/`
  - Tap a part (A/B/C/D) and screen updates instantly

## Optional: verify
- Ensure the top-right corner dot is green
- If dot is red, check SSID/password and signal
