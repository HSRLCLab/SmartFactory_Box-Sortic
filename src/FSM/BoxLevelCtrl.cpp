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
BoxLevelCtrl::BoxLevelCtrl() : currentState(State::emptyState) {
}

void BoxLevelCtrl::loop() {
    //do actions
    currentEvent = (this->*doActionFPtr)();
    process(currentEvent);
}

void BoxLevelCtrl::process(Event e) {
    switch (currentState) {
        case State::emptyState:
            doActionFPtr = &BoxLevelCtrl::doAction_emptyState;
            if (Event::PackageDetected == e) {
                exitAction_emptyState();  // Exit-action current state
                entryAction_fullState();  // Entry-actions next state
            }
            break;

        case State::fullState:
            doActionFPtr = &BoxLevelCtrl::doAction_fullState;
            if (Event::NoPackageDetected == e) {
                exitAction_fullState();    // Exit-action current state
                entryAction_emptyState();  // Entry-actions next state
            }
            break;
        case State::errorState:
            doActionFPtr = &BoxLevelCtrl::doAction_errorState;
            if (Event::Resume == e) {
                exitAction_errorState();  // Exit-action current state
                switch (lastStateBevorError) {
                    case State::emptyState:
                        entryAction_emptyState();  // Entry-actions next state
                        break;
                    case State::fullState:
                        entryAction_fullState();  // Entry-actions next state
                        break;
                    default:
                        break;
                }
            }
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
    currentState = State::emptyState;  // state transition
    digitalWrite(LOADINDICATOR_LED, LOW);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_emptyState() {
    DBINFO2ln("BL State: emptyState");
    //Generate the Event
    if (pSensorArray.getSensorData()) {
        return Event::PackageDetected;
    } else {
        return Event::NoPackageDetected;
    };
    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_emptyState() {
    DBINFO2ln("BL Leaving State: emptyState");
}

//==fullState==========================================================
void BoxLevelCtrl::entryAction_fullState() {
    DBINFO2ln("BL Entering State: fullState");
    currentState = State::fullState;  // state transition
    digitalWrite(LOADINDICATOR_LED, HIGH);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_fullState() {
    DBINFO2ln("BL State: fullState");
    if (pSensorArray.getSensorData()) {
        return Event::PackageDetected;
    } else {
        return Event::NoPackageDetected;
    };
    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_fullState() {
    DBINFO2ln("BL Leaving State: fullState");
}

//==errorState========================================================
void BoxLevelCtrl::entryAction_errorState() {
    DBINFO2ln("BL Entering State: errorState");
    lastStateBevorError = currentState;
    currentState = State::errorState;  // state transition
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_errorState() {
    DBINFO2ln("BL State: errorState");
    //Generate the Event

    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_errorState() {
    DBINFO2ln("BL Leaving State: errorState");
}