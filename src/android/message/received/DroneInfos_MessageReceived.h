#ifndef __DRONEINFOS_MESSAGERECEIVED_H__
#define __DRONEINFOS_MESSAGERECEIVED_H__

#include "./Abstract_AndroidReceivedMessage.h"

/**
 * The message sent in order to ask for drone position, status etc.
 *
 * @author Aldric Vitali Silvestre
 */
struct DroneInfos_MessageReceived : Abstract_AndroidReceivedMessage
{
    DroneInfos_MessageReceived()
    {
        this->messageType = MESSAGE_TYPE::REQ_DRONE_INFOS;
    }

    std::string toString()
    {
        return "DroneInfos_MessageReceived";
    }
};

#endif // __DRONEINFOS_MESSAGERECEIVED_H__