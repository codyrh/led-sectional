# led-sectional
January 19, 2024
----------------
Edited to show METAR string in serial output.  Prior code looked like it should have worked, but did not.

Added a second threshold for very high winds or gusts:
  1. LEDs will blink black/clear for high winds.
  2. LEDs will blink orange for very high winds or gusts.
 
Edited to show count of LEDs with thunderstorms/lightning, high winds, very high winds.

January 17, 2024
 ----------------
 Added line: WiFi.setPhyMode(WIFI_PHY_MODE_11G); after WiFi.mode(WIFI_STA);
 This seems to fix a problem with connection failures to SmartWiFi routers that have the same SSID for both 2.4 and 5 ghz bands.
 Tip was found at https://github.com/esp8266/Arduino/issues/8299
 
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
 1. Added WiFiManger
	See this reference: https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/
 2. High wind (orange) color will now blink vs being a solid orange, and be shown for all flight categories, not just VFR
 3. Lightning (white blink) will occur not just for TS (thunderstorms), but also for LTG and LTNG reported in <raw_text> in response XML

 As a result of 2 and 3, any particular LED could have 3 colors...one for flight category, and blinking either or both of orange and white.

=====================

This uses an Arduino to download METARs for a set of airports and assign a color to a LED representing each airport so that they can be put into a sectional. It is inspired by https://www.reddit.com/r/flying/comments/7avr8q/flight_conditions_sectional_wall_art_thing/

This is designed to run on an ESP8266 and deployed using Arduino, but with minor tweaks (e.g. updated libraries) it should run on most Arduino platforms.

Use ESP8266 Core V2.74, and FastLED V3.30
