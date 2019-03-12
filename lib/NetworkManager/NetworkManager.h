/**
 * @file NetworkManager.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief 
 * @version 0.1
 * @date 2019-03-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi101.h>  ///< https://www.arduino.cc/en/Reference/WiFi101
#include <cstring>
// own files:
#include <LogConfiguration.h>
#include <MQTTTasks.h>
#include <NetworkConfiguration.h>
#include <NetworkManagerStructs.h>

/**
 * @brief default_pins array contains contains on whiche Pins the Wifi-Shield connections are.
 * 
 */
static const int default_pins[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};  ///< Contains Hardware-Pinnumbers
const bool is_vehicle = false;                                                                              ///< true if is vehicle, used for MQTT
static MQTTTasks NetManTask;                                                                                ///< saves all messages, saves all incoming messages as JSON Objects, FIFO order, num of items: \link MAX_JSON_MESSAGES_SAVED \endlink

/**
 * @brief callback2
 * 
 * needs to be outside class!
 * 
 * @param topic 
 * @param payload 
 * @param length 
 * 
 * /todo why outside class?
 */

void callback2(char *topic, byte *payload, unsigned int length);

/**
 * @brief The Class connects the board with WLAN
 * 
 * The Network-Manager Class saves SSID, WLAN-Password, Hostname, IP-Address, IP-Adress of the router, the own MAC-Adress
 * reception quality (RSSI), WLAN-encryption, IP-adress of the MQTT Broker, a pointer on a PubSubClient and the MQTT-Port. \n
 * The Class contains two constructor, a default and one where you can handover the parameters on your own. \n
 * The MQTT functions are in publishMessage, subscrube, unsibscribe, and loop.
 * The functions getInfo, getHostname and getIP are needed for debbugung.
 */
class NetworkManager {
   public:
    /**
    * @brief Construct a new Network Manager object
    * 
    */
    NetworkManager();  // DEFAULT C'tor
    //NetworkManager(IPAddress broker); // COSTUM C'tor 1

    /**
     * @brief Construct a new Network Manager object
     * 
     * @param broker 
     * @param ssid2 - \link DEFAULT_WIFI_SSID \endlink
     * @param pass2 - \link DEFAULT_WIFI_PASSWORD \endlink
     * @param mmQTTport - \link DEFAULT_MQTT_PORT \endlink
     * @param pins - \link default_pins \endlink
     */
    NetworkManager(IPAddress broker, String ssid2 = DEFAULT_WIFI_SSID, String pass2 = DEFAULT_WIFI_PASSWORD, const int mmQTTport = DEFAULT_MQTT_PORT, const int pins[4] = default_pins);  // COSTUM C'tor
    //NetworkManager(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]); // COSTUM C'tor 2

    /**
     * @brief make client ready for receiving messages
     * 
     */
    void loop();

    /**
     * @brief handles outgoing MQTT messages to Server
     * 
     * @param topic - the topic to publish to (const char[])
     * @param msg - the message to publish (const char[])
     * @return true - 
     * @return false - 
     */
    bool publishMessage(const String topic, const String msg);

    /**
     * @brief unsubsribes from MQTT topic on Server
     * 
     * If no WiFi is connected, call \link connectToWiFi() \endlink \n
     *  and afterwards check if the client is connected to the server
     * 
     * @param topic - the topic to unsubscribe from (const char[])
     * @return true - sending the unsubscribe succeeded. The request completes asynchronously.
     * @return false - sending the unsubscribe failed, either connection lost, or message too large.
     */
    bool unsubscribe(const String topic);

    /**
     * @brief subsribes to MQTT topic on Server
     * 
     * If no WiFi is connected, call \link connectToWiFi() \endlink \n
     *  and afterwards check if the client is connected to the server
     * 
     * @param topic - the topic to subscribe to (const char[])
     * @return true -  sending the subscribe succeeded. The request completes asynchronously.
     * @return false - sending the subscribe failed, either connection lost, or message too large.
     */
    bool subscribe(const String topic);

    /**
     * @brief Prints out information via Serial about Network
     * 
     * SSID \n
     * IP Address \n
     * BSSID (MAC Address from Router) \n
     * MAC Address (Device)\n
     * RSSI (signal strenght in dBm)\n
     * Encryption (WLAN) \link NetworkManager::encryption \endlink  \n
     * WLAN status (WiFi lib) WiFiClass::status 
     */
    void getInfo();

    /**
     * @brief get hostname of this object
     * 
     * @return String - hostname
     */
    String getHostName();

    /**
     * @brief Try's to connect to WiFI and Gets the WiFi shield's IP address
     * 
     * @return IPAddress - the IP address of the shield
     */
    IPAddress getIP();

    /**
     * @brief used to see saved Messages from outside this class
     * 
     */
    MQTTTasks *NetManTask_classPointer;

   private:
    /**
    * @brief connects to WiFi based on below stored attributes
    * 
    * If the connection fails it will try to reconnect all 3 seconds.
    * The Function will not be exited without an activ connection.
    */
    void connectToWiFi();

    /**
     * @brief Decodes the Error Values from Wifi status() and returns a description
     * 
     * @param errorcode - errorvalue from wifi101
     * @return String - Errordescription
     */
    String decodeWiFistate(int errorcode);

    /**
     * @brief connects to MQTT Broker based on below stored attributes
     * 
     */
    void connectToMQTT();

    /**
     * @brief Decodes the Error Values from MQTT client.state() and returns a description
     * 
     * @param errorcode - errorvalue from client
     * @return String - Errordescription
     */
    String decodeMQTTstate(int errorcode);

    /**
     * @brief Writes Error-Message about MQTT and WLAN connection
     * 
     */
    void MQTTConnectionFailed();

    /**
     * @brief needed to call diffrent C'tors from each other
     * 
     * @param broker 
     * @param ssid2 
     * @param pass2 
     * @param mmQTTport 
     * @param pins 
     */
    void initializeComponent(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]);

    // WIFI stuff
    String ssid;        ///< Contains the SSID the WiFi shield is currently connected to.
    String pass;        ///< Contains WiFi Password
    String hostname;    ///< Contains Hostname
    IPAddress ip;       ///< Contains own IP-Adress
    byte macRouter[6];  ///< Contains MAC Adress of the Router
    byte mac[6];        ///< Contains own MAC Adress
    long rssi;          ///< reception quality - the current RSSI /Received Signal Strength in dBm
    /**
     * @brief value represents the type of encryption
     * 
     * TKIP (WPA) = 2
     * WEP = 5
     * CCMP (WPA) = 4
     * NONE = 7
     * AUTO = 8
     */
    byte encryption;

    WiFiServer *myServer;  ///<
    WiFiClient myClient;   ///<

    // MQTT stuff
    PubSubClient *myMQTTclient;  ///<
    IPAddress brokerIP;          ///<
    int mQTT_port;               ///<
};

#endif