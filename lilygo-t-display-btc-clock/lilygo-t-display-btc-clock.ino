#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager/       // Arduino IDE - board: ESP32 DEV Module
#include <TFT_eSPI.h>     // https://github.com/Bodmer/TFT_eSPI          // sketch for LILYGO TTGO T-display
#include <Preferences.h>  // https://github.com/vshymanskyy/Preferences  // to save the selected time zone
#include <esp_sntp.h>     // (core) needed for callback function "SNTP sync"
#include <HTTPClient.h>
#include <ArduinoJson.h>

//coindesk api
const int httpsPort = 443;                                                          //Bitcoin price API powered by CoinDesk - https://www.coindesk.com/price/bitcoin
const String url = "http://api.coindesk.com/v1/bpi/currentprice/BTC.json";
const String historyURL = "http://api.coindesk.com/v1/bpi/historical/close.json";
const String cryptoCode = "BTC";


WiFiManager myWiFi;
HTTPClient http;

Preferences flash;
TFT_eSPI tft = TFT_eSPI();  // User_Setup: Setup25_TTGO_T_Display.h
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite leftSp = TFT_eSprite(&tft);
TFT_eSprite rightS = TFT_eSprite(&tft);

const char* weekdays[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* location[] = { "Tokyo" };  // Your City Name
const char* timeZone[] = { "JST-9" }; // Your time zone (refer to https://manpages.ubuntu.com/manpages/focal/man3/DateTime::TimeZone::Catalog.3pm.html)
const char* startTxt[] = { "connecting", "to WiFi", "Time sync" };
const char* noConnec[] = { "WiFi: no connection.", "Connect to hotspot", "'Lily', open browser", "address 192.168.4.1",
                           "for login + password." };
bool sync_OK = false, freshStart = true;
uint8_t count;

void setup() {
  Serial.begin(115200); //Select the baud range where serial output will be printed
  flash.begin("my-clock", true);       // read from flash, true = read only
  count = flash.getInt("counter", 0);  // retrieve the last set time zone - default to first in the array [0]
  flash.end();
  count = count % (sizeof(timeZone) / sizeof(timeZone[0]));  // modulo (# of elements in array) = prevent errors
  pinMode(35, INPUT_PULLUP);                                 // button "switch time zones"
  tft.init(), tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  sprite.createSprite(240, 135);                  // sprite for faster rendering
  show_Logo_WiFi();                               // this is not really necessary but it's nicer.
  myWiFi.setAPCallback(showMessageNoConnection);  // function "showMessageNoConnection" is called when wifi was not found
  myWiFi.autoConnect("Lily");
  showConnected();
  sntp_set_time_sync_notification_cb(SNTP_Sync);  // callback for synchronization SNTP
  configTzTime(timeZone[count], "pool.ntp.org");  // set the time zone
}

void loop() {
  //get the price
  http.begin(url);
  int httpCode = http.GET();                                                            //Get crypto price from API
  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc, http.getString());
 
  if (error)                                                                            //Display error message if unsuccessful
  {
    Serial.print(F("deserializeJson Failed"));
    Serial.println(error.f_str());
    delay(2500);
    return;
  }
 
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
 
  String BTCUSDPrice = doc["bpi"]["USD"]["rate_float"].as<String>();                    //Store crypto price and update date in local variables

  // Convert string to float
  float priceFloat = BTCUSDPrice.toFloat();

  // Convert float to integer to remove decimal places
  int priceInt = (int)priceFloat;

  // Convert integer back to string
  BTCUSDPrice = String(priceInt);

  String lastUpdated = doc["time"]["updated"].as<String>();
  http.end();
 
  Serial.print("Getting history...");
  StaticJsonDocument<2000> historyDoc;
  http.begin(historyURL);                                                               //Get historical crypto price from API
  int historyHttpCode = http.GET();
  DeserializationError historyError = deserializeJson(historyDoc, http.getString());
 
  if (historyError) {                                                                   //Display error message if unsuccessful
    Serial.print(F("deserializeJson(History) failed"));
    Serial.println(historyError.f_str());
    delay(2500);
    return;
  }
 
  Serial.print("History HTTP Status Code: ");
  Serial.println(historyHttpCode);
  JsonObject bpi = historyDoc["bpi"].as<JsonObject>();
  double yesterdayPrice;
  for (JsonPair kv : bpi) {
    yesterdayPrice = kv.value().as<double>();                                           //Store yesterday's crypto price
  }
 
  Serial.print("BTCUSD Price: ");                                                       //Display current price on serial monitor
  Serial.println(BTCUSDPrice.toDouble());
 
  http.end();   

  displayTime(BTCUSDPrice);    

  //if (!digitalRead(35)) switchTimeZone();  // bottom button
  if (!digitalRead(0)) deepSleep();        // top button
}

void show_Logo_WiFi() {
  sprite.fillSprite(sprite.color565(100, 100, 100));
  for (int i = 0; i < 8192; i++) {  // create surface with fine texture
    byte j = random(100) + 50;
    sprite.drawPixel(random(240), random(135), sprite.color565(j, j, j));  // random grayscale
  }
  sprite.setTextColor(TFT_YELLOW);
  sprite.drawCentreString(startTxt[0], 120, 76, 4), sprite.drawCentreString(startTxt[1], 120, 100, 4);
  sprite.fillCircle(120, 56, 6, TFT_BLUE);
  for (byte i = 0; i < 3; i++) sprite.drawSmoothArc(120, 54, 40 - i * 11, 35 - i * 11, 128, 232, TFT_BLUE, TFT_BLUE, 1);
  sprite.pushSprite(0, 0);
}

void showConnected() {
  for (byte i = 0; i < 3; i++) sprite.drawSmoothArc(120, 54, 40 - i * 11, 35 - i * 11, 128, 232, TFT_GREEN, TFT_GREEN, 1);
  sprite.fillCircle(120, 56, 6, TFT_GREEN);
  sprite.pushSprite(0, 0);
  leftSp.createSprite(128, 66);  // for new text to be scrolled up
  leftSp.fillSprite(sprite.color565(100, 100, 100));
  for (int i = 0; i < 3072; i++) {
    byte j = random(100) + 50;
    leftSp.drawPixel(random(128), random(66), sprite.color565(j, j, j));  // random grayscale
  }
  leftSp.setTextColor(TFT_GREEN);
  leftSp.drawCentreString("WiFi  OK", 64, 5, 4);
  leftSp.setTextColor(TFT_YELLOW);
  leftSp.drawCentreString(startTxt[2], 64, 30, 4);
  for (int i = 1024; i > 560; i--) leftSp.pushSprite(56, i / 8);  // high values to slow down the animation
  leftSp.pushToSprite(&sprite, 56, 70);
  leftSp.deleteSprite();
}

void showMessageNoConnection(WiFiManager* myWiFi) {  // message on display when there is no WiFi connection
  tft.fillScreen(TFT_NAVY);                          // also blue color for the leftmost 9 pixels - sprite.pushSprite(9, 0) below
  tft.setTextColor(TFT_YELLOW);
  for (uint8_t i = 0; i < 5; i++) tft.drawCentreString(noConnec[i], 120, i * 27, 4);
}

void displayTime(String BTCUSDPrice) {
  struct tm tInfo;       // https://cplusplus.com/reference/ctime/tm/
  getLocalTime(&tInfo);  // SNTP update every 3 hours (default ESP32) since we did not set an interval
  if (sync_OK) {         // only display the correct time = after SNTP sync
    if (freshStart) splitScreenHor();
    sprite.setTextColor(TFT_CYAN, TFT_BLACK);
    sprite.fillSprite(TFT_BLACK);
    sprite.setFreeFont(&FreeSansBold24pt7b);
    sprite.setCursor(40, 48);
    sprite.printf("%02d:%02d", tInfo.tm_hour, tInfo.tm_min);
    sprite.setFreeFont(&FreeSansBold18pt7b);
    sprite.setCursor(sprite.getCursorX() + 4, 48);  // add 4 px distance between minutes & seconds
    sprite.printf("%02d", tInfo.tm_sec);
    sprite.setTextColor(TFT_YELLOW);
    sprite.setFreeFont(&FreeSans12pt7b);
    sprite.drawString(weekdays[tInfo.tm_wday], 20, 76, 1);                        // weekday
    sprite.drawRect(-1, 65, 242, 38, WiFi.isConnected() ? TFT_GREEN : TFT_RED);  // green lines if WiFi connection = OK
    sprite.setTextColor(TFT_CYAN);
    sprite.setFreeFont(&FreeSans9pt7b);
    sprite.setCursor(136, 93);
    sprite.printf("%02d-%02d-%04d", tInfo.tm_mday, 1 + tInfo.tm_mon, 1900 + tInfo.tm_year);  // date
    sprite.setTextColor(TFT_RED);
    sprite.drawString(location[count], 20, 110, 1);
    sprite.setTextColor(TFT_ORANGE);
    sprite.drawString("1BTC = $" + BTCUSDPrice, 80, 110, 1);

    if (freshStart) mergeScreen();  // before "pushSprite" because the background must be black
    sprite.pushSprite(0, 0);
  }
}

void splitScreenHor() {
  leftSp.createSprite(120, 135);
  rightS.createSprite(120, 135);
  for (byte ver = 0; ver < 135; ver++) {  // divide the sprite into 2 pieces & write to two smaller sprites
    for (byte hor = 0; hor < 120; hor++) leftSp.drawPixel(hor, ver, sprite.readPixel(hor, ver));
    for (byte hor = 120; hor < 240; hor++) rightS.drawPixel(hor - 120, ver, sprite.readPixel(hor, ver));
  }
  leftSp.drawFastVLine(119, 0, 135, TFT_BLACK);
  rightS.drawFastVLine(0, 0, 135, TFT_BLACK);
  for (byte hor = 0; hor < 120; hor++) {  // move both sprites to the left & right outer edges
    leftSp.pushSprite(0 - hor, 0);
    rightS.pushSprite(hor + 120, 0);
  }
  leftSp.deleteSprite(), rightS.deleteSprite();
}

void splitScreenVer() {
  leftSp.createSprite(240, 67), rightS.createSprite(240, 68);
  for (byte hor = 0; hor < 240; hor++) {  // divide the sprite into 2 pieces & write to two smaller sprites
    for (byte ver = 0; ver < 67; ver++) leftSp.drawPixel(hor, ver, sprite.readPixel(hor, ver));
    for (byte ver = 67; ver < 135; ver++) rightS.drawPixel(hor, ver - 67, sprite.readPixel(hor, ver));
  }
  leftSp.drawFastHLine(0, 66, 240, TFT_BLACK);
  rightS.drawFastHLine(0, 0, 240, TFT_BLACK);
  for (byte ver = 0; ver < 68; ver++) {  // move sprites up & down
    leftSp.pushSprite(0, 0 - ver);
    rightS.pushSprite(0, 67 + ver);
  }
  leftSp.deleteSprite(), rightS.deleteSprite();
}

void mergeScreen() {
  leftSp.createSprite(120, 135), rightS.createSprite(120, 135);
  freshStart = false;
  for (byte ver = 0; ver < 135; ver++) {  // divide the sprite into 2 pieces & write to two smaller sprites
    for (byte hor = 0; hor < 120; hor++) leftSp.drawPixel(hor, ver, sprite.readPixel(hor, ver));
    for (byte hor = 120; hor < 240; hor++) rightS.drawPixel(hor - 120, ver, sprite.readPixel(hor, ver));
  }
  for (byte hor = 0; hor < 120; hor++) {  // merge both sprites into 1 whole.
    leftSp.pushSprite(hor - 120, 0);
    rightS.pushSprite(240 - hor, 0);
  }
  leftSp.deleteSprite(), rightS.deleteSprite();
}

void switchTimeZone() {
  leftSp.createSprite(240, 28), leftSp.setFreeFont(&FreeSans9pt7b);
  leftSp.fillSprite(TFT_BLACK);
  leftSp.setTextColor(TFT_RED), leftSp.drawCentreString(location[count], 120, 0, 1);
  for (int tel = 0; tel < 240; tel++) leftSp.pushSprite(tel, 110);  // we need an animation that uses up time = UI debouncing
  count = (count + 1) % (sizeof(timeZone) / sizeof(timeZone[0]));   // increase counter modulo (number of elements in string array)
  configTzTime(timeZone[count], "pool.ntp.org");
  leftSp.fillSprite(TFT_BLACK);
  leftSp.drawCentreString(location[count], 120, 0, 1);
  for (int tel = -960; tel < 1; tel++) leftSp.pushSprite(tel / 4, 110);
  leftSp.deleteSprite();
}

void deepSleep() {
  byte savedZone;
  flash.begin("my-clock");                                 // read from flash memory
  savedZone = flash.getInt("counter", 0);                  // retrieve the last set time zone - default to first in the array [0]
  if (savedZone != count) flash.putInt("counter", count);  // only write the time zone to flash memory when it was changed
  flash.end();                                             // to prevent chip wear from excessive writing
  splitScreenVer();                                        // we need an animation that uses up time = UI debouncing
  sprite.fillSprite(TFT_BLACK);
  sprite.setFreeFont(&FreeSansBold18pt7b);
  sprite.setTextColor(TFT_YELLOW);
  sprite.drawCentreString("Deep Sleep", 120, 60, 1);
  mergeScreen();
  esp_wifi_stop();
  esp_sleep_enable_ext1_wakeup(GPIO_SEL_0, ESP_EXT1_WAKEUP_ALL_LOW);  // IO35 can also be set for awakening
  esp_deep_sleep_start();
}

void SNTP_Sync(struct timeval* tv) {  // callback time sync - we need sync_OK because in "deep sleep" mode,
  sync_OK = true;                     // the ESP32's internal clock does not run accurately.
}