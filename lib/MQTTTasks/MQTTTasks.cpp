#include "MQTTTasks.h"

MQTTTasks::MQTTTasks()
{
    myJSONStr tmp;
    for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
    {
        messages[i] = tmp;
    }
    mqtt_class_counter = 0;
    mqtt_class_counter_full = 0;
    isEmpty = false;
}

myJSONStr MQTTTasks::getLastMessage()
{
    if (!isEmpty)
        return messages[mqtt_class_counter];
    else
        log("no messages", "there has been no messages saved", "");
}

myJSONStr MQTTTasks::getDesiredLastMessage(int fromLast)
{
    if (!isEmpty)
    {
        if ((fromLast <= MAX_JSON_MESSAGES_SAVED) && (fromLast >= 0))
        {
            int res = mqtt_class_counter - fromLast;
            if (res < 0)
                return messages[MAX_JSON_MESSAGES_SAVED + res];
            else
                return messages[res];
        }
        else
        {
            log("request of desired message failed", "you entered: " + String(fromLast) + ", but there is a maximum of: " + String(MAX_JSON_MESSAGES_SAVED) + "and a minimum of 0!", "");
        }
    }
}

int MQTTTasks::returnCurrentIterator() // returns absolute counter, if MAX_JSON_MESSAGES_SAVED reached and first message is overridden it can be detected from outside!
{
    return (mqtt_class_counter + mqtt_class_counter_full * MAX_JSON_MESSAGES_SAVED);
}

bool MQTTTasks::deleteMessage(int fromLast) // sets this struct to default
{
    myJSONStr tmp;
    if (!isEmpty)
    {
        if ((fromLast < MAX_JSON_MESSAGES_SAVED) && (fromLast >= 0))
        {
            int res = mqtt_class_counter - fromLast;
            if (res < 0)
            {
                messages[MAX_JSON_MESSAGES_SAVED + res] = tmp;
                log("", "", "message " + String(MAX_JSON_MESSAGES_SAVED + res) + " deleted successfully");
                return true;
            }
            else
            {
                messages[res] = tmp;
                log("", "", "message " + String(res) + " deleted successfully");
                return true;
            }
        }
        else
        {
            log("deleting failed", "you entered: " + String(fromLast) + ", but there is a maximum of: " + String(MAX_JSON_MESSAGES_SAVED) + "and a minimum of 0!", "");
            return false;
        }
    }
}

bool MQTTTasks::addMessage(myJSONStr mess)
{
    if (mqtt_class_counter == (MAX_JSON_MESSAGES_SAVED - 1))
    {
        mqtt_class_counter = 0;
        mqtt_class_counter_full++;
        messages[mqtt_class_counter] = mess;
        isEmpty = false;
        return true;
    }
    else
    {
        mqtt_class_counter++;
        messages[mqtt_class_counter] = mess;
        isEmpty = false;
        return true;
    }
    log("", "", "message added successfully");
}

MQTTTasks *MQTTTasks::operator=(MQTTTasks *other) // needed in main.cpp
{
    return this;
}

String *MQTTTasks::returnMQTTtopics(myJSONStr &passingMessage)
{
    String tmp[MAX_MQTT_TOPIC_DEPTH];
    int k1 = 0; // lower cut-bound
    int k2 = 0; // upper cut-bound
    int k3 = 0; // num of strings (must be below above!)
    for (int i = 0; i < passingMessage.topic.length(); i++)
    {
        if (passingMessage.topic.charAt(i) == '/')
        {
            k1 = i + 1;
            if (k3 == MAX_MQTT_TOPIC_DEPTH)
                break;
            else
            {
                tmp[k3] = passingMessage.topic.substring(k1, k2);
                k3++;
            }
        }
        else
        {
            k2++;
        }
    }
    String tmp2[k3];
    for (int i = 0; i < k3; i++)
    {
        tmp2[i] = tmp[i];
    }
    return tmp2;
}

int returnNumOfVehicles(String &topicToNumOf)
{
}

myJSONStr *MQTTTasks::getBetween(int from, int to) // from index to index
{
    if ((to < 0) || (from < 0) || (from == to)) // if invalid values
    {
        log("wrong value entered (same or negative)", "enteres from: " + String(from) + ", to: " + String(to), "minimum is 0, maximum is " + String(MAX_JSON_MESSAGES_SAVED));
        return nullptr;
    }
    else
    {
        delete returnBetween;
        returnBetween = nullptr;
        int tmp_to = to % MAX_JSON_MESSAGES_SAVED;
        int tmp_from = from % MAX_JSON_MESSAGES_SAVED;
        int tmp_to_cycles = (int)to / MAX_JSON_MESSAGES_SAVED;     // how many times messages overridden
        int tmp_from_cycles = (int)from / MAX_JSON_MESSAGES_SAVED; // how many times messages overridden
        if (from == to)
        {
            // TODO log???
            return nullptr;
        }
        else if (tmp_from_cycles == tmp_to_cycles)
        {
            int tmp = to - from;
            returnBetween = new myJSONStr[tmp + 1];
            for (int i = 0; i <= tmp; i++)
            {
                returnBetween[i] = messages[tmp_from + i];
            }
        }
        else if ((tmp_from_cycles + 1) < tmp_to_cycles) // if more then one time all messages overridden
        {
            returnBetween = new myJSONStr[MAX_JSON_MESSAGES_SAVED];
            for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
            {
                returnBetween[i] = messages[i]; // not returnBetween = messages since then access to private variable of class
            }
        }
        else if ((tmp_from_cycles + 1) == tmp_to_cycles)
        {
            returnBetween = new myJSONStr[MAX_JSON_MESSAGES_SAVED - tmp_from + tmp_to];
            for (int i = 0; i < (MAX_JSON_MESSAGES_SAVED - tmp_from); i++)
            {
                returnBetween[i] = messages[tmp_from + i];
            }
            for (int i = 0; i < tmp_to; i++)
            {
                returnBetween[i + MAX_JSON_MESSAGES_SAVED - tmp_from] = messages[i];
            }
        }
        else
            log("special case", "", "");
    }
}

void MQTTTasks::log(const String &log1, const String &log2, const String &log3) // logging levels: 0-without, 1 error, 2 info, 3 verbose debugging
{
    switch (log_level)
    {
    case 0:
        break;
    case 1:
        Serial.println(log1);
        break;
    case 2:
        Serial.println(log1);
        Serial.println(log2);
        break;
    case 3:
        Serial.println(log1);
        Serial.println(log2);
        Serial.println(log3);
        break;
    }
}