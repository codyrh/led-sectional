/*
  Arduino Secrets File for Mark L LED Sectional
  
  This file contains sensitive information like WiFi credentials and Arduino Cloud device credentials
  for the Mark L board configuration.
  
  DO NOT commit this file to version control - it's already in .gitignore
*/

// WiFi Network Credentials (used as fallback if WiFiManager fails)
#define SECRET_SSID "YOUR_WIFI_SSID_MARK_L"
#define SECRET_PASS "YOUR_WIFI_PASSWORD_MARK_L"

// Arduino IoT Cloud Device Credentials for Mark L Board
// Get these from your "LED Sectional - Mark L" Thing configuration
#define SECRET_DEVICE_KEY "YOUR_MARK_L_DEVICE_SECRET_KEY"

// Board-specific Thing ID (for API access and identification)
#define THING_ID "your-mark-l-thing-id-here"
#define DEVICE_NAME "ESP32-MarkL-LEDSectional"