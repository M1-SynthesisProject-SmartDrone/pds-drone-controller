#ifndef __RECORD_MESSAGERECEIVED_H__
#define __RECORD_MESSAGERECEIVED_H__


#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for launching drone
 *
 * @author Aldric Vitali Silvestre
 */
struct Record_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool record;

    Record_MessageReceived(bool record)
    {
        this->messageType = MESSAGE_TYPE::RECORD;
        this->record = record;
    }

    std::string toString()
    {
        std::string doRecord = (this->record ? "true" : "false");
        return "Start_MessageReceived [record = " + doRecord + "]";
    }
};

#endif // __RECORD_MESSAGERECEIVED_H__