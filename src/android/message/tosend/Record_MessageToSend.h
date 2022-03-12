#ifndef __RECORD_MESSAGETOSEND_H__
#define __RECORD_MESSAGETOSEND_H__

#include "Abstract_Answer_MessageToSend.h"

struct Record_MessageToSend : Abstract_Answer_MessageToSend
{
    Record_MessageToSend(bool validated, std::string message = "")
    : Abstract_Answer_MessageToSend(MESSAGE_TYPE::RESP_RECORD, validated, message)
    {}
};

#endif // __RECORD_MESSAGETOSEND_H__