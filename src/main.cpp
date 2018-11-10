// Smart Box main file

#include <Arduino.h>
#include <ArduinoJson.h>
#include "NetworkManager.h"
#include "SensorArray.h"
#include "MainConfiguration.h"

// ===================================== Global Variables =====================================
JsonObject *JSarray[];  // defined in NetworkManager.h, used for saving incoming Messages, FIFO order, see also MAX_JSON_MESSAGES_SAVED
int my_json_counter = 0;       //is last element in array, used for referencing to the last Element, attention: pay attention to out of bound see MAX_JSON_MESSAGES_SAVED
NetworkManager *mNetwP = 0;    // used for usign NetworkManager access outside setup()
SensorArray *mSarrP = 0;       // used for using SensorArray access outside setup()
const int log_level = 2;       // can have values from 0-3
const bool is_vehicle = false; // true if is vehicle, used for MQTT
enum SBLevel
{
  full = 0,
  empty = 1
}; // describes Smart Box level states
// ===================================== my helper Functions =====================================

// TODO: NetworkManager Initialisierung hängt Serial Monitor ab??? -> siehe Konstruktor

double calcOptimum(JsonObject &obj) // returns Optimum for given values, higher is better
{
  // mögliche Parameter: Geschwindigkeit, Abstand zu SmartBox, Anzahl noch auszuführende Tasks
  double val = 100 / (double)obj["distance"]; // better for shorter way, 100 just for factoring
  return val;
};

double returnNumOfVehicles()
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

// void getSmartBoxInfo(){}; // read Smart Box Information TODO

// ===================================== my Functions =====================================
void loopEmpty() // loop until Box full
{
  bool isEmpty = false; // TODO set to true
  while (isEmpty)       // while empty
  {
    if (mSarrP->getSensorData()) //  TODO
      isEmpty = false;
    delay(SENSOR_ITERATION_SECONDS * 1000);
  }
  mNetwP->subscribe("Vehicle/+/params"); // when full
  mNetwP->subscribe("Vehicle/+/ack");
  mNetwP->publishMessage("SmartBox/level", "{hostname:" + mNetwP->getHostName() + ",level:" + String(SBLevel::full) + "}"); // TODO: skalar als level variable?
  //(mNetwP->JSarrP[my_json_counter])->prettyPrintTo(Serial);
}

void loopFull() // loop until Box transported, then exit program
{
  bool go_next = false;
  delay(SMARTBOX_WAITFOR_VEHICLES_SECONDS * 1000); // wait for vehicles to respond
  // response has form: {}
  // TODO: read all vehicle params, get number of responses -> NetworkManager???
  byte num_of_vehi = returnNumOfVehicles();
  double dist = JSarray[my_json_counter]->get<double>("distance");
  //String vel = JSarray[my_json_counter]["velocity"];
  //String tasks = JSarray[my_json_counter]["tasks"];
  // TODO what if reading nothing?

  byte number = 0;       // number of vehicle responses
  double best_value = 0; // best optimum value
  // TODO: how to get number, how to save callback values?
  IPAddress best_ip;                // IP from the vehicle with optimal value
  for (byte i = 0; i < number; i++) // call calcOptimal for all active vehicles
  {
    double a = 0;       //calcOptimum(); // TODO: json objekt übergeben
    if (a > best_value) // decide which vehicle should transport box
    {
      best_value = a;
      //best_ip=...   // TODO: how to read ip from best vehicle?
    }
  }
  // netw.publishMessage("SmartBox/decision_IP",best_ip); // publish decision ip (json?)

  // wait for acknoledgement for transport
  // wait for transported
  // TODO
  while (!go_next)
  {
    delay(SMARTBOX_ITERATION_VACKS_SECONDS * 1000);
  };

  while (!go_next)
  {
    delay(SMARTBOX_ITERATION_VTRANSPORTS_SECONDS * 1000);
  };
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
  JSarray=mNetwP->JSarrP; // todo NEXT

  if (true) // degug cycle -- DELETE ON FINAL
  {
    pinMode(13, OUTPUT); // debug LED
    mNetwP->subscribe("SmartBox/level");
  }
}

void loop() // one loop per one cycle (SB full -> transported -> returned empty)
{
  if (true) // degug cycle -- DELETE ON FINAL
  {
    digitalWrite(13, LOW);
    //mNetwP->publishMessage("Vehicle/V1/params", "{city1:Zurich,city2:Bern}");
    delay(1000);
    digitalWrite(13, HIGH);
    delay(1000);
  }

  loopEmpty();

  //loopFull();
}
