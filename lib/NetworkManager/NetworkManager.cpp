#include "NetworkManager.h"

NetworkManager::NetworkManager() //Initialize DEFAULT serial & WiFi Module
{
    ssid = DEFAULT_WIFI_SSID;
    pass = DEFAULT_WIFI_PASSWORD;
    NetManTask_classPointer = &NetManTask;
    if (is_vehicle)
    {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles                              // set hostname to random ID
    }

    WiFi.setPins(DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN);

    connectToWiFi(); // connect to WiFi
    myClient = new WiFiClient;

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();

    IPAddress brokerIP(DEFAULT_MQTT_BROKER_IP1, DEFAULT_MQTT_BROKER_IP2, DEFAULT_MQTT_BROKER_IP3, DEFAULT_MQTT_BROKER_IP4);
    this->brokerIP = brokerIP;
    mQTT_port = DEFAULT_MQTT_PORT;

    myMQTTclient = new PubSubClient(brokerIP, DEFAULT_MQTT_PORT, callback2, *myClient);
    connectToMQTT(); // connecting to MQTT-Broker
}

NetworkManager::NetworkManager(IPAddress &broker, String *ssid2, String *pass2, byte *mmQTTport, byte pins[4]) //Initialize COSTOM serial & WiFi Module
{
    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    ssid = *ssid2;
    pass = *pass2;
    NetManTask_classPointer = &NetManTask;
    if (is_vehicle)
    {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles                              // set hostname to random ID
    }                                                                      // set hostname to random ID

    connectToWiFi(); //connect to WiFi
    myClient = new WiFiClient;

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();

    this->brokerIP = broker;
    this->mQTT_port = *mmQTTport;

    myMQTTclient = new PubSubClient(brokerIP, mQTT_port, callback2, *myClient);
    connectToMQTT(); // connecting to MQTT-Broker
}

void NetworkManager::connectToWiFi()
{
    if (WiFi.status() == WL_NO_SHIELD) // check for the presence of the shield:
    {
        LOG1("NO WiFi shield present");
        LOG2("WiFi Library could not find WiFi shield. WiFi.status returned " + WiFi.status());
        LOG3("programm is not continuing");
        while (true)
            ; // don't continue
    }
    String wifi_firmware = WiFi.firmwareVersion();
    LOG3("WiFi Firmware Version = " + wifi_firmware);

    while (WiFi.status() != WL_CONNECTED) // connect to Wifi network
    {
        LOG1("Attempting WLAN connection (WEP)...");
        LOG2("SSID: " + ssid);
        if (WiFi.begin(ssid, pass) != WL_CONNECTED)
        {
            LOG1("WLAN connection failed");
            LOG2("trying again in 3 seconds");
            delay(3000);
        }
        else
            WiFi.hostname(hostname.c_str());
    };
    LOG1("WLAN connected");
    LOG3("Board is connected to the Network");
}

void NetworkManager::connectToMQTT()
{
    while (!myMQTTclient->connected())
    {
        LOG1("Attempting MQTT connection...");
        LOG2("MQTT Client ID: " + hostname);
        if (myMQTTclient->connect(hostname.c_str()))
        {
            LOG1("MQTT connected");
            LOG2("Variable myMQTT has successfully connected with hostname: " + hostname);
        }
        else
        {
            LOG1("MQTT connection failed, error code: " + String(myMQTTclient->state()));
            LOG2("trying again in 3 seconds");
            delay(3000);
        }
    }
}

bool NetworkManager::publishMessage(const String &topic, const String &msg) // publishes a message to the server
{
    LOG3("try to publish message:" + msg);
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected())
    {
        myMQTTclient->publish(topic.c_str(), msg.c_str());
        LOG2("Publish to topic [" + topic + "] message:" + msg);
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, publish fails: " + msg);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    };
}

bool NetworkManager::subscribe(const String &topic) // subscribes to a new MQTT topic
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected())
    {
        myMQTTclient->subscribe(topic.c_str());
        LOG3("subscribing to:" + topic);
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, subscription fails: " + topic);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    }
}

bool NetworkManager::unsubscribe(const String &topic)
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected())
    {
        myMQTTclient->unsubscribe(topic.c_str());
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, unsubscribe will fail: " + topic);
        LOG3("Client ID: " + hostname);
        connectToMQTT();
        return false;
    }
}

void callback2(char *topic, byte *payload, unsigned int length) // listens to incoming messages (published to Server)
{

    String topic_str;
    for (int i = 0; topic[i] != '\0'; i++) // iterate topic to topic_str
    {
        topic_str += topic[i];
    }
    if (LOGLEVELCONFIGURATION > 2)
    {
        String msg = "Message arrived [" + topic_str + "]: \t message:";
        for (unsigned int i = 0; i < length; i++) // iterate message
        {
            msg += (char)payload[i];
        }
        LOG3(msg);
    }

    char inData[MAX_JSON_PARSE_SIZE]; // convert message to JSON object
    for (unsigned int i = 0; i < length; i++)
        inData[(i)] = (char)payload[i];

    StaticJsonBuffer<MAX_JSON_PARSE_SIZE> jsonBuffer;
    JsonObject &my_JSON = jsonBuffer.parseObject(inData); // saves everything to my_JSON JSON Object
    if (!my_JSON.success())
    {
        LOG1("JSON parsing failed");
        LOG2("my_JSON parsing failed in callback 2");
        return;
    }
    myJSONStr temp;
    temp.hostname = my_JSON.get<String>("hostname");
    temp.level = my_JSON.get<int>("level");
    temp.topic = my_JSON.get<String>("topic");
    temp.request = my_JSON.get<String>("request");
    // MORE TO ADD HERE

    NetManTask.addMessage(temp); // adds to message save

    if (LOGLEVELCONFIGURATION > 2)
    {
        LOG3("------ new JSON Message in JSarray Array ------");
        my_JSON.prettyPrintTo(Serial); // prints JSON to Serial Monitor, pay attention can output BS
        LOG3("\n------ end of JSON Message ------");
    }
}

void NetworkManager::loop()
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    myMQTTclient->loop();
}

String NetworkManager::getHostName()
{
    return this->hostname;
}

IPAddress NetworkManager::getIP()
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    ip = WiFi.localIP();
    return ip;
}

void NetworkManager::getInfo() // prints Information to Network
{
    myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();
    if (Serial)
    {
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
    myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
}