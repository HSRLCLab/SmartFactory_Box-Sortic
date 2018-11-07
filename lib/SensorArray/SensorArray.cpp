#include "SensorArray.h"

SensorArray::SensorArray() // initialisation of Sensor
{
    pinMode(OUTPUT_PIN, OUTPUT); // light LED
    pinMode(INPUT_PIN, INPUT);   // light sensitive diode
    lastValuesSize = 0;
}

bool SensorArray::getSensorData() // read sensor, true if full
{
    bool vals[MAX_SENSOR_ITERATIONS]; // saves measured values
    bool returns;
    byte trues = 0, falses = 0; // number of trues and falses measured from sensor
    double sum=0;
    digitalWrite(13, HIGH);
    digitalWrite(OUTPUT_PIN, HIGH);
    for (int i = 0; i < MAX_SENSOR_ITERATIONS; i++)
    {
        double volts = analogRead(INPUT_PIN) * 0.0049;
        sum+=volts;
        delay(10);  // needed for reading out PIN
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
    
    sum = sum/MAX_SENSOR_ITERATIONS;
    Serial.print("Schnitt:\t");
    Serial.println(sum);

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
        log("You entered a wrong number!", "you entered: " + num, "it must be between 0 and " + (MAX_SENSOR_VALUES - 1));
    }
}

void SensorArray::log(const String &log1, const String &log2, const String &log3) // logging levels: 0-without, 1 error, 2 info, 3 verbose debugging
{
    switch (log_level)
    {
    case 1:
        Serial.println(log1);
    case 2:
        Serial.println(log1);
        Serial.println(log2);
    case 3:
        Serial.println(log1);
        Serial.println(log2);
        Serial.println(log3);
    default:
        Serial.println("YOU HAVE ENTERED A WRONG log_level! -> PLEASE VERIFY");
        Serial.println("logging levels: 0-without, 1 error, 2 info, 3 verbose debugging");
    }
    // TODO: gegebenenfalls anpassen auch auf anstatt Serial.println in File.write ???
    // TODO: evt. debug.h
}