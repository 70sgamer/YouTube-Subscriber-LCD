// YouTube Subscriber LCD Counter
// Target Hardware:  ESP8266 ESP-01 & 16X2 LCD
//
// Uses the Youtube Data API to retrieve channel statistics
// and display the subscriber count on a LCD display as well as serial monitor.
//
// 16X2 LCD connected with ESP8266
//    SCL:  GPIO2
//    SDA:  GPIO0
//
// Required libraries and board files:
//    esp8266               install from boards manager
//    YoutubeApi            for accessing Youtube via API key
//    ArduinoJson           required by YoutubeAPI
//    LiquidCrystal_I2C     for 16X2 LCD
//
// 70sGamer.com

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>                          // I2C library
#include <YoutubeApi.h>                    // Youtube Data API to retrieve public channel stats
#include <ArduinoJson.h>                   // required by Youtube API
#include <LiquidCrystal_I2C.h>       // Include LiquidCrystal_I2C library 

#define API_KEY    "AAAAAAAAAAAAAAAAAAAAAAA"  // your google apps API key for Youtube Data API access
#define CHANNEL_ID "DDDDDDDDDDDDDDDDDDDDDD"  // your Youtube channel ID

char ssid[]     = "WIFISSID";  // your network SSID name
char password[] = "WIFIpassword";   // your network password

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Configure LiquidCrystal_I2C library with 0x27 address, 16 columns and 2 rows

// Instantiate the WiFiClientSecure object for use with Youtube API
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs     = 60000;        // time between api requests, in mS.  60,000 mS = 1 minute intervals
unsigned long api_lasttime = 0;            // last time api request occurred

void setup() {

  Serial.begin(115200);

  // initialize 16 char 2 line LCD and print welcome msg
  lcd.begin(0, 2);
  lcd.backlight();                   // Turn backlight ON
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Pairing...");

  // set WiFi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // show WiFi status in serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("Your IP: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  client.setInsecure();      // Required if you are using ESP8266 V2.5 or above

  // update WiFi info on LCD
  lcd.clear();
  lcd.print("WiFi IP:");
  lcd.setCursor(0, 1);        // lower left character position
  lcd.print(ip);

  // show WiFi info on screens for a few seconds before continuing
  delay(3000);

  // get channel info from Youtube API for initial display
  pollYoutubeAPI();

}  // end setup()

void loop() {

  // if it is time to poll the Youtube API, try to retrieve channel data
  if (millis() > api_lasttime + api_mtbs)
    pollYoutubeAPI();

}  // end loop()

//  Other functions/routines

void pollYoutubeAPI() {
  if (api.getChannelStatistics(CHANNEL_ID)) {

    Serial.println("---------Stats---------");
    Serial.print("Subscriber Count: ");
    Serial.println(api.channelStats.subscriberCount);
    Serial.print("View Count: ");
    Serial.println(api.channelStats.viewCount);
    Serial.print("Comment Count: ");
    Serial.println(api.channelStats.commentCount);
    Serial.print("Video Count: ");
    Serial.println(api.channelStats.videoCount);
    // Probably not needed :)
    //Serial.print("hiddenSubscriberCount: ");
    //Serial.println(api.channelStats.hiddenSubscriberCount);
    Serial.println("-----------------------");

    // show subscriber count on LCD
    lcd.clear();
    lcd.print("View:");
    lcd.print(api.channelStats.viewCount);
    lcd.setCursor(0, 1);     // lower left character position
    lcd.print("Subs:");
    lcd.print(api.channelStats.subscriberCount);
    
  }

  api_lasttime = millis();   // start a new delay for next API poll interval

}  // end pollYoutubeAPI()
