#ifndef LOGCONFIGURATION_H
#define LOGCONFIGURATION_H

#define LOGLEVELCONFIGURATION 3 // can have values from 0-3, 0-without, 1 error, 2 info, 3 verbose debugging

#if LOGLEVELCONFIGURATION == 3
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3) Serial.println(logg3)
#elif LOGLEVELCONFIGURATION == 2
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3)
#elif LOGLEVELCONFIGURATION == 1
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#elif LOGLEVELCONFIGURATION == 0
#define LOG1(logg1)
#define LOG2(logg2)
#define LOG3(logg3)
#else
#define LOG1(logg1) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#define LOG2(logg2) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#define LOG3(logg3) Serial.println("wrong log level value\nlogging levels: 0-without, 1 error, 2 info, 3 verbose debugging")
#endif

#endif