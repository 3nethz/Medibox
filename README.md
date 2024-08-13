# Medibox

Medibox is a smart medication reminder system built using an ESP32 microcontroller. It helps users manage their medication schedule by setting alarms, monitoring environmental conditions, and displaying relevant information on an OLED screen.

## Features

- **Alarm System**: Set up to three customizable alarms to remind users to take their medication.
- **NTP Time Synchronization**: Automatically synchronizes time using an NTP server to ensure accuracy.
- **OLED Display**: Provides a simple interface for setting alarms, checking the time, and viewing notifications.
- **Temperature and Humidity Monitoring**: Continuously monitors the environment and alerts users if the conditions are outside the desired range.
- **Buzzer and LED Alerts**: Audible and visual notifications to ensure alarms and warnings are noticed.
- **User-Friendly Menu Navigation**: Easy-to-use buttons allow for navigating the menu, setting alarms, and configuring time zones.

## Getting Started

1. **Hardware Setup**: 
   - Assemble the Medibox by connecting the OLED display, DHT22 sensor, buzzer, LEDs, and push buttons to the ESP32. Ensure all components are correctly connected and powered.

2. **Software Setup**:
   - Install the necessary libraries such as Adafruit GFX, Adafruit SSD1306, and DHT Sensor.
   - Clone the repository and upload the code to your ESP32 using your preferred IDE.

3. **Wi-Fi Configuration**:
   - Ensure the device is connected to a Wi-Fi network for time synchronization.
![image](https://github.com/user-attachments/assets/0beca7b4-e5d2-4ddf-bfd8-0927920ab400)



## Usage

1. **Set Time Zone**:
   - Navigate through the menu to set your local time zone.

2. **Set Alarms**:
   - Set up to three alarms to remind you when it's time to take your medication.

3. **Environmental Monitoring**:
   - The Medibox will automatically monitor the temperature and humidity, providing alerts if conditions are not ideal.

