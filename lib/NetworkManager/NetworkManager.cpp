/**
 * @file NetworkManager.cpp
 * @brief 
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 *  @bug regularly call myMQTTclient->loop() should not be solved like this
 */

#include "NetworkManager.h"

void callback2(char *topic, byte *payload, unsigned int length) {  // listens to incoming messages (published to Server)
    LOG4("callback2(char *topic, byte *payload, unsigned int length)");
    LOG2("a new message arrived (no: " + String(NetManTask.returnCurrentIterator()) + ")");
    String topic_str;
    for (int i = 0; topic[i] != '\0'; i++)  // iterate topic to topic_str
    {
        topic_str += topic[i];
    }
    if (LOGLEVELCONFIGURATION > 2) {
        String msg = "Message arrived [" + topic_str + "]: \t message:";
        for (unsigned int i = 0; i < length; i++)  // iterate message
        {
            msg += (char)payload[i];
        }
        LOG3(msg);
    }

    char inData[MAX_JSON_PARSE_SIZE];  // convert message to JSON object
    for (unsigned int i = 0; i < length; i++)
        inData[(i)] = (char)payload[i];

    StaticJsonBuffer<MAX_JSON_PARSE_SIZE> jsonBuffer;
    JsonObject &my_JSON = jsonBuffer.parseObject(inData);  // saves everything to my_JSON JSON Object
    if (!my_JSON.success()) {
        LOG1("JSON parsing failed");
        LOG2("my_JSON parsing failed in callback 2");
        return;
    }
    myJSONStr temp;
    temp.hostname = my_JSON.get<String>("hostname");
    temp.level = my_JSON.get<int>("level");
    temp.topic = topic_str;
    temp.request = my_JSON.get<String>("request");
    temp.urgent = my_JSON.get<bool>("urgent");
    JsonArray &parms = my_JSON["params"];
    temp.vehicleParams[0] = parms.get<double>(0);
    temp.vehicleParams[1] = parms.get<double>(1);
    temp.vehicleParams[2] = parms.get<double>(2);
    temp.vehicleParams[3] = parms.get<double>(3);
    // MORE TO ADD HERE

    String *mytop = NetManTask.returnMQTTtopics(temp);

    NetManTask.addMessage(temp);  // adds to message save

    if (LOGLEVELCONFIGURATION > 2) {
        LOG3("------ new JSON Message in JSarray Array ------");
        my_JSON.prettyPrintTo(Serial);  // prints JSON to Serial Monitor, pay attention can output BS
        LOG3("\n------ end of JSON Message ------");
    }
}

NetworkManager::NetworkManager() {  //Initialize DEFAULT serial & WiFi Module
    LOG4("NetworkManager::NetworkManager()");
    IPAddress defaultIPbroker(DEFAULT_MQTT_BROKER_IP1, DEFAULT_MQTT_BROKER_IP2, DEFAULT_MQTT_BROKER_IP3, DEFAULT_MQTT_BROKER_IP4);
    const int ppinss[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};
    initializeComponent(defaultIPbroker, DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD, DEFAULT_MQTT_PORT, ppinss);
}

NetworkManager::NetworkManager(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]) {  //Initialize COSTOM serial & WiFi Module
    LOG4("NetworkManager::NetworkManager(broker, ssid2, pass2, mmQTTport, pins[4])");
    initializeComponent(broker, ssid2, pass2, mmQTTport, pins);
}

void NetworkManager::loop() {
    LOG4("NetworkManager::loop()");
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    else
        myMQTTclient->loop();
}

bool NetworkManager::publishMessage(const String topic, const String msg) {  // publishes a message to the server
    LOG4("NetworkManager::publishMessage(const String topic, const String msg)");
    LOG3("try to publish to[" + topic + "] message: " + msg);
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    //connectToMQTT();
    if (myMQTTclient->connected()) {
        if (myMQTTclient->publish(topic.c_str(), msg.c_str())) {
            LOG2("message published");
            LOG3("Publish to topic [" + topic + "] message:" + msg);
        } else {
            LOG2("publish failed");
            return false;
        }
        myMQTTclient->loop();  // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    } else {
        MQTTConnectionFailed();
        LOG2("Publish fails: " + msg);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    };
}

bool NetworkManager::unsubscribe(const String topic) {
    LOG4("NetworkManager::unsubscribe(const String topic)");
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected()) {
        if (myMQTTclient->unsubscribe(topic.c_str()))
            LOG2("unsubscribed successfully");
        else {
            LOG2("unsubscribe failed");
            return false;
        }
        myMQTTclient->loop();  // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    } else {
        MQTTConnectionFailed();
        LOG2("Unsubscribe will fail: " + topic);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    }
}

bool NetworkManager::subscribe(const String topic) {  // subscribes to a new MQTT topic
    LOG4("NetworkManager::subscribe(const String topic) ");
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    //connectToMQTT();
    if (myMQTTclient->connected()) {
        LOG3("subscribing to: " + topic);
        if (myMQTTclient->subscribe(topic.c_str()))
            LOG2("suscription done");
        else {
            LOG2("suscription failed");
            return false;
        }
        myMQTTclient->loop();  // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    } else {
        MQTTConnectionFailed();
        LOG2("Subscription fails: " + topic);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    }
}

void NetworkManager::getInfo() {  // prints Information to Network
    LOG4("NetworkManager::getInfo()");
    myMQTTclient->loop();  // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();
    if (Serial) {
        Serial.println();
        Serial.println();
        Serial.println("-------------------- Information --------------------");
        Serial.print("SSID: ");
        Serial.println(ssid);
        Serial.print("IP Address: ");
        Serial.println(ip);
        Serial.print("BSSID (MAC Address from Router): ");
        Serial.print(macRouter[5], HEX);
        Serial.print(":");
        Serial.print(macRouter[4], HEX);
        Serial.print(":");
        Serial.print(macRouter[3], HEX);
        Serial.print(":");
        Serial.print(macRouter[2], HEX);
        Serial.print(":");
        Serial.print(macRouter[1], HEX);
        Serial.print(":");
        Serial.println(macRouter[0], HEX);
        Serial.print("MAC Address (Device): ");
        Serial.print(mac[5], HEX);
        Serial.print(":");
        Serial.print(mac[4], HEX);
        Serial.print(":");
        Serial.print(mac[3], HEX);
        Serial.print(":");
        Serial.print(mac[2], HEX);
        Serial.print(":");
        Serial.print(mac[1], HEX);
        Serial.print(":");
        Serial.println(mac[0], HEX);
        Serial.print("RSSI (signal strenght in dBm): ");
        Serial.println(rssi);
        Serial.print("Encryption (WLAN): ");
        Serial.println(encryption);
        Serial.print("WLAN status (WiFi lib): ");
        Serial.println(WiFi.status());
        Serial.println("-------------------- Information END --------------------");
        Serial.println();
        Serial.println();
    }
    // Note: MQTT subscriptions and messages not displayed here!
    myMQTTclient->loop();  // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
}

String NetworkManager::getHostName() {
    LOG4("NetworkManager::getHostName()");
    return this->hostname;
}

IPAddress NetworkManager::getIP() {
    LOG4("NetworkManager::getIP()");
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    return WiFi.localIP();
}

void NetworkManager::initializeComponent(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]) {
    LOG4("NetworkManager::initializeComponent(broker, ssid2, pass2, mmQTTport, pins[4])");
    ssid = ssid2;
    pass = pass2;
    NetManTask_classPointer = &NetManTask;
    if (is_vehicle) {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX);  // Create a random client ID for vehicles
    } else {
        hostname = DEFAULT_HOSTNAME_SMARTBOX + String(random(0xffff), HEX);  // Create a random client ID for vehicles
    }

    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    LOG3("==Connect to Network==");
    connectToWiFi();  //connect to WiFi

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();
    LOG3("Received Signal Strength [dBm]: " + String(rssi));
    LOG3("==Connect to MQTT==");
    this->brokerIP = broker;
    this->mQTT_port = mmQTTport;

    //myMQTTclient = new PubSubClient(brokerIP, mQTT_port, callback2, myClient);
    myMQTTclient = new PubSubClient(myClient);
    myMQTTclient->setServer(brokerIP, mmQTTport);
    myMQTTclient->setCallback(callback2);
    connectToMQTT();  // connecting to MQTT-Broker
}

//===================================PRIVATE======================================================================

/**
 * @bug Should this really return void?
 * 
 */
void NetworkManager::connectToWiFi() {
    LOG4("NetworkManager::connectToWiFi()");
    if (WiFi.status() == WL_NO_SHIELD)  // check if the shield is presence
    {
        LOG1("NO WiFi shield present");
        LOG2("WiFi Library could not find WiFi shield. " + decodeWiFistate(WiFi.status()));
        LOG3("programm is not continuing");
        while (true)
            ;  // don't continue
    }
    String wifi_firmware = WiFi.firmwareVersion();
    LOG3("WiFi Firmware Version = " + wifi_firmware);
    while (WiFi.status() != WL_CONNECTED)  // connect to Wifi network
    {
        LOG1("Attempting WLAN connection (WEP)...");
        LOG3("SSID: " + ssid);
        if (WiFi.begin(ssid, pass) != WL_CONNECTED) {
            LOG1("WLAN connection failed");
            LOG2("trying again in 3 seconds");
            delay(3000);
        } else {
            WiFi.hostname(hostname.c_str());
        }
    };
    LOG1("WLAN connected");
    LOG3("Board is connected to the Network");
}

String NetworkManager::decodeWiFistate(int errorcode) {
    LOG4("NetworkManager::decodeWiFistate(int errorcode)");
    switch (errorcode) {
        case 255:
            return "WL_NO_SHIELD";
        case 0:
            return "WL_IDLE_STATUS";
        case 1:
            return "WL_NO_SSID_AVAIL";
        case 2:
            return "WL_SCAN_COMPLETED";
        case 3:
            return "WL_CONNECTED";
        case 4:
            return "WL_CONNECT_FAILED";
        case 5:
            return "WL_CONNECTION_LOST";
        case 6:
            return "WL_DISCONNECTED";
        case 7:
            return "WL_AP_LISTENING";
        case 8:
            return "WL_AP_CONNECTED";
        case 9:
            return "WL_AP_FAILED";
        case 10:
            return "WL_PROVISIONING";
        case 11:
            return "WL_PROVISIONING_FAILED";
        default:
            return "Error";
    }
}

/**
 * @bug Should this really return void?
 * 
 */
void NetworkManager::connectToMQTT() {
    LOG4("NetworkManager::connectToMQTT()");
    while (!myMQTTclient->connected()) {
        LOG1("Attempting MQTT connection...");
        LOG3("MQTT Client ID: " + hostname);
        if (myMQTTclient->connect(hostname.c_str())) {
            LOG1("MQTT connected");
            LOG3("Variable myMQTT has successfully connected with hostname: " + hostname);
        } else {
            MQTTConnectionFailed();
            LOG2("trying again in 3 seconds");
            delay(3000);
        }
    }
}

String NetworkManager::decodeMQTTstate(int errorcode) {
    LOG4("NetworkManager::decodeMQTTstate(int errorcode)");
    switch (errorcode) {
        case -4:
            return "MQTT_CONNECTION_TIMEOUT";
        case -3:
            return "MQTT_CONNECTION_LOST";
        case -2:
            return "MQTT_CONNECT_FAILED";
        case -1:
            return "MQTT_DISCONNECTED";
        case 0:
            return "MQTT_CONNECTED";
        case 1:
            return "MQTT_CONNECT_BAD_PROTOCOL";
        case 2:
            return "MQTT_CONNECT_BAD_CLIENT_ID";
        case 3:
            return "MQTT_CONNECT_UNAVAILABLE";
        case 4:
            return "MQTT_CONNECT_BAD_CREDENTIALS";
        case 5:
            return "MQTT_CONNECT_UNAUTHORIZED";
        default:
            return "Error";
    }
}

void NetworkManager::MQTTConnectionFailed() {
    LOG4("NetworkManager::MQTTConnectionFailed()");
    LOG1("MQTT connection failed, error: " + decodeMQTTstate(myMQTTclient->state()));
    LOG3("client status: " + decodeMQTTstate(myMQTTclient->state()) + ", WiFi Status: " + decodeWiFistate(WiFi.status()));
}