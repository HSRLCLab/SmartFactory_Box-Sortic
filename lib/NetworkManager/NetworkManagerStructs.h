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

struct myJSONStr {
  String topic = "";
  String hostname = "";
  String request = "";
  int level = -5;
  double vehicleParams[5]; // TODO 5?
} ;



#endif