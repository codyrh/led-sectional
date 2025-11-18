# led-sectional
January 24, 2024
----------------
Added support for ESP32-WROOM-DA.  Can now compile for either that board or the ESP8266.
1.  Added defines at top of code specific to each board.
2.  Added define for each board around call to set mode G on wifi interface.
No other code changes needed to support the ESP32-WROOM_DA.

Made misc edits.

January 20, 2024
----------------
1. Serial output now shows airport code for LEDs with storms/lightning, high winds, or very high winds vs just the LED
   number.  Also shows loop total, e.g. "Loop 1 of 300".
2. Changed default LOOP_INTERVAL to 1000 vs 5000.

January 19, 2024
----------------
Edited to show METAR string in serial output.  Prior code looked like it should have worked, but did not.

Added a second threshold for very high winds or gusts.
  1.  Added constant FADE_FOR_HIGH_WINDS to either blink/fade to 50% of flight category color for high winds, or
      blink black/clear.  Default is true.
  2.  LEDs will blink orange for very high winds or gusts.

Edited to show count of LEDs with thunderstorms/lightning, high winds, very high winds in serial output.

 January 17, 2024
 ----------------
 Added line: WiFi.setPhyMode(WIFI_PHY_MODE_11G); after WiFi.mode(WIFI_STA);
 This seems to fix a problem with connection failures to SmartWiFi routers that have the same SSID for both 2.4 and
 5 ghz bands.
 
 October 19, 2023
 ----------------
 Updated SERVER and BASE_URI to reflect aviationweather.gov changes.
  
 May 5, 2023
 -----------
 Fixed wifi connection logic.  Previously was doing an autoconnect every time a loop
 occurred, which is whenever there is lighting or thunderstorms or high winds found.
 
 April 27, 2023
 CHANGE LIST from Kyle Harmon's Code at https://github.com/WKHarmon/led-sectional
 --------------------------------------------------------------------------------
 1. Added WiFiManger. See this reference: https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
 2. High wind (orange) color will now blink vs being a solid orange, and be shown for all flight categories, not just VFR
 3. Lightning (white blink) will occur not just for TS (thunderstorms), but also for LTG and LTNG reported in <raw_text> in response XML

 As a result of 2 and 3, any particular LED could have 3 colors...one for flight category, and blinking either or both of orange and white.

=====================

This uses an ESP8266 or ESP32 Arduino-compatible device to download METARs for a set of airports and assign a color to a LED representing each airport so that they can be put into a sectional. It is inspired by https://www.reddit.com/r/flying/comments/7avr8q/flight_conditions_sectional_wall_art_thing/

## Features

- **Real-time Weather**: Downloads METAR data from aviationweather.gov
- **Visual Indicators**: LED colors represent flight categories (VFR=Green, MVFR=Blue, IFR=Red, LIFR=Magenta)
- **Weather Alerts**: Blinking effects for lightning/thunderstorms and high winds
- **WiFi Management**: Easy WiFi setup using WiFiManager
- **Arduino Cloud Integration**: Remote monitoring, logging, and OTA updates (NEW!)

## Hardware Requirements

- ESP8266 (NodeMCU 1.0/ESP-12E Module) or ESP32-WROOM-DA
- WS2811/WS2812B LED strip or individual LEDs
- Level shifter (recommended for data signal)
- Power supply appropriate for your LED count

## Software Requirements

Use ESP8266 Core V2.74 for the ESP8266, or esp32 2.0.11 by Espressif for the ESP32, and these libraries:
- FastLED V3.30
- WiFiManager 2.0.15-rc.1
- ArduinoJson 6.21.3
- ArduinoIoTCloud (optional - for cloud features)
- Arduino_ConnectionHandler (optional - for cloud features)

## New: Multi-Board Configuration System

This project now supports **separate configuration files** for each board, making it easy to manage multiple LED sectionals:

### Easy Board Management
- **Config Files**: Each board has its own `config_[name].h` file with all settings
- **Separate Credentials**: Individual Arduino Cloud credentials for each board
- **Quick Switching**: Change boards by modifying one line in the main code
- **No More #defines**: Clean, organized configuration system

### Quick Setup
1. Choose your config: `#define BOARD_CONFIG_FILE "config_home.h"`
2. Customize settings in your config file
3. Set up Arduino Cloud credentials (optional)
4. Upload and run!

**See [MULTI_BOARD_CONFIG.md](MULTI_BOARD_CONFIG.md) for complete setup instructions**

## Arduino Cloud Integration

This project now supports Arduino IoT Cloud for advanced features:

### Features
- **Remote Monitoring**: View real-time status from anywhere
- **OTA Updates**: Update firmware wirelessly
- **Cloud Logging**: Track weather alerts and system status
- **Remote Control**: Enable/disable features remotely
- **Historical Data**: Automatic data logging and analysis

### Quick Setup
1. See [ARDUINO_CLOUD_SETUP.md](ARDUINO_CLOUD_SETUP.md) for detailed setup instructions
2. See [CLOUD_USAGE_EXAMPLES.md](CLOUD_USAGE_EXAMPLES.md) for usage examples
3. Arduino Cloud features are optional - the project works fine without them

### Enable/Disable Cloud Features
In the main .ino file, you can easily enable or disable cloud features:
```cpp
#define USE_ARDUINO_CLOUD true  // Set to false to disable Arduino Cloud features
```
