#include <PubSubClient.h>
#include <WiFi.h>

WifiClient espClient; 
PubSubClient mqttClient(espClient);

void setup() {
    Serial.begin(115200);
    setupWifi();
    setupMqtt();
}

void loop() {   
    if (!mqttClient.connected()) {
        connectToBroker();

    mqttClient.loop();
    mqttClient.publish("esp32/output", "Hello from ESP32");
    delay(1000);

}
}
void setupWifi() {
    WiFi.begin("Wokwi-GUEST", "", );
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
    }
}

void setupMqtt() {
    mqttClient.setServer("test.mosquitto.org", 1883);
}

void connectToBroker() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}




