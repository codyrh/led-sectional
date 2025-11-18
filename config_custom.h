/*
  Board Configuration Template for Custom LED Sectional
  
  Copy this file and customize it for your specific board configuration.
  Rename it to something like "config_[your_name].h"
  
  To use this configuration:
  1. Set #define BOARD_CONFIG_FILE "config_custom.h" in main .ino file
  2. Customize all settings below for your specific setup
  3. Ensure this file is in the same directory as your .ino file
*/

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

// Board identification (for Arduino Cloud and debugging)
#define BOARD_NAME "Custom"
#define BOARD_LOCATION "Your Location"
#define BOARD_DESCRIPTION "Custom LED Sectional Configuration"

// LED Hardware Configuration
#define NUM_AIRPORTS 25          // Total number of LEDs
#define LED_TYPE WS2812B         // WS2811, WS2812B, etc.
#define COLOR_ORDER GRB          // RGB, GRB, BRG, etc.
#define BRIGHTNESS 60            // 0-255
#define DATA_PIN 5               // GPIO pin for LED data

// Weather Alert Thresholds (knots)
#define HIGH_WIND_THRESHOLD 25   // Winds or gusting winds above this cause LED to blink orange
#define WIND_THRESHOLD 15        // Winds above this but below HIGH_WIND_THRESHOLD cause fading/blinking

// Feature Enables
#define ENABLE_LIGHTNING_ALERTS true  // Enable lightning/thunderstorm detection
#define ENABLE_WIND_ALERTS true       // Enable wind alert detection
#define FADE_FOR_HIGH_WINDS true      // true = fade to 50%, false = blink black

// Light Sensor Configuration (optional)
#define USE_LIGHT_SENSOR false        // Enable automatic brightness adjustment
#define LIGHT_SENSOR_TSL2561 false    // true = digital TSL2561, false = analog sensor
#define MIN_BRIGHTNESS 20             // Minimum LED brightness (0-255)
#define MAX_BRIGHTNESS 20             // Maximum LED brightness (0-255)
#define MIN_LIGHT 16                  // Light sensor minimum reading
#define MAX_LIGHT 30                  // Light sensor maximum reading

// Timing Configuration (milliseconds)
#define LOOP_INTERVAL 1000        // ms between updates during active weather
#define REQUEST_INTERVAL 300000   // ms between METAR requests (300000 = 5 minutes)

// Airport Configuration
// IMPORTANT: First 5 entries are ALWAYS the legend: VFR, MVFR, IFR, LIFR, WVFR
// Customize the airport codes below for your sectional chart
const char* AIRPORTS[] = {
  // Legend LEDs (don't change these first 5)
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",
  
  // Airport LEDs - CUSTOMIZE THESE for your sectional
  // Use "NULL" for positions where you don't want an LED
  // Use 4-letter ICAO codes (US airports start with K)
  "KORD", "KMDW", "KPWK", "KIGQ", "KLOT",         // LEDs 6-10
  "KDPA", "KARR", "KJOT", "KC09", "KUGN",         // LEDs 11-15
  "KRPJ", "KENW", "KGYY", "KJVL", "KRAC",         // LEDs 16-20
  "KRFD", "KEFT", "KMSN", "KDKB", "KUES"          // LEDs 21-25
};

// Note: Ensure NUM_AIRPORTS matches the number of entries in AIRPORTS array above
// Count all entries including the 5 legend entries (VFR, MVFR, IFR, LIFR, WVFR)

#endif // BOARD_CONFIG_H