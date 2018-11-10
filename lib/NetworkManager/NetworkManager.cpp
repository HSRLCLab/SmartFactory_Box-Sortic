#include "NetworkManager.h"

NetworkManager::NetworkManager() //Initialize DEFAULT serial & WiFi Module
{
    ssid = DEFAULT_WIFI_SSID;
    pass = DEFAULT_WIFI_PASSWORD;
    if (is_vehicle)
    {
        // String hostname_string = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
        // this->hostname = (char *)hostname_string.c_str();                                // set hostname to random ID
        hostname = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
        // TODO: Doku: mmögliche Verbesserung überall Pointers
    }
    else
    {
        hostname = DEFAULT_HOSTNAME_SARTBOX + String(random(0xffff), HEX); // Create a random client ID for vehicles                              // set hostname to random ID
    }

    status = WL_IDLE_STATUS;
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

    myMQTTclient = new PubSubClient(brokerIP, mQTT_port, callback2, *myClient);
    connectToMQTT(); // connecting to MQTT-Broker
}

NetworkManager::NetworkManager(IPAddress &broker, String *ssid2, String *pass2, byte *mmQTTport, byte pins[4]) //Initialize COSTOM serial & WiFi Module
{
    status = WL_IDLE_STATUS;
    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    ssid = *ssid2;
    pass = *pass2;
    if (is_vehicle)
    {
        // String hostname_string = DEFAULT_HOSTNAME_VEHICLE + String(random(0xffff), HEX); // Create a random client ID for vehicles
        // this->hostname = (char *)hostname_string.c_str();                                // set hostname to random ID
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

void NetworkManager::receiveMessage(char *topic, byte *payload, unsigned int length) // listens to incoming messages  (which were published to the Server to the subsribed topic)
{
    if (!myMQTTclient->connected())
        connectToMQTT();
    myMQTTclient->loop();                        // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
    {
        log("Message arrived [" + String(*topic) + "]", String(*payload), "MQTT recorded a message");
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    }
    else
    {
        log("Please connect to the Internet", "You are not connected to the MQTT Broker", "Client ID: " + hostname);
    };
}

void NetworkManager::connectToWiFi()
{
    if (WiFi.status() == WL_NO_SHIELD) // check for the presence of the shield:
    {
        log("NO WiFi shield present", "WiFi Library could not find WiFi shield. WiFi.status returned " + WiFi.status(), "programm is not continuing");
        while (true)
            ; // don't continue
    }
    status = WiFi.status();
    String wifi_firmware = WiFi.firmwareVersion();
    log("", "", "WiFi Firmware Version = " + wifi_firmware);

    while (status != WL_CONNECTED) // connect to Wifi network
    {
        log("Attempting WLAN connection (WEP)...", "SSID: " + ssid, "");
        status = WiFi.begin(this->ssid, this->pass); // takes ssid, pass from private attributes
        WiFi.hostname(hostname.c_str());
        if (status != WL_CONNECTED)
        {
            log("WLAN connection failed", "trying again in 2 seconds", "");
            delay(2000);
        }
    }
    log("", "", "Board is connected to the Network");
}

void NetworkManager::connectToMQTT()
{
    //myMQTTclient->setServer(broker, mQTTport);
    //  myMQTTclient->setCallback(callback);
    //myMQTTclient->setCallback([this](char *topic, byte *payload, unsigned int length) { this->receiveMessage(topic, payload, length); });

    //myMQTTclient->setCallback([this](char *topic, byte *payload, unsigned int length) { this->receiveMessage(topic, payload, length); });
    // see: https://github.com/knolleary/pubsubclient/issues/21

    while (!myMQTTclient->connected())
    {
        log("Attempting MQTT connection...", "MQTT Client ID: " + hostname, "");
        if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
        {
            log("MQTT connected", "Variable myMQTT has successfully connected to: " + hostname, "");
        }
        else
        {
            log("MQTT connection failed" + myMQTTclient->state(), "trying again in 2 seconds", "");
            delay(2000);
        };
    };
}

bool NetworkManager::publishMessage(const String &topic, const String msg) // publishes a message to the server
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    myMQTTclient->loop();                        // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
    {
        log("", "Publish message:" + msg, "");
        myMQTTclient->publish(topic.c_str(), msg.c_str());
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        log("Please connect to the Internet", "You are not connected to the MQTT Broker", "Client ID: " + hostname);
        return false;
    };
}

bool NetworkManager::subscribe(const String &topic) // subscribes to a new MQTT topic
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
    {
        myMQTTclient->subscribe(topic.c_str());
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        log("Please connect to the Internet", "You are not connected to the MQTT Broker", "Client ID: " + hostname);
        return false;
    };
    myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
}

bool NetworkManager::unsubscribe(const String &topic)
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
    {
        myMQTTclient->unsubscribe(topic.c_str());
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        log("Please connect to the Internet", "You are not connected to the MQTT Broker", "Client ID: " + hostname);
        return false;
    };
    myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
}

/*void NetworkManager::callback(char *topic, byte *payload, unsigned int length) // NOT WORKING! PUBSUBCLIENT CANT TAKE CLASS FUNCTIONS
{
    if (myMQTTclient->connect(String(*hostname).c_str())) // Attempt to connect
    {
        log("Message arrived [" + String(*topic) + "]", String(*payload), "MQTT recorded a message");
        // TODO what to do???? -> see function callback for help
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        //return true;
    }
    else
    {
        log("Please connect to the Internet", "You are not connected to the MQTT Broker", "Client ID: " + *hostname);
        //return false;
    };
}*/

void callback2(char *topic, byte *payload, unsigned int length) // listens to incoming messages (published to Server)
{
    if (log_level > 2)
    {
        String msg = "Message arrived [" + *topic;
        msg += "]: \t";
        for (unsigned int i = 0; i < length; i++)
        {
            msg += Serial.print((char)payload[i]);
        }
        Serial.println(msg);
    };

    // to JSON
    char inData[length];
    for (unsigned int i = 0; i < length; i++)
        inData[(i)] = (char)payload[i];
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &my_JSON = jsonBuffer.parseObject(inData); // schreibt alles in das root JSON Object
    if (array.size() < MAX_JSON_MESSAGES_SAVED)           // saving json_messages to array
    {
        array.add(my_JSON);
    }
    else if (my_json_counter < MAX_JSON_MESSAGES_SAVED)
    {
        array.set(my_json_counter, my_JSON);
        my_json_counter++;
    }
    else
    {
        my_json_counter = 0;
        array.set(my_json_counter, my_JSON);
        my_json_counter++;
    };

    // my_JSON.prettyPrintTo(Serial); // prints JSON to Serial Monitor
    // TODO if requestVehicles in Vehicle/presence answer (if vehicle) "Vehicle MQTT-ID"
    // TODO if requestVehicles in Vehicle/presence answer (if smart box) = Vehicle *, then count & save ids
}

void NetworkManager::loop()
{
    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (!myMQTTclient->connected())
        connectToMQTT();
    myMQTTclient->loop();
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
    Serial.println(status);
    Serial.println("-------------------- Information END --------------------");
    Serial.println();
    Serial.println();
    // Note: MQTT subscriptions and messages not displayed here!
    myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
}

void NetworkManager::log(const String &log1, const String &log2, const String &log3) // logging levels: 0-without, 1 error, 2 info, 3 verbose debugging
{
    switch (log_level)
    {
    case 1:
        Serial.println(log1);
    case 2:
        Serial.println(log1);
        Serial.println(log2);
    case 3:
        Serial.println(log1);
        Serial.println(log2);
        Serial.println(log3);
        /*
    default:
        Serial.println("YOU HAVE ENTERED A WRONG log_level! -> PLEASE VERIFY");
        Serial.println("logging levels: 0-without, 1 error, 2 info, 3 verbose debugging");
        */
    }
    // TODO: gegebenenfalls anpassen auch auf anstatt Serial.println in File.write ???
    // TODO: evt. debug.h
}