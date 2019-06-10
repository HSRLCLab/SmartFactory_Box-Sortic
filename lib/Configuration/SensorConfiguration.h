/**
 * @file SensorConfiguration.h
 * @brief File that contains the default sensor configuration
 * 
 * A6 and greater not existing on board!
 * Sensortyp: SHARP GP2Y0D805Z0F 
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H

const int LB1 = A5;                ///< Pin IR sensor LightBarrier 1
const int LB2 = A4;                ///< Pin IR sensor LightBarrier 2
const int LB3 = A3;                ///< Pin IR sensor LightBarrier 3
const int LOADINDICATOR_LED = 12;  ///< PIn for Loadindicator LED

#endif