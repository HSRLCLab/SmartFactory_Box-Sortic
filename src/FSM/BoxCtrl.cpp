/**
 * @file BoxCtrl.cpp
 * @brief Implementation of the Box Controll-Class
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Implementation Box Controll FSM - Luca Mazzoleni (luca.mazzoleni@hsr.ch) - 2019-04-23
 * 
 * @date 2019-04-23
 * @copyright Copyright (c) 2019
 * 
 */
#include "BoxCtrl.h"
//=====PUBLIC====================================================================================
BoxCtrl::BoxCtrl() : currentState(State::readSensorVal) {
    DBFUNCCALLln("BoxCtrl::BoxCtrl()");
    delay(100);
    publishState(currentState);
    pComm.unsubscribe("#");
    pComm.subscribe("Box/" + String(box.id) + "/error");
    pComm.subscribe("Box/error");
    pComm.subscribe("error");
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
    DBEVENTln(String("BoxCtrl ") + decodeEvent(e));
    switch (currentState) {
        case State::readSensorVal:
            if (Event::SBFull == e) {
                exitAction_readSensorVal();  // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_readSensorVal();  // Exit-action current state
                entryAction_errorState();    // Entry-actions next state
            }
            break;
        case State::publishLevel:
            if (Event::AnswerReceived == e) {
                exitAction_publishLevel();          // Exit-action current state
                entryAction_calculateOptVehicle();  // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_publishLevel();  // Exit-action current state
                entryAction_errorState();   // Entry-actions next state
            }
            break;
        case State::calculateOptVehicle:
            if (Event::CalcOptVal == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_publishOptVehicle();   // Entry-actions next state
            } else if (Event::NoAnswerReceived == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_publishLevel();        // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_errorState();          // Entry-actions next state
            }
            break;
        case State::publishOptVehicle:
            if (Event::AnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_waitForAck();        // Entry-actions next state
            } else if (Event::NoAnswerReceived == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_publishLevel();      // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_publishOptVehicle();  // Exit-action current state
                entryAction_errorState();        // Entry-actions next state
            }
            break;
        case State::waitForAck:
            if (Event::AnswerReceived == e) {
                exitAction_waitForAck();         // Exit-action current state
                entryAction_waitForTransport();  // Entry-actions next state
            } else if (Event::NoAnswerReceived == e) {
                exitAction_waitForAck();     // Exit-action current state
                entryAction_publishLevel();  // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_waitForAck();   // Exit-action current state
                entryAction_errorState();  // Entry-actions next state
            }
            break;
        case State::waitForTransport:
            if (Event::SBReady == e) {
                exitAction_waitForTransport();  // Exit-action current state
                entryAction_readSensorVal();    // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_waitForTransport();  // Exit-action current state
                entryAction_errorState();       // Entry-actions next state
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
    DBSTATUSln("Entering State: emptyState");
    currentState = State::readSensorVal;  // state transition
    doActionFPtr = &BoxCtrl::doAction_readSensorVal;
    publishState(currentState);  //Update Current State and Publish
}

BoxCtrl::Event BoxCtrl::doAction_readSensorVal() {
    DBINFO1ln("State: emptyState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    pBoxlevelctrl.loop(BoxLevelCtrl::Event::CheckForPackage);
    if (BoxLevelCtrl::State::fullState == pBoxlevelctrl.getcurrentState()) {
        return Event::SBFull;
    }

    return Event::NoEvent;
}

void BoxCtrl::exitAction_readSensorVal() {
    DBSTATUSln("Leaving State: emptyState");
}

//==publishLevel========================================================
void BoxCtrl::entryAction_publishLevel() {
    DBSTATUSln("Entering State: publishLevel");
    currentState = State::publishLevel;  // state transition
    doActionFPtr = &BoxCtrl::doAction_publishLevel;
    publishState(currentState);  //Update Current State and Publish

    previousMillis = millis();
    currentMillis = millis();
    //Subscribe to Topics
    pComm.subscribe("Vehicle/+/available");
    // pComm.unsubscribe("Vehicle/+/handshake");
    // pComm.unsubscribe("Vehicle/V1/handshake");
    pComm.unsubscribe("Vehicle/" + String(box.req) + "/handshake");
    //Update Box Infos
    box.ack = "null";
    box.req = "null";
}

BoxCtrl::Event BoxCtrl::doAction_publishLevel() {
    DBINFO1ln("State: publishLevel");
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    //wait time or max response
    currentMillis = millis();
    if ((NUM_OF_MAXVALUES_VEHICLES_STORE + 1 < pComm.size()) ||
        (((currentMillis - previousMillis) > WaitForResponsesInMillis) && 0 < pComm.size())) {
        return Event::AnswerReceived;
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_publishLevel() {
    DBSTATUSln("Leaving State: publishLevel");
    pComm.unsubscribe("Vehicle/+/available");
}

//==calculateOptVehicle========================================================
void BoxCtrl::entryAction_calculateOptVehicle() {
    DBSTATUSln("Entering State: calculateOptVehicle");
    currentState = State::calculateOptVehicle;  // state transition
    doActionFPtr = &BoxCtrl::doAction_calculateOptVehicle;
    publishState(currentState);  //Update Current State and Publish
}

BoxCtrl::Event BoxCtrl::doAction_calculateOptVehicle() {
    DBINFO1ln("State: calculateOptVehicle");
    //Check all saved Messages and save the best Vehicles to an Array
    //Generate the Event
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    DBINFO3ln(String("BuffSize: ") + String(pComm.size()));
    int size = pComm.size();
    // myJSONStr nearestVehicleStr = pComm.getElement(0);
    myJSONStr nearestVehicleStr = pComm.pop();
    myJSONStr temp;

    for (int i = 1; i < size; i++) {
        // temp = pComm.getElement(i);
        temp = pComm.pop();
        DBINFO3ln(decodeSector(box.actualSector) + String("==") + temp.sector);
        if (decodeSector(box.actualSector) == temp.sector) {  //If actual sector same as sector from vehicle
            DBINFO3ln(String(abs(temp.line - box.actualLine)) + String(" < ") + String(abs(nearestVehicleStr.line - box.actualLine)));
            if (abs(temp.line - box.actualLine) < abs(nearestVehicleStr.line - box.actualLine)) {
                nearestVehicleStr = temp;
            }
        }
    }
    if (nearestVehicleStr.line != 0 && decodeSector(nearestVehicleStr.sector) != BoxCtrl::Sector::error) {
        box.req = nearestVehicleStr.id;
        return Event::CalcOptVal;
    }

    return Event::NoAnswerReceived;
    // return
}

void BoxCtrl::exitAction_calculateOptVehicle() {
    DBSTATUSln("Leaving State: calculateOptVehicle");
    // pComm.clear();
}

//==publishOptVehicle========================================================
void BoxCtrl::entryAction_publishOptVehicle() {
    DBSTATUSln("Entering State: publishOptVehicle");
    currentState = State::publishOptVehicle;  // state transition
    doActionFPtr = &BoxCtrl::doAction_publishOptVehicle;
    publishState(currentState);  //Update Current State and Publish
    //Subscribe to Topics
    pComm.subscribe("Vehicle/" + String(box.req) + "/handshake");
    previousMillis = millis();
    currentMillis = millis();
}

BoxCtrl::Event BoxCtrl::doAction_publishOptVehicle() {
    DBINFO1ln("State: publishOptVehicle");
    currentMillis = millis();

    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    //Publish decision
    if ((currentMillis - previousMillis) > TimeBetweenPublish) {  //only publish all xx seconds
        pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"id\":\"" + String(box.id) + "\",\"req\":\"" + String(box.req) + "\"}");
    }

    //Wait for response
    if ((currentMillis - previousMillis) < SMARTBOX_WAITFOR_VEHICLES_SECONDS * 1000) {
        if (!pComm.isEmpty()) {
            myJSONStr temp = pComm.pop();
            if (temp.req == box.id && temp.id == box.req) {
                box.ack = temp.id;
                return Event::AnswerReceived;
            }
        }
    } else {
        return Event::NoAnswerReceived;
    }

    //Generate the Event
    return Event::NoEvent;
}

void BoxCtrl::exitAction_publishOptVehicle() {
    DBSTATUSln("Leaving State: publishOptVehicle");
}

//==waitForAck========================================================
void BoxCtrl::entryAction_waitForAck() {
    DBSTATUSln("Entering State: waitForAck");
    currentState = State::waitForAck;  // state transition
    doActionFPtr = &BoxCtrl::doAction_waitForAck;
    publishState(currentState);  //Update Current State and Publish
    previousMillis = millis();
    currentMillis = millis();
}

BoxCtrl::Event BoxCtrl::doAction_waitForAck() {
    DBINFO1ln("State: waitForAck");
    currentMillis = millis();
    //read responses
    //check for ack from vehicle
    //Generate the Event
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    if ((currentMillis - previousMillis) > TimeBetweenPublish) {  //only publish all xx seconds
        pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"id\":\"" + String(box.id) + "\",\"ack\":\"" + String(box.ack) + "\"}");
    }

    //Wait for response
    if ((currentMillis - previousMillis) < SMARTBOX_ITERATION_VACKS_SECONDS * 1000) {
        DBINFO2ln("Wait for response");
        if (!pComm.isEmpty()) {
            myJSONStr temp = pComm.pop();
            DBINFO2ln(String("temp.ack: ") + String(temp.ack) + String("==") + String("box.id: ") + String(box.id));
            DBINFO2ln(String("temp.id: ") + String(temp.id) + String("==") + String("box.ack: ") + String(box.ack));
            if ((temp.ack == box.id) && (temp.id == box.ack)) {
                return Event::AnswerReceived;
            }
        }
    } else {
        return Event::NoAnswerReceived;
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_waitForAck() {
    DBSTATUSln("Leaving State: waitForAck");
    // pComm.unsubscribe("Vehicle/+/handshake");
    pComm.unsubscribe("Vehicle/" + String(box.req) + "/handshake");
}

//==waitForTransport===================================================
void BoxCtrl::entryAction_waitForTransport() {
    DBSTATUSln("Entering State: waitForTransport");
    currentState = State::waitForTransport;  // state transition
    doActionFPtr = &BoxCtrl::doAction_waitForTransport;
    publishState(currentState);  //Update Current State and Publish
    pComm.subscribe("Box/" + String(box.id) + "/position");
}

BoxCtrl::Event BoxCtrl::doAction_waitForTransport() {
    DBINFO1ln("State: waitForTransport");
    //Generate the Event
    //check message for transportconfirmation
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }
    if (!pComm.isEmpty()) {
        myJSONStr temp = pComm.pop();
        DBINFO2ln(String("temp.id: ") + String(temp.id) + String("==") + String("box.ack: ") + String(box.ack));
        DBINFO2ln(String("temp.sector: ") + String(temp.sector) + String("/") + String("temp.line: ") + String(temp.line));
        Sector sector = decodeSector(temp.sector);
        if ((temp.id == box.ack) && (sector != BoxCtrl::Sector::error) && (temp.line != 0)) {
            box.actualLine = temp.line;
            box.actualSector = sector;
            return Event::SBReady;
        }
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_waitForTransport() {
    DBSTATUSln("Leaving State: waitForTransport");
    //unsubscribe
    pComm.unsubscribe("Box/" + String(box.id) + "/position");
}

//==errorState========================================================
void BoxCtrl::entryAction_errorState() {
    DBERROR("Entering State: errorState");
    lastStateBevorError = currentState;
    currentState = State::errorState;  // state transition
    doActionFPtr = &BoxCtrl::doAction_errorState;
    publishState(currentState);  //Update Current State and Publish
    pComm.clear();
}

BoxCtrl::Event BoxCtrl::doAction_errorState() {
    DBINFO1ln("State: errorState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
                   // if (!pComm.isEmpty()) {
    while (!pComm.isEmpty()) {
        // if (!pComm.first().error) {
        if (!pComm.pop().error) {
            // pComm.shift();
            return Event::Resume;
        }
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_errorState() {
    DBSTATUSln("Leaving State: errorState");
    pComm.clear();
}

//============================================================================
//==Aux-Function==============================================================
String BoxCtrl::decodeState(State state) {
    switch (state) {
        case State::readSensorVal:
            return "readSensorVal";
            break;
        case State::publishLevel:
            return "publishLevel";
            break;
        case State::calculateOptVehicle:
            return "calculateOptVehicle";
            break;
        case State::publishOptVehicle:
            return "publishOptVehicle";
            break;
        case State::waitForAck:
            return "waitForAck";
            break;
        case State::waitForTransport:
            return "waitForTransport";
            break;
        case State::errorState:
            return "errorState";
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

String BoxCtrl::decodeSector(Sector sector) {
    switch (sector) {
        case Sector::SorticHandover:
            return "SorticHandover";
            break;
        case Sector::SorticToHandover:
            return "SorticHandover";
            break;
        case Sector::SorticWaitForGateway:
            return "SorticWaitForGateway";
            break;
        case Sector::SorticGateway:
            return "SorticGateway";
            break;
        case Sector::TransitWaitForGatewaySortic:
            return "TransitWaitForGatewaySortic";
            break;
        case Sector::TransitToSortic:
            return "TransitToSortic";
            break;
        case Sector::TransitToTransfer:
            return "TransitToTransfer";
            break;
        case Sector::Parking:
            return "Parking";
            break;
        case Sector::TransitWaitForGatewayTransfer:
            return "TransitWaitForGatewayTransfer";
            break;
        case Sector::TransferGateway:
            return "TransferGateway";
            break;
        case Sector::TransferWaitForGateway:
            return "TransferWaitForGateway";
            break;
        case Sector::TransferToHandover:
            return "TransferHandover";
            break;
        case Sector::TransferHandover:
            return "TransferHandover";
            break;

        default:
            return "ERROR: No matching sector";
            break;
    }
}

BoxCtrl::Sector BoxCtrl::decodeSector(String sector) {
    if (String("SorticHandover") == sector) {
        return Sector::SorticHandover;
    } else if (String("SorticToHandover") == sector) {
        return Sector::SorticToHandover;
    } else if (String("SorticWaitForGateway") == sector) {
        return Sector::SorticWaitForGateway;
    } else if (String("TransitWaitForGatewaySortic") == sector) {
        return Sector::TransitWaitForGatewaySortic;
    } else if (String("TransitToSortic") == sector) {
        return Sector::TransitToSortic;
    } else if (String("TransitToTransfer") == sector) {
        return Sector::TransitToTransfer;
    } else if (String("Parking") == sector) {
        return Sector::Parking;
    } else if (String("TransitWaitForGatewayTransfer") == sector) {
        return Sector::TransitWaitForGatewayTransfer;
    } else if (String("TransferGateway") == sector) {
        return Sector::TransferGateway;
    } else if (String("TransferWaitForGateway") == sector) {
        return Sector::TransferWaitForGateway;
    } else if (String("TransferToHandover") == sector) {
        return Sector::TransferToHandover;
    } else if (String("TransferHandover") == sector) {
        return Sector::TransferHandover;
    } else {
        return Sector::error;
    }
}

bool BoxCtrl::checkForError() {
    DBFUNCCALLln("BoxCtrl::checkForError()");
    if (!pComm.isEmpty()) {
        DBINFO3ln(pComm.size());
        DBINFO3ln(pComm.last().error);
        if (pComm.first().error) {
            pComm.shift();
            return true;
        }
    }
    return false;
}

void BoxCtrl::publishState(State state) {
    box.status = decodeState(state);
    pComm.publishMessage("Box/" + String(box.id) + "/status", "{\"status\":\"" + String(box.status) + "\"}");
}