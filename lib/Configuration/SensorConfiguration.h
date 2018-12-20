#ifndef SENSORCONFIGURATION_H
#define SENSORCONFIGURATION_H

/*
    file that contains the default sensor configuration
    Note:
        A6 and greater not existing on board!
*/

#define MAX_SENSOR_VALUES 5        // how many sensor values this object can store
#define MAX_SENSOR_ITERATIONS 2    // do it twice to be sure
#define INPUT_PIN1 A4              // IR sensor, must be analog
#define INPUT_PIN2 A5              // IR sensor, must be analog
#define INPUT_PIN3 A3              // IR sensor, must be analog
#define SENSOR_TOLLERANCE 0.20     // when object is detected, 0.18 volts measured if object in box
#define SENSOR_TOLLERANCE_MIN 0.01 // when object is detected, must be bigger than 0 to detect voltage cut
#define OUTPUT_PIN 13              // LED lamp, must be digital

#endif