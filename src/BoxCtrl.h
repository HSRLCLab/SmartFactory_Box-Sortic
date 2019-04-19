/**
 * @file BoxCtrl.h
 * @brief 
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.1 - Description - {author} - {date}
 * 
 * @date 2019-04-03
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BOXCTRL_H__
#define BOXCTRL_H__

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include <MQTTTasks.h>
#include <MainConfiguration.h>
#include <NetworkManager.h>
#include <NetworkManagerStructs.h>
#include <SensorArray.h>

class BoxCtrl {
   public:
    enum Event { evFull,               ///< SB is Full
                 evReady,              ///< SB is Ready
                 evCalcOptVal,         ///< Calculated Optimum Value
                 evAnswerReceived,     ///< Answer received
                 evNoAnswerReceived,   ///< No Answer received
                 evInqVehicRespond,    ///< Inquired Vehicle responded
                 evInqNoVehicRespond,  ///< Inquired Vehicle didnt responded
                 evNoVehicRespond,     ///< None of the Inquired Vehicle did responded
                 evError };            ///< Error occured
    BoxCtrl();
    void process(Event e);
    // changes the state of the FSM based on the event 'e'
    // starts the actions

   private:
    enum State { readSensorVal,
                 publishLevel,
                 calculateOptVehicle,
                 publishOptVehicle,
                 waitForAck,
                 waitForTransport,
                 errorState };

    State currentState;  // holds the current state of the FSM

    Box pbox;  // the valve
    bool pfirstLoopCycle = true;
};
#endif
