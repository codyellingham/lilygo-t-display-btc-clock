### Overview

[Photo]

Setup a live Bitcoin price ticker and clock / date ticker on a LilyGo T-display.

Combines the Date, clock, and wifi functions of [https://espgo.be/index-en.html](https://espgo.be/index-en.html) with the Bitcoin price tracker of [https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/](https://how2electronics.com/bitcoin-price-tracker-using-esp32-oled-display/ ) 

Bitcoin price is sourced from coindesk public API.

Originally tried getting [this repo by oxinon](https://github.com/oxinon/BTC-price-ticker-V2_1-TTGO-T-Display) to work, but I had trouble getting wifi to connect and I did not want to use the coinmarketcap API.

### prequisites

- A lilygo T-display 
- Arduino IDE

Setup instructions are for Mac. Windows and Linux will be similar but may be easier as I had some issues getting the LilyGo t-display drivers working at first.

### Setup Steps

1. Install the LilyGo T-display drivers:
https://github.com/Xinyuan-LilyGO/TTGO-T-Display?tab=readme-ov-file

2. Install Arduioni IDE: https://www.arduino.cc/en/software/

3. Add Arduino settings for the ESP32 boards "Preferences" > "Additional Boards Manager":
  Paste in the url:
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

4. Add Arduino libraries.
  "Sketch" > "Include Library" > "Manage Library".
  Libraries to add:
  - HttpClient by Adrian
  - ArduinoJson by Benoit Blanchon
  - Preferences by Volodymr Shymanskyy
  - TFT_eSPI by Bodmer
  - WiFiManager by Tzapu

5. Download and open the sketch in this repo.
  - Change Line 25 to be your City name and Line 26 to your [time zone](https://manpages.ubuntu.com/manpages/focal/man3/DateTime::TimeZone::Catalog.3pm.html)

7. Click the "Upload" button.

8. The code should verify then upload and the the LilyGo will restart and prompt you to set wifi password by logging in to the temporary access point "Lily" and selecting your wifi network from there.

9. Done