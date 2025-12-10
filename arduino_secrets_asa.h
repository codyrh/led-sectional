/*
  Arduino Secrets File for ASA LED Sectional
  
  This file contains sensitive information like WiFi credentials and Arduino Cloud device credentials
  for the ASA board configuration.
  
  DO NOT commit this file to version control - it's already in .gitignore
*/

// WiFi Network Credentials (used as fallback if WiFiManager fails)
#define SECRET_SSID "YOUR_WIFI_SSID_ASA"
#define SECRET_PASS "YOUR_WIFI_PASSWORD_ASA"

// Arduino IoT Cloud Device Credentials for ASA Board
// Get these from your "LED Sectional - ASA" Thing configuration
#define SECRET_DEVICE_KEY "YOUR_ASA_DEVICE_SECRET_KEY"

// Board-specific Thing ID (for API access and identification)
#define THING_ID "your-asa-thing-id-here"
#define DEVICE_NAME "ESP32-ASA-LEDSectional"