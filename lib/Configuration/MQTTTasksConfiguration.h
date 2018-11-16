#ifndef MQTTTASKSCONFIGURATION_H
#define MQTTTASKSCONFIGURATION_H

/*
    file that contains the default MQTT message save configuration
    Note:

*/

#define MAX_JSON_MESSAGES_SAVED 50 // max num of saved JSON items, must be smaller than num of vehicles!
#define MAX_MQTT_TOPIC_DEPTH 5     // how many topics can be in row, e.g. SmartBox/SB1/level are 3 topic levels

#endif