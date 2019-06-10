/**
 * @file SensorArray.cpp
 * @brief The Sensor Array
 * 
 * This is the implementation of the Sensor Array.
 * 
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Refactoring and changed readingstyle from analog to digital  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.1 - Added Doxygen-Documentation  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

#include "SensorArray.h"

SensorArray::SensorArray(const int Sensor1, const int Sensor2, const int Sensor3, const int LoadIndicatorLED) : pSensor1(Sensor1),
                                                                                                                pSensor2(Sensor2),
                                                                                                                pSensor3(Sensor3),
                                                                                                                pLoadIndicatorLED(LoadIndicatorLED) {
    pinMode(pSensor1, INPUT);
    pinMode(pSensor2, INPUT);
    pinMode(pSensor3, INPUT);
}

/**
 * @todo Check multiple times if a Object is present to increase relaiability
 */
bool SensorArray::getSensorData() {  // read sensor, true if full
    DBFUNCCALL("SensorArray::getSensorData()");
    DBINFO3("getting Sensor Data");
    int sensor1 = !digitalRead(pSensor1);  // if Object detected PIN = LOW
    int sensor2 = !digitalRead(pSensor2);
    int sensor3 = !digitalRead(pSensor3);

    DBINFO3("SensorValue1: " + String(sensor1));
    DBINFO3("SensorValue2: " + String(sensor2));
    DBINFO3("SensorValue3: " + String(sensor3));
    if (sensor1 || sensor2 || sensor3) {
        DBINFO3("Sensor found Element in Box");  // true if within 0.5-5cm from Sensor, otherwise false
        // digitalWrite(LOADINDICATOR_LED, HIGH);
        return true;
    } else {
        DBINFO3("Sensor found no Element");
        // digitalWrite(LOADINDICATOR_LED, LOW);
        return false;
    }
}

//===================================PRIVATE======================================================================
