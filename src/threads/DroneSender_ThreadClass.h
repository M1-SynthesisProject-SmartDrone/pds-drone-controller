/**
 * The thread that will wait for messages comming from the app and sending orders to the drone accordignly.
 * 
 * More later, it could be the one securing the drone if something odd happens.
 * 
 * @author Aldric Vitali Silvestre
 */

#ifndef __DRONESENDER_THREADCLASS_H__
#define __DRONESENDER_THREADCLASS_H__

#include "Abstract_ThreadClass.h"

#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/message/received/Start_MessageReceived.h"
#include "android/message/received/Record_MessageReceived.h"
#include "android/message/received/Manual_MessageReceived.h"

#include "android/message/tosend/StartDrone_MessageToSend.h"

#include "network/Com_Serial.h"
#include "network/Com_Mavlink.h"

#include "drone/Data_Drone.h"
#include "drone/DroneManualCommand.h"

#include "threads/bridges/ToDroneMessagesHolder.h"
#include "threads/bridges/ToAppMessagesHolder.h"

class DroneSender_ThreadClass : public Abstract_ThreadClass
{
private:
    /**
     * This class does not handle the openning of the connection (must be made at least in the calling class)
     */
    std::shared_ptr<Drone> m_drone;

    /**
     * Same for the message handler, but this time it is because we need this instance in two threads.
     */
    std::shared_ptr<ToDroneMessagesHolder> m_droneMessagesHolder;

    std::shared_ptr<ToAppMessagesHolder> m_appMessagesHolder;

    MavlinkTools m_mavlinkTools;

    void onMessageReceived(Abstract_AndroidReceivedMessage* androidMessage);
    void handleStartMessage(Start_MessageReceived* startMessage);
    void handleRecordMessage(Record_MessageReceived* recordMessage);
    void handleManualControlMessage(Manual_MessageReceived* manualControlMessage);

public:
    /**
     * This class does not handle the openning of the connection (must be made at least in the calling class)
     */
    DroneSender_ThreadClass(std::shared_ptr<Drone> drone, 
        std::shared_ptr<ToDroneMessagesHolder> droneMessageHolder, 
        std::shared_ptr<ToAppMessagesHolder> appMessagesHolder);
    ~DroneSender_ThreadClass();

    void run();
};

#endif // __DRONESENDER_THREADCLASS_H__