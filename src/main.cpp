#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <DHT.h>

// stores the sensitive information needed for the program to work
#include "secrets.h"

// Defines wired connection the DHT-11 (KY-015)
#define DHT11_PIN 2
DHT dht11(DHT11_PIN, DHT11);

// Name and password off the Wi-Fi connection respectively
char ssid[] = SECRET_WIFI_SSID;    // your network SSID (name)
char pass[] = SECRET_WIFI_PASS;    // your network password

// Name and password for the MQTT broker respectively
char mqtt_user[] = SECRET_MQTT_USER;
char mqtt_pass[] = SECRET_MQTT_PASS;

// Prepare tot connect to Wi-Fi
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Required broker information, includes ip and port of the broker itself
// Also includes the subscribed topic
const char broker[] = SECRET_MQTT_IP; //IP address of the eclipse broker.
int        port     = 1883;
const char subscribe_topic[]  = "/IO"; // Topic for sending information to subscribed devices

// Define class, this function is build after the loop function
void eclipseReceived(int messageSize);

// Setup function for all the values and connections
void setup() {

    // Create serial connection and wait for it to become available.
    Serial.begin(9600);
    dht11.begin(); // initialize the sensor

    while (!Serial) {
    }

//    // Connect to Wi-Fi
//    Serial.print("Attempting to connect to WPA SSID: ");
//    Serial.println(ssid);
//    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
//        // failed, retry
//        Serial.print(".");
//        delay(5000);
//    }
//
//    Serial.println("You're connected to the network");
//    Serial.println();
//
//    // Provide a username and password for authentication
//    mqttClient.setUsernamePassword(mqtt_user, mqtt_pass);
//
//    Serial.print("Attempting to connect to the MQTT broker.");
//
//    if (!mqttClient.connect(broker, port)) {
//        Serial.print("MQTT connection failed! Error code = ");
//        Serial.println(mqttClient.connectError());
//
//        while (1);
//    }
//
//    Serial.println("You're connected to the MQTT broker!");
//
//    Serial.print("Waiting for messages on topic: ");
//    Serial.println(subscribe_topic);

    // Set function for receiving MQTT messages from the subscribed topics
    mqttClient.onMessage(eclipseReceived);
}

void loop() {

    // Loop delay
    delay(2000);

    // Checks if there are any messages from the MQTT
    // Also sends out a pulse so the connection will not be broken due to inactivity
//    mqttClient.poll();


    // Read humidity
    float humi  = dht11.readHumidity();
    // Read temperature as Celsius
    float tempC = dht11.readTemperature();

    // Check if any reads failed
    if (isnan(humi) || isnan(tempC)) {
        Serial.println("Failed to read from DHT11 sensor!");
    } else {

        // Turn sensor values into readable strings
        String humiString = String((int) humi) + "%";
        String Temp = String(tempC, 1) + " C";

        // Log values in terminal
        Serial.println("Humidity: " + humiString);
        Serial.println("Temperature: " + Temp);
        Serial.println();

    }

}

// Message receive function, logs incoming message and topic in the console
void eclipseReceived(int messageSize) {

    // Prepare string that will contain the full message from the MQTT
    String fullMessage = "";

    // Read the message from the MQTT
    while (mqttClient.available()) {
        fullMessage += (char) mqttClient.read();
    }

    // Store topic in separate value
    const String topic = mqttClient.messageTopic();

    // Deserialize json message from MQTT, if message is not json the results will be empty
    StaticJsonDocument<256> doc;
    deserializeJson(doc, mqttClient);

    // If received message from MQTT was json, store "msg" and "cmd" in separate value for later use
    String message = doc["msg"];
    const String command = doc["cmd"];

    // Print all the information from the incoming MQTT message in the console
    Serial.print("Received a message with topic '");
    Serial.println(topic);

    Serial.println("Full message:");
    Serial.println(fullMessage);
    Serial.println();

    Serial.print("Message: ");
    Serial.println(message);

}