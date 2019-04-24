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
    DBFUNCCALLln("BoxLevelCtrl::loop()");
    process((this->*doActionFPtr)());  //do actions
}

void BoxLevelCtrl::loop(Event currentEvent) {
    DBFUNCCALLln("BoxLevelCtrl::loop(Event)");
    process(currentEvent);
    process((this->*doActionFPtr)());  //do actions
}

const BoxLevelCtrl::State BoxLevelCtrl::getcurrentState() {
    return currentState;
}
//=====PRIVATE====================================================================================
void BoxLevelCtrl::process(Event e) {
    DBFUNCCALL("BoxLevelCtrl::process ")
    DBFUNCCALLln(decodeEvent(e));
    switch (currentState) {
        case State::emptyState:
            if (Event::CheckForPackage == e) {
                exitAction_emptyState();  // Exit-action current state
                entryAction_checking();   // Entry-actions next state
            }
            break;
        case State::checking:
            if (Event::PackageDetected == e) {
                exitAction_checking();    // Exit-action current state
                entryAction_fullState();  // Entry-actions next state
            } else if (Event::NoPackageDetected == e) {
                exitAction_checking();     // Exit-action current state
                entryAction_emptyState();  // Entry-actions next state
            }
            break;
        case State::fullState:
            if (Event::CheckForPackage == e) {
                exitAction_fullState();  // Exit-action current state
                entryAction_checking();  // Entry-actions next state
            }
            break;
        case State::errorState:
            if (Event::Resume == e) {
                exitAction_errorState();  // Exit-action current state
                switch (lastStateBevorError) {
                    case State::emptyState:
                        entryAction_emptyState();  // Entry-actions next state
                        break;
                    case State::checking:
                        entryAction_checking();  // Entry-actions next state
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
//==emptyState==========================================================
void BoxLevelCtrl::entryAction_emptyState() {
    DBINFO2ln("BL Entering State: emptyState");
    currentState = State::emptyState;  // state transition
    doActionFPtr = &BoxLevelCtrl::doAction_emptyState;
    digitalWrite(LOADINDICATOR_LED, LOW);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_emptyState() {
    DBINFO2ln("BL State: emptyState");
    //Generate the Event
    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_emptyState() {
    DBINFO2ln("BL Leaving State: emptyState");
}

//==checking==========================================================
void BoxLevelCtrl::entryAction_checking() {
    DBINFO2ln("BL Entering State: checking");
    currentState = State::checking;  // state transition
    doActionFPtr = &BoxLevelCtrl::doAction_checking;
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_checking() {
    DBINFO2ln("BL State: checking");
    if (pSensorArray.getSensorData()) {
        return Event::PackageDetected;
    } else {
        return Event::NoPackageDetected;
    };
    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_checking() {
    DBINFO2ln("BL Leaving State: checking");
}

//==fullState==========================================================
void BoxLevelCtrl::entryAction_fullState() {
    DBINFO2ln("BL Entering State: fullState");
    currentState = State::fullState;  // state transition
    doActionFPtr = &BoxLevelCtrl::doAction_fullState;
    digitalWrite(LOADINDICATOR_LED, HIGH);
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_fullState() {
    DBINFO2ln("BL State: fullState");
    //Generate the Event

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
    doActionFPtr = &BoxLevelCtrl::doAction_errorState;
}

BoxLevelCtrl::Event BoxLevelCtrl::doAction_errorState() {
    DBINFO2ln("BL State: errorState");
    //Generate the Event

    return Event::NoEvent;
}

void BoxLevelCtrl::exitAction_errorState() {
    DBINFO2ln("BL Leaving State: errorState");
}

//============================================================================
//==Aux-Function==============================================================
String BoxLevelCtrl::decodeState(State state) {
    switch (state) {
        case State::emptyState:
            return "State::emptyState";
            break;
        case State::fullState:
            return "State::fullState";
            break;
        case State::checking:
            return "State::checking";
            break;
        case State::errorState:
            return "State::errorState";
            break;
        default:
            return "ERROR: No matching state";
            break;
    }
}

String BoxLevelCtrl::decodeEvent(Event event) {
    switch (event) {
        case Event::PackageDetected:
            return "Event::PackageDetected";
            break;
        case Event::NoPackageDetected:
            return "Event::NoPackageDetected";
            break;
        case Event::CheckForPackage:
            return "Event::CheckForPackage";
            break;
        case Event::Error:
            return "Event::Error";
            break;
        case Event::Resume:
            return "Event::Resume";
            break;
        case Event::NoEvent:
            return "Event::NoEvent";
            break;
        default:
            return "ERROR: No matching event";
            break;
    }
}