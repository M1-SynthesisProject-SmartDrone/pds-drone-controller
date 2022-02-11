#ifndef __TAKEOFF_MESSAGERECEIVED_H__
#define __TAKEOFF_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * Contains infos for arming / disarming drone
 *
 * ex : {"takeoff":true}
 * 
 * @author Aldric Vitali Silvestre
 */
struct TakeOff_MessageReceived : Abstract_AndroidReceivedMessage
{
    bool takeOff;

    TakeOff_MessageReceived(bool takeOff)
    {
        this->messageType = MESSAGE_TYPE::TAKE_OFF;
        this->takeOff = takeOff;
    }

    std::string toString()
    {
        std::string tv = (this->takeOff ? "true" : "false");
        return "TakeOff_MessageReceived [TakeOff = " + tv + "]";
    }
};

#endif // __TAKEOFF_MESSAGERECEIVED_H__