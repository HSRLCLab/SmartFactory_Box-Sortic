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
            if (Event::SBReadyForTransport == e) {
                exitAction_readSensorVal();    // Exit-action current state
                entryAction_waitForVehicle();  // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_readSensorVal();  // Exit-action current state
                entryAction_errorState();    // Entry-actions next state
            }
            break;
        case State::waitForVehicle:
            if (Event::AnswerReceived == e) {
                exitAction_waitForVehicle();        // Exit-action current state
                entryAction_calculateOptVehicle();  // Entry-actions next state
            } else if (Event::Error == e) {
                exitAction_waitForVehicle();  // Exit-action current state
                entryAction_errorState();     // Entry-actions next state
            }
            break;
        case State::calculateOptVehicle:
            if (Event::CalcOptVal == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_publishOptVehicle();   // Entry-actions next state
            } else if (Event::NoAnswerReceived == e) {
                exitAction_calculateOptVehicle();  // Exit-action current state
                entryAction_waitForVehicle();      // Entry-actions next state
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
                entryAction_waitForVehicle();    // Entry-actions next state
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
                exitAction_waitForAck();       // Exit-action current state
                entryAction_waitForVehicle();  // Entry-actions next state
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
                    case State::waitForVehicle:
                        entryAction_waitForVehicle();  // Entry-actions next state
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
            if (Event::Reset == e) {
                exitAction_errorState();   // Exit-action current state
                entryAction_resetState();  // Entry-actions next state
            }
            break;
        case State::resetState:
            if (Event::Resume == e) {
                exitAction_resetState();      // Exit-action current state
                entryAction_readSensorVal();  // Entry-actions next state
            }
            break;
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
    publishPosition();
    if (box.actualSector == BoxCtrl::Sector::SorticHandover) {
        pComm.subscribe("Sortic/Handover");
    }
    previousMillis = millis();
    previousMillisPublish = previousMillis;
    currentMillis = previousMillis;
}

BoxCtrl::Event BoxCtrl::doAction_readSensorVal() {
    DBINFO1ln("State: emptyState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    currentMillis = millis();
    // Publish/Block HandoverPosition
    if ((currentMillis - previousMillisPublish) > TIME_BETWEEN_PUBLISH) {  //only publish all xx seconds
        previousMillisPublish = millis();
        if (box.actualSector == Sector::SorticHandover) {
            pComm.publishMessage("Sortic/Handover", "{\"id\":\"" + String(box.id) + "\",\"line\":\"" + String(box.actualLine) + "\"}");
        } else if (box.actualSector == Sector::TransferHandover) {
            pComm.publishMessage("Transfer/Handover", "{\"id\":\"" + String(box.id) + "\",\"line\":\"" + String(box.actualLine) + "\"}");
        }
    }

    pBoxlevelctrl.loop(BoxLevelCtrl::Event::CheckForPackage);
    if ((BoxLevelCtrl::State::fullState == pBoxlevelctrl.getcurrentState() &&
         box.actualSector == BoxCtrl::Sector::SorticHandover)) {
        if (!pComm.isEmpty()) {
            myJSONStr temp = pComm.pop();
            //Check if message is from req vehicle and if vehicle does req the correct box
            if ((temp.line == box.actualLine) && (temp.id = String("Sortic")) && (temp.cargo.length() != 0)&& (temp.cargo!= String("null"))) {
                box.cargo = temp.cargo;
                pComm.publishMessage("Box/" + String(box.id) + "/cargo", "{\"cargo\":\"" + String(box.cargo) + "\"}");
                return Event::SBReadyForTransport;
            }
        }

    } else if ((BoxLevelCtrl::State::emptyState == pBoxlevelctrl.getcurrentState() &&
                box.actualSector == BoxCtrl::Sector::TransferHandover)) {
        box.cargo = String("Empty");
        pComm.publishMessage("Box/" + String(box.id) + "/cargo", "{\"cargo\":\"Empty\"}");
        return Event::SBReadyForTransport;
    }

    return Event::NoEvent;
}

void BoxCtrl::exitAction_readSensorVal() {
    DBSTATUSln("Leaving State: emptyState");
    pComm.unsubscribe("Sortic/Handover");
}

//==waitForVehicle========================================================
void BoxCtrl::entryAction_waitForVehicle() {
    DBSTATUSln("Entering State: waitForVehicle");
    currentState = State::waitForVehicle;  // state transition
    doActionFPtr = &BoxCtrl::doAction_waitForVehicle;
    publishState(currentState);  //Update Current State and Publish

    previousMillis = millis();
    previousMillisPublish = previousMillis;
    currentMillis = previousMillis;
    //Subscribe to Topics
    pComm.subscribe("Vehicle/+/available");
    // pComm.unsubscribe("Vehicle/+/handshake");
    // pComm.unsubscribe("Vehicle/V1/handshake");
    pComm.unsubscribe("Vehicle/" + String(box.req) + "/handshake");
    pComm.clear();
    //Update Box Infos
    box.ack = "null";
    box.req = "null";
}

BoxCtrl::Event BoxCtrl::doAction_waitForVehicle() {
    DBINFO1ln("State: waitForVehicle");
    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    //wait time or max response
    currentMillis = millis();
    if ((NUM_OF_MAXVALUES_VEHICLES_STORE + 1 < pComm.size()) ||
        (((currentMillis - previousMillis) > SMARTBOX_WAITFOR_VEHICLES_SECONDS * 1000) && 0 < pComm.size())) {
        return Event::AnswerReceived;
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_waitForVehicle() {
    DBSTATUSln("Leaving State: waitForVehicle");
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
    // pComm.loop();  //Check for new Messages
    // if (checkForError()) {
    //     return Event::Error;
    // }

    DBINFO3ln(String("BuffSize: ") + String(pComm.size()));
    int size = pComm.size();
    // myJSONStr nearestVehicleStr = pComm.getElement(0);
    myJSONStr nearestVehicleStr = pComm.pop();
    myJSONStr temp;

    for (int i = 1; i < size; i++) {
        // temp = pComm.getElement(i);
        temp = pComm.pop();
        DBINFO3ln(decodeSector(box.actualSector) + String("==") + temp.sector);
        //Check if actual sector same as sector from vehicle
        if (decodeSector(box.actualSector) == temp.sector) {
            DBINFO3ln(String(abs(temp.line - box.actualLine)) + String(" < ") + String(abs(nearestVehicleStr.line - box.actualLine)));
            //Check if this vehicle is nearer than the actual near vehicle
            if (abs(temp.line - box.actualLine) < abs(nearestVehicleStr.line - box.actualLine)) {
                //load nearestVehcile with actual vehicle if necessary
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
    previousMillisPublish = previousMillis;
    currentMillis = previousMillis;
}

BoxCtrl::Event BoxCtrl::doAction_publishOptVehicle() {
    DBINFO1ln("State: publishOptVehicle");

    pComm.loop();  //Check for new Messages
    if (checkForError()) {
        return Event::Error;
    }

    //Publish decision
    currentMillis = millis();
    if ((currentMillis - previousMillisPublish) > TIME_BETWEEN_PUBLISH) {  //only publish all xx seconds
        previousMillisPublish = millis();
        pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"id\":\"" + String(box.id) + "\",\"req\":\"" + String(box.req) + "\"}");
    }

    //Wait for response
    if ((currentMillis - previousMillis) < SMARTBOX_ITERATION_VACKS_SECONDS * 1000) {
        if (!pComm.isEmpty()) {
            myJSONStr temp = pComm.pop();
            //Check if message is from req vehicle and if vehicle does req the correct box
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
    previousMillisPublish = previousMillis;
    currentMillis = previousMillis;
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

    if ((currentMillis - previousMillisPublish) > TIME_BETWEEN_PUBLISH) {  //only publish all xx seconds
        previousMillisPublish = millis();
        // pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"id\":\"" + String(box.id) + "\",\"ack\":\"" + String(box.ack) + "\"}");
        pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"id\":\"" + String(box.id) + "\",\"ack\":\"" + String(box.ack) + "\",\"sector\":\"" + decodeSector(box.actualSector) + "\",\"line\":\"" + String(box.actualLine) + "\",\"cargo\":\"" + String(box.cargo) + "\"}");
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
    pComm.publishMessage("Box/" + String(box.id) + "/position", "{\"id\":\"" + String(box.id) + "\",\"sector\":\"" + String(box.ack) + "\",\"line\":\"" + String(box.ack) + "\"}");
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
        Sector tempsector = decodeSector(temp.sector);
        //Check for valid message form ack vehicle
        if ((temp.id == box.ack) && (tempsector != BoxCtrl::Sector::error) && (temp.line != 0)) {
            box.actualLine = temp.line;
            box.actualSector = tempsector;
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
    while (!pComm.isEmpty()) {
        // if (!pComm.first().error) {
        myJSONStr temp = pComm.pop();
        if (!temp.error && !temp.token) {
            // pComm.shift();
            return Event::Resume;
        } else if (temp.error && temp.token) {
            // pComm.shift();
            return Event::Reset;
        }
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_errorState() {
    DBSTATUSln("Leaving State: errorState");
    pComm.clear();
}

//==resetState========================================================
void BoxCtrl::entryAction_resetState() {
    DBERROR("Entering State: resetState");
    lastStateBevorError = currentState;
    currentState = State::resetState;  // state transition
    doActionFPtr = &BoxCtrl::doAction_resetState;
    publishState(currentState);  //Update Current State and Publish
    pComm.clear();
}

BoxCtrl::Event BoxCtrl::doAction_resetState() {
    DBINFO1ln("State: resetState");
    //Generate the Event
    pComm.loop();  //Check for new Messages
    while (!pComm.isEmpty()) {
        myJSONStr temp = pComm.pop();
        if (!temp.error && !temp.token) {
            return Event::Resume;
        }
    }
    return Event::NoEvent;
}

void BoxCtrl::exitAction_resetState() {
    DBSTATUSln("Leaving State: resetState");
    pComm.clear();
    clearGui();
    // box = Box();  //reset struct
    box = {};  //reset struct
}

//============================================================================
//==Aux-Function==============================================================
String BoxCtrl::decodeState(State state) {
    switch (state) {
        case State::readSensorVal:
            return "readSensorVal";
            break;
        case State::waitForVehicle:
            return "waitForVehicle";
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
        case State::resetState:
            return "resetState";
            break;
        default:
            return "ERROR: No matching state";
            break;
    }
}

String BoxCtrl::decodeEvent(Event event) {
    switch (event) {
        case Event::SBReadyForTransport:
            return "Event::SBReadyForTransport";
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
        case Event::Error:
            return "Event::Error";
            break;
        case Event::Resume:
            return "Event::Resume";
            break;
        case Event::Reset:
            return "Event::Reset";
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

void BoxCtrl::publishPosition() {
    pComm.publishMessage("Box/" + String(box.id) + "/position", "{\"sector\":\"" + decodeSector(box.actualSector) + "\",\"line\":\"" + String(box.actualLine) + "\"}");
}

void BoxCtrl::clearGui() {
    pComm.publishMessage("Box/" + String(box.id) + "/status", "{\"status\":\"\"}");
    pComm.publishMessage("Box/" + String(box.id) + "/position", "{\"sector\":\"\",\"line\":\"\"}");
    pComm.publishMessage("Box/" + String(box.id) + "/cargo", "{\"cargo\":\"\"}");
    pComm.publishMessage("Box/" + String(box.id) + "/handshake", "{\"ack\":\"\",\"req\":\"\"}");
}