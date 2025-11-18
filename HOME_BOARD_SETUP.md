# Home Board Setup Checklist

## ✅ Configuration Verification

### 1. Main Code Setup
- [ ] `led_sectional.ino` has `#define BOARD_CONFIG_FILE "config_home.h"`
- [ ] `led_sectional.ino` has `#include "arduino_secrets_home.h"` uncommented
- [ ] Other secrets files are commented out

### 2. Board Configuration (`config_home.h`)
- [ ] **Board Name**: "Home" 
- [ ] **Location**: Update "Living Room" to your actual location
- [ ] **LED Count**: 30 LEDs (verify this matches your hardware)
- [ ] **LED Type**: WS2811 (change to WS2812B if needed)
- [ ] **Color Order**: RGB (change to GRB for WS2812B)
- [ ] **Brightness**: 70 (adjust 0-255 as needed)
- [ ] **Data Pin**: GPIO 5 (verify this matches your wiring)

### 3. Airport Configuration
Current airports (Pacific Northwest):
```
Legend: VFR, MVFR, IFR, LIFR, WVFR (don't change)
KUIL, NULL, KHQM, NULL, KSHN,
KOLM, KGRF, KPLU, KTCM, KTIW,
KPWT, KSEA, KRNT, KBFI, KPAE,
KAWO, K0S9, KNUW, KBVS, KBLI,
KORS, KFHR, CYYJ, NULL, KCLM
```
- [ ] **Verify airports match your sectional chart**
- [ ] **Update airport codes** if needed for your region
- [ ] **Use "NULL"** for positions without LEDs

### 4. Arduino Cloud Credentials (`arduino_secrets_home.h`)
- [ ] **WiFi SSID**: Replace "YOUR_WIFI_SSID_HOME" with your network name
- [ ] **WiFi Password**: Replace "YOUR_WIFI_PASSWORD_HOME" with your password
- [ ] **Device Key**: Get from Arduino Cloud Thing setup
- [ ] **Thing ID**: Get from Arduino Cloud Thing setup (optional)

### 5. Weather Thresholds
- [ ] **High Wind**: 15 knots (yellow fade/blink)
- [ ] **Very High Wind**: 20 knots (orange blink)
- [ ] **Lightning Alerts**: Enabled (white blink)
- [ ] **Fade Mode**: Enabled (50% fade vs black blink)

### 6. Hardware Verification
- [ ] **Power Supply**: Adequate for 30 LEDs
- [ ] **Level Shifter**: Recommended for LED data signal
- [ ] **Wiring**: Data pin connected to GPIO 5
- [ ] **LED Strip**: 30 LEDs total (or update NUM_AIRPORTS)

## 🚀 Upload and Test

### Before Upload:
1. **Compile** the code to check for errors
2. **Verify** all settings one more time
3. **Check** that secrets file has real credentials (not placeholder text)

### After Upload:
1. **Serial Monitor** should show:
   - "Board Name: Home"
   - "Board Location: [Your Location]"
   - Airport codes list
   - WiFi connection status
   - METAR data retrieval

2. **LED Behavior**:
   - First 5 LEDs show legend colors (Green, Blue, Red, Magenta, Yellow)
   - Airport LEDs show weather conditions
   - Orange WiFi connection status initially

3. **Arduino Cloud** (if enabled):
   - Device appears online in dashboard
   - Properties update with current data
   - Can control settings remotely

## 🔧 Common Customizations

### Change Location Description
In `config_home.h`:
```cpp
#define BOARD_LOCATION "Your Actual Location"  // Update this
```

### Adjust Brightness
In `config_home.h`:
```cpp
#define BRIGHTNESS 50  // Lower for dimmer LEDs (0-255)
```

### Change Airport Codes
In `config_home.h`, replace airports with your local ones:
```cpp
"KORD", "KMDW", "KPWK", ...  // Chicago area example
```

### Different LED Hardware
For WS2812B LEDs:
```cpp
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB  // Note: GRB instead of RGB
```

## 📱 Arduino Cloud Setup (Optional)

If you want cloud monitoring:

1. **Create Arduino Account**: https://create.arduino.cc
2. **Create Thing**: "LED Sectional - Home"
3. **Add Device**: Your ESP8266/ESP32
4. **Add Properties**: 
   - `airports_processed` (int, READ)
   - `lightning_alerts` (int, READ)
   - `device_status` (String, READ)
   - `enable_lightning_alerts` (boolean, READ/WRITE)
   - And others as needed...
5. **Copy Credentials**: To `arduino_secrets_home.h`
6. **Create Dashboard**: Add widgets for monitoring

## 🆘 Troubleshooting

### Compilation Errors
- **"Board configuration file not found"**: Check file path and name
- **"NUM_AIRPORTS mismatch"**: Verify airport array size matches NUM_AIRPORTS
- **Library errors**: Install required libraries (FastLED, WiFiManager, ArduinoJson)

### Runtime Issues
- **No WiFi connection**: Check credentials in secrets file
- **LEDs not working**: Verify wiring, LED type, and power supply
- **No weather data**: Check serial monitor for METAR request errors
- **Wrong colors**: Verify COLOR_ORDER setting (RGB vs GRB)

### Arduino Cloud Issues
- **Device offline**: Check WiFi connection and device credentials
- **Properties not updating**: Verify Thing setup and property names
- **Can't control remotely**: Check property permissions (READ/WRITE)

---

**Ready to proceed?** Let me know if you need help with any of these steps!