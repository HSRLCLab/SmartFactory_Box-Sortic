/**
 * @file SensorConfiguration.h
 * @brief file that contains the default sensor configuration
 * 
 * A6 and greater not existing on board!
 * Sensortyp: SHARP GP2Y0D805Z0F 
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H

// #define MAX_SENSOR_VALUES 5  ///< how many sensor values this object can store
#define LB1 A5  ///< IR sensor
#define LB2 A4  ///< IR sensor
#define LB3 A3  ///< IR sensor
#define LOADINDICATOR_LED 12  ///< LED lamp, must be digital

#endif