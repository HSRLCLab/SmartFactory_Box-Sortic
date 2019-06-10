/**
 * @file main.cpp
 * @brief Smart Box main file
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - BA FS 2019  - Luca Mazzoleni (luca.mazzoleni@hsr.ch)  - 2019-03-20
 * @version 0.0 - SA SmartFactroyBox HS 2018
 * 
 * @date 2019-03-20
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>

#include "FSM/BoxCtrl.h"

// ===================================== Global Variables =====================================
// SensorArray *mSarrP;                                   ///< used for SensorArray access outside setup()
BoxCtrl *boxctrl;  ///< Instance of BoxCtrl

// ===================================== Arduino Functions =====================================

void setup() {
    // if (DEBUGGER) {
    //     Serial.begin(9600);  //Initialize serial
    //     while (!Serial)
    //         ;  // wait for serial port to connect
    // }
    boxctrl = new BoxCtrl();
    delay(100);
}

void loop() {
    DBFUNCCALLln("loop()======================================================");
    boxctrl->loop();
    //  mSarrP->SensorArray::getSensorData();
    // delay(1000);
}
