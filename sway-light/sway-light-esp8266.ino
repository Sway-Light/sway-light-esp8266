#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include "SwayLight.h"


/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "172.20.10.4"
#define AIO_SERVERPORT 1883 // use 1883 for SSL
#define AIO_USERNAME ""
#define AIO_KEY ""


/*************************     MQTT Topics   *********************************/

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

void MQTT_connect();

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);
  SwayLight s(mySerial);
  
  WiFiManagerParameter custom_mqtt_ip(mqtt_ip, mqtt_ip, AIO_SERVER, 15);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_ip);
  
  // Uncomment and run it once, if you want to erase all the stored information
  wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
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
}

// Adafruit MQTT所有物件需等待setup()執行結束後再建立!!!
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;
/************ Global State (you don't need to change this!) ******************/
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, mqtt_ip, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// 必須follow topic規則，命名需以/feeds/作為開頭
// Notice MQTT paths for AIO follow the form: /feeds/
Adafruit_MQTT_Subscribe led = Adafruit_MQTT_Subscribe(&mqtt, "feeds/esp8266/slider");

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected). See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if(subscription == &led) {
      Serial.println((char *)led.lastread);
    }
  }
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
    Serial.println("MQTT Disconnected!");
  }
}

void MQTT_connect() {
  int8_t ret;
  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  mqtt.subscribe(&led);
  Serial.println("led subscribed!");

  Serial.print("Connecting to MQTT... ");
  Serial.println(mqtt.connectErrorString(ret));
  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(2000); // wait 2 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
