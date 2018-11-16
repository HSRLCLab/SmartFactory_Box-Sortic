#ifndef MQTTTASKS_H
#define MQTTTASKS_H

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include <MQTTTasksConfiguration.h>
#include <NetworkManagerStructs.h>
#include <LogConfiguration.h>

class MQTTTasks // this class saves all incoming messages in an struct-array and provides all necessary functions to access/handle with messages
{
public:
  MQTTTasks();
  myJSONStr getLastMessage();                          // returns the most current/last message
  myJSONStr getDesiredLastMessage(int fromLast);       // returns the message, which is fromLast from current (e.g. getDesiredLastMessage(1) returns second last/current message)
  int returnCurrentIterator();                         // returns absolute current value of message counter, 0 if empty, if MAX_JSON_MESSAGES_SAVED reached and first message is overridden it can be detected from outside!
  bool deleteMessage(int fromLast);                    // deletes message, which is fromLast from current
  bool addMessage(myJSONStr mess);                     // saves message, FIFO order
  MQTTTasks *operator=(MQTTTasks *other);              // copy constructor
  myJSONStr *getBetween(int from, int to);             // returns messages between indexes, to use for iterations, from index to index, index = returnCurrentIterator
  String *returnMQTTtopics(myJSONStr &passingMessage); // returns String-Array of topics from MQTT topic structure, strings divided by /
    // TODO function to return message from iterator of type returnCurrentIterator, return 0 if overridden
	// TODO function to return difference (used in for loops)

private:
  myJSONStr messages[MAX_JSON_MESSAGES_SAVED];                          // message save
  int mqtt_class_counter;                                               // iterator of last Element
  int mqtt_class_counter_full;                                          // same as above, but if first Element is overritten again, it increases, its equivivalent of how many times the messages have been rewritten
  bool isEmpty;
  myJSONStr *returnBetween;
};

#endif