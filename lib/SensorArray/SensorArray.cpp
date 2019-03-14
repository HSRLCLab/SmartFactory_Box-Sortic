/**
 * @file SensorArray.cpp
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief The Sensor Array
 * 
 * This is the implementation of the Sensor Array.
 * 
 * @version 0.1
 * @date 2019-03-06
 * 
 * @copyright Copyright (c) 2019
 * 
 
 */

#include "SensorArray.h"

extern bool showCase;  // defined in main.cpp

SensorArray::SensorArray() {  // initialisation of Sensor
    LOG4("SensorArray::SensorArray()");
    pinMode(STATUS_LED, OUTPUT);  /// initialize light LED, Output
    pinMode(INPUT_PIN1, INPUT);   /// initialize sharp sensor 1, Input
    pinMode(INPUT_PIN2, INPUT);   /// initialize sharp sensor 2, Input
    pinMode(INPUT_PIN3, INPUT);   /// initialize sharp sensor 3, Input
    // lastValuesSize = 0;           /// initialize lastValuesSize, 0
}

/**
 * @todo Check multiple times if a Object is present for relaiability
 */
bool SensorArray::getSensorData() {  // read sensor, true if full
    LOG4("SensorArray::getSensorData()");
    LOG3("getting Sensor Data");
    int sensor1 = !digitalRead(INPUT_PIN1);  // if Object detected INPUt_PIN = LOW
    int sensor2 = !digitalRead(INPUT_PIN2);
    int sensor3 = !digitalRead(INPUT_PIN3);

    LOG3("SensorValue1: " + String(sensor1));
    LOG3("SensorValue2: " + String(sensor2));
    LOG3("SensorValue3: " + String(sensor3));
    if (sensor1 || sensor2 || sensor3) {
        LOG3("Sensor found Element in Box");  // true if within 0.5-5cm from Sensor, otherwise false
        digitalWrite(STATUS_LED, HIGH);
        return true;
    } else {
        LOG3("Sensor found no Element");
        digitalWrite(STATUS_LED, LOW);
        return false;
    }
}

// double sum1 = 0, sum2 = 0, sum3 = 0;
// digitalWrite(OUTPUT_PIN, HIGH);
// delay(10);  // needed for time delay of Sensor
// //digitalWrite(OUTPUT_PIN, HIGH);
// for (int i = 0; i < MAX_SENSOR_ITERATIONS; i++) {
//     double volts = analogRead(INPUT_PIN1);  // * 0.0049;  //
//     sum1 += volts;
//     volts = analogRead(INPUT_PIN2);  // * 0.0049;
//     sum2 += volts;
//     volts = analogRead(INPUT_PIN3);  // * 0.0049;
//     sum3 += volts;
//     delay(5);  // needed for reading out PIN
// }
// sum1 /= MAX_SENSOR_ITERATIONS;
// sum2 /= MAX_SENSOR_ITERATIONS;
// sum3 /= MAX_SENSOR_ITERATIONS;
// LOG3("SensorValue1: " + String(sum1));
// if (showCase) {
//     if ((sum1 <= SENSOR_RANGE_MAX) && (sum1 >= SENSOR_RANGE_MIN)) {
//         LOG3("Sensor found Element in Box");  // true if within 0.5-5cm from Sensor, otherwise false
//         return true;
//     } else {
//         LOG3("Sensor found no Element");
//         return false;
//     }
// } else {
//     LOG3("SensorValue2: " + String(sum2));
//     LOG3("SensorValue3: " + String(sum3));
//     if ((sum1 <= SENSOR_RANGE_MAX) && (sum1 >= SENSOR_RANGE_MIN) &&
//         (sum2 <= SENSOR_RANGE_MAX) && (sum2 >= SENSOR_RANGE_MIN) &&
//         (sum3 <= SENSOR_RANGE_MAX) && (sum3 >= SENSOR_RANGE_MIN)) {
//         LOG3("Sensor found Element in Box");  // true if within 0.5-5cm from Sensor, otherwise false
//         return true;
//     } else {
//         LOG3("Sensor found no Element");
//         return false;
//     }
// }
//}

//===================================PRIVATE======================================================================

// bool SensorArray::getLastSensorData(int num) {
//     if ((num < MAX_SENSOR_VALUES) && (num > 0)) {
//         return SensValStore[(lastValuesSize + num) % MAX_SENSOR_VALUES];
//     } else {
//         LOG1("You entered a wrong number!");
//         LOG2("you entered: " + num);
//         LOG3("it must be between 0 and " + String(MAX_SENSOR_VALUES - 1));
//         return false;
//     }
// }

// void SensorArray::pushToQueue(bool &val) {
//     if (lastValuesSize == (MAX_SENSOR_VALUES - 2)) {
//         lastValuesSize = -1;
//     };
//     lastValuesSize++;
//     SensValStore[lastValuesSize] = val;
// }
