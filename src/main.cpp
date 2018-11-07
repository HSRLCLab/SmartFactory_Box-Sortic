// Smart Box main file

#include <Arduino.h>
#include <ArduinoJson.h>
#include "NetworkManager.h"
#include "SensorArray.h"
#include "MainConfiguration.h"

// ===================================== Global Variables =====================================
//NetworkManager netw;         // init  Network Connection
SensorArray sarr;            // init Sensor
const int log_level = 3;     // 0-3 erlaubt
extern JsonArray &array;     // defined in NetworkManager.h
extern byte my_json_counter; // defined in NetworkManager.h

// ===================================== my helper Functions =====================================

// TODO: NetworkManager Initialisierung hängt Serial Monitor ab??? -> siehe Konstruktor

double calcOptimum(JsonObject &obj) // returns Optimum for given values, higher is better
{
  // mögliche Parameter: Geschwindigkeit, Abstand zu SmartBox, Anzahl noch auszuführende Tasks
  double val = 100/obj["distance"]; // better for shorter way, 100 just for factoring
  return val;
};

double returnNumOfVehicles()
{
  /*
  netw.subscribe("Vehicle/presence");
  netw.publishMessage("Vehicle/presence", "requestVehicles");
  for (int i = 0; i <= SMARTBOX_WAITFOR_ANSWERS_SECONDS; i++)
  {
    netw.loop();
    delay(1000);
    // TODO see TODO in Networkmanager.cpp/callback2
  };
  netw.unsubscribe("Vehicle/presence");
  */
};


// void getSmartBoxInfo(){}; // read Smart Box Information

// ===================================== my Functions =====================================
/*
void loopEmpty() // loop until Box full
{
  bool isEmpty = true;
  while (isEmpty) // while empty
  {
    if (sarr.getSensorData())
      isEmpty = false;
    delay(SENSOR_ITERATION_SECONDS * 1000);
  }
  netw.publishMessage("SmartBox/level", "SB 1 full");
  //netw.publishMessage("SmartBox/IP", netw.getIP);
}

void loopFull() // loop until Box transported, then exit program
{
  bool go_next = false;
  delay(SMARTBOX_WAITFOR_VEHICLES_SECONDS * 1000); // wait for vehicles to respond
  // TODO: read all vehicle params, get number of responses -> NetworkManager???
  byte num_of_vehi = returnNumOfVehicles();
  String dist = array[my_json_counter]["distance"];
  String vel = array[my_json_counter]["velocity"];
  String tasks = array[my_json_counter]["tasks"];
  // TODO what if reading nothing?

  byte number;           // number of vehicle responses
  double best_value = 0; // best optimum value
  // TODO: how to get number, how to save callback values?
  IPAddress best_ip;                // IP from the vehicle with optimal value
  for (byte i = 0; i < number; i++) // call calcOptimal for all active vehicles
  {
      double a = calcOptimum(); // TODO: json objekt übergeben
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
*/


// ===================================== Arduino Functions =====================================
void setup() // for initialisation
{
  if (log_level >= 1)
  {
    Serial.begin(12000); //Initialize serial
    while (!Serial)
      ; // wait for serial port to connect
  }
  // netw.subscribe("Vehicle/#"); // TODO: mehr ins Detail? Was brauche ich wirklich?

  // pinMode(13, OUTPUT); // debug LED
}

void loop() // one Arduino-loop per one cycle (SB full -> transported -> returned empty)
{

  
  delay(200);

  /*
  loopEmpty();

  loopFull();
  */
}
