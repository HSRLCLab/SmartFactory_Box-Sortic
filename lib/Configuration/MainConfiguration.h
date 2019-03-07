/**
 * @file MainConfiguration.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief contains the default configuration for the main function
 * 
 * SENSOR_* is concerning the sensor
 *       *ITERATION* is an iteration variable (e.g. for loops)
 *      *SECONDS* is stating, that the value is given in seconds and will be recalculated for milliseconds (for func delay())
 * 
 * @version 0.1
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

#define SMARTBOX_WAITFOR_VEHICLES_SECONDS 5       ///< SB waits this amount of seconds for vehicles to respond
#define SMARTBOX_ITERATION_VACKS_SECONDS 5        ///< SB waits this amound of secnods for vehicles to send their acknoledgement to transport SB
#define SMARTBOX_ITERATION_VTRANSPORTS_SECONDS 5  ///< SB waits this amount of seconds for vehicles to respond
#define NUM_OF_VEHICLES_IN_FIELD 2                ///< maximum Number of vehicles in Field, TODO erkennen?
#define NUM_OF_MAXVALUES_VEHICLES_STORE 2         ///< how much best values can be stored, must be smaller or equal than NUM_OF_VEHICLES_IN_FIELD!
#define PIN_FOR_FULL 12                           ///< if SmartBox is full, light up LED

#endif