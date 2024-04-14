### Overview

!["working example"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/final.jpg?raw=true)

!["Lilygo t-display case"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/package.jpg?raw=true)

Setup a live Bitcoin price ticker and clock / date ticker on a LilyGo T-display.

Combines the Date, clock, and wifi functions of [https://espgo.be/index-en.html](https://espgo.be/index-en.html) with the Bitcoin price tracker of [https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/](https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/ ) 

Note: 
- This may work with similar LilyGo models, but has only been tested on the T-display v1.1
- Bitcoin price is sourced from coindesk public API.
- Originally tried getting [this repo by oxinon](https://github.com/oxinon/BTC-price-ticker-V2_1-TTGO-T-Display) to work, but I had trouble getting wifi to connect and I did not want to use the coinmarketcap API.

### prequisites
- A LilyGogo T-display 
- Arduino IDE

Setup instructions are for Mac. Windows and Linux will be similar but may be easier as I had some issues getting the LilyGo T-display drivers working at first.

### Setup Steps

1. Install the LilyGo T-display drivers (For Mac or Windows):
https://github.com/Xinyuan-LilyGO/TTGO-T-Display?tab=readme-ov-file

2. Install Arduino IDE: https://www.arduino.cc/en/software/

3. Add Arduino settings for the ESP32 boards "Preferences" > "Additional Boards Manager":
  Paste in this url: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

4. Add the following Arduino libraries.
  "Sketch" > "Include Library" > "Manage Library".
  Libraries to add:
  - ArduinoJson by Benoit Blanchon
  - Preferences by Volodymr Shymanskyy
  - TFT_eSPI by Bodmer
  - WiFiManager by Tzapu

5. Update the TFT_eSPI user settings by opening the Arduino library, located at `Documents/Arduino/Libraries/TFT_eSPI/User_Setup_Select.h`
- Uncomment line 58 "#include <User_Setups/Setup25_TTGO_T_Display.h> " (or whichever specific device it is you have).


5. Download and open the sketch in this repo.
  - Change Line 25 to be your City name and Line 26 to your [time zone](https://manpages.ubuntu.com/manpages/focal/man3/DateTime::TimeZone::Catalog.3pm.html)

6. Setup your board: "Tools" > "Board" > "ESP32" > "LilyGo T-Display"
You may also need to change the ports. For me it was 
!["board settings"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/board-port-settings.png?raw=true)
All other settings should be OK as defaults.

7. Click the "Upload" button.

8. The code should verify then upload and the the LilyGo will restart and prompt you to set wifi password by logging in to the temporary access point "Lily" and selecting your wifi network from there.

9. Done