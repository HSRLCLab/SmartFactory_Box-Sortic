/**
 * @file MainConfiguration.h
 * @brief contains the default configuration for the main function
 * 
 * SENSOR_* is concerning the sensor
 * *ITERATION* is an iteration variable (e.g. for loops)
 * *SECONDS* is stating, that the value is given in seconds and will be recalculated for milliseconds (for func delay())
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

const unsigned int SMARTBOX_WAITFOR_VEHICLES_SECONDS = 15;  ///< SB waits this amount of seconds for vehicles to respond
const unsigned int SMARTBOX_ITERATION_VACKS_SECONDS = 15;   ///< SB waits this amound of secnods for vehicles to send their acknoledgement to transport SB
const unsigned int TIME_BETWEEN_PUBLISH = 500;
// #define SMARTBOX_ITERATION_VTRANSPORTS_SECONDS 10  ///< SB waits this amount of seconds for vehicles to respond
// #define NUM_OF_VEHICLES_IN_FIELD 2                ///< maximum Number of vehicles in Field, @todo recognize automatic how many vehicles are one the field
#define NUM_OF_MAXVALUES_VEHICLES_STORE 2  ///< how much best values can be stored, must be smaller or equal than NUM_OF_VEHICLES_IN_FIELD!
#define PIN_FOR_FULL 12                    ///< if SmartBox is full, light up LED
// #define SERVICE_MODE 0                            ///< 0 = not activ, 1 = offlinemode, 2 = sensortest,offline

#endif