#ifndef __DRONESTATUS_MESSAGETOSEND_H__
#define __DRONESTATUS_MESSAGETOSEND_H__


#include "Abstract_AndroidToSendMessage.h"

/**
 * A message sent periodically to the app in order to update drone infos on the UI.
 * @author Aldric Vitali Silvestre
 */
struct DroneStatus_MessageToSend : Abstract_AndroidToSendMessage
{
    bool isArmed;

    DroneStatus_MessageToSend(bool isArmed)
    {
        this->messageType = MESSAGE_TYPE::DRONE_STATUS;
        this->isArmed = isArmed;
    }
};

#endif // __DRONESTATUS_MESSAGETOSEND_H__