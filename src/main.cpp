// Smart Box main file

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include <NetworkManager.h>
#include <SensorArray.h>
#include <MainConfiguration.h>
#include <NetworkManagerStructs.h>
#include <MQTTTasks.h>

// ===================================== Global Variables =====================================

/*
myJSONStr *JSarra;    // used in NetworkManager.h, used for saving incoming Messages, FIFO order, see also MAX_JSON_MESSAGES_SAVED
int my_json_counter = 0;                              // is last element in array, used for referencing to the last Element, attention: pay attention to out of bound see MAX_JSON_MESSAGES_SAVED, DON'T TOUCH THIS: https://www.youtube.com/watch?v=otCpCn0l4Wo
bool my_json_counter_isEmpty = true;                  // used in NetworkManager.h
*/

MQTTTasks *TaskMain;                                  // filled in NetworkManager.cpp, used for saving incoming Messages, FIFO order
NetworkManager *mNetwP = 0;                           // used for usign NetworkManager access outside setup()
SensorArray *mSarrP = 0;                              // used for using SensorArray access outside setup()
const int log_level = 1;                              // can have values from 0-3
double value_max[NUM_OF_MAXVALUES_VEHICLES_STORE];    // best optimal value from vehicle, Element 0 ist best, Element 1 is second best, etc. (decending order)
String hostname_max[NUM_OF_MAXVALUES_VEHICLES_STORE]; // name of Vehicle with best value, Element 0 ist best, Element 1 is second best, etc. (decending order)
bool hasAnswered = false;                             // variable used to see if Vehicle have answered
byte isLastRoundonError = 1;                          // currently two max values are included, if both are not responding, this Variable will be set to true, must be min 1
enum SBLevel                                          // describes Smart Box level states, -5 is default if not set!
{
  full = 0,
  empty = 1
};
//void (*myFuncPtr)(int) = NULL; // Pointer for the following Functions: getMaximumFromOptimumValues, checkVehicleAnswers, checkVehicleAck

// ===================================== Function Headers of my helper Functions =====================================
/*
void getMaximumFromOptimumValues(int ii);
void checkVehicleAnswers(int ii);
void checkVehicleAck(int ii);
*/
// ===================================== my helper Functions =====================================

double calcOptimum(myJSONStr &obj) // returns Optimum for given values, higher is better
{
  double val = 100 / obj.vehicleParams[0]; // better for shorter way, 100 just for factoring, TODO
  return val;
};

/*
String *returnMQTTtopics(String top) // returns String-Array of topics from MQTT topic structure, strings divided by /
{
  String tmp[MAX_MQTT_TOPIC_DEPTH];
  int k1 = 0; // lower cut-bound
  int k2 = 0; // upper cut-bound
  int k3 = 0; // num of strings (must be below above!)
  for (int i = 0; i < top.length(); i++)
  {
    if (top.charAt(i) == '/')
    {
      k1 = i + 1;
      if (k3 == MAX_MQTT_TOPIC_DEPTH)
        break;
      else
      {
        tmp[k3] = top.substring(k1, k2);
        k3++;
      }
    }
    else
    {
      k2++;
    }
  }
  String tmp2[k3];
  for (int i = 0; i < k3; i++)
  {
    tmp2[i] = tmp[i];
  }
  return tmp2;
};


double returnNumOfVehicles() // TODO not needed?
{
  mNetwP->subscribe("Vehicle/presence");
  mNetwP->publishMessage("Vehicle/presence", "requestVehicles");
  for (int i = 0; i <= SMARTBOX_WAITFOR_ANSWERS_SECONDS; i++)
  {
    mNetwP->loop();
    delay(1000);
    // TODO see TODO in Networkmanager.cpp/callback2
  };
  mNetwP->unsubscribe("Vehicle/presence");
  return 0;
};

void getMaximumFromOptimumValues(int ii)
{
  String *ttop = returnMQTTtopics(TaskMain->getDesiredLastMessage(ii).topic);
  if ((ttop[0] == "Vehicle") && (ttop[2] == "params")) // if in MQTT topic == Vehicle/+/params
  {
    myJSONStr temp = TaskMain->getDesiredLastMessage(ii);
    double opt = calcOptimum(temp);
    if (value_max[0] < opt)
    {
      value_max[1] = value_max[0];
      hostname_max[1] = hostname_max[0];
      value_max[0] = opt;
      hostname_max[0] = TaskMain->getDesiredLastMessage(ii).hostname;
    }
  }
}

void checkVehicleAnswers(int ii)
{
  String *ttop = returnMQTTtopics(TaskMain->getDesiredLastMessage(ii).topic);
  if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false)) // if desired Vehicle answered
  {
    if (TaskMain->getDesiredLastMessage(ii).hostname == mNetwP->getHostName()) // if answer is to this request
    {
      hasAnswered = true;
    }
  }
}

void checkVehicleAck(int ii)
{
  String *ttop = returnMQTTtopics(TaskMain->getDesiredLastMessage(ii).topic);
  if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false)) // if desired Vehicle answered
  {
    if (TaskMain->getDesiredLastMessage(ii).request == mNetwP->getHostName()) // if answer is to this request
    {
      hasAnswered = true;
    }
  }
}


void iterateAnswers(int mmcount, int mmcount2) // helper function because of FIFO order in TaskMain, nicer way, because needed three times below in loopFull
{
  if (mmcount == mmcount2)
  {
    Serial.println("no answers or exact MAX_JSON_MESSAGES_SAVED answers received");
    return;
  }
  else if (mmcount2 < mmcount)
  {
    for (int i = mmcount; i < MAX_JSON_MESSAGES_SAVED; i++)
    {
      myFuncPtr(i);
    }
    for (int i = 0; i < mmcount2; i++)
    {
      myFuncPtr(i);
    }
  }
  else
  {
    for (int i = mmcount; i < mmcount2 - mmcount; i++)
    {
      myFuncPtr(i);
    }
  }
};
*/

// void getSmartBoxInfo(){}; // print Smart Box Information TODO

// ===================================== my Functions =====================================
void loopEmpty() // loop until Box full
{
  bool isEmpty = false; // TODO set to true
  while (isEmpty)       // while empty
  {
    if (mSarrP->getSensorData())
      isEmpty = false;
    delay(SENSOR_ITERATION_SECONDS * 1000);
  }
}

void loopFull() // loop until Box transported
{
sendRequest:
  int mcount = TaskMain->returnCurrentIterator();  // needed for number of messages received, lower num
  int mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Subscriptions & publish
  mNetwP->subscribe("Vehicle/+/params");
  mNetwP->subscribe("Vehicle/+/ack");
  mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/level", "{hostname:" + mNetwP->getHostName() + ",level:" + String(SBLevel::full) + "}"); // TODO: skalar als level variable?
  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- wait for Answer (which vehicles are there?)
  for (int i = 0; i < SMARTBOX_WAITFOR_VEHICLES_SECONDS; i++) // wait for vehicles to respond
  {
    mNetwP->loop();
    delay(1000);
  }
  mNetwP->loop();
  mcount2 = TaskMain->returnCurrentIterator();
  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- calc Optimum Value & set hostname_max, hostname_max2 & publish
  Serial.println("getMaximumFromOptimumValues");
  hasAnswered = false;
  //myFuncPtr = getMaximumFromOptimumValues; // get all Optimum Values for all vehicles & gets Optimal value
  myJSONStr *tmp_mess = TaskMain->getBetween(mcount, mcount2);
  if (tmp_mess == nullptr)
    Serial.println("no messages");
  else
  {
    for (int i = 0; i < sizeof(tmp_mess) / sizeof(tmp_mess[0]); i++)
    {
      String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
      if ((ttop[0] == "Vehicle") && (ttop[2] == "params")) // if in MQTT topic == Vehicle/+/params
      {
        hasAnswered = true;
        double opt = calcOptimum(tmp_mess[i]);
        if (value_max[0] < opt)
        {
          value_max[1] = value_max[0];
          hostname_max[1] = hostname_max[0];
          value_max[0] = opt;
          hostname_max[0] = tmp_mess[i].hostname;
        }
        // TODO else?
      }
    }
  }
  //iterateAnswers(mcount, mcount2);
  if ((hasAnswered == false) && (isLastRoundonError < NUM_OF_VEHICLES_IN_FIELD)) // if no vehicle responds in time, send error message
  {
    isLastRoundonError++;
    goto sendRequest;
  }
  else
  {
    Serial.print("none of the " + String(NUM_OF_VEHICLES_IN_FIELD) + " Vehicles is responding");
  }
  isLastRoundonError = 1;
  mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/decision", "{hostname:" + hostname_max[0] + "}"); // publishes decision, clientID is in topic

// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- wait for Answer (is Vehicle X responding?)
sendAck:
  mcount = TaskMain->returnCurrentIterator();                // needed for number of messages received during run full
  for (int i = 0; i < SMARTBOX_ITERATION_VACKS_SECONDS; i++) // wait for vehicles to respond
  {
    mNetwP->loop();
    delay(1000);
  }
  mNetwP->loop();
  mcount2 = TaskMain->returnCurrentIterator();

  // TODO überall fail save einbauen (was wenn nichts einliest?) -> keine assertions und auch keine Expeptions
  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- check if right Vehicle answered to get SmartBox transported
  Serial.println("checkVehicleAnswers");
  hasAnswered = false;
  tmp_mess = TaskMain->getBetween(mcount, mcount2);
  if (tmp_mess == nullptr)
    Serial.println("no messages");
  else
  {
    for (int i = 0; i < sizeof(tmp_mess) / sizeof(tmp_mess[0]); i++)
    {
      String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
      if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false)) // if desired Vehicle answered
      {
        if (tmp_mess[i].hostname == mNetwP->getHostName()) // if answer is to this request
        {
          hasAnswered = true;
        }
      }
    }
  }
  //myFuncPtr = checkVehicleAnswers;
  //iterateAnswers(mcount, mcount2);
  if ((hasAnswered == false) && (isLastRoundonError < NUM_OF_VEHICLES_IN_FIELD)) // if no ack in time, send request to next vehicle
  {
    hostname_max[0] = hostname_max[1];
    isLastRoundonError++;
    goto sendAck;
  }
  else
  {
    Serial.print("none of the " + String(NUM_OF_VEHICLES_IN_FIELD) + " Vehicles is responding");
  }
  isLastRoundonError = 1;
// -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- wait for Answers (is SmartBox transported?)
ackReceived:                                                       // when acknoledgement of desired is received, now waits for transport
  mcount = TaskMain->returnCurrentIterator();                      // needed for number of messages received during run full
  for (int i = 0; i < SMARTBOX_ITERATION_VTRANSPORTS_SECONDS; i++) // wait for vehicles to respond
  {
    mNetwP->loop();
    delay(1000);
  }
  mNetwP->loop();
  mcount2 = TaskMain->returnCurrentIterator();

  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- check if right Vehicle answered having SmartBox transported
  Serial.println("checkVehicleAck");
  hasAnswered = false;
  //myFuncPtr = checkVehicleAck;
  tmp_mess = TaskMain->getBetween(mcount, mcount2);
  if (tmp_mess == nullptr)
    Serial.println("no messages");
  else
  {
    for (int i = 0; i < (sizeof(tmp_mess) / sizeof(tmp_mess[0])); i++)
    {
      String *ttop = TaskMain->returnMQTTtopics(tmp_mess[i]);
      if ((ttop[0] == "Vehicle") && (ttop[1] == hostname_max[0]) && (ttop[2] == "ack") && (hasAnswered == false)) // if desired Vehicle answered
      {
        if (tmp_mess[i].request == mNetwP->getHostName()) // if answer is to this request
        {
          hasAnswered = true;
        }
      }
    }
  }
  //iterateAnswers(mcount, mcount2);
  if ((hasAnswered == false) && (isLastRoundonError < NUM_OF_VEHICLES_IN_FIELD)) // if no ack in time, send request to next vehicle
  {
    isLastRoundonError++;
    goto ackReceived;
  }
  else
  {
    Serial.print("none of the " + String(NUM_OF_VEHICLES_IN_FIELD) + " Vehicles is responding");
  }
  // -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- since transported and brought back to factory unsubsribe (is empty again)
  mNetwP->unsubscribe("Vehicle/+/params"); // when transported and brought back to factory
  mNetwP->unsubscribe("Vehicle/+/ack");
}

// ===================================== Arduino Functions =====================================
void setup() // for initialisation
{
  if (log_level > 0)
  {
    Serial.begin(12000); //Initialize serial
    while (!Serial)
      ; // wait for serial port to connect
  }
  mNetwP = new NetworkManager();
  mSarrP = new SensorArray();
  //JSarra = mNetwP->JSarrP;
  TaskMain = mNetwP->NetManTask_classPointer;

  if (true) // for debugging purpose, DELETE ON FINAL TODO
  {
    pinMode(13, OUTPUT); // debug LED
    mNetwP->subscribe("SmartBox/+/level");
  }
}

void loop() // one loop per one cycle (SB full -> transported -> returned empty)
{
  if (true) // degug cycle -- DELETE ON FINAL TODO
  {
    digitalWrite(13, LOW);
    delay(1000);
    digitalWrite(13, HIGH);
    delay(1000);
  }

  loopEmpty(); // loop until full

  loopFull(); // loop until empty
}


// TODO Modularität, siehe Notizen in MainVehicle
// TODO: beide Main Funktionen, so dass Main/koop möglichst schneell durchläuft, augfrund Stati entscheiden, welcher Weg gewählt wird -> Modularität wichtigste, nicht Performance