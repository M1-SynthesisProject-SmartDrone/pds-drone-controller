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
#include "SharedMessage.h"

#include "../android/message/Abstract_AndroidReceivedMessage.h"
#include "../android/message/Arm_MessageReceived.h"
#include "../android/message/Manual_MessageReceived.h"

#include "../network/Com_Serial.h"
#include "../network/Com_Mavlink.h"

#include "../drone/Data_Drone.h"
#include "../drone/DroneManualCommand.h"

class DroneSender_ThreadClass : public Abstract_ThreadClass
{
private:
    /**
     * This class does not handle the openning of the connection (must be made at least in the calling class)
     */
    std::shared_ptr<Drone> m_drone;

    MavlinkTools m_mavlinkTools;

    void onMessageReceived(Abstract_AndroidReceivedMessage androidMessage);
    void sendArmMessage(Arm_MessageReceived armMessage);
    void sendManualControlMessage(Manual_MessageReceived manualControlMessage);

public:
    /**
     * This class does not handle the openning of the connection (must be made at least in the calling class)
     */
    DroneSender_ThreadClass(int task_period, int task_deadline, std::shared_ptr<Drone> drone);
    ~DroneSender_ThreadClass();

    void run();
};

#endif // __DRONESENDER_THREADCLASS_H__