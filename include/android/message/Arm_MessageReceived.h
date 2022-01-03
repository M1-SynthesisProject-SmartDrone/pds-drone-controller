#ifndef __ARM_MESSAGERECEIVED_H__
#define __ARM_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for arming / disarming drone
 * 
 * ex : {"armDrone":true}
 * 
 * @author Aldric Vitali Silvestre
 */
struct Arm_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool armDrone;

    Arm_MessageReceived(bool armDrone)
    {
        this->messageType = MESSAGE_TYPE::ARM_COMMAND;
        this->str =  "Arm_MessageReceived [ArmDrone = " + std::to_string(armDrone) + "]";
        this->armDrone = armDrone;
    }
};

#endif // __ARM_MESSAGERECEIVED_H__