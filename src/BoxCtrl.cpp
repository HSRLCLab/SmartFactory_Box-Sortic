/**
 * @file BoxCtrl.cpp
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
#include "BoxCtrl.h"

BoxCtrl::BoxCtrl() : currentState(readSensorVal) {
}

void BoxCtrl::process(Event e) {
    switch (currentState) {
        case readSensorVal:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: readSensorVal");
                pfirstLoopCycle = false;
            }
            // if (getSensorData()) {
            //     e = evFull;
            // }
            if (evFull == e) {
                LOG2("Changing to State: publishLevel");
                //=======Exit-Action actual State=======
                pfirstLoopCycle = true;

                //=======Entry-Action next State=======
                currentState = publishLevel;
            }
            //wait
            break;

        case publishLevel:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: publishLevel");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evAnswerReceived == e) {
                LOG2("Changing to State: calculateOptVehicle");
                // actions

                // state transition
                currentState = calculateOptVehicle;
            }
            // else if (evNoAnswerReceived == e) {
            //     LOG2("Changing to State: publishLevel");
            //     // actions

            //     // state transition
            //     currentState = publishLevel;
            // }

            //=======Exit-Action=======

            break;

        case calculateOptVehicle:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: calculateOptVehicle");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evCalcOptVal == e) {
                LOG2("Changing to State: publishOptVehicle");
                // actions

                // state transition
                currentState = publishOptVehicle;
            }
            //=======Exit-Action=======

            break;

        case publishOptVehicle:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: publishOptVehicle");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evAnswerReceived == e) {
                LOG2("Changing to State: waitForAck");
                // actions

                // state transition
                currentState = waitForAck;
            } else if (evNoAnswerReceived == e) {
                LOG2("Changing to State: publishLevel");
                // actions

                // state transition
                currentState = publishLevel;
            }
            //=======Exit-Action=======

            break;

        case waitForAck:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: waitForAck");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evInqVehicRespond == e) {
                LOG2("Changing to State: waitForTransport");
                // actions

                // state transition
                currentState = waitForTransport;
            } else if (evInqNoVehicRespond == e) {
                LOG2("Changing to State: publishOptVehicle");
                // actions

                // state transition
                currentState = publishOptVehicle;
            } else if (evNoVehicRespond == e) {
                LOG2("Changing to State: publishLevel");
                // actions

                // state transition
                currentState = publishLevel;
            }
            //=======Exit-Action=======

            break;

        case waitForTransport:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: waitForTransported");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evReady == e) {
                LOG2("Changing to State: readSensorVal");
                // actions

                // state transition
                currentState = readSensorVal;
            }
            //=======Exit-Action=======

            break;

        case errorState:
            if (pfirstLoopCycle) {  //only run on the first function-call
                LOG2("State: errorState");
                pfirstLoopCycle = false;
            }
            //=======Entry-Action=======

            if (evReady == e) {
                LOG2("Changing to State: readSensorVal");
                // actions

                // state transition
                currentState = readSensorVal;
            }
            //=======Exit-Action=======

            break;

        default:
            break;
    }
}
