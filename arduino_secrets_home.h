/*
  Arduino Secrets File for Home LED Sectional
  
  This file contains sensitive information like WiFi credentials and Arduino Cloud device credentials
  for the Home board configuration.
  
  DO NOT commit this file to version control - it's already in .gitignore
  
  To set up Arduino Cloud for this board:
  1. Go to https://create.arduino.cc/iot/
  2. Create a new Thing named "LED Sectional - Home"
  3. Add a device (your ESP8266 or ESP32) for this specific board
  4. Generate device credentials and copy them here
  5. Create properties for your dashboard
  
  Copy this file for each board:
  - arduino_secrets_home.h (this file)
  - arduino_secrets_asa.h
  - arduino_secrets_mark_l.h
  - etc.
*/

// WiFi Network Credentials - NOT NEEDED with WiFiManager approach
// WiFiManager handles all WiFi connections automatically
// These are kept for compatibility but won't be used
#define SECRET_SSID ""  // Not used - WiFiManager handles WiFi
#define SECRET_OPTIONAL_PASS ""  // Not used - WiFiManager handles WiFi

// Arduino IoT Cloud Device Credentials for Home Board
// These should match exactly what Arduino Cloud generated for you
#define SECRET_DEVICE_KEY "fMynSZBfqVDpwz?BloWi7OMkj"   // Device Secret Key from Arduino Cloud

// Thing ID for identification (optional, from Thing URL)
#define THING_ID "eb40615b-3f50-4289-aa0b-87cd84b0492a"
#define DEVICE_NAME "Home"