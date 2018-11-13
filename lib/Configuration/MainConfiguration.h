#ifndef MAINCONFIGURATION_H
#define MAINCONFIGURATION_H

/*
    file that contains the default configuration for the main function
    Note:
        SENSOR_* is concerning the sensor
        *ITERATION* is an iteration variable (e.g- for loops)
        *SECONDS* is stating, that the value is given in seconds and will be recalculated for milliseconds (for func delay())
*/

#define SENSOR_ITERATION_SECONDS 2
#define SMARTBOX_WAITFOR_VEHICLES_SECONDS 2
#define SMARTBOX_ITERATION_VACKS_SECONDS 2
#define SMARTBOX_ITERATION_VTRANSPORTS_SECONDS 2
#define SMARTBOX_WAITFOR_ANSWERS_SECONDS 2
#define MAX_MQTT_TOPIC_DEPTH 5


#endif