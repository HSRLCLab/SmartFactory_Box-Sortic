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
    DBFUNCCALLln("BoxCtrl::BoxCtrl()");
}

void BoxCtrl::loop() {
    DBFUNCCALLln("BoxCtrl::loop()");
    //do actions
    currentEvent = (this->*doActionFPtr)();
    process(currentEvent);
}

void BoxCtrl::process(Event e) {
    switch (currentState) {
        case readSensorVal:
            doActionFPtr = &BoxCtrl::doAction_readSensorVal;
            if (evSBFull == e) {
                exitAction_readSensorVal();  // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            }
            break;
        case publishLevel:
            doActionFPtr = &BoxCtrl::doAction_publishLevel;
            if (evAnswerReceived == e) {
                exitAction_publishLevel();          // Exit-action current state
                entryAction_calculateOptVehicle();  // Entry-actions next state
            }
            break;
        case calculateOptVehicle:
            doActionFPtr = &BoxCtrl::doAction_calculateOptVehicle;
            if (evCalcOptVal == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_publishOptVehicle();   // Entry-actions next state
            }
            break;
        case publishOptVehicle:
            doActionFPtr = &BoxCtrl::doAction_publishOptVehicle;
            if (evAnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_waitForAck();        // Entry-actions next state
            } else if (evAnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_publishLevel();      // Entry-actions next state
            }
            break;
        case waitForAck:
            doActionFPtr = &BoxCtrl::doAction_waitForAck;
            if (evInqVehicRespond == e) {
                exitAction_waitForAck();         // Exit-action current state
                entryAction_waitForTransport();  // Entry-actions next state
            } else if (evInqNoVehicRespond == e) {
                exitAction_waitForAck();          // Exit-action current state
                entryAction_publishOptVehicle();  // Entry-actions next state
            } else if (evNoVehicRespond == e) {
                exitAction_waitForAck();     // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            }
            break;
        case waitForTransport:
            doActionFPtr = &BoxCtrl::doAction_waitForTransport;
            if (evSBReady == e) {
                exitAction_waitForTransport();  // Exit-action current state
                entryAction_readSensorVal();    // Entry-actions next state
            }
            break;
        case errorState:
            doActionFPtr = &BoxCtrl::doAction_errorState;
            // if (  == e) {
            //     exitAction_errorState(); // Exit-action current state
            //     entryAction_(); // Entry-actions next state
            // }
            break;

        default:
            break;
    }
}

//=====PRIVATE====================================================================================
//==readSensorVal========================================================
void BoxCtrl::entryAction_readSensorVal() {
    DBINFO2ln("Entering State: emptyState");
    currentState = readSensorVal;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_readSensorVal() {
    DBINFO2ln("State: emptyState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
    pBoxlevelctrl.loop();
    if (BoxLevelCtrl::fullState == pBoxlevelctrl.getcurrentState()) {
        return evSBFull;
    }
    return evNoEvent;
}

void BoxCtrl::exitAction_readSensorVal() {
    DBINFO2ln("Leaving State: emptyState");
}

//==publishLevel========================================================
void BoxCtrl::entryAction_publishLevel() {
    DBINFO2ln("Entering State: publishLevel");
    currentState = publishLevel;  // state transition
    //Subscribe to Topics
    // pComm.subscribe("Vehicle/+/params");
    // pComm.subscribe("Vehicle/+/ack");
}

BoxCtrl::Event BoxCtrl::doAction_publishLevel() {
    DBINFO2ln("State: publishLevel");
    //Publish SB level

    //Wait for Response
    //Generate the Event based on response

    return evNoEvent;
}

void BoxCtrl::exitAction_publishLevel() {
    DBINFO2ln("Leaving State: publishLevel");
}

//==calculateOptVehicle========================================================
void BoxCtrl::entryAction_calculateOptVehicle() {
    DBINFO2ln("Entering State: calculateOptVehicle");
    currentState = calculateOptVehicle;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_calculateOptVehicle() {
    DBINFO2ln("State: calculateOptVehicle");
    //Check all saved Messages and save the best Vehicles to an Array
    //Generate the Event

    return evNoEvent;
}

void BoxCtrl::exitAction_calculateOptVehicle() {
    DBINFO2ln("Leaving State: calculateOptVehicle");
}

//==publishOptVehicle========================================================
void BoxCtrl::entryAction_publishOptVehicle() {
    DBINFO2ln("Entering State: publishOptVehicle");
    currentState = publishOptVehicle;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_publishOptVehicle() {
    DBINFO2ln("State: publishOptVehicle");
    //Publish decision
    //Wait for response
    //Generate the Event

    return evNoEvent;
}

void BoxCtrl::exitAction_publishOptVehicle() {
    DBINFO2ln("Leaving State: publishOptVehicle");
}

//==waitForAck========================================================
void BoxCtrl::entryAction_waitForAck() {
    DBINFO2ln("Entering State: waitForAck");
    currentState = waitForAck;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_waitForAck() {
    DBINFO2ln("State: waitForAck");
    //read responses
    //check for ack from vehicle
    //Generate the Event

    return evNoEvent;
}

void BoxCtrl::exitAction_waitForAck() {
    DBINFO2ln("Leaving State: waitForAck");
}

//==waitForTransport===================================================
void BoxCtrl::entryAction_waitForTransport() {
    DBINFO2ln("Entering State: waitForTransport");
    currentState = waitForTransport;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_waitForTransport() {
    DBINFO2ln("State: waitForTransport");
    //Generate the Event
    //check message for transportconfirmation

    return evNoEvent;
}

void BoxCtrl::exitAction_waitForTransport() {
    DBINFO2ln("Leaving State: waitForTransport");
    //unsubscribe
}

//==errorState========================================================
void BoxCtrl::entryAction_errorState() {
    DBINFO2ln("Entering State: errorState");
    currentState = errorState;  // state transition
}

BoxCtrl::Event BoxCtrl::doAction_errorState() {
    DBINFO2ln("State: errorState");
    //Generate the Event

    return evNoEvent;
}

void BoxCtrl::exitAction_errorState() {
    DBINFO2ln("Leaving State: errorState");
}