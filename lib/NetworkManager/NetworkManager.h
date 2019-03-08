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
#include <WiFi101.h>
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
static const int default_pins[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};
const bool is_vehicle = false;  ///< true if is vehicle, used for MQTT
static MQTTTasks NetManTask;    ///< saves all messages, saves all incoming messages as JSON Objects, FIFO order, num of items: \link MAX_JSON_MESSAGES_SAVED \endlink

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
 * @brief The Class connects the board wict WLAN
 * 
 * The Network-Manager Class saves SSID, WLAN-Password, Hostname, IP-Address, IP-Adress of the router, the owen mac-Adress
 * reception quality (RSSI), wlan-encryption, IP-adress of the MQTT Broker, a pointer on a PubSubClient and the MQTT-Port. \n
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
     * @param ssid2 
     * @param pass2 
     * @param mmQTTport 
     * @param pins 
     */
    NetworkManager(IPAddress broker, String ssid2 = DEFAULT_WIFI_SSID, String pass2 = DEFAULT_WIFI_PASSWORD, const int mmQTTport = DEFAULT_MQTT_PORT, const int pins[4] = default_pins);  // COSTUM C'tor
    //NetworkManager(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]); // COSTUM C'tor 2

    /**
     * @brief handles outgoing MQTT messages to Server
     * 
     * @param topic 
     * @param msg 
     * @return true 
     * @return false 
     */
    bool publishMessage(const String topic, const String msg);

    /**
     * @brief unsubsribes to MQTT topic on Server
     * 
     * @param topic 
     * @return true 
     * @return false 
     */
    bool unsubscribe(const String topic);

    /**
     * @brief subsribes to MQTT topic on Server
     * 
     * @param topic 
     * @return true 
     * @return false 
     */
    bool subscribe(const String topic);

    /**
     * @brief prints out some information about that object
     * 
     */
    void getInfo();

    /**
     * @brief make client ready for receiving messages
     * 
     */
    void loop();

    /**
     * @brief get hostname of this object
     * 
     * @return String 
     */
    String getHostName();

    /**
     * @brief Get current IP Adress
     * 
     * @return IPAddress 
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
    */
    void connectToWiFi();

    /**
     * @brief connects to MQTT Broker based on below stored attributes
     * 
     */
    void connectToMQTT();

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
    String ssid;           ///< Contains SSID
    String pass;           ///<
    String hostname;       ///< Contains Hostname
    IPAddress ip;          ///<
    byte macRouter[6];     ///<
    byte mac[6];           ///<
    long rssi;             ///<
    byte encryption;       ///<
    WiFiServer *myServer;  ///<
    WiFiClient myClient;   ///<

    // MQTT stuff
    PubSubClient *myMQTTclient;  ///<
    IPAddress brokerIP;          ///<
    int mQTT_port;               ///<
};

#endif