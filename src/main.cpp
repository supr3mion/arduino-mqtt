#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <DHT.h>

#include "secrets.h"

char ssid[] = SECRET_WIFI_SSID;    // your network SSID (name)
char pass[] = SECRET_WIFI_PASS;    // your network password

char mqtt_user[] = SECRET_MQTT_USER;
char mqtt_pass[] = SECRET_MQTT_PASS;


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = SECRET_MQTT_IP; //IP address of the EMQX broker.
int        port     = 1883;
const char subscribe_topic[]  = "/hello";
const char publish_topic[]  = "/hello/world";

void setup() {
    // Create serial connection and wait for it to become available.
    Serial.begin(9600);
    while (!Serial) {
        ;
    }

    // Connect to WiFi
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // failed, retry
        Serial.print(".");
        delay(5000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a username and password for authentication
    mqttClient.setUsernamePassword(mqtt_user, mqtt_pass);

    Serial.print("Attempting to connect to the MQTT broker.");

    if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        while (1);
    }

    Serial.println("You're connected to the MQTT broker!");

    Serial.print("Subscribing to topic: ");
    Serial.println(subscribe_topic);

    // subscribe to a topic
    mqttClient.subscribe(subscribe_topic);

    // topics can be unsubscribed using:
    // mqttClient.unsubscribe(topic);

    Serial.print("Waiting for messages on topic: ");
    Serial.println(subscribe_topic);
}

void loop() {
    int messageSize = mqttClient.parseMessage();
    if (messageSize) {
        // we received a message, print out the topic and contents
        Serial.print("Received a message with topic '");
        Serial.print(mqttClient.messageTopic());
        Serial.print("', length ");
        Serial.print(messageSize);
        Serial.println(" bytes:");

        // use the Stream interface to print the contents
        while (mqttClient.available()) {
            Serial.print((char)mqttClient.read());
        }
        Serial.println();
    }

    // send message, the Print interface can be used to set the message contents
    delay(3000);
    mqttClient.beginMessage(publish_topic);
    mqttClient.print(random(1000));
    mqttClient.endMessage();

}