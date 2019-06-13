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
#include "MQTTCommunication.h"

/**
 * @brief The Box Controll class contains the FSM for the complete Box
 * 
 * @image html BoxCtrl.png width=800
 */
class BoxCtrl {
    //=====PUBLIC====================================================================================
   public:
    /**
    * @brief Sector class holds all possible position on the gametable
    * 
    */
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
        error                           ///< Error - Used to throw parseerror
    };

    /**
     * @brief Box class contains all relevant information about the box and its actual state
     * 
     */
    struct Box {
        String id = DEFAULT_HOSTNAME;                    ///< Boxname / Hostname of the Box
        // Sector actualSector = Sector::TransferHandover;  ///< actual sector initialise with Sortic handover
      Sector actualSector = Sector::SorticHandover;  ///< actual sector initialise with Sortic handover
        int actualLine = DEFAUL_HOSTNAME_NUMBER;         ///< actual line
        String cargo = "";                               ///< cargo;
        String status = "null";                          ///< status of the Box FSM
        String ack = "null";                             ///< ack for handshake vehicle
        String req = "null";                             ///< req for handshake vehicle
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
    enum class State { readSensorVal,        ///< read fill level
                       waitForVehicle,       ///< wait for available vehicle
                       calculateOptVehicle,  ///< Calculate desired vehicle
                       publishOptVehicle,    ///< Publishe desired vehicle
                       waitForAck,           ///< Wait for ack. from vehicle
                       waitForTransport,     ///< Wait for Transport from vehicle
                       resetState,           ///< Reset state
                       errorState            ///< Error state
    };

    State lastStateBevorError;  ///< holds the last state of the FSM so it's possible to resume after error
    State currentState;         ///< holds the current state of the FSM
    Event currentEvent;         ///< holds the current event of the FSM

    /**
     * @brief Functionpointer to call the current states do-function
     * 
     * https://stackoverflow.com/questions/1485983/calling-c-class-methods-via-a-function-pointer
     */
    Event (BoxCtrl::*doActionFPtr)(void) = nullptr;

    BoxLevelCtrl pBoxlevelctrl;                             ///< instance ot BoxLevelCtrl
    Communication pComm = Communication(DEFAULT_HOSTNAME);  ///< instance of Communication with Box.id/hostname

    unsigned long currentMillis = 0;          ///< store current time
    unsigned long previousMillis = 0;         ///< store last time
    unsigned long previousMillisPublish = 0;  ///< store last publish time

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
     * - call Comm.loop and check for incoming Error-Message
     * - call BoxLevelCtrl with Event::CheckForPackage
     * - if BoxLvl is in FullState and at HandoverSortic or in EmptyState and at HandOverTransfer
     *    - return Event SBReadyForTransport
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
     * - publish state to "Box/box.id/status".
	 * - subscribe to "Vehicle/+/available".
	 * - unsubscribe from "Vehicle/box.req/handshake".
	 * - reset box.ack and box.req.
     */
    void entryAction_waitForVehicle();

    /**
     * @brief main action of the waitForVehicle
     * 
     * - call Comm.loop and check for incoming Error-Message.
     * - wait for SMARTBOX_WAITFOR_VEHICLES_SECONDS seconds 
     *     - if a message is received or more than NUM_OF_MAXVALUES_VEHICLES_STORE messages are recived
     *         - return Event::AnswerReceived.
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
     * - check all saved messages.
     * - select all vehicle in same sector and choose the nearest one.
     * - if the nearest one is valid update box.req with id of the nearest vehicle.
     *      - return Event::CalcOptVal.
     * - if not valid
     *      - return Event::NoAnswerReceived.
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
     * - publish state to "Box/box.id/status" 
     * - subscribe to "Vehicle/box.req/handshake" 
     */
    void entryAction_publishOptVehicle();

    /**
     * @brief main action of the publishOptVehicle
     * 
     * - call Comm.loop and check for incoming Error-Message. 
     * - publish box.id and id of req vehicle to "Box/box.id/handshake" all TIME_BETWEEN_PUBLISH seconds.
     * - if requested vehicle also req box in time return Event::AnswerReceived.
     *  @return BoxLevelCtrl::Event - generated Event
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
     * - call Comm.loop and check for incoming Error-Message.
     * - publish box.id and id of ack vehicle to "Box/box.id/handshake".
     * - wait  SMARTBOX_ITERATION_VACKS_SECONDS for responde.
     * - if acknoledge vehicle also ack box in time
     *      - return Event::AnswerReceived.
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
     * @brief entry action of the waitForTransport
     * 
     * - publish state to "Box/box.id/status".
     * - subscribe to "Box/box.id/position".
     */
    void entryAction_waitForTransport();

    /**
     * @brief main action of the waitForTransport
     * 
     * - call Comm.loop and check for incoming Error-Message.
     * - check incoming message if ack. vehicle updated box-position
     *  @return BoxLevelCtrl::Event - generated Event
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

    /**
     * @brief publish actual position via communication
     * 
     */
    void publishPosition();

    /**
     * @brief Publish actual state via communication
     * 
     * @param state - State of type enum State
     */
    void publishState(State state);

    /**
     * @brief Check if the last message was an error
     * 
     * @return true - if the last message was an error
     * @return false - if the last message wasn't an error
     */
    bool checkForError();

    /**
     * @brief clear all box related gui entrys
     * 
     */
    void clearGui();
};
#endif
