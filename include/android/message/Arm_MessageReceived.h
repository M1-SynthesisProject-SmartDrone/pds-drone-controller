#ifndef __ARM_MESSAGERECEIVED_H__
#define __ARM_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for arming / disarming drone
 * 
 * @author Aldric Vitali Silvestre
 */
struct Arm_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool armDrone;

    Arm_MessageReceived(bool armDrone)
    {
        this->messageType = MESSAGE_TYPE::ARM_COMMAND;
        this->armDrone = armDrone;
    }

    std::string toString()
    {
        return "Arm_MessageReceived [ArmDrone = " + std::to_string(this->armDrone) + "]"; 
    }
};

#endif // __ARM_MESSAGERECEIVED_H__