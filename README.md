### Overview

!["working example"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/final.jpg?raw=true)

!["Lilygo t-display case"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/package.jpg?raw=true)

**Setup a live Bitcoin price ticker and clock / date display on a LilyGo T-display.**

Combines the date, clock, and wifi functions of [https://espgo.be/index-en.html](https://espgo.be/index-en.html) with the Bitcoin price tracker of [https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/](https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/ ) 

#### Notes: 
- This may work with similar LilyGo models, but I have only tested on the LilyGo T-display v1.1
- Bitcoin price is sourced from coindesk public API.
- Originally tried getting [this repo by oxinon](https://github.com/oxinon/BTC-price-ticker-V2_1-TTGO-T-Display) to work, but I had trouble getting wifi to connect and I did not want to use the coinmarketcap API.

### Prequisites
- A LilyGogo T-display 
- Arduino IDE

Setup instructions are for Mac. Windows and Linux will be very similar but may be easier as I had some issues getting the LilyGo T-display drivers working at first.

### Setup Steps

1. Connect the LilyGo T-display via USB and install the LilyGo T-display drivers (For Mac or Windows from here: [https://github.com/Xinyuan-LilyGO/TTGO-T-Display?tab=readme-ov-file](https://github.com/Xinyuan-LilyGO/TTGO-T-Display?tab=readme-ov-file)

2. Install Arduino IDE: [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/)

3. Add Arduino settings for the ESP32 boards "Preferences" > "Additional Boards Manager".  
  Paste in this url: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

4. Add the required Arduino libraries in the Arduino package manager by going "Sketch" > "Include Library" > "Manage Library".  
Libraries to add:
  - ArduinoJson by Benoit Blanchon
  - Preferences by Volodymr Shymanskyy
  - TFT_eSPI by Bodmer
  - WiFiManager by Tzapu

5. Update the TFT_eSPI user settings by opening the Arduino library folder, located on mac at `[Username]/Documents/Arduino/Libraries/TFT_eSPI/User_Setup_Select.h`
- Uncomment line 58 "#include <User_Setups/Setup25_TTGO_T_Display.h> " (or whichever specific device it is you have).

5. Download and open the file `lilygo-t-display-btc-clock/lilygo-t-display-btc-clock.ino` to your Arduino folder at `[Username]/Documents/Arduino/`
  - Change Line 25 to be your City name and Line 26 to your [time zone](https://manpages.ubuntu.com/manpages/focal/man3/DateTime::TimeZone::Catalog.3pm.html)

6. Setup your board: "Tools" > "Board" > "ESP32" > "ESP32 Dev Module". The device should now show up as connected in the top left with a USB icon. If this does not work you might need to try changing the ports to one of the other options from the menu. For me it was '/dev/cu.wchusbserial56230381151`.
!["board settings"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/board-port-settings.png?raw=true)
All other settings should be OK as defaults.
If things are not showing up or working as expected try restarting the Arduino IDE or the computer or try a different USB port or cable.

7. Click the "Upload" button.
!["Upload"](https://github.com/codyellingham/lilygo-t-display-btc-clock/blob/master/images/upload.png?raw=true)

8. The code should verify then upload and the the LilyGo will restart and prompt you to set wifi password by logging in to the temporary access point "Lily" and selecting your wifi network from there.

9. Done.