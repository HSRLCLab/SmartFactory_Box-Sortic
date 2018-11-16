#ifndef SENSORARRAY_H
#define SENSORARRAY_H

#include <Arduino.h>
#include <SensorConfiguration.h>
#include <LogConfiguration.h>

class SensorArray // class for Sharp Sensor
{
public:
  SensorArray();                   // DEFAULT C'tor
  bool getSensorData();            // getting current Sensor Values, true if full
  bool getLastSensorData(int num); // getting mth-previous Sensor Values, if 0 its the current

private:
  bool SensValStore[MAX_SENSOR_VALUES]; // saving the last Sensor Values, LIFO Queue
  int lastValuesSize;

  // helper functions
  void pushToQueue(bool &val); // push Sensor values to lastValues[] in LIFO order
};

#endif