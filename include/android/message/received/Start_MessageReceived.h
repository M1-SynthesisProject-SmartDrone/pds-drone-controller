#ifndef __START_MESSAGERECEIVED_H__
#define __START_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for launching drone
 *
 * @author Aldric Vitali Silvestre
 */
struct Start_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool startDrone;

    Start_MessageReceived(bool startDrone)
    {
        this->messageType = MESSAGE_TYPE::ARM_COMMAND;
        this->startDrone = startDrone;
    }

    std::string toString()
    {
        std::string av = (this->startDrone ? "true" : "false");
        return "Start_MessageReceived [startDrone = " + av + "]";
    }
};

#endif // __START_MESSAGERECEIVED_H__