#ifndef __STARTDRONE_MESSAGETOSEND_H__
#define __STARTDRONE_MESSAGETOSEND_H__

#include "Abstract_Answer_MessageToSend.h"

struct StartDrone_MessageToSend : Abstract_Answer_MessageToSend
{
    StartDrone_MessageToSend(bool validated, std::string message = "")
    : Abstract_Answer_MessageToSend(MESSAGE_TYPE::RESP_START_DRONE, validated, message)
    {}
};

#endif // __STARTDRONE_MESSAGETOSEND_H__