#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <cstring>
// own files:
#include <NetworkConfiguration.h>
#include <NetworkManagerStructs.h>
#include <MQTTTasks.h>
#include <LogConfiguration.h>

static const int default_pins[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};
const bool is_vehicle = false; // true if is vehicle, used for MQTT
static MQTTTasks NetManTask;   // saves all messages, saves all incoming messages as JSON Objects, FIFO order, num of items: MAX_JSON_MESSAGES_SAVED


void callback2(char *topic, byte *payload, unsigned int length); // needs to be outside class!

class NetworkManager
{
public:
  NetworkManager(); // DEFAULT C'tor
  //NetworkManager(IPAddress broker); // COSTUM C'tor 1
  NetworkManager(IPAddress broker, String ssid2 = DEFAULT_WIFI_SSID, String pass2 = DEFAULT_WIFI_PASSWORD, const int mmQTTport = DEFAULT_MQTT_PORT, const int pins[4] = default_pins); // COSTUM C'tor
  //NetworkManager(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]); // COSTUM C'tor 2
  bool publishMessage(const String topic, const String msg); // handles outgoing MQTT messages to Server
  bool unsubscribe(const String topic);
  bool subscribe(const String topic); // subsribes to MQTT topic on Server
  void getInfo();                     // prints out some information about that object
  void loop();                        // make client ready for receiving messages
  String getHostName();               // returns hostname of this object
  IPAddress getIP();                  // return current IP Address
  MQTTTasks *NetManTask_classPointer; // used to see saved Messages from outside this file

private:
  void connectToWiFi();                                                                                           // connects to WiFi based on below stored attributes
  void connectToMQTT();                                                                                           // connects to MQTT Broker based on below stored attributes
  void initializeComponent(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]); // needed to call diffrent C'tors from each other
  // WIFI stuff
  String ssid;
  String pass;
  String hostname;
  IPAddress ip;
  byte macRouter[6];
  byte mac[6];
  long rssi;
  byte encryption;
  WiFiServer *myServer;
  WiFiClient myClient;
  // MQTT stuff
  PubSubClient *myMQTTclient;
  IPAddress brokerIP;
  int mQTT_port;
};

#endif