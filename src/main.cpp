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

// ===================================== my helper Functions =====================================

double calcOptimum(myJSONStr &obj) // returns Optimum for given values, higher is better
{
  double val = 100 / obj.vehicleParams[0]; // better for shorter way, 100 just for factoring, TODO
  return val;
};

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
  switch (stat)
  {
  case status_main::status_isEmpty:
  {
    loopEmpty();
    break;
  }
  case status_main::status_justFullPublish:
  {
    pubishLevel();
    break;
  }
  case status_main::status_getOptimalVehicle:
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    getOpcimalVehiclefromResponses();
    mcount = TaskMain->returnCurrentIterator();
    break;
  }
  case status_main::status_hasOptVehiclePublish:
  {
    hasOptVehiclePublish();
    break;
  }
  case status_main::status_checkIfAckReceived:
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    checkIfAckReceivedfromResponses();
    mcount = TaskMain->returnCurrentIterator();
    break;
  }
  case status_main::status_checkIfTranspored:
  {
    mcount2 = TaskMain->returnCurrentIterator(); // needed for number of messages received, upper num
    checkIfTransporedfromResponses();
    mcount = TaskMain->returnCurrentIterator();
    break;
  }
  default:
  {
    Serial.print("Wrong Status");
  }
  }
}