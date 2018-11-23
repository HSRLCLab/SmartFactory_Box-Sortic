#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

/*
    file that contains the default configuration for the main function
    Note:
        SENSOR_* is concerning the sensor
        *ITERATION* is an iteration variable (e.g- for loops)
        *SECONDS* is stating, that the value is given in seconds and will be recalculated for milliseconds (for func delay())
*/

#define SMARTBOX_WAITFOR_VEHICLES_TURNS 3      // SB waits this amount of turns for vehicles to respond
#define SMARTBOX_ITERATION_VACKS_TURNS 3       // SB wait for vehicles to send their acknoledgement to transport SB
#define SMARTBOX_ITERATION_VTRANSPORTS_TURNS 3 // SB waits this amount of turns for vehicles to respond
#define NUM_OF_VEHICLES_IN_FIELD 2             // maximum Number of vehicles in Field, TODO erkennen?
#define NUM_OF_MAXVALUES_VEHICLES_STORE 2      // how much best values can be stored, must be smaller than NUM_OF_VEHICLES_IN_FIELD!
#define PIN_FOR_FULL 12                        // if SmartBox is full, light up LED

#endif