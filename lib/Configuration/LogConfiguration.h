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
 */
#ifndef LOGCONFIGURATION_H
#define LOGCONFIGURATION_H

#define LOGLEVELCONFIGURATION 3  // can have values from 0-3, 0-without, 1 error, 2 info, 3 verbose debugging

#if LOGLEVELCONFIGURATION == 3  ///< detailed logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3) Serial.println(logg3)
#elif LOGLEVELCONFIGURATION == 2  ///< only information logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3)
#elif LOGLEVELCONFIGURATION == 1  ///< only error logging
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#elif LOGLEVELCONFIGURATION == 0  ///< no logging
#define LOG1(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#else
#define LOG1(logg1) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#define LOG2(logg2) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#define LOG3(logg3) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#endif

#endif