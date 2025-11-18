# Multi-Board Configuration System

This document explains how to set up and manage multiple LED sectional boards using the new configuration file system.

## Overview

The LED sectional project now uses separate configuration files for each board, making it much easier to:
- Manage multiple boards with different settings
- Keep board-specific settings organized
- Integrate with Arduino Cloud for individual board monitoring
- Deploy the same code to different hardware configurations

## Configuration File Structure

Each board configuration consists of two files:
1. **Board Config File** (`config_[name].h`) - Hardware and airport settings
2. **Secrets File** (`arduino_secrets_[name].h`) - WiFi and Arduino Cloud credentials

## Quick Setup for Multiple Boards

### Step 1: Choose Your Board Configuration

In the main `led_sectional.ino` file, set which config file to use:

```cpp
// SELECT YOUR BOARD CONFIGURATION FILE:
#define BOARD_CONFIG_FILE "config_home.h"  // Change this to your config file
```

Available configurations:
- `config_home.h` - Home board (30 LEDs, WS2811, Pacific Northwest airports)
- `config_asa.h` - ASA board (34 LEDs, WS2811, Chapter airports)
- `config_mark_l.h` - Mark L board (29 LEDs, WS2812B, custom layout)
- `config_custom.h` - Template for your own configuration

### Step 2: Set Up Arduino Cloud Credentials

If using Arduino Cloud, uncomment the correct secrets file:

```cpp
#include "arduino_secrets_home.h"    // For Home board
// #include "arduino_secrets_asa.h"     // For ASA board  
// #include "arduino_secrets_mark_l.h"  // For Mark L board
// #include "arduino_secrets_custom.h"  // For custom board
```

### Step 3: Configure Your Board Settings

Edit your chosen config file (e.g., `config_home.h`) to customize:
- Airport codes for your sectional
- LED hardware type and count
- Wind thresholds
- Brightness settings
- Board identification

### Step 4: Set Up Arduino Cloud (Optional)

1. Create separate "Things" in Arduino Cloud for each board
2. Configure each secrets file with the appropriate credentials
3. Upload firmware to each board

## Creating a New Board Configuration

### Method 1: Copy Existing Configuration

1. Copy `config_custom.h` to `config_yourname.h`
2. Edit the new file with your settings:
   ```cpp
   #define BOARD_NAME "YourName"
   #define BOARD_LOCATION "Your Location"
   #define BOARD_DESCRIPTION "Your Description"
   
   // Customize all other settings...
   const char* AIRPORTS[] = {
     "VFR", "MVFR", "IFR", "LIFR", "WVFR",  // Keep these
     // Add your airport codes here...
   };
   ```
3. Update main .ino file: `#define BOARD_CONFIG_FILE "config_yourname.h"`

### Method 2: Arduino Cloud Integration

1. Copy `arduino_secrets_custom.h` to `arduino_secrets_yourname.h`
2. Fill in your WiFi and Arduino Cloud credentials
3. Update main .ino file to include your secrets file

## Configuration File Reference

### Board Config File (`config_[name].h`)

```cpp
// Board identification
#define BOARD_NAME "BoardName"              // Used for Arduino Cloud identification
#define BOARD_LOCATION "Physical Location"  // Descriptive location
#define BOARD_DESCRIPTION "Description"     // Board description

// LED Hardware
#define NUM_AIRPORTS 30        // Total number of LEDs
#define LED_TYPE WS2811        // LED chip type
#define COLOR_ORDER RGB        // Color channel order
#define BRIGHTNESS 70          // Default brightness (0-255)
#define DATA_PIN 5             // GPIO pin for LED data

// Weather Thresholds
#define HIGH_WIND_THRESHOLD 25  // Very high wind threshold (knots)
#define WIND_THRESHOLD 15       // High wind threshold (knots)

// Features
#define ENABLE_LIGHTNING_ALERTS true  // Enable lightning detection
#define ENABLE_WIND_ALERTS true       // Enable wind detection
#define FADE_FOR_HIGH_WINDS true      // Fade vs blink for high winds

// Light Sensor (optional)
#define USE_LIGHT_SENSOR false         // Enable light sensor
#define LIGHT_SENSOR_TSL2561 false     // Digital vs analog sensor
#define MIN_BRIGHTNESS 20              // Minimum brightness
#define MAX_BRIGHTNESS 20              // Maximum brightness

// Timing
#define LOOP_INTERVAL 1000      // Update interval during active weather (ms)
#define REQUEST_INTERVAL 300000 // METAR request interval (ms)

// Airport List
const char* AIRPORTS[] = {
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",  // Legend (required)
  // Your airport codes here...
};
```

### Secrets File (`arduino_secrets_[name].h`)

```cpp
// WiFi Credentials
#define SECRET_SSID "Your_WiFi_Name"
#define SECRET_PASS "Your_WiFi_Password"

// Arduino Cloud Credentials
#define SECRET_DEVICE_KEY "Your_Device_Secret_Key"
#define THING_ID "your-thing-id"
#define DEVICE_NAME "ESP32-YourBoard-LEDSectional"
```

## Managing Multiple Boards

### Deployment Strategy

1. **Development Board**: Test new configurations on one board first
2. **Staging**: Deploy to a subset of boards for testing
3. **Production**: Roll out to all boards after validation

### Version Control Best Practices

```bash
# Create branches for different configurations
git checkout -b config/home-board
git checkout -b config/asa-board
git checkout -b config/mark-l-board

# Or use tags for releases
git tag -a v2.0-home -m "Home board configuration v2.0"
git tag -a v2.0-asa -m "ASA board configuration v2.0"
```

### File Organization

```
led_sectional/
├── led_sectional.ino              # Main code
├── config_home.h                  # Home board config
├── config_asa.h                   # ASA board config
├── config_mark_l.h                # Mark L board config
├── config_custom.h                # Template config
├── arduino_secrets_home.h         # Home credentials (gitignored)
├── arduino_secrets_asa.h          # ASA credentials (gitignored)
├── arduino_secrets_mark_l.h       # Mark L credentials (gitignored)
├── thingProperties.h              # Arduino Cloud properties
└── README.md                      # Documentation
```

### Arduino Cloud Setup for Multiple Boards

1. **Create separate Things**:
   - "LED Sectional - Home"
   - "LED Sectional - ASA"
   - "LED Sectional - Mark L"

2. **Use consistent property names** across all Things for easier management

3. **Create master dashboard** showing all boards

4. **Set up webhooks** for notifications from any board

## Troubleshooting

### "Board configuration file not found"
- Check that `BOARD_CONFIG_FILE` points to an existing file
- Ensure the config file is in the same directory as the .ino file
- Verify the config file has the required #define statements

### "Secrets file not found"
- Check that the correct secrets file is uncommented
- Ensure credentials files exist and aren't gitignored accidentally
- Copy from template files if missing

### "Arduino Cloud connection failed"
- Verify credentials in secrets file match Arduino Cloud Thing
- Check that Thing ID and Device Key are correct
- Ensure WiFi credentials are valid for the board's location

### "Airport codes not working"
- Verify AIRPORTS array size matches NUM_AIRPORTS
- Check that airport codes are valid ICAO identifiers
- Ensure first 5 entries are legend codes (VFR, MVFR, etc.)

## Migration from Old System

If upgrading from the old `BOARD_CONFIG_*` system:

1. **Identify your current configuration** (ASA, HOME, MARK_L, CUSTOM)
2. **Copy settings** to appropriate config file
3. **Update main .ino file** to use `BOARD_CONFIG_FILE`
4. **Test compilation** before deploying
5. **Update Arduino Cloud** credentials if using cloud features

## Advanced Features

### Conditional Compilation

You can add board-specific features in your config files:

```cpp
// In config_home.h
#define HAS_TEMPERATURE_SENSOR true
#define TEMPERATURE_PIN A1

// In config_asa.h  
#define HAS_TEMPERATURE_SENSOR false
```

### Runtime Configuration

Access board info at runtime:

```cpp
void setup() {
  Serial.print("Board: ");
  Serial.println(BOARD_NAME);
  
  Serial.print("Location: ");
  Serial.println(BOARD_LOCATION);
  
  Serial.print("Config file: ");
  Serial.println(BOARD_CONFIG_FILE);
}
```

### Build Automation

Create build scripts for different boards:

```bash
#!/bin/bash
# build-home.sh
sed -i 's/BOARD_CONFIG_FILE .*/BOARD_CONFIG_FILE "config_home.h"/' led_sectional.ino
arduino-cli compile --fqbn esp32:esp32:esp32 led_sectional.ino
```

## Support

- Check the main [ARDUINO_CLOUD_SETUP.md](ARDUINO_CLOUD_SETUP.md) for Arduino Cloud setup
- See [CLOUD_USAGE_EXAMPLES.md](CLOUD_USAGE_EXAMPLES.md) for usage examples
- Review individual config files for specific board settings