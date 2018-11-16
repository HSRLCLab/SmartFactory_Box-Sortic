#include "SensorArray.h"

SensorArray::SensorArray() // initialisation of Sensor
{
    pinMode(OUTPUT_PIN, OUTPUT); // light LED
    pinMode(INPUT_PIN, INPUT);   // light sensitive diode
    lastValuesSize = 0;
}

bool SensorArray::getSensorData() // read sensor, true if full
{
    return false;                     // TODO
    bool vals[MAX_SENSOR_ITERATIONS]; // saves measured values
    bool returns;
    byte trues = 0, falses = 0; // number of trues and falses measured from sensor
    double sum = 0;
    digitalWrite(13, HIGH);
    digitalWrite(OUTPUT_PIN, HIGH);
    for (int i = 0; i < MAX_SENSOR_ITERATIONS; i++)
    {
        double volts = analogRead(INPUT_PIN) * 0.0049;
        sum += volts;
        delay(10); // needed for reading out PIN
        /*
        if (volts > 0) // TODO: richtig? -> überprüfen
        {
            vals[i] = true;
            trues++;
        }
        else
        {
            vals[i] = false;
            falses++;
        }
        */
    }

    sum = sum / MAX_SENSOR_ITERATIONS;
    LOG1("Schnitt:\t");
    LOG1(sum);

    if (falses > trues)
        returns = false;
    else
        returns = true;
    pushToQueue(returns);
    digitalWrite(OUTPUT_PIN, LOW);
    digitalWrite(13, LOW);
    return returns;
}

void SensorArray::pushToQueue(bool val)
{
    if (this->lastValuesSize == (MAX_SENSOR_VALUES - 1))
    {
        this->lastValuesSize = -1;
    };
    this->lastValuesSize++;
    lastValues[lastValuesSize] = val;
}

bool SensorArray::getLastSensorData(int num)
{
    if ((num < MAX_SENSOR_VALUES) && (num > 0))
    {
        return this->lastValues[(this->lastValuesSize + num) % MAX_SENSOR_VALUES];
    }
    else
    {
        LOG1("You entered a wrong number!");
        LOG2("you entered: " + num);
        LOG3("it must be between 0 and " + String(MAX_SENSOR_VALUES - 1));
    }
}