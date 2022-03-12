#ifndef __ACK_MESSAGETOSEND_H__
#define __ACK_MESSAGETOSEND_H__

#include "Abstract_Answer_MessageToSend.h"

struct Ack_MessageToSend : Abstract_Answer_MessageToSend
{
    Ack_MessageToSend(bool validated, std::string message = "")
    : Abstract_Answer_MessageToSend(MESSAGE_TYPE::RESP_ACK, validated, message)
    {}
};

#endif // __ACK_MESSAGETOSEND_H__