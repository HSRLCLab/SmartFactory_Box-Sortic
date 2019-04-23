/**
 * @file BoxLevelCtrl.cpp
 * @brief 
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Description - {author} - {date}
 * 
 * @date 2019-04-23
 * @copyright Copyright (c) 2019
 * 
 */

#include "BoxLevelCtrl.h"
//=====PUBLIC====================================================================================
BoxLevelCtrl::BoxLevelCtrl() : currentState(emptyState) {
}

void BoxLevelCtrl::loop() {
    //do actions
    currentEvent = (this->*doActionFPtr)();
    process(currentEvent);
}

void BoxLevelCtrl::process(Event e) {
    switch (currentState) {
        case emptyState:
            doActionFPtr = &BoxLevelCtrl::doAction_emptyState;
            if (evPackageDetected == e) {
                exitAction_emptyState();  // Exit-action current state
                entryAction_fullState();  // Entry-actions next state
            }
            break;

        case fullState:
            doActionFPtr = &BoxLevelCtrl::doAction_fullState;
            if (evNoPackageDetected == e) {
                exitAction_fullState();    // Exit-action current state
                entryAction_emptyState();  // Entry-actions next state
            }
            break;

        default:
            break;
    }
}

const BoxLevelCtrl::State BoxLevelCtrl::getcurrentState() {
    return currentState;
}
//=====PRIVATE====================================================================================
//==emptyState==========================================================
void BoxLevelCtrl::entryAction_emptyState() {
    DBINFO2ln("BL Entering State: emptyState");
    currentState = emptyState;  // state transition
    digitalWrite(LOADINDICATOR_LED, LOW);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_emptyState() {
    DBINFO2ln("BL State: emptyState");
    //Generate the Event
    if (pSensorArray.getSensorData()) {
        return evPackageDetected;
    } else {
        return evNoPackageDetected;
    };
    return evNoEvent;
}

void BoxLevelCtrl::exitAction_emptyState() {
    DBINFO2ln("BL Leaving State: emptyState");
}

//==fullState==========================================================
void BoxLevelCtrl::entryAction_fullState() {
    DBINFO2ln("BL Entering State: fullState");
    currentState = fullState;  // state transition
    digitalWrite(LOADINDICATOR_LED, HIGH);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_fullState() {
    DBINFO2ln("BL State: fullState");
    if (pSensorArray.getSensorData()) {
        return evPackageDetected;
    } else {
        return evNoPackageDetected;
    };
    return evNoEvent;
}

void BoxLevelCtrl::exitAction_fullState() {
    DBINFO2ln("BL Leaving State: fullState");
}