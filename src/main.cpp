// Smart Box main file
/**
 * @file main.cpp
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief main file
 * @version 0.1
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include <MQTTTasks.h>
#include <MainConfiguration.h>
#include <NetworkManager.h>
#include <NetworkManagerStructs.h>
#include <SensorArray.h>

// ===================================== Global Variables =====================================
MQTTTasks *TaskMain;                                   ///< filled in NetworkManager.cpp, used for saving incoming Messages, FIFO order
NetworkManager *mNetwP;                                ///< used for usign NetworkManager access outside setup()
SensorArray *mSarrP;                                   ///< used for using SensorArray access outside setup()
double value_max[NUM_OF_MAXVALUES_VEHICLES_STORE];     ///< best optimal value from vehicle, Element 0 ist best, Element 1 is second best, etc. (decending order)
String hostname_max[NUM_OF_MAXVALUES_VEHICLES_STORE];  ///< name of Vehicle with best value, Element 0 ist best, Element 1 is second best, etc. (decending order)
bool hasAnswered = false;                              ///< variable used to see if Vehicle have answered
byte isLastRoundonError = 1;                           ///< currently two max values are included, if both are not responding, this Variable will be set to true, must be min 1
myJSONStr *tmp_mess;                                   ///< pointer to array of messages, used for iteration of messages

// -.-.-.-.-.-.-.- used for Statuses -.-.-.-.-.-.-.-
/**
 * \enum status_main
 * @brief stores main status for Program run (main.cpp)
 * 
 */
enum status_main {
    status_isEmpty = 0,               ///< 0
    status_justFullPublish = 1,       ///< 1
    status_getOptimalVehicle = 2,     ///< 2
    status_hasOptVehiclePublish = 3,  ///< 3
    status_checkIfAckReceived = 4,    ///< 4
    status_checkIfTranspored = 5      ///< 5
};

int mcount = 0;   ///< needed for number of messages received, lower num
int mcount2 = 0;  ///< needed for number of messages received, lower num
status_main stat = status_main::status_isEmpty;
bool toNextStatus = true;          ///< true if changing state, false if staying in state, it's enshuring that certain code will only run once
void (*myFuncToCall)() = nullptr;  ///< func to call in main-loop, for finite state machine
unsigned long currentMillis = 0;   ///< will store current time
unsigned long previousMillis = 0;  ///< will store last time

// -.-.-.-.-.-.-.- used for Show-Case -.-.-.-.-.-.-.-
bool showCase = true;
unsigned int waitSeconds1 = 0;   ///< wait between loops, without blinking LED
unsigned int waitSeconds2 = 10;  ///< wait between loops, blinking LED
int ledState = LOW;              ///< used for debugging LED

// ===================================== my Function-Prototyps =====================================
/**
 * @brief Returns Optimum for given values, higher is better, -1 if not valuable entries
 * 
 * @param obj 
 * @return double 
 */
double calcOptimum(myJSONStr &obj);

/**
 * @brief Get the Smart Box Info if \link LOG2 \endlink is defined
 * 
 */
void getSmartBoxInfo();

/**
 * @brief FSM loop until Box full 
 * 
 * if Box full switch state to \link publishLevel() \endlink
 */
void loopEmpty();

/**
 * @brief FSM publishes SmartBox Level
 * 
 * publish SB level and wait for \link SMARTBOX_WAITFOR_VEHICLES_SECONDS \endlink
 * befor switching state to \link getOptimalVehiclefromResponses() \endlink
 */
void publishLevel();

/**
 * @brief FSM Get the Optimal Vehicle from Responses object
 * 
 * checks all responses and gets Vehicle with best Params due to calcOptimum(), calc Optimum Value & set hostname_max, hostname_max2
 * afterwards switch state to \link hasOptVehiclePublish() \endlink
 */
void getOptimalVehiclefromResponses();

/**
 * @brief FSM publishes decision for which vehicle should get the Smart Box
 * 
 *  if no answers ar recived switch state back to \link publishLevel() \endlink
 * else switch state to \link checkIfAckReceivedfromResponses() \endlink
 */
void hasOptVehiclePublish();

/**
 * @brief runs until acknoledgement of desired Vehicle arrived, check if right Vehicle answered to get SmartBox transported
 * 
 *if the right vehicle answer
 * 
 */
void checkIfAckReceivedfromResponses();

/**
 * @brief runs until SmartBox is transpored, emtied and brought back to factory
 * 
 */
void checkIfTransporedfromResponses();

// ===================================== Arduino Functions =====================================
/**
 * @brief for initialisation of the Board
 * 
 * Use it to initialize variables, pin modes, start using libraries, etc.
 * The setup() function will only run once,
 * after each powerup or reset of the board
 */
void setup() {
    if (LOGLEVELCONFIGURATION > 0) {
        Serial.begin(12000);  //Initialize serial
        while (!Serial)
            ;  // wait for serial port to connect
    }
    LOG1("-.-.-.- SETUP -.-.-.-");
    LOG3("entering setup - initializing components");
#if !(SERVICE_MODE > 0)
    LOG3("new NetworkManager()");
    mNetwP = new NetworkManager();
    LOG3("new SensorArray()");
#endif
    mSarrP = new SensorArray();
#if !(SERVICE_MODE > 0)
    TaskMain = mNetwP->NetManTask_classPointer;
#endif
    pinMode(PIN_FOR_FULL, OUTPUT);
    myFuncToCall = loopEmpty;
#if SERVICE_MODE == 2
    pinMode(13, OUTPUT);
    LOG3("=== RUN SENSORTEST ===");
#endif
    if (showCase) {
        pinMode(13, OUTPUT);  // debug LED
    }
}

/**
 * @brief one loop per cycle (SB full -> transported -> returned empty)
 * 
 * After creating a setup() function, which initializes and sets the initial values,
 * the loop() function does precisely what its name suggests,
 * and loops consecutively, allowing your program to change and respond.
 * Use it to actively control the board.
 */
void loop() {
#if SERVICE_MODE == 2
    LOG3("=====");
    mSarrP->SensorArray::getSensorData();
    delay(1000);
    return;
#endif
    static int i;
    // TaskMain->printAllMessages(0); // to show all saved Tasks (hostnames)

    if (showCase) {
        unsigned int timeIt = 0;
        int previousMilliss = millis();
        while ((timeIt < waitSeconds2) && (showCase))  // wait before next loop
        {
            if (currentMillis - previousMilliss >= 500) {
                timeIt++;
                previousMilliss = currentMillis;
                if (ledState == LOW) {
                    ledState = HIGH;
                } else {
                    ledState = LOW;
                }
                // digitalWrite(13, ledState);
                mNetwP->loop();  // needed to be called regularly to keep connection alive
            }
            currentMillis = millis();
        }
        LOG1();
        LOG1();
        LOG1("-------------------------- now going to loop again: " + String(i) + "-------------------------- ");
        i++;
        LOG2("STATE: " + String(stat));
        LOG2("--------------------------");
        myFuncToCall();
    } else {
        LOG1("-------------------------- now going to loop again: " + String(i) + "-------------------------- ");
        i++;
        LOG2("STATE: " + String(stat));
        LOG2("--------------------------");
        myFuncToCall();  // calls function for FSM
        mNetwP->loop();  // needed to be called regularly to keep connection alive
    }
}

// ===================================== my Helper-Functions =====================================
double calcOptimum(myJSONStr &obj) {
    if (obj.vehicleParams[0] != 0 && obj.vehicleParams[1] != 0 && obj.vehicleParams[2] != 0 && obj.vehicleParams[3] != 0) {
        double val = 100 * (0.2 / obj.vehicleParams[0] + 0.2 / obj.vehicleParams[1] + 0.4 / obj.vehicleParams[2] + 0.2 / obj.vehicleParams[3]);  // better for shorter way, 100 just for factoring
        // LOG3("Hostname: " + obj.hostname +"\tobj.vehicleParams[0]: " + String(obj.vehicleParams[0]) + ", obj.vehicleParams[1]: " + String(obj.vehicleParams[1]) + ", obj.vehicleParams[2]: " + String(obj.vehicleParams[2]) + ", obj.vehicleParams[3]: " + String(obj.vehicleParams[3]));
        return val;
    } else {
        LOG2("wrong parameters given to calcOptimum");
        LOG3("obj.vehicleParams[0]: " + String(obj.vehicleParams[0]) + ", obj.vehicleParams[1]: " + String(obj.vehicleParams[1]) + ", obj.vehicleParams[2]: " + String(obj.vehicleParams[2]) + ", obj.vehicleParams[3]: " + String(obj.vehicleParams[3]));
        return -1;
    }
};

void getSmartBoxInfo() {
    LOG2("Smart Box state: " + String(stat));
    LOG2("Hostname with best value (" + String(value_max[0]) + "): " + hostname_max[0]);
    LOG2("max Num of Vehicles in Field: " + String(NUM_OF_VEHICLES_IN_FIELD));
    LOG2("num of maxvalues to store: " + String(NUM_OF_MAXVALUES_VEHICLES_STORE));
    LOG2("Variable has answered: " + String(hasAnswered));
    LOG2("Variable isLastRoundonError: " + String(isLastRoundonError));
    LOG2("Variable mcount: " + String(mcount));
    LOG2("Variable mcount2: " + String(mcount2));
    LOG2("Variable toNextStatus: " + String(toNextStatus));
    LOG2("Time passed (currentMillis - previousMillis): " + String(currentMillis - previousMillis) + "\t\t[values: previousMillis: " + String(previousMillis) + "\tcurrentMillis: " + String(currentMillis) + "]");
};

// ===================================== my Functions =====================================
void loopEmpty() {
    digitalWrite(PIN_FOR_FULL, LOW);  ///< if empty leave LED off or turn it off
    if (toNextStatus)                 ///< only subscribe once but publish repeatedly
    {
        LOG1("-.-.-.- reading Sensor Values -.-.-.-");
        LOG3("entering new state: loopEmpty");
        toNextStatus = false;
    }
    if (mSarrP->getSensorData()) {
        LOG3("is full, go next to status_justFullPublish");
        if (showCase)
            LOG2(">>>>>>>>>>>>>>>>>>>>> please now choose Option 1 in the Script (after level publish)");
        stat = status_main::status_justFullPublish;
        myFuncToCall = publishLevel;
        toNextStatus = true;
        digitalWrite(PIN_FOR_FULL, HIGH);  ///< if the Box is full turn the LED on
    }
    previousMillis = millis();
    while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
        currentMillis = millis();
    }
}

void publishLevel() {
    if (toNextStatus)  // only subscribe once but publish repeatedly
    {
        LOG1("-.-.-.- pubilsh SmartBox level -.-.-.-");
        LOG3("entering new state: pubishLevel");
        mNetwP->subscribe("Vehicle/+/params");
        mNetwP->subscribe("Vehicle/+/ack");
        hasAnswered = false;
        mcount = TaskMain->returnCurrentIterator();
        toNextStatus = false;
        previousMillis = millis();
    }
    mNetwP->publishMessage("SmartBox/" + String(mNetwP->getHostName()) + "/level", "{\"hostname\":\"" + String(mNetwP->getHostName()) + "\",\"level\":" + String(SBLevel::full) + "}");
    mNetwP->loop();
    currentMillis = millis();
    if ((currentMillis - previousMillis) / 1000 < SMARTBOX_WAITFOR_VEHICLES_SECONDS)  // wait for vehicles to send their params
    {
        currentMillis = millis();
        LOG3("now on time: " + String((currentMillis - previousMillis) / 1000) + "\t\t currentMillis: " + String(currentMillis) + "\t previousMillis: " + String(previousMillis));
        mNetwP->loop();
    } else if ((currentMillis - previousMillis) / 1000 >= SMARTBOX_WAITFOR_VEHICLES_SECONDS) {
        mNetwP->loop();
        mcount2 = TaskMain->returnCurrentIterator();  // needed for number of messages received, upper num
        tmp_mess = TaskMain->getBetween(mcount, mcount2);
        if (tmp_mess == nullptr) {
            LOG2("no messages");
        } else {
            LOG3("go to status_getOptimalVehicle");
            toNextStatus = true;
            stat = status_main::status_getOptimalVehicle;
            myFuncToCall = getOptimalVehiclefromResponses;
            previousMillis = millis();
            while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
                currentMillis = millis();
            }
        }
    } else
        LOG1("Error, time has wrong value;\tpreviousMillis: " + String(previousMillis) + "\tcurrentMillis: " + String(currentMillis));
}

void getOptimalVehiclefromResponses() {
    if (toNextStatus)  // do once
    {
        LOG1("-.-.-.- calculation of Optimal Values through Iterations -.-.-.-");
        LOG3("entering new state: getOpcimalVehiclefromResponses");
        toNextStatus = false;
        hasAnswered = false;
        mNetwP->loop();
        mcount2 = TaskMain->returnCurrentIterator();  // needed for number of messages received, upper num
        tmp_mess = TaskMain->getBetween(mcount, mcount2);
        for (int i = 1; i < tmp_mess[0].level; i++) {
            String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
            if (ttop == nullptr)
                LOG1("no topics found, function returnMQTTtopics failed to return topics");
            // LOG3("received Topics: " + ttop[0] + ", " + ttop[1] + ", " + ttop[2]); // could be uncommented for debugging
            if ((ttop[0] == "Vehicle") && (ttop[2] == "params"))  // if in MQTT topic == Vehicle/+/params
            {
                hasAnswered = true;
                LOG3("has answered, calculating Optimum for:\t" + ttop[1]);
                double opt = calcOptimum(tmp_mess[i]);
                LOG3("\t\tOptimum is:\t" + String(opt));
                if (value_max[0] < opt) {
                    value_max[1] = value_max[0];
                    hostname_max[1] = hostname_max[0];
                    value_max[0] = opt;
                    hostname_max[0] = tmp_mess[i].hostname;
                }
                ///< else case not needed, two best hostnames are enough, if more are requested: code can be added here
            }
        }
    }
    toNextStatus = true;
    LOG2("has calculated max Optimal Values");
    LOG3("optimal is Vehicle: " + hostname_max[0]);
    LOG3("go next to status_hasOptVehiclePublish");
    if (showCase)
        LOG2(">>>>>>>>>>>>>>>>>>>>> please now choose Option 2 in the Script (after decision getOptimalVehiclefromResponses)");
    stat = status_main::status_hasOptVehiclePublish;
    myFuncToCall = hasOptVehiclePublish;
    previousMillis = millis();
    while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
        currentMillis = millis();
    }
}

void hasOptVehiclePublish() {
    if (toNextStatus)  // only subscribe once but publish repeatedly
    {
        LOG1("-.-.-.- publish decision for Vehicle -.-.-.-");
        LOG3("entering new state: hasOptVehiclePublish");
        toNextStatus = false;
        mcount = TaskMain->returnCurrentIterator();
        previousMillis = millis();
    }
    if (hasAnswered) {
        mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/decision", "{\"hostname\":\"" + hostname_max[0] + "\"}");  // publishes decision, clientID is in topic
        mNetwP->loop();
        currentMillis = millis();
        if ((currentMillis - previousMillis) < SMARTBOX_ITERATION_VACKS_SECONDS)  // wait for vehicles to send their acknoledgement to transport SB
        {
            LOG3("now on time: " + String(currentMillis - previousMillis));
            currentMillis = millis();
            mNetwP->loop();
        } else if ((currentMillis - previousMillis) >= SMARTBOX_ITERATION_VACKS_SECONDS) {
            mcount2 = TaskMain->returnCurrentIterator();  // needed for number of messages received, upper num
            tmp_mess = TaskMain->getBetween(mcount, mcount2);
            if (tmp_mess == nullptr) {
                LOG2("no messages");
                LOG3("going back to publish level!");
                stat = status_main::status_justFullPublish;
                myFuncToCall = publishLevel;
            } else {
                LOG3("go next to status_checkIfAckReceived");
                if (showCase)
                    LOG2(">>>>>>>>>>>>>>>>>>>>> please now choose Option 2 in the Script (after decision publish)");
                toNextStatus = true;
                stat = status_main::status_checkIfAckReceived;
                myFuncToCall = checkIfAckReceivedfromResponses;
                mcount = TaskMain->returnCurrentIterator();
                mNetwP->loop();
                previousMillis = millis();
                while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
                    currentMillis = millis();
                }
            }
        } else
            LOG1("Error, time has wrong value;\tpreviousMillis: " + String(previousMillis) + "\tcurrentMillis: " + String(currentMillis));
    } else {
        LOG2("unfortunately no answeres arrived");
        stat = status_main::status_justFullPublish;
        myFuncToCall = publishLevel;
    }
}

void checkIfAckReceivedfromResponses() {
    mNetwP->loop();
    mcount2 = TaskMain->returnCurrentIterator();
    if (toNextStatus)  // only subscribe once but publish repeatedly
    {
        LOG1("-.-.-.- check for transport acknoledgement -.-.-.-");
        LOG3("entering new state: checkIfAckReceivedfromResponses");
        hasAnswered = false;
        toNextStatus = false;
        isLastRoundonError = 0;
    }
    tmp_mess = TaskMain->getBetween(mcount, mcount2);
    if (tmp_mess == nullptr) {
        LOG2("no messages for transport acknoledgement received yet");
    } else {
        for (int i = 1; i < tmp_mess[0].level; i++) {
            String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
            if (ttop == nullptr)
                LOG1("no topics found, function returnMQTTtopics failed to return topics");
            // LOG3("ttop[0]: " + ttop[0] + "\t ttop[1]: " + ttop[1] + "\t ttop[2]: " + ttop[2] + "\t tmp_mess[i].hostname: " + tmp_mess[i].hostname);                        // could be uncommented for debugging
            if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false) && (tmp_mess[i].hostname == mNetwP->getHostName()))  // if desired Vehicle answered
            {
                hasAnswered = true;
                LOG3("right Vehicle has answered and send acknoledgement to transport it");
            }
        }
        if (hasAnswered)  // if right Vehicle answered, go next
        {
            LOG3("go forward to status_checkIfTranspored");
            if (showCase)
                LOG2(">>>>>>>>>>>>>>>>>>>>> please now choose Option 3 in the Script");
            toNextStatus = true;
            stat = status_main::status_checkIfTranspored;
            myFuncToCall = checkIfTransporedfromResponses;
        } else if (isLastRoundonError <= NUM_OF_VEHICLES_IN_FIELD) {
            LOG3("go next to status_hasOptVehiclePublish, reset hostname");
            toNextStatus = true;
            stat = status_main::status_hasOptVehiclePublish;
            myFuncToCall = hasOptVehiclePublish;
            hostname_max[0] = hostname_max[isLastRoundonError];
        } else {
            LOG1("none of the two desired vehicles ansered");
            toNextStatus = true;
            stat = status_main::status_justFullPublish;
            myFuncToCall = publishLevel;
        }
    }
    int timeIt = 0;
    previousMillis = millis();
    while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
        currentMillis = millis();
    }
}

void checkIfTransporedfromResponses() {
    if (toNextStatus)  // only subscribe once but publish repeatedly
    {
        LOG1("-.-.-.- check if SmartBox is transported -.-.-.-");
        LOG3("entering new state: checkIfTransporedfromResponses");
        toNextStatus = false;
        hasAnswered = false;
        mcount = TaskMain->returnCurrentIterator();
    }
    mNetwP->loop();
    mcount2 = TaskMain->returnCurrentIterator();
    tmp_mess = TaskMain->getBetween(mcount, mcount2);
    // LOG3("size of array: " + String(tmp_mess[0].level)); // could be used for debugging
    if (tmp_mess == nullptr) {
        LOG2("no messages");
    } else {
        LOG3("now im here");
        for (int i = 1; i < tmp_mess[0].level; i++) {
            String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
            if (ttop == nullptr)
                LOG1("no topics found, function returnMQTTtopics failed to return topics");
            // LOG3("ttop[0]: " + ttop[0] + "\t ttop[1]: " + ttop[1] + "\t ttop[2]: " + ttop[2] + "\t tmp_mess[i].request:" + tmp_mess[i].request);                          // could be uncommented for debugging
            if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false) && (tmp_mess[i].request == mNetwP->getHostName()))  // if desired Vehicle answered
            {
                hasAnswered = true;
                LOG2("right Vehicle has answered and sent transported acknoledgement");
            } else
                LOG3("not the right answer");
        }
        if (hasAnswered)  // if Vehicle is transported, since transported and brought back to factory unsubsribe (is empty again)
        {
            LOG3("go next to status_isEmpty");
            toNextStatus = true;
            mNetwP->unsubscribe("Vehicle/+/params");  // when transported and brought back to factory
            mNetwP->unsubscribe("Vehicle/+/ack");
            stat = status_main::status_isEmpty;
            myFuncToCall = loopEmpty;
        }
    }
    previousMillis = millis();
    while (((currentMillis - previousMillis) / 1000 < waitSeconds1) && (showCase)) {
        currentMillis = millis();
    }
}