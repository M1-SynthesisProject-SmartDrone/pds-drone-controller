#include "../../include/threads/DroneSender_ThreadClass.h"

#include "../../lib/loguru/loguru.hpp"
#include "../../include/threads/SharedMessage.h"

#include "../../include/network/Com_Mavlink.h"
#include "../../include/global_variables.h" 

#include <sstream>

using namespace std;

DroneSender_ThreadClass::DroneSender_ThreadClass(int task_period, int task_deadline, std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass(task_period, task_deadline)
{
    m_drone = drone;
}

DroneSender_ThreadClass::~DroneSender_ThreadClass()
{}

void DroneSender_ThreadClass::run()
{
    loguru::set_thread_name("drone sender");
    LOG_F(INFO, "Run drone sender thread");

    while (isRunFlag())
    {

        // wait for message to come, then send it to the drone
        auto message = SharedMessage::getInstance()->pop();
        // LOG_F(INFO, "Process message : Action = %ld Value =%lf", message.action, message.value);

        // while (m_drone.get()->motors == Drone_Motors::UNARM);

        // depending on the action, do something
        try
        {
            onMessageReceived(message);
        }
        catch (std::exception& exception)
        {
            LOG_F(ERROR, "Error while handling android message : %s", exception.what());
        }

    }
    LOG_F(INFO, "End of thread");
    m_drone.get()->command_arm(0);
}

void DroneSender_ThreadClass::sendArmMessage(Arm_MessageReceived armMessage)
{
    LOG_F(INFO, "Send arming command");
    int command = armMessage.armDrone ? 1 : 0;
    m_drone.get()->command_arm(command);
}

void DroneSender_ThreadClass::sendManualControlMessage(Manual_MessageReceived manualControlMessage)
{
    
}

void DroneSender_ThreadClass::onMessageReceived(Abstract_AndroidReceivedMessage androidMessage)
{
    switch (androidMessage.messageType)
    {
    case MESSAGE_TYPE::ARM_COMMAND:
        Arm_MessageReceived *armMessage = static_cast<Arm_MessageReceived*>(&androidMessage);
        sendArmMessage(*armMessage);
        free(armMessage);
        break;
    case MESSAGE_TYPE::MANUAL_CONTROL:
        Manual_MessageReceived *manualMessage = static_cast<Manual_MessageReceived*>(&androidMessage);
        sendManualControlMessage(*manualMessage);
        free(manualMessage);
        break;
    case MESSAGE_TYPE::UNKNOWN:
    default:
        throw runtime_error("Unknown message type");
        break;
    }

}