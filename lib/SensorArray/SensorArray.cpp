#include "SensorArray.h"

extern bool showCase; // defined in main.cpp

SensorArray::SensorArray() // initialisation of Sensor
{
    pinMode(OUTPUT_PIN, OUTPUT); // light LED
    pinMode(INPUT_PIN1, INPUT); // sharp sensor 1
    pinMode(INPUT_PIN2, INPUT); // sharp sensor 2
    pinMode(INPUT_PIN3, INPUT); // sharp sensor 3
    lastValuesSize = 0;
}

bool SensorArray::getSensorData() // read sensor, true if full
{
    LOG3("getting Sensor Data");
    double sum1 = 0, sum2 = 0, sum3 = 0;
    digitalWrite(13, HIGH);
    delay(10); // needed for time delay of Sensor
    //digitalWrite(OUTPUT_PIN, HIGH);
    for (int i = 0; i < MAX_SENSOR_ITERATIONS; i++)
    {
        double volts = analogRead(INPUT_PIN1) * 0.0049;
        sum1 += volts;
        volts = analogRead(INPUT_PIN2) * 0.0049;
        sum2 += volts;
        volts = analogRead(INPUT_PIN3) * 0.0049;
        sum3 += volts;
        delay(5); // needed for reading out PIN
    }
    sum1 = sum1 / MAX_SENSOR_ITERATIONS;
    sum2 = sum2 / MAX_SENSOR_ITERATIONS;
    sum3 = sum3 / MAX_SENSOR_ITERATIONS;
    LOG3("SensorValue1: " + String(sum1));
    if (showCase)
    {
        if (sum1 <= SENSOR_TOLLERANCE)
        {
            LOG3("Sensor found Element in Box"); // true if within 0.5-5cm from Sensor, otherwise false
            return true;
        }
        else
        {
            LOG3("Sensor found no Element");
            return false;
        }
    }
    else if ((sum1 <= SENSOR_TOLLERANCE) && (sum2 <= SENSOR_TOLLERANCE) && (sum3 <= SENSOR_TOLLERANCE))
    {
        LOG3("Sensor found Element in Box"); // true if within 0.5-5cm from Sensor, otherwise false
        return true;
    }
    else
    {
        LOG3("Sensor found no Element");
        return false;
    }
}

void SensorArray::pushToQueue(bool &val)
{
    if (lastValuesSize == (MAX_SENSOR_VALUES - 2))
    {
        lastValuesSize = -1;
    };
    lastValuesSize++;
    SensValStore[lastValuesSize] = val;
}

bool SensorArray::getLastSensorData(int num)
{
    if ((num < MAX_SENSOR_VALUES) && (num > 0))
    {
        return SensValStore[(lastValuesSize + num) % MAX_SENSOR_VALUES];
    }
    else
    {
        LOG1("You entered a wrong number!");
        LOG2("you entered: " + num);
        LOG3("it must be between 0 and " + String(MAX_SENSOR_VALUES - 1));
    }
}