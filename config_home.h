/*
  Board Configuration for Home LED Sectional
  
  This file contains all the configuration settings specific to the Home board.
  Copy this file and modify for each different board configuration.
  
  To use this configuration:
  1. Set #define BOARD_CONFIG_FILE "config_home.h" in main .ino file
  2. Ensure this file is in the same directory as your .ino file
*/

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

// Board identification (for Arduino Cloud and debugging)
#define BOARD_NAME "Home"
#define BOARD_LOCATION "Foyer"
#define BOARD_DESCRIPTION "PNW LED Sectional"

// LED Hardware Configuration
#define NUM_AIRPORTS 30
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 70
#define DATA_PIN 5  // GPIO pin for LED data

// Weather Alert Thresholds
#define HIGH_WIND_THRESHOLD 20   // Winds or gusting winds above this cause LED to blink orange
#define WIND_THRESHOLD 15        // Winds above this but below HIGH_WIND_THRESHOLD cause fading/blinking

// Feature Enables
#define ENABLE_LIGHTNING_ALERTS true
#define ENABLE_WIND_ALERTS true
#define FADE_FOR_HIGH_WINDS true  // true = fade to 50%, false = blink black

// Light Sensor Configuration (if used)
#define USE_LIGHT_SENSOR false
#define LIGHT_SENSOR_TSL2561 false
#define MIN_BRIGHTNESS 20
#define MAX_BRIGHTNESS 20
#define MIN_LIGHT 16
#define MAX_LIGHT 30

// Timing Configuration
#define LOOP_INTERVAL 1000        // ms between updates during active weather
#define REQUEST_INTERVAL 300000   // ms between METAR requests (5 minutes)

// Airport Configuration
// IMPORTANT: First 5 entries are ALWAYS the legend: VFR, MVFR, IFR, LIFR, WVFR
const char* AIRPORTS[] = {
  // Legend LEDs (don't change these)
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",
  
  // Airport LEDs (customize these for your sectional)
  "KUIL", "NULL", "KHQM", "NULL", "KSHN",         // LEDs 6-10
  "KOLM", "KGRF", "KPLU", "KTCM", "KTIW",         // LEDs 11-15  
  "KPWT", "KSEA", "KRNT", "KBFI", "KPAE",         // LEDs 16-20
  "KAWO", "K0S9", "KNUW", "KBVS", "KBLI",         // LEDs 21-25
  "KORS", "KFHR", "CYYJ", "NULL", "KCLM"          // LEDs 26-30
};

// Note: Ensure NUM_AIRPORTS (30) matches the number of entries in AIRPORTS array above
// The array should have exactly 30 entries (5 legend + 25 airports)

#endif // BOARD_CONFIG_H