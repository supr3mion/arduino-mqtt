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

// Define class, this function is build after the loop function
void eclipseReceived(int messageSize);

// Global connection status, by default false upon running code
bool connected = false;
// Function for connecting arduino to Wi-Fi
void connectWifi() {
    // Check arduino EPS32 firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
        while (1);
    }

    // Attempt to connect to Wi-Fi connection using the provided credentials
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // Failed to connect, retry
        Serial.print(".");
        delay(5000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    Serial.println("Waiting for gateway IP");

    // Wait for the Wi-Fi to assign a gateway ip-address
    while (WiFi.gatewayIP() == "0.0.0.0");

    // Setting global connection status to True
    connected = true;

    Serial.print("received gateway IP:");
    Serial.println(WiFi.gatewayIP());
    Serial.println();

}

// Function for connecting to the broker
void connectBroker() {

    // Since it is on a local network it is easier to get the gateway ip and connect to the broker using that
    String gatewayIP = WiFi.gatewayIP().toString();
    const char* broker = gatewayIP.c_str();

    // Provide a username and password for authentication
    mqttClient.setUsernamePassword(mqtt_user, mqtt_pass);

    Serial.println("Attempting to connect to the MQTT broker.");

    // Pre-define the port integer
    int port     = 1883;

    // Keep trying to connect to the broker
    while (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        // In case the broker won't connect due to a disconnected Wi-Fi run the function to reconnect
        if (WiFi.status() != 0) {
            connectWifi();
        }
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}

// Setup function for all the values and connections
void setup() {

    // Create serial connection and wait for it to become available.
    Serial.begin(9600);
    dht11.begin(); // initialize the sensor

    while (!Serial) {
    }

    // Connect to Wi-Fi
    connectWifi();

    // Connect to MQTT broker
    connectBroker();

    // Set function for receiving MQTT messages from the subscribed topics
    mqttClient.onMessage(eclipseReceived);

    // Subscribe to topic for receiving information or commands
    mqttClient.subscribe("/IO");
}

void loop() {

    // Loop delay
    delay(2000);

    // Checks if there are any messages from the MQTT
    // Also sends out a pulse so the connection will not be broken due to inactivity
    mqttClient.poll();


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