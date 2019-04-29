/**
 * @file LogConfiguration.h
 * @brief Contains Pre-Compiler directives for diffent Serialprints for Datalogin
 * 
 * can have values from 0-3, 0-without, 1 error, 2 info, 3 verbose debugging
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * 
 * @version 1.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 1.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 * @todo rewrite LOGs with ERROR, WARNINGS println print etc.
 */

#ifndef LOGCONFIGURATION_H
#define LOGCONFIGURATION_H

#define DEBUGGER
#define LOGLEVELCONFIGURATION 3  // can have values from 0-4, 0-without, 1 error, 2 info, 3 verbose debugging,4 disp functioncall
#if LOGLEVELCONFIGURATION == 4   ///< detailed logging (every function call)
#define LOG1(logg1) Serial.println(logg1)
#define LOG2(logg2) Serial.println(logg2)
#define LOG3(logg3) Serial.println(logg3)
#define LOG4(logg4)             \
    if (Serial) {               \
        Serial.print("Func: "); \
        Serial.println(logg4);  \
    }                             //< protects serial if called befor setup() eg in an constructor
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


/**
 * @file LogConfiguration.h
 * @brief Contains Pre-Compiler directives for diffent Serialprints for Datalogin
 * 
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 -  Implement diffrent debug functions - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

// #ifndef LOGCONFIGURATION_H
// #define LOGCONFIGURATION_H

#define DEBUGGER true  ///< Option to activate the Serial.print global

#ifdef DEBUGGER
#define DEBUG_ERROR     ///< Define DEBUG_ERROR global to print all  occuring errors via serial
#define DEBUG_WARNING   ///< Define DEBUG_WARNING global to print all occuring warnings via serial
#define DEBUG_STATUS    ///< Define DEBUG_STATUS global to print all occuring FSM status-updates via serial
#define DEBUG_EVENT     ///< Define DEBUG_EVENT global to print all occuring EVENTS which lead to a statechange via serial
#define DEBUG_INFO1     ///< Define DEBUG_INFO1 global to print all occuring FSM status changes (entering/leaving) via serial
#define DEBUG_INFO2     ///< Define DEBUG_INFO2 global to print all occuring infos inside the FSM via serial
#define DEBUG_INFO3     ///< Define DEBUG_INFO3 global to print all occuring message on hardwarelevel via serial
#define DEBUG_FUNCCALL  ///< Define DEBUG_FUNCCALL global to print all occuring functioncalls via serial
#endif

#ifdef DEBUG_ERROR
#define DBERROR(x)           \
    Serial.print("ERROR: "); \
    Serial.println(x);
#else
#define DBERROR(x)
#endif

#ifdef DEBUG_WARNING
#define DBWARNING(x) Serial.print(x)
#define DBWARNINGln(x) Serial.println(x)
#else
#define DBWARNING(x)
#define DBWARNINGln(x)
#endif

#ifdef DEBUG_STATUS
#define DBSTATUS(x) Serial.print(x)
#define DBSTATUSln(x) Serial.println(x)
#else
#define DBSTATUS(x)
#define DBSTATUSln(x)
#endif

#ifdef DEBUG_EVENT
#define DBEVENT(x) Serial.print(x)
#define DBEVENTln(x) Serial.println(x)
#else
#define DBEVENT(x)
#define DBEVENTln(x)
#endif

#ifdef DEBUG_INFO1
#define DBINFO1(x)       \
    if (Serial) {        \
        Serial.print(x); \
    };
#define DBINFO1ln(x)       \
    if (Serial) {          \
        Serial.println(x); \
    };
#else
#define DBINFO1(x)
#define DBINFO1ln(x)
#endif

#ifdef DEBUG_INFO2
#define DBINFO2(x)       \
    if (Serial) {        \
        Serial.print(x); \
    };
#define DBINFO2ln(x)       \
    if (Serial) {          \
        Serial.println(x); \
    };
#else
#define DBINFO2(x)
#define DBINFO2ln(x)
#endif

#ifdef DEBUG_INFO3
#define DBINFO3(x)       \
    if (Serial) {        \
        Serial.print(x); \
    };
#define DBINFO3ln(x)       \
    if (Serial) {          \
        Serial.println(x); \
    };
#else
#define DBINFO3(x)
#define DBINFO3ln(x)
#endif

#ifdef DEBUG_FUNCCALL
#define DBFUNCCALL(x)    \
    if (Serial) {        \
        Serial.print(x); \
    }
#define DBFUNCCALLln(x)    \
    if (Serial) {          \
        Serial.println(x); \
    }
#else
#define DBFUNCCALL(x)
#define DBFUNCCALLln(x)
#endif

#endif