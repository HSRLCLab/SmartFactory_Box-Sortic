/**
 * @file BoxCtrl.h
 * @brief The Box Controll class contains the FSM for the BoxLevel
 * 
 * @author Luca Mazzoleni (luca.mazzoleni@hsr.ch)
 * 
 * @version 1.0 - Implementation Box Controll FSM - Luca Mazzoleni (luca.mazzoleni@hsr.ch) - 2019-04-23
 *  
 * @date 2019-04-03
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BOXCTRL_H__
#define BOXCTRL_H__

#include <Arduino.h>

// own files:
#include "MainConfiguration.h"

#include "BoxLevelCtrl.h"
#include "Communication.h"

/**
 * @brief The Box Controll class contains the FSM for the complete Box
 * 
 * @image html BoxCtrl.png width=800
 */
class BoxCtrl {
    //=====PUBLIC====================================================================================
   public:
    enum class Sector {
        SorticHandover,                 ///< Sortic - Handover
        SorticToHandover,               ///< Sortic - to Handover
        SorticWaitForGateway,           ///< Sortic - Wait for Gateway
        SorticGateway,                  ///< Sortic - Gateway
        TransitWaitForGatewaySortic,    ///< Transit - Wait for Gateway Sortic
        TransitToSortic,                ///< Transit - to Sortic
        TransitToTransfer,              ///< Transit - to Transfer
        Parking,                        ///< Parking (not used atm)
        TransitWaitForGatewayTransfer,  ///< Transit - Wait for Gateway Transfer
        TransferGateway,                ///< Transfer - Gateway
        TransferWaitForGateway,         ///< Transfer - wait for Gateway
        TransferToHandover,             ///< Transfer - to Handover
        TransferHandover,               ///< Transfer - Handover
        error
    };

    struct Box {
        String id = DEFAULT_HOSTNAME;                  ///< Boxname
        Sector actualSector = Sector::SorticHandover;  ///< actual sector initialise with Sortic handover
        int actualLine = DEFAUL_HOSTNAME_NUMBER;                            ///< actual line initialised with 1
        String cargo = "";                         ///< cargo; not used atm
        String status = "null";                        ///< status of the Box FSM
        String ack = "null";                           ///< ack for handshake vehicle
        String req = "null";                           ///< req for handshake vehicle
    } box;

    /**
    * @brief Enum holds all possible events
    * 
    */
    enum class Event { SBReadyForTransport,  ///< SB is ready for transport
                       SBReady,              ///< SB is Ready
                       CalcOptVal,           ///< Calculated Optimum Value
                       AnswerReceived,       ///< Answer received
                       NoAnswerReceived,     ///< No Answer received
                       Error,                ///< Error occured
                       Reset,                ///< Reset after Error occured
                       Resume,               ///< Resume after Error occured
                       NoEvent               ///< No event generated
    };

    /**
     * @brief Construct a new Box Ctrl object
     * 
     */
    BoxCtrl();
    /**
     * @brief Calls the do-function of the active state and hence generates Events
     * 
     */
    void loop();

    /**
     * @brief procceses the current Event and calls the do-function of the active state
     * 
     * @param currentEvent - Event
     */
    void loop(Event currentEvent);

    //=====PRIVATE====================================================================================
   private:
    /**
     * @brief changes the state of the FSM based on the event
     * 
     * @param e - Event
     */
    void process(Event e);

    /**
    * @brief Enum holds all possible state's
    * 
    * https://stackoverflow.com/questions/18335861/why-is-enum-class-preferred-over-plain-enum
    */
    enum class State { readSensorVal,        //loopEmpty()
                       waitForVehicle,       //waitForVehicle()
                       calculateOptVehicle,  //getOptimalVehiclefromResponses()
                       publishOptVehicle,    //hasOptVehiclePublish()
                       waitForAck,           //checkIfAckReceivedfromResponses()
                       waitForTransport,     //checkIfTransporedfromResponses()
                       resetState,
                       errorState };

    State lastStateBevorError;  ///< holds the last state of the FSM so it's possible to resume after error
    State currentState;         ///< holds the current state of the FSM
    Event currentEvent;         ///< holds the current event of the FSM

    /**
     * @brief Functionpointer to call the current states do-function
     * 
     * https://stackoverflow.com/questions/1485983/calling-c-class-methods-via-a-function-pointer
     */
    Event (BoxCtrl::*doActionFPtr)(void) = &BoxCtrl::doAction_readSensorVal;

    BoxLevelCtrl pBoxlevelctrl;
    Communication pComm;
    // NetworkManager pComm;

    // int pMaxLoopCountForMessages = 1;
    // void checkMessages();
    // myJSONStr pTemp;

    unsigned long currentMillis = 0;          ///< will store current time
    unsigned long previousMillis = 0;         ///< will store last time
    unsigned long previousMillisPublish = 0;  ///< will store last publish time
    // unsigned long WaitForResponsesInMillis = 5000;
    // int NUM_OF_MAXVALUES_VEHICLES_STORE = 2;
    // int pVehicleRating[NUM_OF_MAXVALUES_VEHICLES_STORE];
    // int increment = 0;

    // int TimeBetweenPublish = 500;

    //=====StateFunctions===============================================================
    //==readSensorVal==========================================================
    /**
     * @brief entry action of the readSensorVal
     * 
     * publish actual state to "Box/box.id/status"
     */
    void
    entryAction_readSensorVal();

    /**
     * @brief main action of the readSensorVal
     * 
     * call Comm.loop and check for incoming Error-Message \n
     * call BoxLevelCtrl with Event::CheckForPackage \n
     * if BoxLvl is in FullState and at HandoverSortic or in EmptyState and at HandOverTransfer return Event SBReadyForTransport \n
     * 
     * @return BoxCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_readSensorVal();

    /**
     * @brief exit action of the readSensorVal
     * 
     */
    void exitAction_readSensorVal();

    //==waitForVehicle==========================================================
    /**
     * @brief entry action of the waitForVehicle
     * 
     * publish state to "Box/box.id/status". \n
	 * subscribe to "Vehicle/+/available". \n
	 * unsubscribe from "Vehicle/box.req/handshake". \n
	 * reset box.ack and box.req. \n
     */
    void entryAction_waitForVehicle();

    /**
     * @brief main action of the waitForVehicle
     * 
     * call Comm.loop and check for incoming Error-Message. \n
     * wait for SMARTBOX_WAITFOR_VEHICLES_SECONDS seconds and  \n
     * if a message is received or more than NUM_OF_MAXVALUES_VEHICLES_STORE messages \n
     * are recived return Event::AnswerReceived. \n
     * 
     *  @return BoxCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_waitForVehicle();

    /**
     * @brief exit action of the waitForVehicle
     * 
     * unsubscribe from "Vehicle/+/available"
     */
    void exitAction_waitForVehicle();

    //==calculateOptVehicle==========================================================
    /**
     * @brief entry action of the calculateOptVehicle
     * 
     * publish state to "Box/box.id/status"
     */
    void entryAction_calculateOptVehicle();

    /**
     * @brief main action of the calculateOptVehicle
     * 
     * check all saved messages. \n
     * select all vehicle in same sector and choose the nearest one. \n
     * if the nearest one is valid update box.req with id of the nearest vehicle \n
     * and return Event::CalcOptVal. \n
     * if not valid return Event::NoAnswerReceived. \n
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_calculateOptVehicle();

    /**
     * @brief exit action of the calculateOptVehicle
     * 
     */
    void exitAction_calculateOptVehicle();

    //==publishOptVehicle==========================================================
    /**
     * @brief entry action of the publishOptVehicle
     * 
     * publish state to "Box/box.id/status" \n
     * subscribe to "Vehicle/box.req/handshake" \n
     */
    void entryAction_publishOptVehicle();

    /**
     * @brief main action of the publishOptVehicle
     * 
     * call Comm.loop and check for incoming Error-Message. \n
     * publish box.id and id of req vehicle to "Box/box.id/handshake" all TIME_BETWEEN_PUBLISH seconds. \n
     * if requested vehicle also req box in time return Event::AnswerReceived. \n
     *  @return BoxLevelCtrl::Event - generated Event \n
     */
    BoxCtrl::Event doAction_publishOptVehicle();

    /**
     * @brief exit action of the publishOptVehicle
     * 
     */
    void exitAction_publishOptVehicle();

    //==waitForAck==========================================================
    /**
     * @brief entry action of the waitForAck
     * publish state to "Box/box.id/status"
     */
    void entryAction_waitForAck();

    /**
     * @brief main action of the waitForAck
     * 
     * call Comm.loop and check for incoming Error-Message. \n
     * publish box.id and id of ack vehicle to "Box/box.id/handshake". \n
     * wait  SMARTBOX_ITERATION_VACKS_SECONDS for response. \n
     * if acknoledge vehicle also ack box in time return Event::AnswerReceived. \n
     *  
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_waitForAck();

    /**
     * @brief exit action of the waitForAck
     * 
     * unsubscribe from "Vehicle/box.req/handshake"
     * 
     */
    void exitAction_waitForAck();

    //==waitForTransport==========================================================
    /**
     * @brief entry action of the waitForTransport \n
     * publish state to "Box/box.id/status". \n
     * subscribe to "Box/box.id/position". \n
     */
    void entryAction_waitForTransport();

    /**
     * @brief main action of the waitForTransport
     * 
     * call Comm.loop and check for incoming Error-Message. \n
     * check incoming message if ack. vehicle updated box-position \n
     *  @return BoxLevelCtrl::Event - generated Event \n
     */
    BoxCtrl::Event doAction_waitForTransport();

    /**
     * @brief exit action of the waitForTransport
     * 
     * unsubscribe from "Box/box.id/position"
     */
    void exitAction_waitForTransport();

    //==errorState==========================================================
    /**
     * @brief entry action of the errorState
     * 
     */
    void entryAction_errorState();

    /**
     * @brief main action of the errorState
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_errorState();

    /**
     * @brief exit action of the errorState
     * 
     */
    void exitAction_errorState();

    //==resetState==========================================================
    /**
     * @brief entry action of the resetState
     * 
     */
    void entryAction_resetState();

    /**
     * @brief main action of the resetState
     * 
     *  @return BoxCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_resetState();

    /**
     * @brief exit action of the resetState
     * 
     */
    void exitAction_resetState();
    //============================================================================
    //==Aux-Function==============================================================
    /**
     * @brief Decodes the State-Enum and returns a description
     * 
     * @param state - enum State
     * @return String - State as String
     */
    String decodeState(State state);

    /**
     * @brief Decodes the Event-Enum and returns a description
     * 
     * @param event - enum Event
     * @return String - Event as String
     */
    String decodeEvent(Event event);

    /**
     * @brief Decodes the Sector-Enum and returns a description
     * 
     * @param sector - Sector to decode
     * @return String - Sector as String
     */
    String decodeSector(Sector sector);

    /**
     * @brief Decodes the Sector-Enum and returns a description
     * 
     * @param sector - String to decode
     * @return BoxCtrl::Sector - Sector as Sector
     */
    BoxCtrl::Sector decodeSector(String sector);

    void publishPosition();

    void publishState(State state);
    /**
     * @brief 
     * 
     * @return true - 
     * @return false - 
     */
    bool checkForError();

    void clearGui();
};
#endif
