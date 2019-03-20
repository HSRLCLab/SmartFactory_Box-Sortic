/**
 * @file NetworkManagerStructs.h
 * @brief saving Messages to use them later on (asynchronous Communication)
 * 
 * used in:
 * FILE                
 * -----------------------------------------------------
 * NetworkManager.h  
 * NetworkManager.cpp
 * MQTTTasks.h
 * MQTTTasks.cpp
 * main.cpp 
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-08
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef NETWORKMANAGERSTRUCTS_H
#define NETWORKMANAGERSTRUCTS_H

#include <Arduino.h>

/**
 * @brief myJSONStr
 * 
 */
struct myJSONStr {
    bool urgent = false;
    String topic = "default";
    String hostname = "default";
    String request = "default";
    int level = -5;  ///< describes Smart Box level states, -5 is default if not set!
    /**
     * @brief vehilce Params
     * 
     * [0]: velocity v \n
     * [1]: movingdirection vd \n
     * [2]: distance d \n
     * [3]: open tasks t \n
     */
    double vehicleParams[5];
};

/**
 * \enum SBLevel
 * 
 * @brief SmartBox Levels
 * 
 */
enum SBLevel {
    dead = -1,  ///> -1
    full = 0,   ///> 0
    empty = 1   ///> 1
};

#endif