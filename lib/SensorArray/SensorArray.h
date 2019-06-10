/**
 * @file SensorArray.h
 * @brief The Sensor Array detects the box fill level
 * 
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Refactoring Release BA  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SENSORARRAY_H
#define SENSORARRAY_H

#include <Arduino.h>
#include <LogConfiguration.h>

/**
 * @brief The Class SensorArray detects the box fill level
 * 
 */
class SensorArray {
   public:
    /**
    * @brief Construct a new Sensor Array object
    * 
    * @param Sensor1 - Pin for LevelDetector 1
    * @param Sensor2 - Pin for LevelDetector 2
    * @param Sensor3 - Pin for LevelDetector 3
    * @param LoadIndicatorLED - Pin for loadindicator LED
    */
    SensorArray(const int Sensor1, const int Sensor2, const int Sensor3, const int LoadIndicatorLED);

    /**
    * @brief Read the Sensor Values
    *
    * Checks for an Element within 0.5-5cm distance from Sensor.
    * If an Elemenet is detectet the sensor will be LOW.
    *
    * @return true Sensor found an Element (full)
    * @return false Sensor found no Element (empty)
    */
    bool getSensorData();

   private:
    const int pSensor1;           ///< Pin LevelDetector 1
    const int pSensor2;           ///< Pin LevelDetector 2
    const int pSensor3;           ///< Pin LevelDetector 3
    const int pLoadIndicatorLED;  ///< Pin for loadindicator LED
};

#endif