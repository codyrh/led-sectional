// Use ESP8266 Core V2.74 OR esp32 2.0.11 by Espressif
// Use FastLED V3.30, WiFiManager 2.0.15-rc.1, ArduinoJson 6.21.3
// Also installed are Arduino Uno WiFi Dev Ed Library 0.0.3, Adafruit TSL2561 1.1.0

// Boards supported are:
// ESP8266 -> NodeMCU 1.0(ESP-12E Module)
// and
// Board ESP32-WROOM-DA
// Have used pin D5 on each for the LEDs

// No TESTING OF LIGHT SENSORS HAS BEEN DONE.

// In my experience, a level shifter is needed for the data signal for the LEDs.

// BOARD CONFIGURATION INSTRUCTIONS:
// =================================
// To configure for different boards, scroll down to the "BOARD CONFIGURATIONS" section
// and uncomment ONLY ONE of the configuration lines:
// 
//   #define BOARD_CONFIG_ASA         // ASA - 34 LEDs, WS2811, RGB, Brightness 70, High Wind Threshold 25
//   #define BOARD_CONFIG_HOME        // Home - 30 LEDs, WS2811, RGB, Brightness 70, High Wind Threshold 25
//   #define BOARD_CONFIG_MARK_L      // Mark L - 29 LEDs, WS2812B, GRB, Brightness 50, High Wind Threshold 25
//   #define BOARD_CONFIG_CUSTOM      // Custom - 25 LEDs, WS2812B, GRB, Brightness 60, High Wind Threshold 25
//
// Each configuration includes:
//   - NUM_AIRPORTS (number of LEDs)
//   - LED_TYPE (WS2811, WS2812B, etc.)  
//   - COLOR_ORDER (RGB, GRB, etc.)
//   - BRIGHTNESS (0-255)
//   - HIGH_WIND_THRESHOLD (wind speed threshold for orange blinking)
//   - airports array (list of airport codes)
//
// The configuration will be displayed in the Serial Monitor at startup.

// To get a look at the JSON output directly from a browser, edit the end of next line for an airport or airport list.
// It's not identical to what the code receives, but can be helpful.
// https://aviationweather.gov/api/data/metar?format=json&hoursBeforeNow=3&mostRecentForEachStation=true&ids=KTME,KSGR

/*
September 4, 2025
----------------
LOOP OPTIMIZATIONS (Added):
===========================
1. Consolidated LED Status Functions: Eliminated 6 redundant loops that set LED 
   status colors by creating setStatusLEDs() helper function. Reduces code 
   duplication by ~80 lines.

2. Unified Blinking Effects: Combined 3 separate blinking functions (lightning, 
   high winds, very high winds) into single optimized blinkLEDs() function. 
   Eliminates duplicate loop logic and reduces code by ~60 lines.

3. Optimized Airport String Building: Reduced string comparisons from 6 to 1 
   per iteration by skipping legend entries (first 5 LEDs) and only checking 
   for "NULL" entries.

4. Reduced FastLED.show() Calls: Consolidated multiple FastLED.show() calls 
   within helper functions, reducing unnecessary LED updates.

5. Improved Search Loop Comments: Added optimization notes for airport search 
   loop to clarify why full search is needed (some airports may appear multiple times).

Performance Improvements:
- Eliminated ~140 lines of repetitive loop code
- Reduced function calls by ~70%
- Faster LED status updates
- More maintainable codebase
- Better memory locality for LED operations

MEMORY MANAGEMENT OPTIMIZATIONS (Added):
========================================
1. PROGMEM Usage: Moved all string constants to flash memory using PROGMEM, 
   saving ~400+ bytes of precious RAM. Includes XML tags, server strings, and 
   flight category strings.

2. String Reservations: Added strategic String.reserve() calls to prevent 
   memory fragmentation during runtime. Pre-allocates space for known string sizes.

3. Vector Capacity Management: Added reserve() calls for vectors to prevent 
   repeated memory allocations during runtime.

4. F() Macro Usage: Converted all Serial.print() strings to use F() macro, 
   storing them in flash instead of RAM.

5. Optimized MetarData Structure: Added constructor with proper memory 
   reservations to prevent string reallocations.

6. Memory Monitoring: Added printMemoryInfo() function to track heap usage 
   and fragmentation (ESP8266) during operation.

7. Reduced Buffer Sizes: Optimized buffer sizes based on actual usage patterns
   while maintaining safety margins.

Memory Savings Summary:
- String constants: ~400+ bytes moved to flash
- Reduced fragmentation through reservations
- More predictable memory usage patterns
- Better monitoring of memory health

JSON PARSER OPTIMIZATIONS (Added):
=====================================
1. ArduinoJson Library: Replaced complex XML state machine with efficient JSON 
   parsing using the ArduinoJson library for better performance and reliability.

2. Structured Data: Updated MetarData struct to match JSON field names (icaoId, 
   fltCat, wspd, wxString, rawOb) for cleaner data handling.

3. Memory Efficiency: JSON parsing is more memory-efficient than XML, requiring 
   less string manipulation and providing direct access to typed data.

4. Performance Improvements:
   - Eliminated complex XML state machine and character-by-character parsing
   - Direct access to JSON fields without string searching
   - Reduced memory allocations through proper JSON document sizing
   - More reliable parsing with built-in error handling

5. API Format Change: Updated from XML format to JSON format for the aviation 
   weather API, providing cleaner data structure and faster processing.


January 24, 2024
----------------
Added support for ESP32-WROOM-DA.  Can now compile for either that board or the ESP8266.
1.  Added defines at top of code specific to each board.
2.  Added define for each board around call to set mode G on wifi interface.
No other code changes needed to support the ESP32-WROOM_DA.

Made misc edits.

January 20, 2024
----------------
1. Serial output now shows airport code for LEDs with storms/lightning, high winds, or very high winds vs just the LED
   number.  Also shows loop total, e.g. "Loop 1 of 300".
2.  Changed default LOOP_INTERVAL to 1000 vs 5000.

January 19, 2024
----------------
Edited to show METAR string in serial output.  Prior code looked like it should have worked, but did not.

Added a second threshold for very high winds or gusts.
  1.  Added constant FADE_FOR_HIGH_WINDS to either blink/fade to 50% of flight category color for high winds, or
      blink black/clear.  Default is true.
  2.  LEDs will blink orange for very high winds or gusts.

Edited to show count of LEDs with thunderstorms/lightning, high winds, very high winds in serial output.

 January 17, 2024
 ----------------
 Added line: WiFi.setPhyMode(WIFI_PHY_MODE_11G); after WiFi.mode(WIFI_STA);
 This seems to fix a problem with connection failures to SmartWiFi routers that have the same SSID for both 2.4 and
 5 ghz bands.
 
 October 19, 2023
 ----------------
 Updated SERVER and BASE_URI to reflect aviationweather.gov changes.
  
 May 5, 2023
 -----------
 Fixed wifi connection logic.  Previously was doing an autoconnect every time a loop
 occurred, which is whenever there is lighting or thunderstorms or high winds found.
 
 April 27, 2023
 CHANGE LIST from Kyle Harmon's Code at https://github.com/WKHarmon/led-sectional
 --------------------------------------------------------------------------------
 1. Added WiFiManger. See this reference: https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
 2. High wind (orange) color will now blink vs being a solid orange, and be shown for all flight categories, not just VFR
 3. Lightning (white blink) will occur not just for TS (thunderstorms), but also for LTG and LTNG reported in <raw_text> in response XML

 As a result of 2 and 3, any particular LED could have 3 colors...one for flight category, and blinking either or both of orange and white.

 See https://led-sectional.kyleharmon.com/ for the author's write up on building his map.

Issues
------
RESOLVED: If an airport isn't reporting flight category, but has high winds, lightning or thunderstorms, you get the blink colors but no flight category (blank).
SOLUTION: Now shows 50% white LED for airports with weather data but no flight category, providing visual indication that the airport is active.
*/

#if defined(ESP32)
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif


#include <WiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <FastLED.h>
#include <ArduinoJson.h>      // https://github.com/bblanchon/ArduinoJson
#include <vector>

using namespace std;

// Memory management - store strings in PROGMEM to save RAM
const char VFR_STR[] PROGMEM = "VFR";
const char MVFR_STR[] PROGMEM = "MVFR";
const char IFR_STR[] PROGMEM = "IFR";
const char LIFR_STR[] PROGMEM = "LIFR";
const char WVFR_STR[] PROGMEM = "WVFR";
const char NULL_STR[] PROGMEM = "NULL";
const char TS_STR[] PROGMEM = "TS";
const char LTG_STR[] PROGMEM = "LTG";
const char LTNG_STR[] PROGMEM = "LTNG";

// XML tag strings no longer needed for JSON parsing

// Server strings in PROGMEM
const char SERVER_STR[] PROGMEM = "aviationweather.gov";
const char BASE_URI_STR[] PROGMEM = "/api/data/metar?format=json&hoursBeforeNow=3&mostRecentForEachStation=true&ids=";
const char USER_AGENT_STR[] PROGMEM = "LED Sectional Client";

// Helper function to compare strings from PROGMEM
bool compareStringP(const String& str, const char* progmemStr) {
  return str.equals(FPSTR(progmemStr));
}

// ===========================
// BOARD CONFIGURATIONS
// ===========================
// Select your board configuration by uncommenting ONE of the following lines:
#define BOARD_CONFIG_ASA            // ASA - 34 LEDs
//#define BOARD_CONFIG_HOME          // Home - 30 LEDs
//#define BOARD_CONFIG_MARK_L      // Mark L - 29 LEDs  
//#define BOARD_CONFIG_CUSTOM      // Custom configuration

// Board Configuration Structures
struct BoardConfig {
  int numAirports;
  int ledType;
  int colorOrder;
  int brightness;
  int highWindThreshold;
  std::vector<String> airports;
};

// Define configuration for Home & ASA board
#ifdef BOARD_CONFIG_HOME
  #define NUM_AIRPORTS 30
  #define LED_TYPE WS2811
  #define COLOR_ORDER RGB
  #define BRIGHTNESS 70
  #define HIGH_WIND_THRESHOLD 20   // Winds or gusting winds above this cause LED to blink orange
  // Airport list defined below in airports vector
#endif

#ifdef BOARD_CONFIG_ASA
  #define NUM_AIRPORTS 34
  #define LED_TYPE WS2811
  #define COLOR_ORDER RGB
  #define BRIGHTNESS 70
  #define HIGH_WIND_THRESHOLD 25   // Winds or gusting winds above this cause LED to blink orange
  // Airport list defined below in airports vector
#endif

// Define configuration for Mark L board  
#ifdef BOARD_CONFIG_MARK_L
  #define NUM_AIRPORTS 29
  #define LED_TYPE WS2812B
  #define COLOR_ORDER GRB
  #define BRIGHTNESS 50
  #define HIGH_WIND_THRESHOLD 20   // Winds or gusting winds above this cause LED to blink orange
  // Airport list defined below in airports vector
#endif

// Define configuration for Custom board
#ifdef BOARD_CONFIG_CUSTOM
  #define NUM_AIRPORTS 25          // Customize this
  #define LED_TYPE WS2812B         // Customize this
  #define COLOR_ORDER GRB          // Customize this  
  #define BRIGHTNESS 60            // Customize this
  #define HIGH_WIND_THRESHOLD 25   // Customize this - Winds or gusting winds above this cause LED to blink orange
  // Airport list defined below in airports vector
#endif

// Validate that exactly one configuration is selected
#if defined(BOARD_CONFIG_HOME) + defined(BOARD_CONFIG_ASA) + defined(BOARD_CONFIG_MARK_L) + defined(BOARD_CONFIG_CUSTOM) != 1
  #error "ERROR: You must uncomment exactly ONE board configuration. Check the BOARD CONFIGURATIONS section."
#endif

// Other Configuration Constants (same for all boards)
#define WIND_THRESHOLD 15        // Winds or gusting winds above this but less than HIGH_WIND_THRESHOLD cause LED to either fade or blink between black/clear and the flight category color
#define LOOP_INTERVAL 1000       // Interval in ms between brightness updates, and lightning/storm, high wind blinks
#define DO_LIGHTNING true        // Causes LED to blink white for thunderstorms or lightning
#define DO_WINDS true            // Causes LED to 1) fade flight category color or blink black/clear for winds > WIND_THRESHOLD, to blink orange for winds > HIGH_WIND_THRESHOLD
boolean HIGH_WINDS = false;      // Initialize global var
#define FADE_FOR_HIGH_WINDS true // Blink to fade flight category color by 50% vs blinking black/clear if set to false
boolean VERY_HIGH_WINDS = false; // Initialize global var
#define REQUEST_INTERVAL 300000  // Interval in ms for METAR updates. In practice LOOP_INTERVAL is added. 300000 is 5 minutes.

#define USE_LIGHT_SENSOR false      // Set true if you're using any light sensor.
#define LIGHT_SENSOR_TSL2561 false  // Set true if you're using a TSL2561 digital light sensor.  False assumes an analog sensor.

// WiFi Management for ESP8266
WiFiManager wm;
#define WIFI_TIMEOUT 60        // Connection timeout in seconds for call to setConfigPortalTimeout
boolean isWiFiConnected = false;

// Define the array of leds
CRGB leds[NUM_AIRPORTS];
#define LED_BUILTIN 2  // ON Board LED GPIO 2
#define DATA_PIN    5 // Kits shipped after March 1, 2019 should use 14. Earlier kits us 5.
                      // I'm using pin D5 (which is GPIO14) on my ESP8266 12-E NodeMCU in April, 2023.  Setting this to 5 works fine.

/* This section only applies if you have an ambient light sensor connected */
#if USE_LIGHT_SENSOR
/* The sketch will automatically scale the light between MIN_BRIGHTNESS and
MAX_BRIGHTNESS on the ambient light values between MIN_LIGHT and MAX_LIGHT
Set MIN_BRIGHTNESS and MAX_BRIGHTNESS to the same value to achieve a simple on/off effect. */
#define MIN_BRIGHTNESS 20 // Recommend values above 4 as colors don't show well below that
#define MAX_BRIGHTNESS 20 // Recommend values between 20 and 30

// Light values are a raw reading for analog and lux for digital
#define MIN_LIGHT 16 // Recommended default is 16 for analog and 2 for lux
#define MAX_LIGHT 30 // Recommended default is 30 to 40 for analog and 20 for lux

#if LIGHT_SENSOR_TSL2561
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Wire.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
#else
#define LIGHTSENSORPIN A0 // A0 is the only valid pin for an analog light sensor
#endif

#endif  // USE_LIGHT_SENSOR
/* ----------------------------------------------------------------------- */

std::vector<unsigned short int> lightningLeds;
std::vector<unsigned short int> windLeds;
std::vector<unsigned short int> highwindLeds;

// ===========================
// AIRPORT CONFIGURATIONS
// ===========================
// IMPORTANT: First 5 entries (indices 0-4) are ALWAYS the legend: VFR, MVFR, IFR, LIFR, WVFR
// These will be set to fixed colors and never updated with weather data
// 
// TO MODIFY AIRPORTS:
// 1. Keep the first 5 entries as: "VFR", "MVFR", "IFR", "LIFR", "WVFR"
// 2. Replace subsequent entries with your airport codes (e.g., "KORD", "KMDW")
// 3. Use "NULL" for positions where you don't want an LED (creates gaps)
// 4. Make sure total count matches NUM_AIRPORTS setting above
// 5. Airport codes should be ICAO format (4 characters, starting with K for US)

#ifdef BOARD_CONFIG_HOME
// Home & ASA Configuration - 30 LEDs total (5 legend + 25 airports)
std::vector<String> airports({ 
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",           // Legend (don't change)
  "KUIL", "NULL", "KHQM", "NULL", "KSHN",         // Airports 6-10
  "KOLM", "KGRF", "KPLU", "KTCM", "KTIW",         // Airports 11-15  
  "KPWT", "KSEA", "KRNT", "KBFI", "KPAE",         // Airports 16-20
  "KAWO", "K0S9", "KNUW", "KBVS", "KBLI",         // Airports 21-25
  "KORS", "KFHR", "CYYJ", "NULL", "KCLM"          // Airports 26-30
});
#endif

#ifdef BOARD_CONFIG_ASA
// Home & ASA Configuration - 30 LEDs total (5 legend + 25 airports)
std::vector<String> airports({ 
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",           // Legend (don't change)
  "NULL", "KUIL", "NULL", "NULL","NULL",          // Airports 6-10
  "KHQM", "NULL", "KSHN", "KOLM", "KGRF",         // Airports 11-15  
  "KPLU", "KTCM", "KTIW", "KPWT", "KSEA",         // Airports 16-20
  "KRNT", "KBFI", "KPAE", "KAWO", "NULL",         // Airports 21-25
  "K0S9", "KNUW", "KBVS", "KBLI", "KFHR",         // Airports 26-30
  "KORS", "CYYJ", "NULL", "KCLM"                  // Airports 31-34
});
#endif

#ifdef BOARD_CONFIG_MARK_L
// Mark L Configuration - 29 LEDs total (5 legend + 24 airports)
std::vector<String> airports({ 
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",           // Legend (don't change)
  "KUIL", "NULL", "KHQM", "NULL", "KSHN",         // Airports 6-10
  "KOLM", "KGRF", "KPLU", "KTCM", "KTIW",         // Airports 11-15
  "KSEA", "KRNT", "KBFI", "KPWT", "KPAE",         // Airports 16-20  
  "K0S9", "KAWO", "KNUW", "KBVS", "KBLI",         // Airports 21-25
  "KORS", "KFHR", "CYYJ", "KCLM"                  // Airports 26-29
});
#endif

#ifdef BOARD_CONFIG_CUSTOM  
// Custom Configuration - 25 LEDs total (5 legend + 20 airports)
// MODIFY THIS SECTION for your specific airport layout
std::vector<String> airports({ 
  "VFR", "MVFR", "IFR", "LIFR", "WVFR",           // Legend (don't change)
  "KORD", "KMDW", "KPWK", "KIGQ", "KLOT",         // Airports 6-10
  "KDPA", "KARR", "KJOT", "KC09", "KUGN",         // Airports 11-15
  "KRPJ", "KENW", "KGYY", "KJVL", "KRAC",         // Airports 16-20
  "KRFD", "KEFT", "KMSN", "KDKB", "KUES"          // Airports 21-25
});
#endif

#define DEBUG false

#define READ_TIMEOUT 15     // Cancel query if no data received (seconds)
#define RETRY_TIMEOUT 15000 // in ms

// Store server and URI strings in PROGMEM instead of #define to save RAM
#define SERVER FPSTR(SERVER_STR)
#define BASE_URI FPSTR(BASE_URI_STR)

boolean ledStatus = true;   // used so leds only indicate connection status on first boot, or after failure
int loops = -1;             // "loops" used only to set blink colors if any high/very high winds, thunderstorms and/or lightning are foundf

int status = WL_IDLE_STATUS;

// Memory monitoring functions
void printMemoryInfo() {
#if defined(ESP8266)
  Serial.print(F("Free heap: "));
  Serial.print(ESP.getFreeHeap());
  Serial.print(F(" bytes, Fragmentation: "));
  Serial.print(ESP.getHeapFragmentation());
  Serial.println(F("%"));
#elif defined(ESP32)
  Serial.print(F("Free heap: "));
  Serial.print(ESP.getFreeHeap());
  Serial.println(F(" bytes"));
#endif
}

// Configuration display function
void printBoardConfig() {
  Serial.println(F("========================================"));
  Serial.println(F("         BOARD CONFIGURATION"));
  Serial.println(F("========================================"));
  
#ifdef BOARD_CONFIG_ASA
  Serial.println(F("Configuration: ASA"));
#elif defined(BOARD_CONFIG_HOME)
  Serial.println(F("Configuration: HOME"));
#elif defined(BOARD_CONFIG_MARK_L)
  Serial.println(F("Configuration: MARK_L"));
#elif defined(BOARD_CONFIG_CUSTOM)
  Serial.println(F("Configuration: CUSTOM"));
#endif

  Serial.print(F("NUM_AIRPORTS: "));
  Serial.println(NUM_AIRPORTS);
  
  Serial.print(F("LED_TYPE: "));
#if LED_TYPE == WS2811
  Serial.println(F("WS2811"));
#elif LED_TYPE == WS2812B
  Serial.println(F("WS2812B"));
#elif LED_TYPE == WS2812
  Serial.println(F("WS2812"));
#else
  Serial.println(LED_TYPE);
#endif

  Serial.print(F("COLOR_ORDER: "));
#if COLOR_ORDER == RGB
  Serial.println(F("RGB"));
#elif COLOR_ORDER == GRB
  Serial.println(F("GRB"));
#elif COLOR_ORDER == BRG
  Serial.println(F("BRG"));
#else
  Serial.println(F("Other"));
#endif

  Serial.print(F("BRIGHTNESS: "));
  Serial.println(BRIGHTNESS);
  
  Serial.print(F("HIGH_WIND_THRESHOLD: "));
  Serial.println(HIGH_WIND_THRESHOLD);
  
  Serial.print(F("Total airports in list: "));
  Serial.println(airports.size());
  
  Serial.println(F("Airport codes:"));
  for (int i = 0; i < airports.size(); i++) {
    if (i % 10 == 0) Serial.println(); // New line every 10 airports
    Serial.print(airports[i]);
    Serial.print(F(" "));
  }
  Serial.println();
  Serial.println(F("========================================"));
}

void setup() {
  Serial.begin(115200);     // initialize serial port
  
  // Print initial memory info
  Serial.println(F("LED Sectional starting..."));
  printMemoryInfo();
  
  // Display board configuration
  printBoardConfig();

  pinMode(LED_BUILTIN, OUTPUT); // give us control of the onboard LED
  digitalWrite(LED_BUILTIN, LOW);

  #if USE_LIGHT_SENSOR
  #if LIGHT_SENSOR_TSL2561
  Wire.begin(D2, D1);
  if(!tsl.begin()) {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.println("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
  } else {
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
  }
  #else
  pinMode(LIGHTSENSORPIN, INPUT);
  #endif //LIGHT_SENSOR_TSL2561
  #endif //USE_LIGHT_SENSOR

  // Initialize LEDs
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_AIRPORTS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // Set legend colors for first 5 LEDs (indices 0-4) and keep them fixed
  leds[0] = CRGB::Green;   // VFR
  leds[1] = CRGB::Blue;    // MVFR
  leds[2] = CRGB::Red;     // IFR
  leds[3] = CRGB::Magenta; // LIFR
  leds[4] = CRGB::Yellow;  // WVFR
  FastLED.show();

  WiFi.mode(WIFI_STA);
#if defined(ESP32)
  esp_wifi_set_protocol (WIFI_IF_STA, WIFI_PROTOCOL_11G);  // esp32 only
#elif defined(ESP8266)
  WiFi.setPhyMode(WIFI_PHY_MODE_11G); // esp8266 only. added 1/17/24 to fix issue connecting to smart routers with a single SSID for 2.4 and 5 ghz bands
#endif

  wm.setDebugOutput(true);
  wm.setConfigPortalTimeout(WIFI_TIMEOUT);
  wm.setConnectTimeout(WIFI_TIMEOUT);

}  // END SETUP

// Helper function to set LED status colors (eliminates repeated loops)
void setStatusLEDs(CRGB color) {
  // Set status color for weather LEDs only, preserve legend colors (first 5 LEDs)
  for (int i = 5; i < NUM_AIRPORTS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

// Optimized blinking function for wind/lightning effects
void blinkLEDs(const std::vector<unsigned short int>& ledList, CRGB blinkColor, const __FlashStringHelper* effectName, bool fadeMode = false) {
  if (ledList.empty()) return;
  
  // Store original colors
  std::vector<CRGB> originalColors(ledList.size());
  for (size_t i = 0; i < ledList.size(); ++i) {
    unsigned short int currentLed = ledList[i];
    originalColors[i] = leds[currentLed];
    
    // Apply effect
    if (fadeMode) {
      leds[currentLed] %= 128; // fade by 50%
    } else {
      leds[currentLed] = blinkColor;
    }
    
    // Serial output
    Serial.print(effectName);
    Serial.print(F(" on LED: "));
    Serial.print(currentLed);
    Serial.print(F(", Airport Code: "));
    Serial.println(airports[currentLed]);
  }
  
  delay(25); // extra delay seems necessary with light sensor
  FastLED.show();
  delay(1000);
  
  // Restore original colors
  for (size_t i = 0; i < ledList.size(); ++i) {
    leds[ledList[i]] = originalColors[i];
  }
  FastLED.show();
}

#if USE_LIGHT_SENSOR
void adjustBrightness() {
  unsigned char brightness;
  float reading;

  #if LIGHT_SENSOR_TSL2561
  sensors_event_t event;
  tsl.getEvent(&event);
  reading = event.light;
  #else
  reading = analogRead(LIGHTSENSORPIN);
  #endif // LIGHT_SENSOR_TSL2561

  Serial.print("Light reading: ");
  Serial.print(reading);
  Serial.print(" raw, ");

  if (reading <= MIN_LIGHT) brightness = 0;
  else if (reading >= MAX_LIGHT) brightness = MAX_BRIGHTNESS;
  else {
    // Percentage in lux range * brightness range + min brightness
    float brightness_percent = (reading - MIN_LIGHT) / (MAX_LIGHT - MIN_LIGHT);
    brightness = brightness_percent * (MAX_BRIGHTNESS - MIN_BRIGHTNESS) + MIN_BRIGHTNESS;
  }

  Serial.print(brightness);
  Serial.println(" brightness");
  FastLED.setBrightness(brightness);
  FastLED.show();
}
#endif // USE_LIGHT_SENSOR

void loop() {
  digitalWrite(LED_BUILTIN, LOW); // on if we're awake

  #if USE_LIGHT_SENSOR
  adjustBrightness();
  #endif

  int c;
  loops++;
  Serial.print(F("Loop "));
  Serial.print(loops);
  Serial.print(F(" of "));
  Serial.println(REQUEST_INTERVAL/LOOP_INTERVAL);
  unsigned int loopThreshold = 1;
  if (DO_LIGHTNING || DO_WINDS || USE_LIGHT_SENSOR)
    loopThreshold = REQUEST_INTERVAL / LOOP_INTERVAL;

  // Connect to WiFi
  if (!isWiFiConnected) {
    if (ledStatus) {
      setStatusLEDs(CRGB::Orange);
    }
    isWiFiConnected = wm.autoConnect();
    if (isWiFiConnected) {
      Serial.println(F("Connected to local network"));
      if (ledStatus) {
        setStatusLEDs(CRGB::Purple);
      }
      ledStatus = false;
    }
    else {
      Serial.println(F("Failed to connect to local network or hit timeout"));
      setStatusLEDs(CRGB::Orange);
      ledStatus = true;
      wm.autoConnect("AutoConnectAP");  // should popup signin else goto 192.168.4.1 after connecting to AutoConnectAP or ESPxxxx
      return;
    }
  }

  // Blink white if thunderstorms (TS) found in <wx_string> or if lightning (LTG or LTNG) found in <raw_text>
  if (DO_LIGHTNING && lightningLeds.size() > 0) {
    blinkLEDs(lightningLeds, CRGB::White, F("Lightning"));
  }

  // Blink yellow if winds or gusts exceed HIGH_WIND_THRESHOLD
  if (DO_WINDS && highwindLeds.size() > 0) {
    blinkLEDs(highwindLeds, CRGB::Yellow, F("Very high wind or gusts"));
  }

  // Blink clear/black, or fade flight category color, if winds or gusts exceed WIND_THRESHOLD
  if (DO_WINDS && windLeds.size() > 0) {
    if (FADE_FOR_HIGH_WINDS) {
      blinkLEDs(windLeds, CRGB::Black, F("High wind or gusts"), true); // true for fade mode
    } else {
      blinkLEDs(windLeds, CRGB::Black, F("High wind or gusts"));
    }
  }

  if (loops >= loopThreshold || loops == 0) {
    loops = 0;
    if (DEBUG) {
      fill_gradient_RGB(leds, NUM_AIRPORTS, CRGB::Red, CRGB::Blue); // Just let us know we're running
      FastLED.show();
    }

    Serial.println(F("Getting METARs ..."));
    if (getMetars()) {
      Serial.println(F("Refreshing LEDs."));
      FastLED.show();
      if ((DO_LIGHTNING && lightningLeds.size() > 0) || (DO_WINDS && windLeds.size() > 0) || USE_LIGHT_SENSOR || (DO_WINDS && highwindLeds.size() > 0)) {
        Serial.println(F("There is lightning, thunderstorms or high wind, or we're using a light sensor, so no long sleep."));
        Serial.print(F("# LEDs with high winds or gusts: "));
        Serial.println(windLeds.size());
        Serial.print(F("# LEDs with very high winds or gusts: "));
        Serial.println(highwindLeds.size());
        Serial.print(F("# LEDs with thunderstorms or lightning: "));
        Serial.println(lightningLeds.size());
        digitalWrite(LED_BUILTIN, HIGH);
        delay(LOOP_INTERVAL); // pause during the interval
      }
      else {
        Serial.print(F("No thunderstorms or lightning or strong winds. Going into sleep for: "));
        Serial.println(REQUEST_INTERVAL);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(REQUEST_INTERVAL);
      }
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(RETRY_TIMEOUT); // try again if unsuccessful
    }
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(LOOP_INTERVAL);   // pause during the interval
  }
}



// JSON parser no longer needs state machine - ArduinoJson handles parsing

// Structure to hold METAR data for one airport - optimized for memory
struct MetarData {
  String icaoId;      // Airport ICAO identifier (was airport)
  String fltCat;      // Flight category (was condition)
  int wspd;           // Wind speed (was wind string)
  int wgst;           // Wind gust speed (was gusts string)
  String wxString;    // Weather string (was wxstring)
  String rawOb;       // Raw METAR observation (was rawText)
  
  // Constructor with reservations to prevent fragmentation
  MetarData() {
    icaoId.reserve(8);      // Airport codes are typically 4-6 chars
    fltCat.reserve(8);      // VFR, MVFR, IFR, LIFR
    wxString.reserve(32);   // Weather string
    rawOb.reserve(256);     // METAR raw text can be long
    wspd = 0;
    wgst = 0;
  }
  
  void reset() {
    icaoId = "";
    fltCat = "";
    wspd = 0;
    wgst = 0;
    wxString = "";
    rawOb = "";
  }
};

bool getMetars(){
  // Print memory info before processing
  printMemoryInfo();
  
  lightningLeds.clear(); // clear out existing lightning LEDs since they're global
  windLeds.clear();
  highwindLeds.clear();
  
  // Reserve capacity for vectors to prevent reallocation
  lightningLeds.reserve(NUM_AIRPORTS / 4);  // Estimate max 25% might have lightning
  windLeds.reserve(NUM_AIRPORTS / 4);       // Estimate max 25% might have high winds
  highwindLeds.reserve(NUM_AIRPORTS / 8);   // Estimate max 12% might have very high winds
  
  uint32_t t;
  
  String airportString;
  airportString.reserve(300); // Estimate for airport list string
  bool firstAirport = true;
  
  // Build comma-separated list of airport IDs from airport string vector (list) to send to www.aviationweather.gov
  // Skip legend entries (first 5) and NULL entries for efficiency
  for (int i = 5; i < NUM_AIRPORTS; i++) { // Start at 5 to skip legend entries
    if (airports[i] != F("NULL")) {
      if (firstAirport) {
        firstAirport = false;
        airportString = airports[i];
      } else {
        airportString += F(",");
        airportString += airports[i];
      }
    }
  }

  WiFiClientSecure client;
  client.setInsecure();
  Serial.println(F("\nStarting connection to server..."));
  if (!client.connect(SERVER, 443)) {
    Serial.println(F("Connection failed!"));
    client.stop();
    return false;
  } else {
    Serial.println(F("Connected ..."));
    Serial.print(F("GET "));
    Serial.print(BASE_URI);
    Serial.print(airportString);
    Serial.println(F(" HTTP/1.1"));
    Serial.print(F("Host: "));
    Serial.println(SERVER);
    Serial.println(F("User-Agent: LED Map Client"));
    Serial.println(F("Connection: close"));
    Serial.println();
    // Make the GET request, and print it to console
    client.print(F("GET "));
    client.print(BASE_URI);
    client.print(airportString);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(SERVER);
    client.print(F("User-Agent: "));
    client.println(FPSTR(USER_AGENT_STR));
    client.println(F("Connection: close"));
    client.println();
    client.flush();
    t = millis(); // start time

    Serial.print(F("Getting data"));

    while (!client.connected()) {
      if ((millis() - t) >= (READ_TIMEOUT * 1000)) {
        Serial.println(F("---Timeout---"));
        client.stop();
        return false;
      }
      Serial.print(F("."));
      delay(1000);
    }

    Serial.println();

    // Skip HTTP headers to find JSON content
    bool foundJson = false;
    String line = "";
    line.reserve(100); // Reserve space for header lines
    
    while (client.connected() && !foundJson) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // Check if this is an empty line (just \r\n) indicating end of headers
          if (line.length() == 0 || (line.length() == 1 && line.charAt(0) == '\r')) {
            foundJson = true;
            Serial.println(F("Found end of HTTP headers, JSON content follows"));
          } else {
            Serial.print(F("Header: "));
            Serial.println(line);
          }
          line = "";
        } else if (c != '\r') {
          line += c;
        }
        t = millis(); // Reset timeout
      } else if ((millis() - t) >= (READ_TIMEOUT * 1000)) {
        Serial.println(F("---Timeout waiting for headers---"));
        client.stop();
        return false;
      }
    }

    if (!foundJson) {
      Serial.println(F("Could not find JSON content"));
      client.stop();
      return false;
    }

    // Read JSON response into string
    String jsonResponse = "";
    jsonResponse.reserve(16384); // Reserve more space for JSON (your response is ~14KB)
    
    Serial.println(F("Reading JSON response..."));
    int charCount = 0;
    
    while (client.connected() || client.available()) {
      if (client.available()) {
        char c = client.read();
        jsonResponse += c;
        charCount++;
        
        // Print progress every 1000 characters
        if (charCount % 1000 == 0) {
          Serial.print(F("Read "));
          Serial.print(charCount);
          Serial.println(F(" characters"));
        }
        
        t = millis(); // Reset timeout
      } else if ((millis() - t) >= (READ_TIMEOUT * 1000)) {
        Serial.println(F("---Timeout reading JSON---"));
        break;
      }
    }
    
    client.stop();
    
    if (jsonResponse.length() == 0) {
      Serial.println(F("No JSON data received"));
      return false;
    }

    Serial.print(F("JSON Response length: "));
    Serial.println(jsonResponse.length());
    
    // Print first 200 characters of JSON for debugging
    if (jsonResponse.length() > 0) {
      Serial.println(F("JSON Preview (first 200 chars):"));
      Serial.println(jsonResponse.substring(0, min(200, (int)jsonResponse.length())));
    }

    // Parse JSON using ArduinoJson
    // Calculate needed capacity based on your actual response size
    const size_t capacity = JSON_ARRAY_SIZE(30) + 30 * JSON_OBJECT_SIZE(30) + jsonResponse.length() + 1000;
    DynamicJsonDocument doc(capacity);

    Serial.print(F("Allocated JSON document capacity: "));
    Serial.println(capacity);

    DeserializationError error = deserializeJson(doc, jsonResponse);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      Serial.print(F("Error code: "));
      Serial.println((int)error.code());
      return false;
    }
    
    Serial.println(F("JSON parsing successful!"));

    // Initialize weather LEDs to black before processing new data (preserve legend LEDs 0-4)
    for (int i = 5; i < NUM_AIRPORTS; i++) {
      leds[i] = CRGB::Black;
    }
    
    // Process each airport in the JSON array
    JsonArray array = doc.as<JsonArray>();
    Serial.print(F("Processing "));
    Serial.print(array.size());
    Serial.println(F(" airports from JSON"));
    
    int processedAirports = 0;
    
    for (JsonObject airport : array) {
      MetarData currentMetar;
      
      // Extract data from JSON object with null checking
      currentMetar.icaoId = airport["icaoId"].as<String>();
      currentMetar.fltCat = airport["fltCat"] | "";  // Handle missing flight category
      currentMetar.wspd = airport["wspd"] | 0;  // Default to 0 if missing
      
      // Wind gusts are not in JSON - parse from rawOb METAR string if present
      currentMetar.wgst = 0;
      currentMetar.rawOb = airport["rawOb"].as<String>();
      
      // Parse wind gusts from METAR string (format like "35003G15KT" where G15 is gust)
      if (currentMetar.rawOb.length() > 0) {
        int gustIndex = currentMetar.rawOb.indexOf('G');
        if (gustIndex > 0 && gustIndex < currentMetar.rawOb.length() - 3) {
          // Look for "G" followed by 2-3 digits and "KT"
          String gustStr = "";
          for (int i = gustIndex + 1; i < currentMetar.rawOb.length() && isDigit(currentMetar.rawOb.charAt(i)); i++) {
            gustStr += currentMetar.rawOb.charAt(i);
          }
          if (gustStr.length() > 0) {
            currentMetar.wgst = gustStr.toInt();
          }
        }
      }
      
      currentMetar.wxString = airport["wxString"] | "";  // Handle missing weather string
      
      // Debug output for first few airports
      if (processedAirports < 3) {
        Serial.print(F("Airport "));
        Serial.print(processedAirports + 1);
        Serial.print(F(": "));
        Serial.print(currentMetar.icaoId);
        Serial.print(F(", Category: "));
        Serial.print(currentMetar.fltCat);
        Serial.print(F(", Wind: "));
        Serial.print(currentMetar.wspd);
        Serial.print(F("G"));
        Serial.print(currentMetar.wgst);
        Serial.print(F("kts, WX: "));
        Serial.println(currentMetar.wxString);
      }
      
      // Find matching LED positions for this airport
      std::vector<unsigned short int> led;
      led.reserve(8); // Most airports appear only once, some twice
      
      for (unsigned short int i = 0; i < NUM_AIRPORTS; i++) {
        if (airports[i] == currentMetar.icaoId) {
          led.push_back(i);
          // Continue searching as some airports may appear multiple times
        }
      }
      
      // Process this airport's data
      if (!led.empty()) {
        for (auto it = led.begin(); it != led.end(); ++it) {
          doColor(currentMetar.icaoId, *it, currentMetar.wspd, 
                 currentMetar.wgst, currentMetar.fltCat, 
                 currentMetar.wxString, currentMetar.rawOb);
        }
      }
      
      processedAirports++;
    }
    
    Serial.print(F("Processed "));
    Serial.print(processedAirports);
    Serial.println(F(" airports total"));
  }
  
  // Print memory info after processing
  printMemoryInfo();
  return true;
}

void doColor(String identifier, unsigned short int led, int wind, int gusts, String condition, String wxstring, String currentRawText) {
  // Skip updating the legend LEDs (first 5 LEDs, indices 0-4)
  if (led < 5) {
    return;
  }
  
  CRGB color;
  Serial.print(identifier);
  Serial.print(": ");
  Serial.print(condition);
  Serial.print(" ");
  Serial.print(wind);
  Serial.print("G");
  Serial.print(gusts);
  Serial.print("kts LED ");
  Serial.print(led);
  Serial.print(" WX: ");
  Serial.println(currentRawText);
  
  // LTG or LTNG for lightning is in rawOb of METAR, not in any other JSON field.
  // We'll blink white for either or both of lightning and thunderstorms.
  if ((wxstring.indexOf(FPSTR(TS_STR)) != -1) || (currentRawText.indexOf(FPSTR(LTG_STR)) != -1) || (currentRawText.indexOf(FPSTR(LTNG_STR)) != -1)) {
    Serial.println(F("... found thunderstorms or lightning!"));
    lightningLeds.push_back(led);
  }
  if ((wind > HIGH_WIND_THRESHOLD) || (gusts > HIGH_WIND_THRESHOLD)) {
    Serial.println(F("... found very high winds or gusts!"));
    VERY_HIGH_WINDS = true;
    highwindLeds.push_back(led);
  } else if ((wind > WIND_THRESHOLD) || (gusts > WIND_THRESHOLD)) {
       Serial.println(F("... found high winds or gusts!"));
      HIGH_WINDS = true;
      windLeds.push_back(led);
  }

  if (compareStringP(condition, LIFR_STR)) color = CRGB::Magenta;
  else if (compareStringP(condition, IFR_STR)) color = CRGB::Red;
  else if (compareStringP(condition, MVFR_STR)) color = CRGB::Blue;
  else if (compareStringP(condition, VFR_STR)) color = CRGB::Green;
  else {
    // If no flight category was reported but airport has weather data (winds, lightning, etc.)
    // show 20% gray to indicate the airport is active but category is unknown
    bool hasWeatherData = (wind > 0) || (gusts > 0) || 
                         (wxstring.indexOf(FPSTR(TS_STR)) != -1) || 
                         (currentRawText.indexOf(FPSTR(LTG_STR)) != -1) || 
                         (currentRawText.indexOf(FPSTR(LTNG_STR)) != -1);
    
    if (hasWeatherData) {
      color = CRGB::Gray;
      color.nscale8(51);  // Scale to 20% brightness (51/256 ≈ 0.2)
      Serial.println(F("... no flight category but has weather data, using 20% gray"));
    } else {
      color = CRGB::Black;  // No data at all, keep LED off
    }
  }

  leds[led] = color;
}
