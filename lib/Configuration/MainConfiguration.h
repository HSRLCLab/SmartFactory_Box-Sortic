#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

/*
    file that contains the default configuration for the main function
    Note:
        SENSOR_* is concerning the sensor
        *ITERATION* is an iteration variable (e.g- for loops)
        *SECONDS* is stating, that the value is given in seconds and will be recalculated for milliseconds (for func delay())
*/

#define SENSOR_ITERATION_SECONDS 10 // 
//#define SMARTBOX_WAITFOR_VEHICLES_SECONDS 10
#define SMARTBOX_WAITFOR_VEHICLES_TURNS 10  // SB waits this amount of turns for vehicles to respond
//#define SMARTBOX_ITERATION_VACKS_SECONDS 10
#define SMARTBOX_ITERATION_VACKS_TURNS 10
//#define SMARTBOX_ITERATION_VTRANSPORTS_SECONDS 10
#define SMARTBOX_ITERATION_VTRANSPORTS_TURNS 10 // SB waits this amount of turns for vehicles to respond
#define SMARTBOX_WAITFOR_ANSWERS_SECONDS 10
//#define MAX_MQTT_TOPIC_DEPTH 5
#define NUM_OF_VEHICLES_IN_FIELD 2        // TODO erkennen?
#define NUM_OF_MAXVALUES_VEHICLES_STORE 2 // how much best values can be stored, must be smaller than NUM_OF_VEHICLES_IN_FIELD!

// TODO: alle Werte beschreiben

#endif