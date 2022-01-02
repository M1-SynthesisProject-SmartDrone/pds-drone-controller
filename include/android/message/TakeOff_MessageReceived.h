#ifndef __TAKEOFF_MESSAGERECEIVED_H__
#define __TAKEOFF_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for arming / disarming drone
 *
 * @author Aldric Vitali Silvestre
 */
struct TakeOff_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool takeOff;

    TakeOff_MessageReceived(bool armDrone)
    {
        this->messageType = MESSAGE_TYPE::TAKE_OFF;
        this->takeOff = takeOff;
    }

    std::string toString()
    {
        return "TakeOff_MessageReceived [TakeOff = " + std::to_string(this->takeOff) + "]";
    }
};

#endif // __TAKEOFF_MESSAGERECEIVED_H__