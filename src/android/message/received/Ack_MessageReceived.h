#ifndef __ACK_MESSAGERECEIVED_H__
#define __ACK_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * The message sent in order to confirm server responsiveness
 *
 * @author Aldric Vitali Silvestre
 */
struct Ack_MessageReceived : Abstract_AndroidReceivedMessage
{
    Ack_MessageReceived()
    {
        this->messageType = MESSAGE_TYPE::REQ_ACK;
    }

    std::string toString()
    {
        return "Ack_MessageReceived";
    }
};

#endif // __ACK_MESSAGERECEIVED_H__