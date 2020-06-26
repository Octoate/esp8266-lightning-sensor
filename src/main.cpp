#include <Arduino.h>

// file system access for configuration storage
#include <LittleFS.h>

#include <ESP8266WiFi.h>
#include <Wire.h>

// needed for the WiFiManager library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// JSON support
#include <ArduinoJson.h> 

#include "config.h"

// flag for saving data
bool shouldSaveConfig = false;

// callback notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();

  // clean FS, for testing
#ifdef RESET_PARAMETER_CONFIG
  LittleFS.format();
#endif

  // read configuration from FS json
  Serial.println("mounting FS...");

  if (LittleFS.begin())
  {
    Serial.println("mounted file system");
    if (LittleFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument jsonBuffer(1024);
        DeserializationError error = deserializeJson(jsonBuffer, buf.get());
        if (error)
        {
          Serial.println("failed to load json config");
        }
        else
        {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
          strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
        }

        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // set static ip
  // wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
   
  // add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
 
#ifdef RESET_WIFI_CONFIG
  // reset settings - for testing
  wifiManager.resetSettings();
#endif
   
  // sets timeout until configuration portal gets turned off
  // useful to make it all retry or go to sleep
  // in seconds
  wifiManager.setTimeout(180);
 
  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if  (!wifiManager.autoConnect(wifiConfigSsid, wifiConfigPassword))
  {
     Serial.println("failed to connect and hit timeout");
     delay(3000);

     //reset and try again, or maybe put it to deep sleep
     ESP.reset();
     delay(5000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());

  // save the custom parameters to FS
  if (shouldSaveConfig)
  {
    Serial.println("saving config");
    DynamicJsonDocument jsonBuffer(1024);
    jsonBuffer["mqtt_server"] = mqtt_server;
    jsonBuffer["mqtt_port"] = mqtt_port;

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(jsonBuffer, Serial);
    serializeJson(jsonBuffer, configFile);
    configFile.close();
    // end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
}