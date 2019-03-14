/**
 * @file SensorConfiguration.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief file that contains the default sensor configuration
 * 
 * A6 and greater not existing on board!
 * Sensortyp: SHARP GP2Y0D805Z0F 
 * 
 * @version 0.1
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H

#define MAX_SENSOR_VALUES 5  ///< how many sensor values this object can store
// #define MAX_SENSOR_ITERATIONS 1  ///< How many values are neede to build the mean-val. Needs to be >0
#define LB1 A5  ///< IR sensor
#define LB2 A4  ///< IR sensor
#define LB3 A3  ///< IR sensor
// #define SENSOR_RANGE_MAX 0.20    ///< when object is detected, 0.18 volts measured if object in box
// #define SENSOR_RANGE_MIN 0.01    ///< when object is detected, must be bigger than 0 to detect voltage cut
#define STATUS_LED 13  ///< LED lamp, must be digital

#endif