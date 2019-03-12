/**
 * @file MQTTTasks.h
 * @author Luciano Bettinaglio (luciano.bettinaglio@hsr.ch)
 * @brief 
 * @version 0.1
 * @date 2019-03-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MQTTTASKS_H
#define MQTTTASKS_H

#include <Arduino.h>
#include <ArduinoJson.h>
// own files:
#include <LogConfiguration.h>
#include <MQTTTasksConfiguration.h>
#include <NetworkManagerStructs.h>

/**
 * @brief this class saves all incoming messages in an struct-array
 * and provides all necessary functions to access/handle with messages
 * 
 */
class MQTTTasks {
   public:
    /**
   * @brief Construct a new MQTTTasks object
   *  
   */
    MQTTTasks();

    /**
   * @brief copy constructor
   * 
   * @param other 
   * @return MQTTTasks* 
   */
    MQTTTasks *operator=(MQTTTasks *other);

    /**
   * @brief Get the most current/last message
   * 
   * @return myJSONStr 
   */
    myJSONStr getLastMessage();

    /**
   * @brief Get the most current/last message and call deleteMessage
   * 
   * @return myJSONStr 
   */
    myJSONStr doLastMessage();

    /**
   * @brief Get the most current/last message but only for urgent messages
   * 
   * @return myJSONStr 
   */
    myJSONStr doLastUrgentMessage();

    /**
   * @brief shows if urgent messages arrived
   * 
   * @return true 
   * @return false 
   */
    bool hasUrgentMessage();

    /**
   * @brief returns the message,which is fromLast from current
   * (e.g. getDesiredLastMessage(1) returns second last/current message)
   * 
   * @param fromLast 
   * @return myJSONStr 
   */
    myJSONStr getDesiredLastMessage(int fromLast);

    /**
   * @brief  returns the message,
   * which the certainCurrentIterator % \link MAX_JSON_MESSAGES_SAVED \endlink message in the array
   * 
   * @param certainCurrentIterator 
   * @return myJSONStr 
   */
    myJSONStr getDesiredMessage(int certainCurrentIterator);

    /**
   * @brief same \link getDesiredMessage \endlink , but does message at same time
   * 
   * @param certainCurrentIterator 
   * @return myJSONStr 
   */
    myJSONStr getDoDesiredMessage(int certainCurrentIterator);

    /**
   * @brief Set the Startfor Iterations variable
   * 
   * @param fromCurrentIterator 
   * @return true 
   * @return false 
   */
    bool setStartforIterations(int fromCurrentIterator);

    /**
   * @brief Set the Current Iteratorfor Iterations (mqtt_class_counter) variable
   * 
   * @return true 
   * @return false 
   */
    bool setCurrentIteratorforIterations();

    /**
   * @brief used for iterations, will return messages from startIteration to current
   * 
   * @return myJSONStr 
   */
    myJSONStr iterateAndDoMessages();

    /**
   * @brief returns absolute current value of message counter,
   * 0 if empty, if \link MAX_JSON_MESSAGES_SAVED \endlink
   * reached and first message is overridden it can be detected from outside!
   * 
   * @return int 
   */
    int returnCurrentIterator();
    /**
   * @brief deletes message, which is fromLast from current
   * 
   * @param fromLast 
   * @return true 
   * @return false 
   */
    bool deleteMessage(int fromLast);

    /**
   * @brief saves message, FIFO order
   * 
   * @param mess 
   * @return true 
   * @return false 
   */
    bool addMessage(myJSONStr mess);

    /**
   * @brief  returns messages between indexes, to use for iterations, from index to index, index = returnCurrentIterator, 0-th Element.level is array size
   * 
   * @param from 
   * @param to 
   * @return myJSONStr* 
   */
    myJSONStr *getBetween(int from, int to);

    /**
   * @brief returns String-Array of topics from MQTT topic structure, strings divided by /
   * 
   * @param passingMessage 
   * @return String* 
   */
    String *returnMQTTtopics(myJSONStr passingMessage);

    /**
   * @brief  prints all saved messages on serial output; 0 for hostname, 1 level, 2 request, 3 params
   * 
   * @param choice 
   */
    void printAllMessages(byte choice);

   private:
    myJSONStr messages[MAX_JSON_MESSAGES_SAVED];           ///< message save
    int mqtt_class_counter;                                ///< iterator of last Element
    int mqtt_class_counter_full;                           ///< iterator of last Element, but if first Element is overritten again, it increases, its equivivalent of how many times the messages have been rewritten
    bool isEmpty;                                          ///<
    myJSONStr returnBetween[MAX_JSON_MESSAGES_SAVED + 2];  ///<
    //myJSONStr *returnBetween;
    String *stringpassing;  ///<
    int urgentMessage;      ///<
    int startIteration;     ///<
};

#endif