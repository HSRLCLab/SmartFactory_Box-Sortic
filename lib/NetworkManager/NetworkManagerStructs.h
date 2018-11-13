#ifndef NETWORKMANAGERSTRUCTS_H
#define NETWORKMANAGERSTRUCTS_H

#include <Arduino.h>

/*
used in:
  FILE                PARAMS
  -----------------------------------------------------
  NetworkManager.h    (*JSarray, *JSarrP)
  NetworkManager.cpp  (callback2)
  main.cpp            (*JSarra, calcOptimum())
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