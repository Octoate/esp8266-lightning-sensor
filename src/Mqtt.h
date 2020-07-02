#ifndef mqtt_h
#define mqtt_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

// support for MQTT
#include <PubSubClient.h>

class Mqtt
{
    public:
        Mqtt();
        ~Mqtt();
        void Loop();
        void Reconnect();
        void Setup(char* mqtt_server, char* mqtt_port, char* mqtt_topic);

    private:
        WiFiClient espClient;
        PubSubClient client;
        char* server;
        char* port;
        char* topic;
        void Callback(char* topic, byte* payload, unsigned int length);
};

#endif
