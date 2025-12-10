/*
  Arduino Secrets File
  
  This file contains sensitive information like WiFi credentials and Arduino Cloud device credentials.
  DO NOT commit this file to version control - add it to .gitignore
  
  To set up Arduino Cloud:
  1. Go to https://create.arduino.cc/iot/
  2. Create a new Thing
  3. Add a device (your ESP8266 or ESP32)
  4. Generate device credentials and copy them here
  5. Create properties for your dashboard
*/

// WiFi Network Credentials (used as fallback if WiFiManager fails)
#define SECRET_SSID "YOUR_WIFI_SSID"
#define SECRET_PASS "YOUR_WIFI_PASSWORD"

// Arduino IoT Cloud Device Credentials
// Get these from your Arduino Cloud Thing configuration
#define SECRET_DEVICE_KEY "YOUR_DEVICE_SECRET_KEY"