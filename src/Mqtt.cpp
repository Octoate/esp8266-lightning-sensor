#include <Arduino.h>

#include "mqtt.h"

Mqtt::Mqtt(/* args */)
{
    this->client.setClient(espClient);
}

Mqtt::~Mqtt()
{
}

void Mqtt::Loop()
{
    // check MQTT client connection and call loop method of the MQTT client
    if (!this->client.connected()) {
        this->Reconnect();
    }

    this->client.loop();
}

void Mqtt::Reconnect() 
{
    // Loop until we're reconnected
    while (!this->client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-LightningSensor";
        // Attempt to connect
        if (this->client.connect(clientId.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(this->topic, "Lightning sensor online.");
            // ... and resubscribe
            this->client.subscribe("inTopic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(this->client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void Mqtt::Setup(char* mqtt_server, char* mqtt_port, char* mqtt_topic)
{
    this->server = mqtt_server;
    this->port = mqtt_port;
    this->topic = mqtt_topic;

    // configure MQTT connection
    this->client.setServer(mqtt_server, atoi(mqtt_port));
    this->client.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->Callback(topic, payload, length); });
}

void Mqtt::Callback(char* topic, byte* payload, unsigned int length) 
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
    } else {
        digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    }
}
