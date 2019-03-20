/**
 * @file MQTTTasksConfiguration.h
 * @brief file that contains the default MQTT message save configuration
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

#ifndef MQTTTASKSCONFIGURATION_H
#define MQTTTASKSCONFIGURATION_H

#define MAX_JSON_MESSAGES_SAVED 20  ///< max num of saved JSON items, must be smaller than num of vehicles!
#define MAX_MQTT_TOPIC_DEPTH 5      ///< how many topics can be in row, e.g. SmartBox/SB1/level are 3 topic levels

#endif