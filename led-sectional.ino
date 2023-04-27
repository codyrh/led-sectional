// To get a look at the XML output directly from a browser, edit the end of next line for an airport or airport list.
// It's not identical to what the code receives, but can help a lot.
// https://aviationweather.gov/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xmlreading&hoursBeforeNow=3&mostRecentForEachStation=true&stationString=KCLL,KOCF

/*
 April 27, 2023
 CHANGE LIST from Kyle Harmon's Code at https://github.com/WKHarmon/led-sectional
 --------------------------------------------------------------------------------
 1. Added WiFiManger
	See this reference: https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
	This has two benefits:
		a) Permits easy setup if moving to a new WiFi network,
		b) Makes a WiFi connection reliably vs being problematic with some routers that won't connect without forcing 802.11g, e.g.
           WiFi.setPhyMode(WIFI_PHY_MODE_11G); // from a tip at https://github.com/esp8266/Arduino/issues/8299
           Good reference: https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
 2. High wind (orange) color will now blink vs being a solid orange, and be shown for all flight categories, not just VFR
 3. Lightning (white blink) will occur not just for TS (thunderstorms), but also for LTG and LTNG reported in <raw_text> in response XML

 As a result of 2 and 3, any particular LED could have 3 colors...one for flight category, and blinking either or both of orange and white.
*/

// Use EXP8266 Core V2.74, and FastLED V3.30


/*
Issues
------
IF an airport isn't reporting flight category, but has high winds, lightning or thunderstorms, you get the blink colors but no flight category (blank).
Better behavior might be to skip that LED?

Still need to test winds, lightning and storms at same airport*/

#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <vector>
#include <DNSServer.h>        // for WiFiManager
#include <ESP8266WebServer.h> // for WiFiManager
#include <WiFiManager.h>      // https://github.com/tzapu/WiFiManager
using namespace std;

#define FASTLED_ESP8266_RAW_PIN_ORDER

#define NUM_AIRPORTS 25        // This is the number of airports in list, including nulls, NOT # LEDs in string.
#define WIND_THRESHOLD 15      // Winds or gusting winds above this cause LED to blink orange
#define LOOP_INTERVAL 5000     // Interval in ms between brightness updates, and lightning/storm, high wind blinks
#define DO_LIGHTNING true      // Causes LED to blink white for thunderstorms or lightning
#define DO_WINDS true          // Causes LED to blink orange if winds exceed WIND_THRESHOLD
boolean HIGH_WINDS = false;    // Initialize global var
#define REQUEST_INTERVAL 60000 // Interval in ms for METAR updates. In practice LOOP_INTERVAL is added.

#define USE_LIGHT_SENSOR false      // Set true if you're using any light sensor.
#define LIGHT_SENSOR_TSL2561 false  // Set true if you're using a TSL2561 digital light sensor.  False assumes an analog sensor.

// WiFi Management for ESP8266
WiFiManager wm;
#define WIFI_TIMEOUT 60        // Connection timeout in seconds for call to setConfigPortalTimeout

// Define the array of leds
CRGB leds[NUM_AIRPORTS];
#define DATA_PIN    5 // Kits shipped after March 1, 2019 should use 14. Earlier kits us 5.
                      // I'm using pin D5 (which is GPIO14) on my ESP8266 12-E NodeMCU in April, 2023.  Setting this to 5 works fine.

//WS2812 and RGB for LED roll
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 100 // 20-30 suggested for LED strip, 3+ times that for bulbs.
                       // If using a light sensor, this is the initial brightness on boot.

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

#endif
/* ----------------------------------------------------------------------- */

std::vector<unsigned short int> lightningLeds;
std::vector<unsigned short int> windLeds;

std::vector<String> airports({
"KDEN", //1  Note LED # in serial output is 1 less, e.g. third in this list will be LED 2.
"KORD", //2
"KMAI", //3 
"KBGE",
"KT41", //4
"KEFD", //5
"KHOU", //6
"KLVJ", //7
"KAXH", //8
"KSGR", //9
"KBYY", //10
"KARM", //11
"KELA", //12
"KTME", //13
"KDWH", //14
"KIAH", //15
"KT78", //16
"K6R3", //17
"KCXO", //18
"KUTS", //19
"KCFD", //20
"KCLL", //21
"K11R", //22
"KLBX", //23
"KGLS"  //24
});

#define DEBUG false

#define READ_TIMEOUT 15     // Cancel query if no data received (seconds)
#define RETRY_TIMEOUT 15000 // in ms

#define SERVER "www.aviationweather.gov"
#define BASE_URI "/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecentForEachStation=true&stationString="

boolean ledStatus = true;   // used so leds only indicate connection status on first boot, or after failure
int loops = -1;             // "loops" used only to set blink colors for high winds, thunderstorms and/or lightning, if any found

int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);     // initial serial port

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
  #endif
  #endif

  // Initialize LEDs
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_AIRPORTS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}  // END SETUP

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
  #endif

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
#endif

void loop() {
  digitalWrite(LED_BUILTIN, LOW); // on if we're awake

  #if USE_LIGHT_SENSOR
  adjustBrightness();
  #endif

  int c;
  loops++;
  Serial.print("Loop: ");
  Serial.println(loops);
  unsigned int loopThreshold = 1;
  if (DO_LIGHTNING || DO_WINDS || USE_LIGHT_SENSOR)
    loopThreshold = REQUEST_INTERVAL / LOOP_INTERVAL;

  // Connect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    if (ledStatus) fill_solid(leds, NUM_AIRPORTS, CRGB::Orange); // indicate status with LEDs, but only on first run or error
    FastLED.show();
    WiFi.mode(WIFI_STA);
    wm.setConfigPortalTimeout(WIFI_TIMEOUT);
  }
  if (wm.autoConnect()) {
    Serial.println("Connected to local network");
    if (ledStatus) fill_solid(leds, NUM_AIRPORTS, CRGB::Purple);
    FastLED.show();
    ledStatus = false;
  }
  else {
    Serial.println("Failed to connect to local network or hit timeout");
    fill_solid(leds, NUM_AIRPORTS, CRGB::Orange);
    FastLED.show();
    ledStatus = true;
    wm.autoConnect("AutoConnectAP");  // should popup signin else goto 192.168.4.1 after connecting to AutoConnectAP or ESPxxxx
    return;
  }

  // Blink white if thunderstorms (TS) found in <wx_string> or if lightning (LTG or LTNG) found in <raw_text>
  if (DO_LIGHTNING && lightningLeds.size() > 0) {
    std::vector<CRGB> lightning(lightningLeds.size());
      for (unsigned short int i = 0; i < lightningLeds.size(); ++i) {
      unsigned short int currentLed = lightningLeds[i];
      lightning[i] = leds[currentLed]; // temporarily store original color
      leds[currentLed] = CRGB::White;  // set to white briefly
      Serial.print("Lightning on LED: ");
      Serial.println(currentLed);
    }
    delay(25); // extra delay seems necessary with light sensor
    FastLED.show();
    delay(1000);
    for (unsigned short int i = 0; i < lightningLeds.size(); ++i) {
      unsigned short int currentLed = lightningLeds[i];
      leds[currentLed] = lightning[i]; // restore original color
    }
    FastLED.show();
  }

  // Blink orange if winds or gusts exceed WIND_THRESHOLD
  if (DO_WINDS && windLeds.size() > 0) {
    std::vector<CRGB> highwind(windLeds.size());
    for (unsigned short int i = 0; i < windLeds.size(); ++i) {
      unsigned short int currentLed = windLeds[i];
      highwind[i] = leds[currentLed];  // temporarily store original color
      leds[currentLed] = CRGB::Yellow; // set to yellow briefly
      Serial.print("High wind on LED: ");
      Serial.println(currentLed);
    }
    delay(25); // extra delay seems necessary with light sensor
    FastLED.show();
    delay(1000);
    for (unsigned short int i = 0; i < windLeds.size(); ++i) {
      unsigned short int currentLed = windLeds[i];
      leds[currentLed] = highwind[i]; // restore original color
    }
    FastLED.show();
  }

  if (loops >= loopThreshold || loops == 0) {
    loops = 0;
    if (DEBUG) {
      fill_gradient_RGB(leds, NUM_AIRPORTS, CRGB::Red, CRGB::Blue); // Just let us know we're running
      FastLED.show();
    }

    Serial.println("Getting METARs ...");
    if (getMetars()) {
      Serial.println("Refreshing LEDs.");
      FastLED.show();
      if ((DO_LIGHTNING && lightningLeds.size() > 0) || (DO_WINDS && windLeds.size() > 0) || USE_LIGHT_SENSOR) {
        Serial.println("There is lightning, thunderstorms or high wind, or we're using a light sensor, so no long sleep.");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(LOOP_INTERVAL); // pause during the interval
      }
      else {
        Serial.print("No thunderstorms or lightning. Going into sleep for: ");
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

bool getMetars(){
  lightningLeds.clear(); // clear out existing lightning LEDs since they're global
  windLeds.clear();
  fill_solid(leds, NUM_AIRPORTS, CRGB::Black); // Set everything to black just in case there is no report
  uint32_t t;
  char c;
  boolean readingRawText = false;
  boolean readingAirport = false;
  boolean readingCondition = false;
  boolean readingWind = false;
  boolean readingGusts = false;
  boolean readingWxstring = false;
  
  std::vector<unsigned short int> led;
  String currentRawText = "";
  String currentAirport = "";
  String currentCondition = "";
  String currentLine = "";
  String currentWind = "";
  String currentGusts = "";
  String currentWxstring = "";
  String airportString = "";
  bool firstAirport = true;
  
  // Build comma-separated list of airport IDs from airport string vector (list) to send to www.aviationweather.gov
  for (int i = 0; i < NUM_AIRPORTS; i++) {
    if (airports[i] != "NULL" && airports[i] != "VFR" && airports[i] != "MVFR" && airports[i] != "IFR" && airports[i] != "LIFR") {
      if (firstAirport) {
        firstAirport = false;
        airportString = airports[i];
      } else airportString = airportString + "," + airports[i];
    }
  }

  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  Serial.println("\nStarting connection to server...");
  if (!client.connect(SERVER, 443)) {
    Serial.println("Connection failed!");
    client.stop();
    return false;
  } else {
    Serial.println("Connected ...");
    Serial.print("GET ");
    Serial.print(BASE_URI);
    Serial.print(airportString);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(SERVER);
    Serial.println("User-Agent: LED Map Client");
    Serial.println("Connection: close");
    Serial.println();
    // Make the GET request, and print it to console
    client.print("GET ");
    client.print(BASE_URI);
    client.print(airportString);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(SERVER);
    client.println("User-Agent: LED Sectional Client");
    client.println("Connection: close");
    client.println();
    client.flush();
    t = millis(); // start time
    FastLED.clear();

    Serial.print("Getting data");

    while (!client.connected()) {
      if ((millis() - t) >= (READ_TIMEOUT * 1000)) {
        Serial.println("---Timeout---");
        client.stop();
        return false;
      }
      Serial.print(".");
      delay(1000);
    }

    Serial.println();

    while (client.connected()) {
      if ((c = client.read()) >= 0) {
        yield(); // Otherwise the WiFi stack can crash
        currentLine += c;
        if (c == '\n') currentLine = "";
        if (currentLine.endsWith("<raw_text>")) { // start paying attention
             if (!led.empty()) {                  // we assume we are recording results at each change in airport
               for (vector<unsigned short int>::iterator it = led.begin(); it != led.end(); ++it)  {
                 doColor(currentAirport, *it, currentWind.toInt(), currentGusts.toInt(), currentCondition, currentWxstring, currentRawText);
               }
             led.clear();
             }
          currentRawText = "";
          currentAirport = "";                    // Reset everything when the airport changes
          readingRawText = true;
          currentCondition = "";
          currentWind = "";
          currentGusts = "";
          currentWxstring = "";
        } else if (readingRawText)  {
            if (!currentLine.endsWith("<"))
              currentRawText += c;
            else
              readingRawText = false;
         } else if (currentLine.endsWith("<station_id>")) { 
          readingAirport = true;
        } else if (readingAirport) {
            if (!currentLine.endsWith("<"))
              currentAirport += c;
            else {
              readingAirport = false;
              for (unsigned short int i = 0; i < NUM_AIRPORTS; i++) {
                if (airports[i] == currentAirport)
                  led.push_back(i);
              }
            }
        } else if (currentLine.endsWith("<wind_speed_kt>")) {
          readingWind = true;
        } else if (readingWind) {
          if (!currentLine.endsWith("<")) {
            currentWind += c;
          } else {
            readingWind = false;
          }
        } else if (currentLine.endsWith("<wind_gust_kt>")) {
          readingGusts = true;
        } else if (readingGusts) {
          if (!currentLine.endsWith("<")) {
            currentGusts += c;
          } else {
            readingGusts = false;
          }
        } else if (currentLine.endsWith("<flight_category>")) {
          readingCondition = true;
        } else if (readingCondition) {
          if (!currentLine.endsWith("<")) {
            currentCondition += c;
          } else {
            readingCondition = false;
          }
        } else if (currentLine.endsWith("<wx_string>")) {
          readingWxstring = true;
        } else if (readingWxstring) {
          if (!currentLine.endsWith("<"))
            currentWxstring += c;
          else
            readingWxstring = false;
        }
        t = millis(); // Reset timeout clock
      } else if ((millis() - t) >= (READ_TIMEOUT * 1000)) {
        Serial.println("---Timeout---");
        fill_solid(leds, NUM_AIRPORTS, CRGB::Cyan); // indicate status with LEDs
        FastLED.show();
        ledStatus = true;
        client.stop();
        return false;
      }
    }
  }
  // need to doColor this for the last airport
  for (vector<unsigned short int>::iterator it = led.begin(); it != led.end(); ++it) {
    doColor(currentAirport, *it, currentWind.toInt(), currentGusts.toInt(), currentCondition, currentWxstring, currentRawText);
  }
  led.clear();

  // Do the key LEDs now if they exist
  for (int i = 0; i < (NUM_AIRPORTS); i++) {
    // Use this opportunity to set colors for LEDs in our key
    if (airports[i] == "VFR") leds[i] = CRGB::Green;
    else if (airports[i] == "MVFR") leds[i] = CRGB::Blue;
    else if (airports[i] == "IFR") leds[i] = CRGB::Red;
    else if (airports[i] == "LIFR") leds[i] = CRGB::Magenta;
  }

  client.stop();
  return true;
}

void doColor(String identifier, unsigned short int led, int wind, int gusts, String condition, String wxstring, String currentRawText) {
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
  Serial.println(wxstring);
  Serial.println(currentRawText);
  
  // LTG or LTNG for lightning is in raw_text of METAR, not in any other XML field.
  // We'll blink white for either or both of lightning and thunderstorms.
  if ((wxstring.indexOf("TS") != -1) || (currentRawText.indexOf("LTG") != -1) || (currentRawText.indexOf("LTNG") != -1)) {
    Serial.println("... found thunderstorms or lightning!");
    lightningLeds.push_back(led);
  }
  if ((wind > WIND_THRESHOLD) || (gusts > WIND_THRESHOLD)) {
    Serial.println("... found high winds or gusts!");
    HIGH_WINDS = true;
    windLeds.push_back(led);
  }

  if (condition == "LIFR" || identifier == "LIFR") color = CRGB::Magenta;
  else if (condition == "IFR") color = CRGB::Red;
  else if (condition == "MVFR") color = CRGB::Blue;
  else if (condition == "VFR")color = CRGB::Green;
  else color = CRGB::Black;  // if no flight category was reported

  leds[led] = color;
}
