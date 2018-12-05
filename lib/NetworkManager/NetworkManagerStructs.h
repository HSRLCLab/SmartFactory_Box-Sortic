#ifndef NETWORKMANAGERSTRUCTS_H
#define NETWORKMANAGERSTRUCTS_H

#include <Arduino.h>

/*
used in:
  FILE                
  -----------------------------------------------------
  NetworkManager.h  
  NetworkManager.cpp
  MQTTTasks.h
  MQTTTasks.cpp
  main.cpp    
used for:
  saving Messages to use them later on (asynchronous Communication)
*/

struct myJSONStr
{
  bool urgent = false;
  String topic = "default";
  String hostname = "default";
  String request = "default";
  int level = -5;          // describes Smart Box level states, -5 is default if not set!
  double vehicleParams[5]; // see documentation for parameters
};

enum SBLevel
{
  dead = -1,
  full = 0,
  empty = 1
};

#endif