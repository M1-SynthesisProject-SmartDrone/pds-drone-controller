#ifndef __ANSWER_MESSAGETOSEND_H__
#define __ANSWER_MESSAGETOSEND_H__

#include "Abstract_AndroidToSendMessage.h"

/**
 * This is another abstract class to simplify writing simple validation messages
 */
struct Abstract_Answer_MessageToSend : Abstract_AndroidToSendMessage
{
    bool validated;
    std::string message;

    Abstract_Answer_MessageToSend(MESSAGE_TYPE messageType, bool validated, std::string message="")
    {
        this->messageType = messageType;
        this->validated = validated;
        this->message = message;
    }

    std::string toString()
    {
        return "Answer_MessageToSend [type=" + std::to_string(messageType) + " validated = " + std::to_string(validated) + "]";
    }
};


#endif // __ANSWER_MESSAGETOSEND_H__