#include "NetworkManager.h"

NetworkManager::NetworkManager() //Initialize DEFAULT serial & WiFi Module
{
    IPAddress defaultIPbroker(DEFAULT_MQTT_BROKER_IP1, DEFAULT_MQTT_BROKER_IP2, DEFAULT_MQTT_BROKER_IP3, DEFAULT_MQTT_BROKER_IP4);
    const int ppinss[4] = {DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN};
    initializeComponent(defaultIPbroker, DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD, DEFAULT_MQTT_PORT, ppinss);

    /*
    if (is_vehicle)
    {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles                              // set hostname to random ID
    }
    */

    /*
    NetManTask_classPointer = &NetManTask;
    WiFi.setPins(DEFAULT_WIFI_CS, DEFAULT_WIFI_IRQ, DEFAULT_WIFI_RST, DEFAULT_WIFI_EN);

    connectToWiFi(); // connect to WiFi

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();

    this->brokerIP = bbrokerIIP;
    mQTT_port = DEFAULT_MQTT_PORT;

    // myMQTTclient = new PubSubClient(brokerIP, DEFAULT_MQTT_PORT, callback2, *myClient);
    myMQTTclient = new PubSubClient(myClient);
    myMQTTclient->setServer(brokerIP, DEFAULT_MQTT_PORT);
    myMQTTclient->setCallback(callback2);
    connectToMQTT(); // connecting to MQTT-Broker
    */
}

NetworkManager::NetworkManager(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4]) //Initialize COSTOM serial & WiFi Module
{
    initializeComponent(broker, ssid2, pass2, mmQTTport, pins);
    /*
    ssid = ssid2;
    pass = pass2;
    NetManTask_classPointer = &NetManTask;
    if (is_vehicle)
    {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }

    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    connectToWiFi(); //connect to WiFi

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();

    this->brokerIP = broker;
    this->mQTT_port = mmQTTport;

    //myMQTTclient = new PubSubClient(brokerIP, mQTT_port, callback2, myClient);
    myMQTTclient = new PubSubClient(myClient);
    myMQTTclient->setServer(brokerIP, mmQTTport);
    myMQTTclient->setCallback(callback2);
    connectToMQTT(); // connecting to MQTT-Broker
    */
}

void NetworkManager::initializeComponent(IPAddress broker, String ssid2, String pass2, const int mmQTTport, const int pins[4])
{
    ssid = ssid2;
    pass = pass2;
    NetManTask_classPointer = &NetManTask;
    if (is_vehicle)
    {
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles
    }

    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    connectToWiFi(); //connect to WiFi

    ip = WiFi.localIP();
    ssid = WiFi.SSID();
    WiFi.macAddress(mac);
    WiFi.BSSID(macRouter);
    rssi = WiFi.RSSI();
    encryption = WiFi.encryptionType();

    this->brokerIP = broker;
    this->mQTT_port = mmQTTport;

    //myMQTTclient = new PubSubClient(brokerIP, mQTT_port, callback2, myClient);
    myMQTTclient = new PubSubClient(myClient);
    myMQTTclient->setServer(brokerIP, mmQTTport);
    myMQTTclient->setCallback(callback2);
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
        LOG3("SSID: " + ssid);
        if (WiFi.begin(ssid, pass) != WL_CONNECTED)
        {
            LOG1("WLAN connection failed");
            LOG2("trying again in 3 seconds");
            delay(3000);
        }
        else
        {
            WiFi.hostname(hostname.c_str());
        }
    };
    LOG1("WLAN connected");
    LOG3("Board is connected to the Network");
    /*
    myClient.stop();    // TODO why fails?
    bool connec = false;
    while (!connec)
    {
        if (myClient.connect(brokerIP, 80))
        {
            LOG1("client connected");
            connec = true;
        }
        else
        {
            LOG1("client not connected");
            LOG3("status: " + String(myClient.status())); // 0 = WL_IDLE_STATUS
        }
    }
    LOG3("Board is connected to the Network and the client is connected");
    */
}

void NetworkManager::connectToMQTT()
{
    while (!myMQTTclient->connected())
    {
        LOG1("Attempting MQTT connection...");
        LOG3("MQTT Client ID: " + hostname);
        if (myMQTTclient->connect(hostname.c_str()))
        {
            LOG1("MQTT connected");
            LOG3("Variable myMQTT has successfully connected with hostname: " + hostname);
        }
        else
        {
            LOG1("MQTT connection failed, error code: " + String(myMQTTclient->state()));
            LOG2("trying again in 3 seconds");
            LOG3("client status:" + String(myMQTTclient->state()) + ", WiFi Status: " + String(WiFi.status()));
            delay(3000);
        }
    }
}

bool NetworkManager::publishMessage(const String topic, const String msg) // publishes a message to the server
{
    LOG3("try to publish to[" + topic + "] message: " + msg);
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    //connectToMQTT();
    if (myMQTTclient->connected())
    {
        myMQTTclient->publish(topic.c_str(), msg.c_str());
        LOG2("message published");
        LOG3("Publish to topic [" + topic + "] message:" + msg);
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, publish fails: " + msg);
        LOG3("Client ID: " + hostname);
        LOG3("client status:" + String(myMQTTclient->state()) + ", WiFi Status: " + String(WiFi.status()));
        connectToMQTT();
        return false;
    };
}

bool NetworkManager::subscribe(const String topic) // subscribes to a new MQTT topic
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    //connectToMQTT();
    if (myMQTTclient->connected())
    {
        LOG3("subscribing to: " + topic);
        //LOG3("hello" + String(myMQTTclient->subscribe("SmartBox")));
        bool done = myMQTTclient->subscribe(topic.c_str());
        if (done)
            LOG2("suscription done");
        else
            LOG2("suscription failed");
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, subscription fails: " + topic);
        LOG3("Client ID: " + hostname);
        LOG3("client status:" + String(myMQTTclient->state()) + ", WiFi Status: " + String(WiFi.status()));
        connectToMQTT();
        return false;
    }
}

bool NetworkManager::unsubscribe(const String topic)
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected())
    {
        myMQTTclient->unsubscribe(topic.c_str());
        LOG2("unsubscribed successfully");
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        LOG1("MQTT not connected");
        LOG2("You are not connected to the MQTT Broker, unsubscribe will fail: " + topic);
        LOG3("Client ID: " + hostname);
        LOG3("client status:" + String(myMQTTclient->state()) + ", WiFi Status: " + String(WiFi.status()));
        connectToMQTT();
        return false;
    }
}

void callback2(char *topic, byte *payload, unsigned int length) // listens to incoming messages (published to Server)
{
    LOG2("a new message arrived (no: " + String(NetManTask.returnCurrentIterator()) + ")");
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
    temp.topic = topic_str;
    temp.request = my_JSON.get<String>("request");
    temp.urgent = my_JSON.get<bool>("urgent");
    // MORE TO ADD HERE

    String *mytop = NetManTask.returnMQTTtopics(temp);

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
    else
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
    return WiFi.localIP();
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