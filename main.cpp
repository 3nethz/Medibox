#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <DHTesp.h>

#define LDR_left 33
#define LDR_right 32
#define SERVO_PIN 18
#define DHTPIN 15

char LDRmaxstr[5];
float D;
float Gamma = 0.75;
float OFFSET = 30;
float I;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
Servo servo;
DHTesp dhtSensor;

// Setup WiFi connection
void setupWifi()
{
    WiFi.begin("Wokwi-GUEST", "");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
    }
}

// MQTT callback function
void recieveCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char payLoadCharArr[length];
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        payLoadCharArr[i] = (char)payload[i];
    }
    Serial.println();
    if (strcmp(topic, "Medibox-from-node-red") == 0)
    {
        if (length == 1)
        {
            OFFSET = payLoadCharArr[0] - '0';
        }
        else
        {
            OFFSET = atof(payLoadCharArr);
        }
    }
    else if (strcmp(topic, "Medibox- Gamma") == 0)
    {
        if (length == 1)
        {
            Gamma = payLoadCharArr[0] - '0';
        }
        else
        {
            Gamma = atof(payLoadCharArr);
        }
    }
}


// Setup MQTT client
void setupMqtt()
{
    mqttClient.setServer("test.mosquitto.org", 1883);
    mqttClient.setCallback(recieveCallback);
}

// Connect to MQTT broker
void connectToBroker()
{
    Serial.print("Attempting MQTT connection...");
    while (!mqttClient.connected())
    {
        if (mqttClient.connect("ESP32Client642456"))
        {
            Serial.print("Connected");
            mqttClient.subscribe("Medibox-from-node-red");
            mqttClient.subscribe("Medibox- Gamma");
        }
        else
        {
            delay(5000);
        }
    }
}

// Read DHT sensor data and publish to MQTT
void readDHTSensorDataAndPublish()
{
    TempAndHumidity newValues = dhtSensor.getTempAndHumidity();
    mqttClient.publish("Medibox-Temp-Data", String(newValues.temperature).c_str());
}

// Calculate maximum lux
void getMaxLux(int LDRleft, int LDRright)
{
    if (LDRleft < LDRright)
    {
        itoa(LDRleft, LDRmaxstr, 10);
        D = 1.5;
    }
    else
    {
        itoa(LDRright, LDRmaxstr, 10);
        D = 0.5;
    }
    I = 1 - (atof(LDRmaxstr) / 1024);
}

// Calculate angle
float calCulateAngle(float D, float I)
{
    return min(OFFSET * D + (180 - OFFSET) * I * Gamma, (float)180);
}

void setup()
{
    servo.attach(SERVO_PIN, 500, 2400);
    dhtSensor.setup(DHTPIN, DHTesp::DHT22);
    Serial.begin(115200);

    pinMode(LDR_left, INPUT);
    pinMode(LDR_right, INPUT);

    analogReadResolution(10);

    setupWifi();
    setupMqtt();
}

void loop()
{
    readDHTSensorDataAndPublish();
    getMaxLux(analogRead(LDR_left), analogRead(LDR_right));

    if (!mqttClient.connected())
    {
        connectToBroker();
    }
    mqttClient.loop();
    mqttClient.publish("Medibox-Light-Data", LDRmaxstr);

    servo.write(calCulateAngle(D, I));
    // Serial.println(Gamma);
    delay(1000);
}