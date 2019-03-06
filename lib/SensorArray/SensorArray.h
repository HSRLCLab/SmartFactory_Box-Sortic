/**
 * @file SensorArray.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief 
 * @version 0.1
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SENSORARRAY_H
#define SENSORARRAY_H

#include <Arduino.h>
#include <SensorConfiguration.h>
#include <LogConfiguration.h>

/**
 * @brief class for Sharp Sensor
 * 
 */
class SensorArray
{
public:
  /**
 * @brief Construct a new Sensor Array object
 * 
 */
  SensorArray();

  /**
   * @brief Read the Sensor Values
   * 
   * Checks for an Element within 0.5-5cm (\link SENSOR_TOLLERANCE_MIN \endlink - \link SENSOR_TOLLERANCE \endlink) from Sensor
   * 
   * @return true Sensor found an Element (full)
   * @return false Sensor found no Element (empty)
   */
  bool getSensorData();

  /**
   * @brief Get the nth-previous Sensor Values
   * 
   * if num==0 the current SensorValue will be returned.
   * num needs to be between 0 and \link MAX_SENSOR_VALUES \endlink
   * or the function will return false.
   * 
   * @param num 
   * @return true 
   * @return false 
   * @bug true return isn't a bool
   */
  bool getLastSensorData(int num);

private:
  /**
 * @brief saving the last Sensor Values, LIFO Queue
 * @param \link MAX_SENSOR_VALUES \endlink
 */
  bool SensValStore[MAX_SENSOR_VALUES];

  /**
   * @brief last Values Size
   * 
   */
  int lastValuesSize;

  // helper functions
  /**
   * @brief push Sensor values to lastValues[] in LIFO order.
   * 
   * This is a Helper-Function
   * @param val 
   */
  void pushToQueue(bool &val);
};

#endif