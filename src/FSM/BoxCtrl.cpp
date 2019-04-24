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

BoxCtrl::BoxCtrl() : currentState(State::readSensorVal) {
    DBFUNCCALLln("BoxCtrl::BoxCtrl()");
}

void BoxCtrl::loop() {
    DBFUNCCALLln("BoxCtrl::loop()");
    process((this->*doActionFPtr)());  //do actions
}

void BoxCtrl::loop(Event currentEvent) {
    DBFUNCCALLln("BoxCtrl::loop(Event)");
    process(currentEvent);
    process((this->*doActionFPtr)());  //do actions
}

//=====PRIVATE====================================================================================
void BoxCtrl::process(Event e) {
    DBFUNCCALL("BoxCtrl::process ")
    DBFUNCCALLln(decodeEvent(e));
    switch (currentState) {
        case State::readSensorVal:
            if (Event::SBFull == e) {
                exitAction_readSensorVal();  // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            }
            break;
        case State::publishLevel:
            if (Event::AnswerReceived == e) {
                exitAction_publishLevel();          // Exit-action current state
                entryAction_calculateOptVehicle();  // Entry-actions next state
            }
            break;
        case State::calculateOptVehicle:
            if (Event::CalcOptVal == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_publishOptVehicle();   // Entry-actions next state
            }
            break;
        case State::publishOptVehicle:
            if (Event::AnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_waitForAck();        // Entry-actions next state
            } else if (Event::AnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_publishLevel();      // Entry-actions next state
            }
            break;
        case State::waitForAck:
            if (Event::InqVehicRespond == e) {
                exitAction_waitForAck();         // Exit-action current state
                entryAction_waitForTransport();  // Entry-actions next state
            } else if (Event::InqNoVehicRespond == e) {
                exitAction_waitForAck();          // Exit-action current state
                entryAction_publishOptVehicle();  // Entry-actions next state
            } else if (Event::NoVehicRespond == e) {
                exitAction_waitForAck();     // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            }
            break;
        case State::waitForTransport:
            if (Event::SBReady == e) {
                exitAction_waitForTransport();  // Exit-action current state
                entryAction_readSensorVal();    // Entry-actions next state
            }
            break;
        case State::errorState:
            if (Event::Resume == e) {
                exitAction_errorState();  // Exit-action current state
                switch (lastStateBevorError) {
                    case State::readSensorVal:
                        entryAction_readSensorVal();  // Entry-actions next state
                        break;
                    case State::publishLevel:
                        entryAction_publishLevel();  // Entry-actions next state
                        break;
                    case State::calculateOptVehicle:
                        entryAction_calculateOptVehicle();  // Entry-actions next state
                        break;
                    case State::publishOptVehicle:
                        entryAction_publishOptVehicle();  // Entry-actions next state
                        break;
                    case State::waitForAck:
                        entryAction_waitForAck();  // Entry-actions next state
                        break;
                    case State::waitForTransport:
                        entryAction_waitForTransport();  // Entry-actions next state
                        break;
                    default:
                        break;
                }
            }
        default:
            break;
    }
}
//==readSensorVal========================================================
void BoxCtrl::entryAction_readSensorVal() {
    DBINFO2ln("Entering State: emptyState");
    currentState = State::readSensorVal;  // state transition
    doActionFPtr = &BoxCtrl::doAction_readSensorVal;
}

BoxCtrl::Event BoxCtrl::doAction_readSensorVal() {
    DBINFO2ln("State: emptyState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
    pBoxlevelctrl.loop(BoxLevelCtrl::Event::CheckForPackage);
    if (BoxLevelCtrl::State::fullState == pBoxlevelctrl.getcurrentState()) {
        return Event::SBFull;
    }

    return Event::NoEvent;
}

void BoxCtrl::exitAction_readSensorVal() {
    DBINFO2ln("Leaving State: emptyState");
}

//==publishLevel========================================================
void BoxCtrl::entryAction_publishLevel() {
    DBINFO2ln("Entering State: publishLevel");
    currentState = State::publishLevel;  // state transition
    doActionFPtr = &BoxCtrl::doAction_publishLevel;
    //Subscribe to Topics
    // pComm.subscribe("Vehicle/+/params");
    // pComm.subscribe("Vehicle/+/ack");
}

BoxCtrl::Event BoxCtrl::doAction_publishLevel() {
    DBINFO2ln("State: publishLevel");
    //Publish SB level

    //Wait for Response
    //Generate the Event based on response

    return Event::NoEvent;
}

void BoxCtrl::exitAction_publishLevel() {
    DBINFO2ln("Leaving State: publishLevel");
}

//==calculateOptVehicle========================================================
void BoxCtrl::entryAction_calculateOptVehicle() {
    DBINFO2ln("Entering State: calculateOptVehicle");
    currentState = State::calculateOptVehicle;  // state transition
    doActionFPtr = &BoxCtrl::doAction_calculateOptVehicle;
}

BoxCtrl::Event BoxCtrl::doAction_calculateOptVehicle() {
    DBINFO2ln("State: calculateOptVehicle");
    //Check all saved Messages and save the best Vehicles to an Array
    //Generate the Event

    return Event::NoEvent;
}

void BoxCtrl::exitAction_calculateOptVehicle() {
    DBINFO2ln("Leaving State: calculateOptVehicle");
}

//==publishOptVehicle========================================================
void BoxCtrl::entryAction_publishOptVehicle() {
    DBINFO2ln("Entering State: publishOptVehicle");
    currentState = State::publishOptVehicle;  // state transition
    doActionFPtr = &BoxCtrl::doAction_publishOptVehicle;
}

BoxCtrl::Event BoxCtrl::doAction_publishOptVehicle() {
    DBINFO2ln("State: publishOptVehicle");
    //Publish decision
    //Wait for response
    //Generate the Event

    return Event::NoEvent;
}

void BoxCtrl::exitAction_publishOptVehicle() {
    DBINFO2ln("Leaving State: publishOptVehicle");
}

//==waitForAck========================================================
void BoxCtrl::entryAction_waitForAck() {
    DBINFO2ln("Entering State: waitForAck");
    currentState = State::waitForAck;  // state transition
    doActionFPtr = &BoxCtrl::doAction_waitForAck;
}

BoxCtrl::Event BoxCtrl::doAction_waitForAck() {
    DBINFO2ln("State: waitForAck");
    //read responses
    //check for ack from vehicle
    //Generate the Event

    return Event::NoEvent;
}

void BoxCtrl::exitAction_waitForAck() {
    DBINFO2ln("Leaving State: waitForAck");
}

//==waitForTransport===================================================
void BoxCtrl::entryAction_waitForTransport() {
    DBINFO2ln("Entering State: waitForTransport");
    currentState = State::waitForTransport;  // state transition
    doActionFPtr = &BoxCtrl::doAction_waitForTransport;
}

BoxCtrl::Event BoxCtrl::doAction_waitForTransport() {
    DBINFO2ln("State: waitForTransport");
    //Generate the Event
    //check message for transportconfirmation

    return Event::NoEvent;
}

void BoxCtrl::exitAction_waitForTransport() {
    DBINFO2ln("Leaving State: waitForTransport");
    //unsubscribe
}

//==errorState========================================================
void BoxCtrl::entryAction_errorState() {
    DBINFO2ln("Entering State: errorState");
    lastStateBevorError = currentState;
    currentState = State::errorState;  // state transition
    doActionFPtr = &BoxCtrl::doAction_errorState;
}

BoxCtrl::Event BoxCtrl::doAction_errorState() {
    DBINFO2ln("State: errorState");
    //Generate the Event

    return Event::NoEvent;
}

void BoxCtrl::exitAction_errorState() {
    DBINFO2ln("Leaving State: errorState");
}

//============================================================================
//==Aux-Function==============================================================
String BoxCtrl::decodeState(State state) {
    switch (state) {
        case State::readSensorVal:
            return "State::readSensorVal";
            break;
        case State::publishLevel:
            return "State::publishLevel";
            break;
        case State::calculateOptVehicle:
            return "State::calculateOptVehicle";
            break;
        case State::publishOptVehicle:
            return "State::publishOptVehicle";
            break;
        case State::waitForAck:
            return "State::waitForAck";
            break;
        case State::waitForTransport:
            return "State::waitForTransport";
            break;
        case State::errorState:
            return "State::errorState";
            break;
        default:
            return "ERROR: No matching state";
            break;
    }
}

String BoxCtrl::decodeEvent(Event event) {
    switch (event) {
        case Event::SBFull:
            return "Event::SBFull";
            break;
        case Event::SBReady:
            return "Event::SBReady";
            break;
        case Event::CalcOptVal:
            return "Event::CalcOptVal";
            break;
        case Event::AnswerReceived:
            return "Event::AnswerReceived";
            break;
        case Event::NoAnswerReceived:
            return "Event::NoAnswerReceived";
            break;
        case Event::InqVehicRespond:
            return "Event::InqVehicRespond";
            break;
        case Event::InqNoVehicRespond:
            return "Event::InqNoVehicRespond";
            break;
        case Event::NoVehicRespond:
            return "Event::NoVehicRespond";
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