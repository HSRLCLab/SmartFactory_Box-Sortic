/**
 * @file CommunicationConfiguration.h
 * @brief file that contains the default network configuration
 * 
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-07
 * 
 * @copyright Copyright (c) 2019
 * 
 * @todo Switch Password to .ini file \n
 * https://community.platformio.org/t/how-to-specify-ota-upload-password-outside-platformio-ini/3588 \n
 * http://docs.platformio.org/en/latest/envvars.html#building
 */

#ifndef COMMUNICATIONCONFIGURATION_H
#define COMMUNICATIONCONFIGURATION_H

//=============WLAN CONNECTION==================================
#define DEFAULT_WIFI_SSID "DigitalLab"        ///< SSID to connect to
#define DEFAULT_WIFI_PASSWORD "digital42HSR"  ///< Password to corresponding SSID

// Pins for Adafruit ATWINC1500 Feather https://learn.adafruit.com/adafruit-feather-m0-wifi-atwinc1500/using-the-wifi-module
#define DEFAULT_WIFI_CS 8   ///<
#define DEFAULT_WIFI_IRQ 7  ///<
#define DEFAULT_WIFI_RST 4  ///<
#define DEFAULT_WIFI_EN 2   ///<

//=============MQTT==================================
// * DEFAULT_MQTT_* are MQTT defaults
#define DEFAULT_MQTT_BROKER_IP1 192  ///< broker IP distributed in 4 values
#define DEFAULT_MQTT_BROKER_IP2 168  ///<
#define DEFAULT_MQTT_BROKER_IP3 1    ///<
#define DEFAULT_MQTT_BROKER_IP4 22   ///<
#define DEFAULT_MQTT_PORT 1883       ///< MQTT connection port

// #define DEFAULT_HOSTNAME_RAND String(random(0xffff), HEX)  ///< for Vehicles, used for MQTT and WiFi, must be unique in Network
const int DEFAUL_HOSTNAME_NUMBER = 3; 
#define DEFAULT_HOSTNAME (String("SB") + String(DEFAUL_HOSTNAME_NUMBER))  // + DEFAULT_HOSTNAME_RAND)
// #define DEFAULT_HOSTNAME_SMARTBOX "SmartBox"  ///< for SmartBoxes, used for MQTT and WiFi, must be unique in Network
// #define DEFAULT_HOSTNAME_VEHICLE "Vehicle"    ///< for Vehicles, used for MQTT and WiFi, must be unique in Network

//=============JSON==================================
#define MAX_JSON_PARSE_SIZE 300     ///< max buffer size to parse JSON objects, size of the pool in bytes, can be calculated in https://arduinojson.org/v5/assistant/
#define MAX_JSON_MESSAGES_SAVED 20  ///< max num of saved JSON items, must be smaller than num of vehicles!
#define MAX_MQTT_TOPIC_DEPTH 5      ///< how many topics can be in row, e.g. SmartBox/SB1/level are 3 topic levels

// {
//   "id": "hostanme",
//   "topic": "tpois/topic",
//   "status": "driving",
//   "sector": "transit",
//   "line": 1,
//   "ack":"hostname",
//   "req":"hostname",
//   "cargo":"gemuse",
//   "error":false
// }

struct myJSONStr {
    String id = "-1";
    String topic = "-1";
    String status = "-1";
    String sector = "-1";
    int line = -1;
    String ack = "-1";
    String req = "-1";
    String cargo = "-1";
    bool token = false;
    bool error = false;
};


#endif