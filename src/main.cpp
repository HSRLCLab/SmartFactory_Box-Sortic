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
myJSONStr *tmp_mess; // pointer to array of messages, used for iteration of messages
// -.-.-.-.-.-.-.- used for Statuses -.-.-.-.-.-.-.-
enum status_main // stores main status for Program run (main.cpp)
{
  status_isEmpty = 0,
  status_justFullPublish = 1,
  status_getOptimalVehicle = 2,
  status_hasOptVehiclePublish = 3,
  status_checkIfAckReceived = 4,
  status_checkIfTranspored = 5
};
int mcount = 0; // needed for number of messages received, lower num
int mcount2 = 0;
status_main stat = status_main::status_isEmpty;
bool toNextStatus = true; // true if changing state, false if staying in state, it's enshuring that certain code will only run once
int loopTurns = 0;
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
  if (toNextStatus) // only subscribe once but publish repeatedly
  {
    Serial.println("loopEmpty");
    toNextStatus = false;
  }
  Serial.print(".");
  if (mSarrP->getSensorData())
    stat = status_main::status_justFullPublish;
}

void pubishLevel() // publishes SmartBox Level
{
  if (toNextStatus) // only subscribe once but publish repeatedly
  {
    Serial.println("\npubishLevel");
    mNetwP->subscribe("Vehicle/+/params");
    mNetwP->subscribe("Vehicle/+/ack");
    toNextStatus = false;
    hasAnswered = false;
    mcount = TaskMain->returnCurrentIterator();
  }
  mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/level", "{hostname:" + mNetwP->getHostName() + ",level:" + String(SBLevel::full) + "}"); // TODO: skalar als level variable?
  mNetwP->loop();
  if (loopTurns < SMARTBOX_WAITFOR_VEHICLES_TURNS) // wait for vehicles to send their params
  {
    loopTurns++;
    mNetwP->loop();
  }
  else if (loopTurns == SMARTBOX_WAITFOR_VEHICLES_TURNS)
  {
    toNextStatus = true;
    loopTurns = 0;
    stat = status_main::status_getOptimalVehicle;
  }
  else
    Serial.println("Error, loopTurns has wrong value: " + String(loopTurns));
}

void getOpcimalVehiclefromResponses() // gets Vehicle with best Params due to calcOptimum(), calc Optimum Value & set hostname_max, hostname_max2
{
  if (toNextStatus) // do once
  {
    Serial.println("getOpcimalVehiclefromResponses");
    toNextStatus = false;
    hasAnswered = false;
    //myFuncPtr = getMaximumFromOptimumValues; // get all Optimum Values for all vehicles & gets Optimal value
    tmp_mess = TaskMain->getBetween(mcount, mcount2);
    if (tmp_mess == nullptr)
    {
      Serial.println("no messages");
      stat = status_main::status_justFullPublish; // jump back to publish
    }
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
  }
  //iterateAnswers(mcount, mcount2);
  toNextStatus = true;
  stat = status_main::status_hasOptVehiclePublish;
}

void hasOptVehiclePublish() // publishes decision for vehicle to transport Smart Box
{
  if (toNextStatus) // only subscribe once but publish repeatedly
  {
    Serial.println("hasOptVehiclePublish");
    toNextStatus = false;
    mcount = TaskMain->returnCurrentIterator();
  }
  if (hasAnswered)
  {
    mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/decision", "{hostname:" + hostname_max[0] + "}"); // publishes decision, clientID is in topic
    mNetwP->loop();
    if (loopTurns < SMARTBOX_ITERATION_VACKS_TURNS) // wait for vehicles to send their acknoledgement to transport SB
    {
      loopTurns++;
      mNetwP->loop();
    }
    else if (loopTurns == SMARTBOX_ITERATION_VACKS_TURNS)
    {
      toNextStatus = true;
      loopTurns = 0;
      stat = status_main::status_checkIfAckReceived;
    }
    else
      Serial.println("Error, loopTurns has wrong value: " + String(loopTurns));
  }
  else
  {
    Serial.println("no answeres arrived");
    stat = status_main::status_justFullPublish;
  }
}

void checkIfAckReceivedfromResponses() // runs until acknoledgement of desired Vehicle arrived, check if right Vehicle answered to get SmartBox transported
{
  if (toNextStatus) // only subscribe once but publish repeatedly
  {
    Serial.println("checkIfAckReceivedfromResponses");
    hasAnswered = false;
    toNextStatus = false;
    isLastRoundonError = 0;
    mcount = TaskMain->returnCurrentIterator();
  }
  tmp_mess = TaskMain->getBetween(mcount, mcount2);
  mNetwP->loop();
  if (tmp_mess == nullptr)
  {
    Serial.println("no messages");
    stat = status_main::status_getOptimalVehicle; // jump back to publish
  }
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
  if (hasAnswered) // if right Vehicle answered, go next
  {
    toNextStatus = true;
    stat = status_main::status_justFullPublish;
  }
  else if (isLastRoundonError <= NUM_OF_VEHICLES_IN_FIELD)
  {
    toNextStatus = true;
    stat = status_main::status_hasOptVehiclePublish;
    hostname_max[0] = hostname_max[isLastRoundonError];
  }
  else
  {
    Serial.println("none of the two desired vehicles ansered");
    toNextStatus = true;
    stat = status_main::status_justFullPublish;
  }
}

void checkIfTransporedfromResponses() // runs until SmartBox is transpored, emtied and brought back to factory
{
  if (toNextStatus) // only subscribe once but publish repeatedly
  {
    Serial.println("checkIfTransporedfromResponses");
    toNextStatus = false;
    hasAnswered = false;
    mcount = TaskMain->returnCurrentIterator();
  }
  tmp_mess = TaskMain->getBetween(mcount, mcount2);
  mNetwP->loop();
  if (tmp_mess == nullptr)
  {
    Serial.println("no messages");
  }
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
    //myFuncPtr = checkVehicleAck;
    //iterateAnswers(mcount, mcount2);

    if (hasAnswered) // if Vehicle is transported, since transported and brought back to factory unsubsribe (is empty again)
    {
      toNextStatus = true;
      mNetwP->unsubscribe("Vehicle/+/params"); // when transported and brought back to factory
      mNetwP->unsubscribe("Vehicle/+/ack");
      stat = status_main::status_isEmpty;
    }
  }
}

// TODO überall fail save einbauen (was wenn nichts einliest?) -> keine assertions und auch keine Expeptions

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

  // TODO switch statement
  // TODO: Abfolge Logik überprüfen!

  if (stat == status_main::status_isEmpty)
    loopEmpty();
  else if (stat == status_main::status_justFullPublish)
    pubishLevel();
  else if (stat == status_main::status_getOptimalVehicle)
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    getOpcimalVehiclefromResponses();
    mcount = TaskMain->returnCurrentIterator();
  }
  else if (stat == status_main::status_hasOptVehiclePublish)
    hasOptVehiclePublish();
  else if (stat == status_main::status_checkIfAckReceived)
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    checkIfAckReceivedfromResponses();
    mcount = TaskMain->returnCurrentIterator();
  }
  else if (stat == status_main::status_checkIfTranspored)
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    checkIfTransporedfromResponses();
    mcount = TaskMain->returnCurrentIterator();
  }
  else
    Serial.print("Wrong Status");
}