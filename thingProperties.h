/*
  Arduino IoT Cloud Properties Configuration
  
  This file defines the cloud variables that will be synchronized with Arduino IoT Cloud.
  These properties can be monitored and controlled from the Arduino Cloud dashboard.
  
  To use this:
  1. Create corresponding properties in your Arduino Cloud Thing dashboard
  2. Make sure property names match exactly
  3. Set appropriate data types and permissions for each property
*/

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

// Forward declarations for callback functions
void onEnableLightningAlertsChange();
void onEnableWindAlertsChange();
void onBrightnessOverrideChange();
void onForceUpdateChange();
void onConfigMessageChange();

// Cloud Properties - these will sync with Arduino IoT Cloud dashboard
// Create these properties in your Arduino Cloud Thing with matching names and types

// Status properties (READ ONLY from cloud dashboard)
int airports_processed = 0;          // Number of airports processed in last update
int lightning_alerts = 0;           // Number of airports with lightning/thunderstorms  
int high_wind_alerts = 0;           // Number of airports with high winds
int very_high_wind_alerts = 0;      // Number of airports with very high winds
String device_status = "Starting";  // Current device status
String last_update = "";            // Timestamp of last METAR update
String log_message = "";            // Latest log message
bool cloud_connected = false;       // Arduino Cloud connection status

// Control properties (READ/WRITE from cloud dashboard)
bool enable_lightning_alerts = true;   // Enable/disable lightning detection
bool enable_wind_alerts = true;        // Enable/disable wind alerts  
int brightness_override = -1;          // Override brightness (-1 = auto, 0-255 = manual)
bool force_update = false;             // Force immediate METAR update
String config_message = "";           // Configuration/control messages

// Connection handler for WiFi
#if defined(ESP8266)
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_SSID, SECRET_PASS);
#elif defined(ESP32)
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_SSID, SECRET_PASS);
#endif

void initProperties() {
  
  // Add cloud properties with callbacks
  ArduinoCloud.addProperty(airports_processed, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(lightning_alerts, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(high_wind_alerts, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(very_high_wind_alerts, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(device_status, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(last_update, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(log_message, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloud_connected, READ, ON_CHANGE, NULL);
  
  // Control properties with callbacks
  ArduinoCloud.addProperty(enable_lightning_alerts, READWRITE, ON_CHANGE, onEnableLightningAlertsChange);
  ArduinoCloud.addProperty(enable_wind_alerts, READWRITE, ON_CHANGE, onEnableWindAlertsChange);
  ArduinoCloud.addProperty(brightness_override, READWRITE, ON_CHANGE, onBrightnessOverrideChange);
  ArduinoCloud.addProperty(force_update, READWRITE, ON_CHANGE, onForceUpdateChange);
  ArduinoCloud.addProperty(config_message, READWRITE, ON_CHANGE, onConfigMessageChange);
}

// Callback functions for cloud property changes
void onEnableLightningAlertsChange() {
  Serial.print(F("Lightning alerts "));
  Serial.println(enable_lightning_alerts ? F("enabled") : F("disabled"));
  
  log_message = "Lightning alerts " + String(enable_lightning_alerts ? "enabled" : "disabled");
}

void onEnableWindAlertsChange() {
  Serial.print(F("Wind alerts "));
  Serial.println(enable_wind_alerts ? F("enabled") : F("disabled"));
  
  log_message = "Wind alerts " + String(enable_wind_alerts ? "enabled" : "disabled");
}

void onBrightnessOverrideChange() {
  // Note: Actual brightness changes are handled in the main loop
  // This callback just logs the change request
  if (brightness_override >= 0 && brightness_override <= 255) {
    Serial.print(F("Brightness override requested: "));
    Serial.println(brightness_override);
    log_message = "Brightness override: " + String(brightness_override);
  } else if (brightness_override == -1) {
    Serial.println(F("Automatic brightness requested"));
    log_message = "Brightness: automatic mode";
  }
  // The main loop will check brightness_override and apply changes
}

void onForceUpdateChange() {
  if (force_update) {
    Serial.println(F("Force update requested from cloud"));
    log_message = "Force update requested";
    
    // Reset the flag
    force_update = false;
    
    // Force the main loop to update immediately by resetting loops counter
    extern int loops;
    loops = 1000;  // This will trigger an immediate METAR update
  }
}

void onConfigMessageChange() {
  if (config_message.length() > 0) {
    Serial.print(F("Config message received: "));
    Serial.println(config_message);
    
    log_message = "Config: " + config_message;
    
    // Process configuration commands
    if (config_message.startsWith("restart")) {
      log_message = "Restart requested - rebooting device";
      delay(1000);
      ESP.restart();
    }
    else if (config_message.startsWith("reset_wifi")) {
      log_message = "WiFi reset requested";
      // Reset WiFiManager settings - reference external WiFiManager instance
      Serial.println(F("WiFi reset requested - will restart"));
      ESP.restart();
    }
    
    // Clear the message after processing
    config_message = "";
  }
}