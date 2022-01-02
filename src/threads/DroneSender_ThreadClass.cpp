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
}

void DroneSender_ThreadClass::sendArmMessage(Arm_MessageReceived armMessage)
{
    // we probably want to make some verfications here
    LOG_F(INFO, "Send arming command");
    int command = armMessage.armDrone ? 1 : 0;
    m_drone.get()->command_arm(command);
}

void DroneSender_ThreadClass::sendManualControlMessage(Manual_MessageReceived manualControlMessage)
{
    // Don't log here, we will have a LOT of commands here
    m_drone.get()->command_directControl(
        // -1000 = back, 1000 = forward
        manualControlMessage.forwardMove * 1000,
        // -1000 = left, 1000 = right
        manualControlMessage.leftMove * -1000,
        // -1000 = min thrust, 1000 = max thrust
        manualControlMessage.motorPower * 1000,
        // -1000 = clockwise, 1000 = counter-clockwise
        manualControlMessage.leftRotation * 1000
    );
}

void DroneSender_ThreadClass::sendTakeOffMessage(TakeOff_MessageReceived takeOffMessage)
{
    // we probably want to check some conditions here (grounded, etc.)
    if (takeOffMessage.takeOff)
    {
        LOG_F(INFO, "Send Take off command");
        m_drone.get()->take_off();
    }
    else
    {
        LOG_F(INFO, "Send landing command");
        m_drone.get()->landing();
    }
}

void DroneSender_ThreadClass::onMessageReceived(Abstract_AndroidReceivedMessage androidMessage)
{
    switch (androidMessage.messageType)
    {
    case MESSAGE_TYPE::ARM_COMMAND:
    // * Brackets are made to avoid cross initialization error (var defined in all scopes)
    // see : https://stackoverflow.com/questions/11578936/getting-a-bunch-of-crosses-initialization-error#answer-11578973
    {
        Arm_MessageReceived* armMessage = static_cast<Arm_MessageReceived*>(&androidMessage);
        sendArmMessage(*armMessage);
        free(armMessage); // can it break ?
    }
    break;
    case MESSAGE_TYPE::MANUAL_CONTROL:
    {
        Manual_MessageReceived* manualMessage = static_cast<Manual_MessageReceived*>(&androidMessage);
        sendManualControlMessage(*manualMessage);
        free(manualMessage);
    }
    break;
    case MESSAGE_TYPE::TAKE_OFF:
    {
        TakeOff_MessageReceived* takeOffMessage = static_cast<TakeOff_MessageReceived*>(&androidMessage);
        sendTakeOffMessage(*takeOffMessage);
        free(takeOffMessage);
    }
    break;
    case MESSAGE_TYPE::UNKNOWN:
    default:
        throw runtime_error("Unknown message type");
        break;
    }

}