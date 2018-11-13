// Smart Box main file

#include <Arduino.h>
#include <ArduinoJson.h>
#include <NetworkManager.h>
#include <SensorArray.h>
#include <MainConfiguration.h>
#include <NetworkManagerStructs.h>

// ===================================== Global Variables =====================================
myJSONStr *JSarra;                   // defined in NetworkManager.h, used for saving incoming Messages, FIFO order, see also MAX_JSON_MESSAGES_SAVED
int my_json_counter = 0;             // is last element in array, used for referencing to the last Element, attention: pay attention to out of bound see MAX_JSON_MESSAGES_SAVED
bool my_json_counter_isEmpty = true; // defined in NetworkManager.h
NetworkManager *mNetwP = 0;          // used for usign NetworkManager access outside setup()
SensorArray *mSarrP = 0;             // used for using SensorArray access outside setup()
const int log_level = 1;             // can have values from 0-3
enum SBLevel                         // -5 is default if not set!
{
  full = 0,
  empty = 1
}; // describes Smart Box level states
// ===================================== my helper Functions =====================================

double calcOptimum(myJSONStr &obj) // returns Optimum for given values, higher is better
{
  // mögliche Parameter: Geschwindigkeit, Abstand zu SmartBox, Anzahl noch auszuführende Tasks
  double val = 100 / obj.level; // better for shorter way, 100 just for factoring
  // TODO above
  return val;
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
};

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
  int mcount = my_json_counter;          // needed for number of messages received during run full
  mNetwP->subscribe("Vehicle/+/params"); // when full
  mNetwP->subscribe("Vehicle/+/ack");
  mNetwP->publishMessage("SmartBox/" + mNetwP->getHostName() + "/level", "{hostname:" + mNetwP->getHostName() + ",level:" + String(SBLevel::full) + "}"); // TODO: skalar als level variable?

  for (int i = 0; i < SMARTBOX_WAITFOR_VEHICLES_SECONDS; i++) // wait for vehicles to respond
  {
    mNetwP->loop();
    delay(1000);
  }
  mNetwP->loop();

  // JSarra[my_json_counter].hostname

  // i'm here
  // next: Netzwerk-Kommunikation definieren (JSON Objekte), allenfalls neue Structs (aber aufwendig!)

  int mcount2 = my_json_counter;
  if (mcount == mcount2)
    Serial.println("no answers or exact MAX_JSON_MESSAGES_SAVED answers received");
  double *best_values;
  if (mcount2 < mcount) // get optimal vehicle values in best_values array
  {
    best_values = new double[(MAX_JSON_MESSAGES_SAVED - mcount) + mcount2 + 1]; // TODO überall bei new auch delete!
    for (int i = mcount; i < MAX_JSON_MESSAGES_SAVED; i++)
    {
      // TODO if topic: Vehicle/VXXX/params
      //best_values[i] = JSarra[i].get(...); // calcs optimum and puts result in best_values[]
    }
    for (int i = 0; i < mcount2; i++)
    {
      // TODO if topic: Vehicle/VXXX/params
      //best_values[i] = JSarra[i];
    }
  }
  else
  {
    best_values = new double[mcount2 - mcount + 1]; // TODO überall bei new auch delete!
    for (int i = mcount; i < mcount2 - mcount; i++)
    {
      // TODO if topic: Vehicle/VXXX/params
    }
  }
  // get vehicle with best optimal value

  /*
  
  // number of vehicle necessary? byte num_of_vehi = returnNumOfVehicles(); // TODO what if reading nothing?
  double index_max = 0; // index from best vehicle
  // index max useless, da i oben anders! -> auslesen aus json!
  double value_max = 0; // best optimal value from vehicle
  for (int i = 0; i < sizeof(best_values) / sizeof(best_values[0]); i++)
  {
    if (best_values[i] > value_max)
    {
      value_max = best_values[i];
      index_max = i;
    }
    else if (best_values[i] == value_max)
    {
      log("has two same optimal values", "", "");
    }
    else
      ;
  }

  mNetwP->publishMessage("") // publish to SmartBox/SBX/decision
  ... waits for answer, if not within time, send to next (remember 2-3 best choices), jump back in function?
  ... waits until transported
  */

  mNetwP->unsubscribe("Vehicle/+/params"); // when transported and brought back to factory
  mNetwP->unsubscribe("Vehicle/+/ack");
}

// ===================================== Arduino Functions =====================================
void setup() // for initialisation
{
  if (log_level >= 1)
  {
    Serial.begin(12000); //Initialize serial
    while (!Serial)
      ; // wait for serial port to connect
  }
  mNetwP = new NetworkManager();
  mSarrP = new SensorArray();
  JSarra = mNetwP->JSarrP;

  if (true) // degug cycle -- DELETE ON FINAL
  {
    pinMode(13, OUTPUT); // debug LED
    mNetwP->subscribe("SmartBox/+/level");
  }
}

void loop() // one loop per one cycle (SB full -> transported -> returned empty)
{
  if (true) // degug cycle -- DELETE ON FINAL
  {
    digitalWrite(13, LOW);
    //mNetwP->publishMessage("SmartBox/level", "{city1:Zurich,city2:Bern}");
    delay(1000);
    digitalWrite(13, HIGH);
    delay(1000);
  }

  loopEmpty();

  loopFull();
}
