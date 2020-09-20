#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#include <NTPClient.h>

#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <Adafruit_MQTT.h>        // https://github.com/adafruit/Adafruit_MQTT_Library
#include <Adafruit_MQTT_Client.h>
#include "SwayLight.h"
#include "SwayLight_MQTT_topic.h"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "172.20.10.6"
#define AIO_SERVERPORT 1883 // use 1883 for SSL
#define AIO_USERNAME ""
#define AIO_KEY ""

/*************************     Serial data   *********************************/

// Set web server port number to 80
//WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState = "off";

// Assign output variables to GPIO pins
char mqtt_ip[20] = "mqtt_ip";

//flag for saving data
bool shouldSaveConfig = false;

SoftwareSerial mySerial(13, 15);
SwayLight s(mySerial);

StaticJsonDocument<200> doc;
StaticJsonDocument<200> pubDoc;

void serialProcess(void);
void getNtpTime();

void MQTT_connect();
void subscribeAllTopics();
void printSubscribeInfo(Adafruit_MQTT_Subscribe *subscription);

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

WiFiManagerParameter custom_mqtt_ip(mqtt_ip, mqtt_ip, AIO_SERVER, 15);

// WiFiManager
// Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_ip);
  
  // Uncomment and run it once, if you want to erase all the stored information
  // wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality(15);
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected!");
  
  strcpy(mqtt_ip, custom_mqtt_ip.getValue());

  // Initialize the output variables as outputs
  Serial.print("mqtt_ip:");
  Serial.println(mqtt_ip);
  //  server.begin();
  getNtpTime();
}

// Adafruit MQTT所有物件需等待setup()執行結束後再建立!!!
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;
/************ Global State (you don't need to change this!) ******************/
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, mqtt_ip, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
uint8_t qos = 0;
// 必須follow topic規則，命名需以/feeds/作為開頭
// Notice MQTT paths for AIO follow the form: /feeds/
Adafruit_MQTT_Subscribe sub_power        = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC POWER);
Adafruit_MQTT_Subscribe sub_powerOnTime  = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC ON_TIME);
Adafruit_MQTT_Subscribe sub_powerOffTime = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC OFF_TIME);
Adafruit_MQTT_Subscribe sub_currMode     = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC CURR_MODE);
Adafruit_MQTT_Subscribe sub_lightColor   = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC LIGHT_COLOR);
Adafruit_MQTT_Subscribe sub_lightOffset  = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC LIGHT_DISPLAY_OFFSET);
Adafruit_MQTT_Subscribe sub_lightZoom    = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC LIGHT_ZOOM);
Adafruit_MQTT_Subscribe sub_musicColor   = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC MUSIC_COLOR);
Adafruit_MQTT_Subscribe sub_musicOffset  = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC MUSIC_DISPLAY_OFFSET);
Adafruit_MQTT_Subscribe sub_musicStyle   = Adafruit_MQTT_Subscribe(&mqtt, MY_DEVICE_TOPIC MUSIC_STYLE);

Adafruit_MQTT_Publish   pub_power        = Adafruit_MQTT_Publish(&mqtt, MY_DEVICE_TOPIC POWER, qos);
Adafruit_MQTT_Publish   pub_currMode     = Adafruit_MQTT_Publish(&mqtt, MY_DEVICE_TOPIC CURR_MODE, qos);
Adafruit_MQTT_Publish   pub_lightColor   = Adafruit_MQTT_Publish(&mqtt, MY_DEVICE_TOPIC LIGHT_COLOR, qos);
Adafruit_MQTT_Publish   pub_lightZoom    = Adafruit_MQTT_Publish(&mqtt, MY_DEVICE_TOPIC LIGHT_ZOOM, qos);

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected). See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  
  // 處理HT32資料
  serialProcess();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(500))) {
    printSubscribeInfo(subscription);
    char *json = (char *)subscription->lastread;
    DeserializationError error = deserializeJson(doc, json);
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }else {
      if(s.isFromMyself()) {
        Serial.println("msg from my self");
        break;
      }
      if(subscription == &sub_power) {
        s.setPower((int)doc[SL_VALUE]);
      }else if(subscription == &sub_powerOffTime || subscription == &sub_powerOnTime) {
        int enable = 0;
        for (int i = 0; i < 7; i++) {
          enable <<= 1;
          enable += (int)(doc[SL_ENABLE][i]) & 0x01;
        }
        int hour = doc[SL_HOUR];
        int min = doc[SL_MIN];
        int sec = doc[SL_SEC];
        if(hour > 23 || hour < 0 || min > 59 || min < 0 || sec > 59 || sec < 0) {
          Serial.println("json data error!!!");
        }else {
          if(subscription == &sub_powerOnTime) {
            s.setPower(true, enable, hour, min, sec);
          }else{
            s.setPower(false, enable, hour, min, sec);
          }
        }
      }else if(subscription == &sub_currMode) {
        s.setMode((int)doc[SL_VALUE]);
      }else if (subscription == &sub_lightColor || subscription == &sub_musicColor) {
        uint32_t colorInfo = 0;
        colorInfo += (uint8_t)doc[SL_RED] << 24;
        colorInfo += (uint8_t)doc[SL_GREEN] << 16;
        colorInfo += (uint8_t)doc[SL_BLUE]  <<  8;
        colorInfo += (subscription == &sub_lightColor)? (uint8_t)doc[SL_BRIGHT]: (uint8_t)doc[SL_LEV];
        if(subscription == &sub_lightColor) {
          s.setLedColor(_CONTROL_TYPE::LIGHT, _LED::COLOR, colorInfo);
        }else {
          s.setLedColor(_CONTROL_TYPE::MUSIC, _LED::COLOR, colorInfo);
        }
      }else if (subscription == &sub_lightOffset) {
        s.setLedOffset(_CONTROL_TYPE::LIGHT, (int)doc[SL_VALUE]);
      }else if (subscription == &sub_lightZoom) {
        s.setLedZoom((int)doc[SL_VALUE]);
      }else if (subscription == &sub_musicOffset) {
        s.setLedOffset(_CONTROL_TYPE::MUSIC, (int)doc[SL_VALUE]);
      }else if (subscription == &sub_musicStyle) {
        s.setLedStyle((int)doc[SL_VALUE]);
      } 
    }
  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  // if(! mqtt.ping()) {
  //   mqtt.disconnect();
  //   Serial.println("MQTT Disconnected!");
  // }
}

void serialProcess() {
  while (mySerial.available()) {
    uint8_t c = mySerial.read();
    //Serial.println(c, HEX);
    s.dataFromHt32[s.currIndex] = c;
    if (s.currIndex == 0 && s.dataFromHt32[0] != 0x95) {
      s.clearReciveBuff();
      s.currIndex = 0;
    }else {
      s.currIndex++;
    }
    if(s.currIndex == CMD_SIZE) {
      
      if(s.isValid()) {
        char pubMsg[200];
        pubDoc.clear();
        pubDoc[SL_ID] = CLIENT_ID;
        switch(s.getControlType()) {
          case _CONTROL_TYPE::MODE_SWITCH:
            if (s.getStatus() == _STATUS::OFF) {
              pubDoc[SL_VALUE] = _STATUS::OFF;
              serializeJson(pubDoc, pubMsg);
              pub_power.publish(pubMsg);
            }
            else if (s.getStatus() == _STATUS::ON) {
              pubDoc[SL_VALUE] = _STATUS::ON;
              serializeJson(pubDoc, pubMsg);
              pub_power.publish(pubMsg);
            }
            else if (s.getStatus() == _STATUS::STATUS_LIGHT) {
              pubDoc[SL_VALUE] = _STATUS::STATUS_LIGHT;
              serializeJson(pubDoc, pubMsg);
              pub_currMode.publish(pubMsg);
            }
            else if (s.getStatus() == _STATUS::STATUS_MUSIC) {
              pubDoc[SL_VALUE] = _STATUS::STATUS_MUSIC;
              serializeJson(pubDoc, pubMsg);
              pub_currMode.publish(pubMsg);
            }
            Serial.println(pubMsg);
            break;

          case _CONTROL_TYPE::LIGHT:
            if(s.getLedType() == _LED::COLOR) {
              pubDoc[SL_RED] = s.getRed();
              pubDoc[SL_GREEN] = s.getGreen();
              pubDoc[SL_BLUE] = s.getBlue();
              pubDoc[SL_BRIGHT] = s.getLedParamVal();
              serializeJson(pubDoc, pubMsg);
              pub_lightColor.publish(pubMsg);
            }else if(s.getLedType() == _LED::ZOOM) {
              pubDoc["zoom"] = s.getLedParamVal();
              serializeJson(pubDoc, pubMsg);
              pub_lightZoom.publish(pubMsg);
            }
            break;

          default:
            Serial.println("control type error");
          }
      }
      s.printReciveBuff();
      s.clearReciveBuff();
      s.currIndex = 0;
    }
  }
}

void getNtpTime() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, 28800);
  
  timeClient.begin();
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();

  Serial.print("epochTime: ");
  Serial.println(epochTime);
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int currentDay    = ptm->tm_mday;
  int currentMonth  = ptm->tm_mon+1;
  int currentYear   = ptm->tm_year+1900;
  int currentHour   = ptm->tm_hour;
  int currentMinute = ptm->tm_min;
  int currentSecond = ptm->tm_sec;
  //Print complete date:
  String currentDate = String(currentYear) + "-" + 
                       String(currentMonth) + "-" + 
                       String(currentDay) + " " +
                       String(currentHour) + ":" +
                       String(currentMinute) + ":" +
                       String(currentSecond);
  s.setDatetime(epochTime);
  Serial.println(currentDate);
 }
void MQTT_connect() {
  int8_t ret;
  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  subscribeAllTopics();
  Serial.println("All topics subscribed!");
  
  Serial.print("Connecting to MQTT... ");
  Serial.println(mqtt.connectErrorString(ret));
  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 2 seconds...");
    mqtt.disconnect();
    delay(2000); // wait 2 seconds
    retries--;
    if (retries == 0) {
      wifiManager.resetSettings();
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void subscribeAllTopics() {
  mqtt.subscribe(&sub_power);
  mqtt.subscribe(&sub_powerOnTime);
  mqtt.subscribe(&sub_powerOffTime);

  mqtt.subscribe(&sub_currMode);

  mqtt.subscribe(&sub_lightColor);
  mqtt.subscribe(&sub_lightOffset);
  mqtt.subscribe(&sub_lightZoom);

  mqtt.subscribe(&sub_musicColor);
  mqtt.subscribe(&sub_musicOffset);
  mqtt.subscribe(&sub_musicStyle);
}

void printSubscribeInfo(Adafruit_MQTT_Subscribe *subscription) {
  Serial.print((char *)subscription->topic);
  Serial.print(": ");
  Serial.println((char *)subscription->lastread);
}
