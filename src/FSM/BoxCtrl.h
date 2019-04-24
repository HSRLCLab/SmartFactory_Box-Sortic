/**
 * @file BoxCtrl.h
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

#ifndef BOXCTRL_H__
#define BOXCTRL_H__

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include "MainConfiguration.h"

#include "BoxLevelCtrl.h"
#include "MQTTTasks.h"
#include "NetworkManager.h"
#include "NetworkManagerStructs.h"

class BoxCtrl {
    //=====PUBLIC====================================================================================
   public:
    /**
    * @brief Enum holds all possible events
    * 
    */
    enum class Event { SBFull,             ///< SB is Full
                       SBReady,            ///< SB is Ready
                       CalcOptVal,         ///< Calculated Optimum Value
                       AnswerReceived,     ///< Answer received
                       NoAnswerReceived,   ///< No Answer received
                       InqVehicRespond,    ///< Inquired Vehicle responded
                       InqNoVehicRespond,  ///< Inquired Vehicle didnt responded
                       NoVehicRespond,     ///< None of the Inquired Vehicle did responded
                       Error,              ///< Error occured
                       Resume,             ///< Resume after Error occured
                       NoEvent             ///< No event generated
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
     * @brief changes the state of the FSM based on the event
     * 
     * @param e - Event
     */
    void process(Event e);

    //=====PRIVATE====================================================================================
   private:
    /**
    * @brief Enum holds all possible state's
    * 
    * https://stackoverflow.com/questions/18335861/why-is-enum-class-preferred-over-plain-enum
    */
    enum class State { readSensorVal,        //loopEmpty()
                       publishLevel,         //publishLevel()
                       calculateOptVehicle,  //getOptimalVehiclefromResponses()
                       publishOptVehicle,    //hasOptVehiclePublish()
                       waitForAck,           //checkIfAckReceivedfromResponses()
                       waitForTransport,     //checkIfTransporedfromResponses()
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
    //  Communication pComm;
    NetworkManager pComm;

    //=====StateFunctions===============================================================
    //==readSensorVal==========================================================
    /**
     * @brief entry action of the readSensorVal
     * 
     */
    void entryAction_readSensorVal();

    /**
     * @brief main action of the readSensorVal
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_readSensorVal();

    /**
     * @brief exit action of the readSensorVal
     * 
     */
    void exitAction_readSensorVal();

    //==publishLevel==========================================================
    /**
     * @brief entry action of the publishLevel
     * 
     */
    void entryAction_publishLevel();

    /**
     * @brief main action of the publishLevel
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_publishLevel();

    /**
     * @brief exit action of the publishLevel
     * 
     */
    void exitAction_publishLevel();

    //==calculateOptVehicle==========================================================
    /**
     * @brief entry action of the calculateOptVehicle
     * 
     */
    void entryAction_calculateOptVehicle();

    /**
     * @brief main action of the calculateOptVehicle
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
     */
    void entryAction_publishOptVehicle();

    /**
     * @brief main action of the publishOptVehicle
     * 
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
     * 
     */
    void entryAction_waitForAck();

    /**
     * @brief main action of the waitForAck
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_waitForAck();

    /**
     * @brief exit action of the waitForAck
     * 
     */
    void exitAction_waitForAck();

    //==waitForTransport==========================================================
    /**
     * @brief entry action of the waitForTransport
     * 
     */
    void entryAction_waitForTransport();

    /**
     * @brief main action of the waitForTransport
     * 
     *  @return BoxLevelCtrl::Event - generated Event
     */
    BoxCtrl::Event doAction_waitForTransport();

    /**
     * @brief exit action of the waitForTransport
     * 
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
};
#endif
