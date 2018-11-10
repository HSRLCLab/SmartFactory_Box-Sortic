#include "NetworkManager.h"

NetworkManager::NetworkManager() //Initialize DEFAULT serial & WiFi Module
{
    ssid = DEFAULT_WIFI_SSID;
    pass = DEFAULT_WIFI_PASSWORD;
    JSarrP = JSarray;
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

    myMQTTclient = new PubSubClient(brokerIP, DEFAULT_MQTT_PORT, callback2, *myClient);
    connectToMQTT(); // connecting to MQTT-Broker
}

NetworkManager::NetworkManager(IPAddress &broker, String *ssid2, String *pass2, byte *mmQTTport, byte pins[4]) //Initialize COSTOM serial & WiFi Module
{
    status = WL_IDLE_STATUS;
    WiFi.setPins(pins[0], pins[1], pins[2], pins[3]);
    ssid = *ssid2;
    pass = *pass2;
    JSarrP = JSarray;
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

/*void NetworkManager::receiveMessage(char *topic, byte *payload, unsigned int length) // listens to incoming messages  (which were published to the Server to the subsribed topic)
{
    if (myMQTTclient->connected())
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
*/

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

    while (WiFi.status() != WL_CONNECTED) // connect to Wifi network
    {
        log("Attempting WLAN connection (WEP)...", "SSID: " + ssid, "");
        status = WiFi.begin(ssid, pass); // takes ssid, pass from private attributes
        WiFi.hostname(hostname.c_str());
        if (status != WL_CONNECTED)
        {
            log("WLAN connection failed", "trying again in 3 seconds", "");
            delay(3000);
        }
    };
    log("", "", "Board is connected to the Network");
    status = WiFi.status(); // TODO status needed as class member?
}

void NetworkManager::connectToMQTT()
{
    while (!myMQTTclient->connected()) // TODO: willTopic, willQoS, willRetain, willMessage)
    {
        log("Attempting MQTT connection...", "MQTT Client ID: " + hostname, "");
        if (myMQTTclient->connect(hostname.c_str())) // Attempt to connect
        {
            log("MQTT connected", "Variable myMQTT has successfully connected with hostname: " + hostname, "");
        }
        else
        {
            log("MQTT connection failed, error code: " + String(myMQTTclient->state()), "trying again in 3 seconds", "");
            delay(3000);
        }
    }
}

bool NetworkManager::publishMessage(const String &topic, const String &msg) // publishes a message to the server
{
    log("", "", "Publish message:" + msg);

    if (WiFi.status() != WL_CONNECTED)
        connectToWiFi();
    if (myMQTTclient->connected())
    {
        myMQTTclient->publish(topic.c_str(), msg.c_str());
        log("", "", "Publish message:" + msg);
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        log("MQTT not connected", "You are not connected to the MQTT Broker, publish fails: " + msg, "Client ID: " + hostname);
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
        log("", "", "subscribing to:" + topic);
        myMQTTclient->loop(); // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        return true;
    }
    else
    {
        log("MQTT not connected", "You are not connected to the MQTT Broker, subscription fails: " + topic, "Client ID: " + hostname);
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
        log("MQTT not connected", "You are not connected to the MQTT Broker, unsubscribe will fail: " + topic, "Client ID: " + hostname);
        connectToMQTT();
        return false;
    }
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
    String topic_str;
    for (int i = 0; topic[i] != '\0'; i++) // iterate topic to topic_str
    {
        topic_str += topic[i];
    }
    if (log_level > 2)
    {
        String msg = "Message arrived [" + topic_str + msg += "]: \t";
        for (unsigned int i = 0; i < length; i++) // iterate message
        {
            msg += (char)payload[i];
        }
        Serial.println(msg);
    }

    char inData[length + 1]; // convert message to JSON object
    for (unsigned int i = 0; i < length; i++)
        inData[(i)] = (char)payload[i];
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &my_JSON = jsonBuffer.parseObject(inData); // saves everything to my_JSON JSON Object
    my_JSON["MQTT_topic"] = topic_str;
    if (my_json_counter < MAX_JSON_MESSAGES_SAVED) // saving json_messages to array
    {
        my_json_counter++;
        JSarray[my_json_counter] = &my_JSON;
    }
    else
    {
        my_json_counter = 0;
        JSarray[my_json_counter] = &my_JSON;
    }
    if (log_level > 2)
        my_JSON.prettyPrintTo(Serial); // prints JSON to Serial Monitor

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
    case 0:
        break;
    case 1:
        Serial.println(log1);
        break;
    case 2:
        Serial.println(log1);
        Serial.println(log2);
        break;
    case 3:
        Serial.println(log1);
        Serial.println(log2);
        Serial.println(log3);
        break;
        /*
    default:
        Serial.println("YOU HAVE ENTERED A WRONG log_level! -> PLEASE VERIFY");
        Serial.println("logging levels: 0-without, 1 error, 2 info, 3 verbose debugging");
        */
    }
    // TODO: gegebenenfalls anpassen auch auf anstatt Serial.println in File.write ???
    // TODO: files ändern, dass nur immer der String geprintet wird, von welchem log level (dass nicht zu viele Zeieln!)
    // TODO: evt. debug.h
}