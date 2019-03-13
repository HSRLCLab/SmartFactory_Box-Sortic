/**
 * @file LogConfiguration.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief Contains Pre-Compiler directives for diffent Serialprints for Datalogin
 * @version 0.1
 * @date 2019-03-08
 * 
 * can have values from 0-3, 0-without, 1 error, 2 info, 3 verbose debugging
 * 
 * @copyright Copyright (c) 2019
 * 
 * @todo rewrite LOGs with ERROR, WARNINGS println print etc.
 */
#ifndef LOGCONFIGURATION_H
#define LOGCONFIGURATION_H

#define LOGLEVELCONFIGURATION 4  // can have values from 0-4, 0-without, 1 error, 2 info, 3 verbose debugging,4 disp functioncall
#if LOGLEVELCONFIGURATION == 4   ///< detailed logging (every function call)
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3) Serial.println(logg3)
#define LOG4(logg4) Serial.println(logg4)
#elif LOGLEVELCONFIGURATION == 3  ///< detailed logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3) Serial.println(logg3)
#define LOG4(logg4)
#elif LOGLEVELCONFIGURATION == 2  ///< only information logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3)
#define LOG4(logg4)
#elif LOGLEVELCONFIGURATION == 1  ///< only error logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#define LOG4(logg4)
#elif LOGLEVELCONFIGURATION == 0  ///< no logging
#define LOG1(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#define LOG4(logg4)
#else
#define LOG1(logg1) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#define LOG2(logg2) LOG1(logg1)
#define LOG3(logg3) LOG1(logg1)
#define LOG4(logg4) LOG1(logg1)
#endif

#endif