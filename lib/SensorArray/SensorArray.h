#ifndef SENSORARRAY_H
#define SENSORARRAY_H

#include <Arduino.h>
#include <SensorConfiguration.h>
#include <LogConfiguration.h>

class SensorArray
{
public:
  SensorArray();                                                        // initialize everything, Übergabeparameter?
  bool getSensorData();                                                 // getting current Sensor Value, true if full
  bool getLastSensorData(int num);                                      // getting previous Sensor Values, 0 if error

private:
  bool lastValues[MAX_SENSOR_VALUES]; // saving the last Sensor Values, LIFO Queue
  int lastValuesSize;

  // helper functions
  void pushToQueue(bool val); // push Sensor values to lastValues[] in LIFO order
};

#endif