# QNA Display for Waveshare ESP32-S3 LCD 1.47

This project turns a Waveshare `ESP32-S3-LCD-1.47` board into a small Wi-Fi controlled prompt display.

It is designed for situations like:
- interviews
- presentations
- speaker notes
- silent prompts for "what to talk about next"

You control the screen from a phone or laptop on the same Wi-Fi network using a simple web page.

## What the device can do

The display can show:
- `A`
- `B`
- `C`
- `D`
- custom text
- `Blank` mode
- `Thinking` mode with animated dots
- `Flag` marker for a question or important moment

The web controller updates the device live over Wi-Fi.

## Hardware required

You need:
- `Waveshare ESP32-S3-LCD-1.47`
- a `USB-C cable` that supports data, not just charging
- a Windows PC, Mac, or Linux computer
- a Wi-Fi network

This project was built for:
- Waveshare product page / wiki:
  [https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47](https://www.waveshare.com/wiki/ESP32-S3-LCD-1.47)

## Software required

Install:
- `Arduino IDE`
- `ESP32 by Espressif Systems` board package
- `Adafruit GFX Library`
- `Adafruit ST7735 and ST7789 Library`

## Project files

- `interview_controller.ino`
  Main source file
- `interview_controller_sketch/interview_controller_sketch.ino`
  Sketch-folder copy used for Arduino CLI / upload workflows

## Board details

Board used:
- `Waveshare ESP32-S3-LCD-1.47`

Display driver:
- `ST7789`

Display size:
- `320 x 172`

Pin mapping used in the firmware:
- `MOSI = GPIO45`
- `SCLK = GPIO40`
- `CS = GPIO42`
- `DC = GPIO41`
- `RST = GPIO39`
- `BL = GPIO48`

You do not need to wire these manually if you are using the Waveshare board as sold. They are already connected on the PCB.

## Before you start

Make sure:
- the board is connected by USB
- the USB cable supports data
- your computer can detect a serial port when the board is plugged in

## Step 1: Install Arduino IDE

Download and install Arduino IDE:
- [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

After install, open Arduino IDE once.

## Step 2: Install ESP32 board support

In Arduino IDE:
1. Open `File > Preferences`
2. Find `Additional Boards Manager URLs`
3. Add this URL:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

4. Click `OK`
5. Open `Tools > Board > Boards Manager`
6. Search for `esp32`
7. Install `ESP32 by Espressif Systems`

## Step 3: Install required libraries

In Arduino IDE:
1. Open `Tools > Manage Libraries`
2. Search for `Adafruit GFX Library`
3. Install it
4. Search for `Adafruit ST7735 and ST7789 Library`
5. Install it

If Arduino asks to install dependencies like `Adafruit BusIO`, install those too.

## Step 4: Open the project

Open:
- `interview_controller.ino`

If Arduino IDE asks to move or rename files into a sketch folder, allow it.

## Step 5: Configure Wi-Fi

In the source code, find these lines:

```cpp
const char* WIFI_SSID = "M3_Devices";
const char* WIFI_PASSWORD = "Marzook7494.";
```

Replace them with your own Wi-Fi details if needed.

Important:
- the phone/laptop you use as the remote control must be on the same Wi-Fi network
- the ESP32 only serves the controller page inside that local network

## Step 6: Select the correct board and port

In Arduino IDE:
1. Open `Tools > Board`
2. Choose:
   - `Waveshare ESP32-S3-LCD-1.47` if available
   - otherwise try `ESP32S3 Dev Module`
3. Open `Tools > Port`
4. Select the serial port that appears when the board is connected

On Windows this usually looks like:
- `COM5`
- `COM6`
- `USB Serial Device`

## Step 7: Upload the firmware

In Arduino IDE:
1. Click the `Upload` button
2. Wait for compile to finish
3. Wait for the board to flash and reboot

If upload fails:
- try a different USB cable
- unplug and reconnect the board
- select the correct port again
- hold the board `BOOT` button during upload if needed

## Step 8: Confirm the device started

After boot:
- while connecting to Wi-Fi, the top-right dot blinks red
- when connected, the top-right dot becomes green
- the screen shows the current IP address

Example:

```text
192.168.5.89
```

That IP address is the address of the controller web page.

## Step 9: Open the web controller

On a phone, tablet, or laptop connected to the same Wi-Fi:
1. Open a browser
2. Type the IP address shown on the display

Example:

```text
http://192.168.5.89
```

You should see a control page with buttons for:
- `PART A`
- `PART B`
- `PART C`
- `PART D`
- `BLANK`
- `. . .`
- `FLAG`
- `SHOW TEXT`
- `CLEAR`

## How to use the controller

### `A / B / C / D`
Shows the selected section on the display.

### `SHOW TEXT`
Type custom text in the box, then press `SHOW TEXT`.

### `CLEAR`
Clears the custom text value.

### `BLANK`
Turns the screen fully black.

Pressing another action brings the content back.

### `. . .`
Shows a smooth animated thinking indicator.

### `FLAG`
Marks a question or important moment with a red `FLAG` badge on screen.

Pressing any other action clears the flag automatically.

## Screen behavior

- green dot:
  Wi-Fi connected
- blinking red dot:
  trying to connect to Wi-Fi
- blank screen:
  intentional blank mode

The firmware also disables Wi-Fi sleep so the controller stays responsive.

## Typical workflow

Example interview workflow:
1. Power the board
2. Wait for Wi-Fi to connect
3. Open the IP address on your phone
4. Press `PART A`
5. Switch to `PART B`, `PART C`, `PART D` as needed
6. Use `FLAG` when a question needs follow-up
7. Use custom text when you need a one-off prompt
8. Use `BLANK` when you want the display hidden

## Troubleshooting

### The board does not appear in Arduino IDE

Try:
- a different USB cable
- a different USB port
- reinstalling the ESP32 board package
- reconnecting the board after Arduino IDE is open

### Upload fails

Try:
- selecting the correct COM port
- pressing `BOOT` while upload starts
- unplugging and reconnecting the board
- closing any serial monitor or app using the COM port

### The screen turns on but the controller page does not open

Check:
- your phone/laptop is on the same Wi-Fi as the ESP32
- you typed the IP exactly as shown on the screen
- the top-right dot is green

### The controller page opens but button presses do not work

Try:
- refreshing the page
- waiting 1 to 2 seconds for state sync
- checking that the board still has power

### The board never connects to Wi-Fi

Check:
- SSID spelling
- password spelling
- Wi-Fi signal strength
- whether the network allows normal local-device access

## Open source notes

This repo is intended to be easy for non-embedded developers to use.

If you are new to hardware:
- you do not need to design a PCB
- you do not need to solder anything for the standard Waveshare board
- you mostly just need the right board, Arduino IDE, libraries, and Wi-Fi credentials

## Repository setup suggestion

If you are sharing this project publicly, a good next improvement would be:
- move Wi-Fi credentials out of source before publishing production use
- add screenshots of the device and controller page
- add a short demo video
- add a wiring/photo section for beginners

## License

Add a license file before wide public reuse if you want to make the usage terms explicit.
