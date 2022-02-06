#ifndef __ANSWER_MESSAGETOSEND_H__
#define __ANSWER_MESSAGETOSEND_H__

#include "Abstract_AndroidToSendMessage.h"

/**
 * A response for any important action (take_off, land, arm, etc.)
 */
struct Answer_MessageToSend : Abstract_AndroidToSendMessage
{
    bool validated;
    std::string name;
    std::string message;

    Answer_MessageToSend(std::string name, bool validated, std::string message="")
    {
        this->messageType = MESSAGE_TYPE::ANSWER;
        this->name = name;
        this->validated = validated;
        this->message = message;
    }

    std::string toString()
    {
        return "Answer_MessageToSend [name="+ name +" validated = " + std::to_string(validated) + "]";
    }
};


#endif // __ANSWER_MESSAGETOSEND_H__