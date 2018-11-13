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
*/

struct myJSONStr {
  String topic = "";
  String hostname;
  int level = -5;
  double vehicleParams[5]; // TODO
} ;



#endif