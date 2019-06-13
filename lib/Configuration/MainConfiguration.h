/**
 * @file MainConfiguration.h
 * @brief contains the default configuration for the smartbox
 * 
 *
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Release BA FS19  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-06-10
 * @version 0.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

const unsigned int SMARTBOX_WAITFOR_VEHICLES_SECONDS = 5;                 ///< SB waits this amount of seconds for vehicles to respond
const unsigned int SMARTBOX_ITERATION_VACKS_SECONDS = 5;                  ///< SB waits this amound of secnods for vehicles to send their acknoledgement to transport SB
const unsigned int TIME_BETWEEN_PUBLISH = 300;                            ///< Time between MQTT-Message publish
const int DEFAUL_HOSTNAME_NUMBER = 1;                                     ///< Number of the Vehicle. Used to dermine startline and for GUI
#define DEFAULT_HOSTNAME (String("SB") + String(DEFAUL_HOSTNAME_NUMBER))  ///< Hostname used for identification of SV
const unsigned int NUM_OF_MAXVALUES_VEHICLES_STORE = 2;                   ///< how many best values can be stored!

#endif