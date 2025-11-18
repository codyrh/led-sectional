# Arduino IoT Cloud Integration for LED Sectional

This document explains how to add Arduino IoT Cloud connectivity to your LED sectional project for remote monitoring, logging, and Over-The-Air (OTA) updates.

## Overview

The integration adds the following features:
- **Remote Monitoring**: View real-time status of your LED sectional from anywhere
- **OTA Updates**: Update your device firmware wirelessly through Arduino Cloud
- **Cloud Logging**: Track weather alerts, airport processing, and system status
- **Remote Control**: Enable/disable features and adjust settings remotely
- **Historical Data**: Arduino Cloud automatically stores data for analysis

## Prerequisites

1. **Arduino Account**: Create a free account at [create.arduino.cc](https://create.arduino.cc)
2. **ESP8266/ESP32 Device**: Your existing LED sectional hardware
3. **Arduino IDE**: Version 1.8.13 or newer, or Arduino IDE 2.x
4. **Required Libraries** (install via Library Manager):
   - ArduinoIoTCloud (by Arduino)
   - Arduino_ConnectionHandler (by Arduino)
   - WiFiManager (existing)
   - FastLED (existing)
   - ArduinoJson (existing)

## Step 1: Install Arduino IoT Cloud Libraries

In Arduino IDE:
1. Go to **Tools > Manage Libraries**
2. Search for and install:
   - "ArduinoIoTCloud" by Arduino
   - "Arduino_ConnectionHandler" by Arduino

## Step 2: Set Up Arduino IoT Cloud

### Create a Thing
1. Go to [Arduino IoT Cloud](https://create.arduino.cc/iot/)
2. Click **"Create Thing"**
3. Give your Thing a name (e.g., "LED Sectional Weather")

### Add a Device
1. In your Thing, click **"Select Device"**
2. Click **"Set up new device"**
3. Choose your board type (ESP8266 or ESP32)
4. Follow the setup wizard to connect your device
5. **Save the Device ID and Secret Key** - you'll need these

### Create Properties
Add these properties to your Thing dashboard:

**Status Properties (READ ONLY):**
- `airports_processed` (int) - Number of airports processed
- `lightning_alerts` (int) - Airports with lightning/thunderstorms
- `high_wind_alerts` (int) - Airports with high winds
- `very_high_wind_alerts` (int) - Airports with very high winds
- `device_status` (String) - Current device status
- `last_update` (String) - Last update timestamp
- `log_message` (String) - Latest log message
- `cloud_connected` (boolean) - Cloud connection status

**Control Properties (READ/WRITE):**
- `enable_lightning_alerts` (boolean) - Enable/disable lightning detection
- `enable_wind_alerts` (boolean) - Enable/disable wind alerts
- `brightness_override` (int) - Manual brightness control (-1 = auto, 0-255 = manual)
- `force_update` (boolean) - Force immediate weather update
- `config_message` (String) - Send configuration commands

## Handling Multiple LED Sectional Boards

If you have multiple LED sectional boards (e.g., one at home, one at hangar, different regional maps), you have several Arduino Cloud setup options:

### Option 1: Separate Things (Recommended)

**Best for**: Different locations, different purposes, or independent operation

**Setup**:
1. Create a separate "Thing" for each board:
   - "LED Sectional - Home"
   - "LED Sectional - Hangar" 
   - "LED Sectional - Regional"
2. Each Thing gets its own device and properties
3. Each board runs independently with its own credentials

**Advantages**:
- Complete independence - one board failure doesn't affect others
- Different configurations per location (different airports, thresholds)
- Separate dashboards and controls for each location
- Individual OTA updates and maintenance
- Better security isolation
- Easier troubleshooting

**Disadvantages**:
- More Things to manage in Arduino Cloud
- May hit Arduino Cloud free plan limits faster
- Separate dashboards (though you can create a combined view)

### Option 2: Single Thing with Multiple Devices

**Best for**: Similar boards that should be managed together

**Setup**:
1. Create one "Thing" called "LED Sectional Network"
2. Add multiple devices to the same Thing
3. Use device-specific property naming:
   - `home_airports_processed`, `hangar_airports_processed`
   - `home_lightning_alerts`, `hangar_lightning_alerts`
   - etc.

**Advantages**:
- Single dashboard shows all boards
- Unified management and control
- More efficient use of Arduino Cloud quotas
- Easier to compare status across boards

**Disadvantages**:
- More complex property management
- All boards share same Thing configuration
- Harder to give different people access to different boards
- More complex firmware (need to identify which board)

### Option 3: Hybrid Approach

**Best for**: Mix of scenarios (e.g., personal boards + shared boards)

**Setup**:
1. Group related boards into Things:
   - "Personal LED Sectionals" (home + portable)
   - "Club LED Sectional" (shared hangar board)
2. Each Thing can have multiple devices if needed

### Recommendation: Use Separate Things

For most users, **Option 1 (Separate Things)** is recommended because:

1. **Simplicity**: Each board is completely independent
2. **Flexibility**: Different airport lists, settings, and configurations
3. **Reliability**: One board's issues don't affect others
4. **Security**: Can share access to specific boards without exposing others
5. **Maintenance**: Individual OTA updates and troubleshooting

### Implementation for Multiple Boards

#### Code Changes for Board Identification
Add this to your main .ino file to identify different boards:

```cpp
// Board identification - customize for each board
#define BOARD_NAME "Home"        // Change to "Hangar", "Regional", etc.
#define BOARD_LOCATION "Living Room"  // Descriptive location

// This will be included in cloud properties
String board_identifier = BOARD_NAME;
String board_location = BOARD_LOCATION;
```

#### Separate Credentials Files
For each board, create board-specific credential files:
- `arduino_secrets_home.h`
- `arduino_secrets_hangar.h` 
- `arduino_secrets_regional.h`

Then in your main code:
```cpp
// Select which credentials to use
#if defined(BOARD_CONFIG_HOME)
  #include "arduino_secrets_home.h"
#elif defined(BOARD_CONFIG_ASA)  
  #include "arduino_secrets_hangar.h"
#elif defined(BOARD_CONFIG_CUSTOM)
  #include "arduino_secrets_regional.h"
#endif
```

#### Dashboard Organization
Create a master dashboard that includes:
1. **Overview widgets** showing status of all boards
2. **Individual board sections** with detailed controls
3. **Comparative charts** showing differences between locations
4. **Alert summary** showing any issues across all boards

### Arduino Cloud Plan Considerations

**Free Plan Limits** (check current limits on Arduino website):
- Limited number of Things
- Limited number of properties per Thing
- Limited API calls per month
- Limited data retention

**Paid Plan Benefits**:
- More Things and properties
- Higher API rate limits
- Longer data retention
- Advanced features like webhooks
- Priority support

### Managing Multiple Boards

#### Naming Convention
Use consistent naming across boards:
- Things: "LED Sectional - [Location]"
- Devices: "ESP32-[Location]-[Date]"
- Properties: Keep same names across all boards for consistency

#### Firmware Management
1. **Use version control** (Git) with branches for different board configurations
2. **Tag releases** for easy rollback if issues occur
3. **Test updates** on one board before deploying to all
4. **Maintain configuration documentation** for each board

#### Monitoring Strategy
1. **Create alerts** for when any board goes offline
2. **Set up dashboards** that show comparative status
3. **Use webhooks** to get notifications about issues
4. **Regular health checks** - weekly review of all boards

## Step 3: Configure Your Project

### Update arduino_secrets.h
Edit the `arduino_secrets.h` file with your credentials:

```cpp
// WiFi Network Credentials (fallback)
#define SECRET_SSID "YOUR_WIFI_SSID"
#define SECRET_PASS "YOUR_WIFI_PASSWORD"

// Arduino IoT Cloud Device Credentials
#define SECRET_DEVICE_KEY "YOUR_DEVICE_SECRET_KEY"
```

**Important**: Add `arduino_secrets.h` to your `.gitignore` file to keep credentials secure.

### Enable/Disable Arduino Cloud
In the main `.ino` file, you can enable or disable Arduino Cloud:

```cpp
#define USE_ARDUINO_CLOUD true  // Set to false to disable Arduino Cloud features
```

## Step 4: Upload and Test

1. **Compile**: Verify your code compiles without errors
2. **Upload**: Upload to your device via USB
3. **Monitor**: Open Serial Monitor to see connection status
4. **Check Cloud**: Go to your Arduino Cloud dashboard to see live data

## Features

### Remote Monitoring
View real-time data from your LED sectional:
- Number of airports being monitored
- Active weather alerts (lightning, high winds)
- Device status and connectivity
- Last update timestamp

### Remote Control
Control your device from anywhere:
- Enable/disable lightning or wind alerts
- Override brightness settings  
- Force immediate weather updates
- Send configuration commands

### Configuration Commands
Send these commands via the `config_message` property:
- `restart` - Restart the device
- `reset_wifi` - Reset WiFi settings and restart

### OTA Updates
Arduino Cloud provides automatic OTA update capability:
1. Upload new firmware to Arduino Cloud
2. Device automatically downloads and installs updates
3. No physical access to device required

### Logging and History
Arduino Cloud automatically:
- Logs all property changes with timestamps
- Stores historical data for analysis
- Provides charts and graphs of your data
- Allows data export for external analysis

## Troubleshooting

### Cloud Connection Issues
- Verify WiFi credentials in `arduino_secrets.h`
- Check device credentials match your Arduino Cloud Thing
- Ensure device is properly associated with your Thing
- Monitor Serial output for connection errors

### Multiple Board Issues
- **Wrong credentials**: Ensure each board uses correct `arduino_secrets.h` file
- **Duplicate device names**: Each board needs unique device name in Arduino Cloud
- **Property conflicts**: If using single Thing approach, ensure property names don't conflict
- **Dashboard confusion**: Use clear board identification in device and property names
- **Version mismatches**: Keep firmware versions consistent across boards or document differences

### Library Issues
- Ensure all required libraries are installed
- Check library versions are compatible
- Try reinstalling ArduinoIoTCloud library

### Compilation Errors
- If you get include errors, set `USE_ARDUINO_CLOUD false` to disable cloud features
- Verify all required libraries are installed
- Check that `arduino_secrets.h` and `thingProperties.h` exist
- For multiple boards, verify correct secrets file is being included

### Property Synchronization
- Properties may take 30-60 seconds to synchronize initially
- Check that property names in code match exactly with cloud dashboard
- Verify property types match (int, String, boolean)
- For multiple boards, ensure each board's properties are updating independently

## Security Considerations

1. **Keep Credentials Secure**: Never commit `arduino_secrets*.h` files to version control
2. **Use Strong WiFi Passwords**: Ensure your WiFi network is secure
3. **Regular Updates**: Keep Arduino Cloud libraries updated
4. **Monitor Access**: Check your Arduino Cloud account regularly
5. **Multiple Board Security**:
   - Use different device keys for each board
   - Consider separate Arduino accounts for different locations/users
   - Limit dashboard sharing to necessary personnel only
   - Regular audit of who has access to which boards
   - Use descriptive but not location-specific names for public-facing elements

## Advanced Features

### Custom Dashboards
Create custom dashboards in Arduino Cloud:
- Add gauges for wind speeds and airport counts
- Create charts for historical trends
- Set up alerts for specific conditions
- Share dashboards with others

### API Integration
Arduino Cloud provides REST APIs:
- Access your data programmatically
- Integrate with other systems
- Create mobile apps
- Build custom monitoring solutions

### Webhooks
Set up webhooks for real-time notifications:
- Email alerts for severe weather
- SMS notifications for device issues
- Integration with home automation systems
- Custom web service calls

## Support

- **Arduino Cloud Documentation**: [docs.arduino.cc/cloud/](https://docs.arduino.cc/cloud/)
- **Community Forum**: [forum.arduino.cc](https://forum.arduino.cc)
- **Arduino Cloud Support**: Available through your Arduino account

## Cost Information

- **Arduino Cloud Free Plan**: Includes basic features with limitations
- **Arduino Cloud Paid Plans**: Available for advanced features and higher data limits
- **Check Current Pricing**: [arduino.cc/cloud/plans](https://www.arduino.cc/cloud/plans)