#ifndef NETWORKCONFIGURATION_H
#define NETWORKCONFIGURATION_H

/*
    file that contains the default network configuration
    Note:
        DEFAULT_WIFI_* are WLAN Pins from Module on Adafruit Feather M0 (see Adafruit ATWINC1500 Feather)
        DEFAULT_MQTT_* are MQTT defaults
*/

#define DEFAULT_WIFI_SSID "DigitalLab"
#define DEFAULT_WIFI_PASSWORD "digital42HSR"
#define DEFAULT_HOSTNAME "Wagen_default_"
#define DEFAULT_WIFI_CS 8 // Pins for Adafruit ATWINC1500 Feather
#define DEFAULT_WIFI_IRQ 7
#define DEFAULT_WIFI_RST 4
#define DEFAULT_WIFI_EN 2
#define DEFAULT_MQTT_BROKER_IP1 192
#define DEFAULT_MQTT_BROKER_IP2 168
#define DEFAULT_MQTT_BROKER_IP3 1
#define DEFAULT_MQTT_BROKER_IP4 7
#define DEFAULT_MQTT_PORT 1883
#define MAX_JSON_MESSAGES_SAVED 50 // max num of saved JSON items

#endif