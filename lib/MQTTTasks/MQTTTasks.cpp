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
    urgentMessage = 0;
    startIteration = 0;
}

myJSONStr MQTTTasks::getLastMessage()
{
    if (!isEmpty)
        return messages[mqtt_class_counter];
    else
    {
        LOG1("no messages");
        LOG2("there has been no messages saved");
    }
}
myJSONStr MQTTTasks::doLastMessage()
{
    myJSONStr tmp = getLastMessage();
    deleteMessage(0);
    return tmp;
}

myJSONStr MQTTTasks::doLastUrgentMessage()
{
    myJSONStr tmp;
    for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
    {
        if (getDesiredLastMessage(i).urgent)
        {
            tmp = getDesiredLastMessage(i);
            deleteMessage(i);
            return tmp;
            break;
        }
    }
    return tmp;
}

bool MQTTTasks::hasUrgentMessage()
{
    if (urgentMessage == 0)
        return false;
    else
        return true;
}

myJSONStr MQTTTasks::getDesiredLastMessage(int fromLast)
{
    myJSONStr tmp;
    if (!isEmpty)
    {
        if ((fromLast <= MAX_JSON_MESSAGES_SAVED) && (fromLast >= 0))
        {
            int res = mqtt_class_counter - fromLast;
            if (res < 0)
                tmp = messages[MAX_JSON_MESSAGES_SAVED + res];
            else
                tmp = messages[res];
        }
        else
        {
            LOG1("request of desired message failed");
            LOG2("you entered: " + String(fromLast) + ", but there is a maximum of: " + String(MAX_JSON_MESSAGES_SAVED) + "and a minimum of 0!");
        }
    }
    return tmp;
}

myJSONStr MQTTTasks::getDesiredMessage(int certainCurrentIterator)
{
    myJSONStr tmp;
    if (!isEmpty)
    {
        if (certainCurrentIterator >= 0)
        {
            int currIt = certainCurrentIterator % MAX_JSON_MESSAGES_SAVED;
            tmp = messages[currIt];
        }
        else
        {
            LOG1("request of desired message failed");
            LOG2("you entered: " + String(certainCurrentIterator) + ", but there is a minimum of 0!");
        }
    }
    return tmp;
}
myJSONStr MQTTTasks::getDoDesiredMessage(int certainCurrentIterator)
{
    myJSONStr tmp;
    if (!isEmpty)
    {
        if (certainCurrentIterator >= 0)
        {
            int currIt = certainCurrentIterator % MAX_JSON_MESSAGES_SAVED;
            tmp = doLastMessage();
        }
        else
        {
            LOG1("request of desired message failed");
            LOG2("you entered: " + String(certainCurrentIterator) + ", but there is a minimum of 0!");
        }
    }
    return tmp;
}

bool MQTTTasks::setStartforIterations(int fromCurrentIterator)
{
    if (fromCurrentIterator <= fromCurrentIterator)
    {
        startIteration = fromCurrentIterator;
        return true;
    }

    else
    {
        LOG2("Iterator is wrong");
        LOG3("you entered: " + String(fromCurrentIterator));
        return false;
    }
}

bool MQTTTasks::setCurrentIteratorforIterations()
{
    startIteration = mqtt_class_counter;
    return true;
}

myJSONStr MQTTTasks::iterateAndDoMessages()
{
    myJSONStr tmp;
    if (startIteration == mqtt_class_counter)
    {
        ; // if default values from myJSONStr are returned, it will be shown here
    }
    else
    {
        tmp = getDesiredMessage(startIteration);
        startIteration++;
    }
    return tmp;
}

int MQTTTasks::returnCurrentIterator() // returns absolute counter, if MAX_JSON_MESSAGES_SAVED reached and first message is overridden it can be detected from outside!
{
    return (mqtt_class_counter + mqtt_class_counter_full * MAX_JSON_MESSAGES_SAVED);
}

bool MQTTTasks::deleteMessage(int fromLast) // sets this struct to default
{
    myJSONStr tmp;
    tmp.hostname = "del"; // to clearly see which one were deleted
    if (!isEmpty)
    {
        if ((fromLast < MAX_JSON_MESSAGES_SAVED) && (fromLast >= 0))
        {
            int res = mqtt_class_counter - fromLast;
            if (res < 0)
            {
                if (messages[MAX_JSON_MESSAGES_SAVED + res].urgent)
                    urgentMessage--;
                messages[MAX_JSON_MESSAGES_SAVED + res] = tmp;
                LOG3("message " + String(MAX_JSON_MESSAGES_SAVED + res) + " deleted successfully");
                return true;
            }
            else
            {
                if (messages[res].urgent)
                    urgentMessage--;
                messages[res] = tmp;
                LOG3("message " + String(res) + " deleted successfully");
                return true;
            }
        }
        else
        {
            LOG1("deleting failed");
            LOG2("you entered: " + String(fromLast) + ", but there is a maximum of: " + String(MAX_JSON_MESSAGES_SAVED) + "and a minimum of 0!");
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
        mqtt_class_counter++; // Attention: messages[0] is the MAX_JSON_MESSAGES_SAVED-th message!
        messages[mqtt_class_counter] = mess;
        isEmpty = false;
        return true;
    }
    if (mess.urgent)
        urgentMessage++;
    LOG3("message added successfully");
}

MQTTTasks *MQTTTasks::operator=(MQTTTasks *other) // needed in main.cpp
{
    return this;
}

String *MQTTTasks::returnMQTTtopics(myJSONStr passingMessage)
{
    String tmp[MAX_MQTT_TOPIC_DEPTH];
    int k1 = 0; // lower cut-bound
    int k2 = 0; // upper cut-bound
    int k3 = 0; // num of strings (must be below above!)
    for (int i = 0; i < passingMessage.topic.length() - 2; i++)
    {
        if (passingMessage.topic[i] == '/')
        {
            k2 = i;
            if (k3 == MAX_MQTT_TOPIC_DEPTH)
                break;
            else
            {
                tmp[k3] = passingMessage.topic.substring(k1, k2);
                k1 = i + 1;
                k3++;
            }
        }
    }
    k2 = passingMessage.topic.length();
    tmp[k3] = passingMessage.topic.substring(k1, k2);
    delete stringpassing;
    if (k3 > 0)
    {
        stringpassing = new String[k3 + 1];
        for (int i = 0; i <= k3; i++)
        {
            stringpassing[i] = tmp[i];
        }
        return stringpassing;
    }
    else
    {
        LOG3("k3 has no reasonable value: " + String(k3));
        return nullptr;
    }
}

int returnNumOfVehicles(String &topicToNumOf)
{
    //TODO
}

myJSONStr *MQTTTasks::getBetween(int from, int to) // from index to index
{
    /*
    if (from == MAX_JSON_MESSAGES_SAVED)
        from = 0;
    else
        from++;
    */
    int tmp_to = to % MAX_JSON_MESSAGES_SAVED;
    int tmp_from = from % MAX_JSON_MESSAGES_SAVED;
    int tmp_to_cycles = (int)to / MAX_JSON_MESSAGES_SAVED;     // how many times messages overridden
    int tmp_from_cycles = (int)from / MAX_JSON_MESSAGES_SAVED; // how many times messages overriddenF
    if ((to < 0) || (from < 0) || (from == to) || (from > to)) // if invalid values
    {
        LOG1("wrong value entered (same or negative)");
        LOG2("entered from: " + String(from) + ", to: " + String(to) + "\t\t relative from: " + String(tmp_from) + ", to: " + String(tmp_to));
        LOG3("minimum is 0, maximum is " + String(MAX_JSON_MESSAGES_SAVED));
        return nullptr;
    }
    else
    {
        LOG3("tmp_from_cycles :" + String(tmp_from_cycles) + "\t tmp_to_cycles: " + String(tmp_to_cycles) + "\t tmp_from: " + String(tmp_from) + "\t tmp_to: " + String(tmp_to)); // TODO

        if (from == to)
        {
            LOG2("nothing to do");
            LOG3("you entered twice the same value: " + String(from));
            return nullptr;
        }
        else if (tmp_from_cycles == tmp_to_cycles)
        {
            int tmp = tmp_to - tmp_from;
            // returnBetween = new myJSONStr[tmp + 2];
            returnBetween[0].level = tmp + 1; // this[0].level is array size
            for (int i = 1; i < tmp + 1; i++)
            {
                returnBetween[i] = messages[tmp_from + i];
            }
        }
        else if ((tmp_from_cycles + 1) < tmp_to_cycles) // if more then one time all messages overridden
        {
            // returnBetween = new myJSONStr[MAX_JSON_MESSAGES_SAVED + 2];
            returnBetween[0].level = MAX_JSON_MESSAGES_SAVED + 2; // this.level is array size
            for (int i = 1; i < MAX_JSON_MESSAGES_SAVED + 1; i++)
            {
                returnBetween[i] = messages[i]; // not returnBetween = messages since then access to private variable of class
            }
        }
        else if ((tmp_from_cycles + 1) == tmp_to_cycles)
        {
            // returnBetween = new myJSONStr[MAX_JSON_MESSAGES_SAVED - tmp_from + tmp_to + 2];
            returnBetween[0].level = MAX_JSON_MESSAGES_SAVED - tmp_from + tmp_to + 2; // this.level is array size
            for (int i = 0; i < (MAX_JSON_MESSAGES_SAVED - tmp_from); i++)
            {
                returnBetween[i + 1] = messages[tmp_from + i];
            }
            for (int i = 0; i < tmp_to + 1; i++)
            {
                returnBetween[i + 1 + MAX_JSON_MESSAGES_SAVED - tmp_from] = messages[i];
            }
        }
        else
            LOG1("special case");

        //LOG2("size to return: " + String(returnBetween[0].level)); // TODO
        //for (int i = 1; i < returnBetween[0].level; i++)
        //    LOG3("the " + String(i) + "-th Element to return is: " + returnBetween[i].hostname);
        return returnBetween;
        //delete[] returnBetween;
    }
}

void MQTTTasks::printAllMessages(byte choice) // 0 for hostname, 1 level, 2 request, 3 params
{
    String printable = "";
    int myModulo = 5; // number of how many entries in one row
    switch (choice)
    {
    case 0: // hostname
        for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
        {
            //LOG1("in the save are the following (" + String(i) + "): " + messages[i].hostname);
            printable += "\t[" + String(i) + "]: " + messages[i].hostname;
            if ((i % myModulo == 0) && (i > 1))
            {
                LOG1(printable);
                printable = "";
            }
        }
        LOG1(printable);
        break;
    case 1: // level
        for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
        {
            //LOG1("in the save are the following (" + String(i) + "): " + messages[i].hostname);
            printable += "\t[" + String(i) + "]: " + messages[i].level;
            if ((i % myModulo == 0) && (i > 1))
            {
                LOG1(printable);
                printable = "";
            }
        }
        LOG1(printable);
        break;
    case 2: // request
        for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
        {
            //LOG1("in the save are the following (" + String(i) + "): " + messages[i].hostname);
            printable += "\t[" + String(i) + "]: " + messages[i].request;
            if ((i % myModulo == 0) && (i > 1))
            {
                LOG1(printable);
                printable = "";
            }
        }
        LOG1(printable);
        break;
    case 3: // params
        for (int i = 0; i < MAX_JSON_MESSAGES_SAVED; i++)
        {
            //LOG1("in the save are the following (" + String(i) + "): " + messages[i].hostname);
            printable += "\t[" + String(i) + "]: [";
            for (int j = 0; j < 3; j++)
                printable += String(messages[i].vehicleParams[j]) + "; ";
            printable += String(messages[i].vehicleParams[4]) + "]";
            if ((i % myModulo == 0) && (i > 1))
            {
                LOG1(printable);
                printable = "";
            }
        }
        LOG1(printable);
        break;
    }
}