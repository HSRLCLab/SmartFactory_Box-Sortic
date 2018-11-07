#ifndef SENSORARRAY_H
#define SENSORARRAY_H

#include <Arduino.h>
#include <SensorConfiguration.h>

extern const int log_level;     // 0-3 erlaubt

class SensorArray
{
public:
  SensorArray();                                                        // initialize everything, Übergabeparameter?
  bool getSensorData();                                                 // getting current Sensor Value, true if full
  bool getLastSensorData(int num);                                      // getting previous Sensor Values, 0 if error

private:
  void log(const String &log1, const String &log2, const String &log3); // logging levels: 0-without, 1 error, 2 info, 3 verbose debugging
  bool lastValues[MAX_SENSOR_VALUES]; // saving the last Sensor Values, LIFO Queue
  int lastValuesSize;

  // helper functions
  void pushToQueue(bool val); // push Sensor values to lastValues[] in LIFO order
};

#endif