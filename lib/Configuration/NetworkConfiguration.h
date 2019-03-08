/**
 * @file NetworkConfiguration.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief file that contains the default network configuration
 * 
 * DEFAULT_WIFI_* are WLAN Pins from Module on Adafruit Feather M0 (see Adafruit ATWINC1500 Feather)
 * DEFAULT_MQTT_* are MQTT defaults
 * 
 * @version 0.1
 * @date 2019-03-07
 * 
 * @copyright Copyright (c) 2019
 * 
 * @todo Switch Password to .ini file \n
 * https://community.platformio.org/t/how-to-specify-ota-upload-password-outside-platformio-ini/3588 \n
 * http://docs.platformio.org/en/latest/envvars.html#building
 */

#ifndef NETWORKCONFIGURATION_H
#define NETWORKCONFIGURATION_H

#define DEFAULT_WIFI_SSID "DigitalLab"        ///< SSID to connect to
#define DEFAULT_WIFI_PASSWORD "digital42HSR"  ///< Password to corresponding SSID
#define DEFAULT_HOSTNAME_SMARTBOX "SmartBox"  ///< for SmartBoxes, used for MQTT and WiFi, must be unique in Network
#define DEFAULT_HOSTNAME_VEHICLE "Vehicle"    ///< for Vehicles, used for MQTT and WiFi, must be unique in Network
#define DEFAULT_WIFI_CS 8                     ///< Pins for Adafruit ATWINC1500 Feather
#define DEFAULT_WIFI_IRQ 7                    ///<
#define DEFAULT_WIFI_RST 4                    ///<
#define DEFAULT_WIFI_EN 2                     ///<
#define DEFAULT_MQTT_BROKER_IP1 192           ///< broker IP distributed in 4 values
#define DEFAULT_MQTT_BROKER_IP2 168           ///<
#define DEFAULT_MQTT_BROKER_IP3 1             ///<
#define DEFAULT_MQTT_BROKER_IP4 7             ///<
#define DEFAULT_MQTT_PORT 1883                ///< MQTT connection port
#define MAX_JSON_PARSE_SIZE 300               ///< max buffer size to parse JSON objects, size of the pool in bytes, can be calculated in https://arduinojson.org/v5/assistant/

#endif