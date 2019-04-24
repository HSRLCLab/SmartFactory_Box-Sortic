/**
 * @file BoxLevelCtrl.h
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

#ifndef BOXLEVELCTRL_H__
#define BOXLEVELCTRL_H__

#include "SensorArray.h"

class BoxLevelCtrl {
    //=====PUBLIC====================================================================================
   public:
    /**
    * @brief Enum holds all possible events
    * 
    */
    enum class Event { PackageDetected,    ///< Package detected
                       NoPackageDetected,  ///< No package detected
                       Error,              ///< Error occured
                       Resume,             ///< Resume after Error occured
                       NoEvent             ///< No event generated
    };

    /**
    * @brief Enum holds all possible states
    * 
    */
    enum class State { emptyState,  ///< empty State
                       fullState,   ///< full State
                       errorState
    };
    /**
     * @brief Construct a new Box Level Ctrl object
     * and initailize the currentState with emptyState
     * 
     */
    BoxLevelCtrl();

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

    /**
     * @brief Get the current State
     * 
     * @return State - current State
     */
    const State getcurrentState();

    //=====PRIVATE====================================================================================
   private:
    State lastStateBevorError;  ///< holds the last state of the FSM so it's possible to resume after error
    State currentState;         ///< holds the current state of the FSM
    Event currentEvent;         ///< holds the current event of the FSM

    /**
     * @brief Functionpointer to call the current states do-function
     * 
     * https://stackoverflow.com/questions/1485983/calling-c-class-methods-via-a-function-pointer
     */
    Event (BoxLevelCtrl::*doActionFPtr)(void) = &BoxLevelCtrl::doAction_emptyState;

    SensorArray pSensorArray;  ///< SensorArray Object

    //=====StateFunctions=====
    //=====emptyState==========================================================
    /**
     * @brief executes the entry action of the emptyState
     * 
     * Turns on the Loadindicator-LED
     * 
     */
    void entryAction_emptyState();

    /**
     * @brief executes the main action of the emptyState
     * 
     * checks if the sensor has a package detected and generates the appropriate event
     * 
     * @return BoxLevelCtrl::Event - generated Event
     */
    BoxLevelCtrl::Event doAction_emptyState();

    /**
     * @brief executes the exit action of the emptyState
     * 
     */
    void exitAction_emptyState();

    //=====fullState==========================================================
    /**
     * @brief executes the entry action of the fullState 
     * 
     * Turns off the Loadindicator-LED
     */
    void entryAction_fullState();

    /**
     * @brief executes the main action of the fullState
     * 
     * checks if the sensor has a package detected and generates the appropriate event
     * 
     * @return BoxLevelCtrl::Event - generated Event
     */
    BoxLevelCtrl::Event doAction_fullState();

    /**
     * @brief executes the exit action of the fullState
     * 
     */
    void exitAction_fullState();

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
    BoxLevelCtrl::Event doAction_errorState();

    /**
     * @brief exit action of the errorState
     * 
     */
    void exitAction_errorState();
};
#endif
