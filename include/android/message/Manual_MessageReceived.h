#ifndef __ANDROIDRECEIVEDMESSAGE_H__
#define __ANDROIDRECEIVEDMESSAGE_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for manual control.
 * 
 * @author Aldric Vitali Silvestre
 */
struct Manual_MessageReceived : Abstract_AndroidReceivedMessage
{
    double leftMove;
    double leftRotation;
    double forwardMove;
    double motorPower;

    Manual_MessageReceived(double leftMove, double leftRotation, double forwardMove, double motorPower)
    {
        this->messageType = MESSAGE_TYPE::MANUAL_CONTROL;
        this->leftMove = leftMove;
        this->leftRotation = leftRotation;
        this->forwardMove = forwardMove;
        this->motorPower = motorPower;
    }

    std::string toString()
    {
        return "Manual_MessageReceived [LeftMove = " + std::to_string(this->leftMove) 
            + ", LeftRotation = " + std::to_string(this->leftRotation)
            + ", ForwardMove = " + std::to_string(this->forwardMove)
            + ", MotorPower = " + std::to_string(this->motorPower)
            + "]";
    }
};

#endif // __ANDROIDRECEIVEDMESSAGE_H__