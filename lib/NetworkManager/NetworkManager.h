#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <cstring>
// own files
#include <NetworkConfiguration.h>
#include <NetworkManagerStructs.h>

extern const int log_level;    // defined in main.cpp
const bool is_vehicle = false; // true if is vehicle, used for MQTT
extern int my_json_counter;
extern bool my_json_counter_isEmpty;
const byte default_pins[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};

void callback2(char *topic, byte *payload, unsigned int length); // needs to be outside class!
// static DynamicJsonBuffer dynjsonBuffer;
static myJSONStr JSarray[MAX_JSON_MESSAGES_SAVED]; // saves all incoming messages as JSON Objects, FIFO order, num of items: MAX_JSON_MESSAGES_SAVED

class NetworkManager
{
public:
  NetworkManager();                                                                                                                                                                                                  // DEFAULT C'tor
  NetworkManager(IPAddress &broker, String *ssid2 = (String *)DEFAULT_WIFI_SSID, String *pass2 = (String *)DEFAULT_WIFI_PASSWORD, byte *mmQTTport = (byte *)DEFAULT_MQTT_PORT, byte pins[4] = (byte *)default_pins); // COSTUM C'tor
  bool publishMessage(const String &topic, const String &msg);                                                                                                                                                       // handles outgoing MQTT messages to Server
  bool unsubscribe(const String &topic);
  bool subscribe(const String &topic); // subsribes to MQTT topic on Server
  void getInfo();                      // prints out some information about that object
  void loop();                         // make client ready for receiving messages
  String getHostName();                // returns hostname of this object
  IPAddress getIP();                   // return current IP Address
  myJSONStr *JSarrP;                 // used to see saved Messages from outside this file

private:
  void log(const String &log1, const String &log2, const String &log3); // logging function, see log_level
  void connectToWiFi();                                                 // connects to WiFi based on below stored attributes
  void connectToMQTT();                                                 // connects to MQTT Broker based on below stored attributes
  // WIFI stuff
  String ssid;
  String pass;
  String hostname;
  IPAddress ip;
  byte macRouter[6];
  byte mac[6];
  long rssi;
  byte encryption;
  int status;
  WiFiServer *myServer;
  WiFiClient *myClient;
  // MQTT stuff
  //MQTT_CALLBACK_SIGNATURE;
  PubSubClient *myMQTTclient;
  IPAddress brokerIP;
  int mQTT_port;
  // TODO: how to save messages?
};

#endif