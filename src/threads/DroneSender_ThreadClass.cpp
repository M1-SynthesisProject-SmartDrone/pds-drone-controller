#include "threads/DroneSender_ThreadClass.h"

#include <loguru/loguru.hpp>

#include "network/Com_Mavlink.h"
#include "global_variables.h" 

#include <sstream>

using namespace std;

DroneSender_ThreadClass::DroneSender_ThreadClass(
    std::shared_ptr<Drone> drone,
    std::shared_ptr<ToDroneMessagesHolder> droneMessageHolder,
    std::shared_ptr<ToAppMessagesHolder> appMessagesHolder)
    : Abstract_ThreadClass(1000, 200)
{
    m_drone = drone;
    m_droneMessagesHolder = droneMessageHolder;
    m_appMessagesHolder = appMessagesHolder;
}

DroneSender_ThreadClass::~DroneSender_ThreadClass()
{}

void DroneSender_ThreadClass::run()
{
    loguru::set_thread_name("drone sender");
    LOG_F(INFO, "Run drone sender thread");

    while (isRunFlag())
    {
        usleep(task_period);
        // wait for message to come, then send it to the drone
        auto message = m_droneMessagesHolder->getLastMessage();
        // LOG_F(INFO, "Process message : Action = %ld Value =%lf", message.action, message.value);

        // while (m_drone.get()->motors == Drone_Motors::UNARM);

        // depending on the action, do something
        try
        {
            onMessageReceived(message.get());
        }
        catch (std::exception& exception)
        {
            LOG_F(ERROR, "Error while handling android message : %s", exception.what());
        }

    }
    LOG_F(INFO, "End of thread");
}

void DroneSender_ThreadClass::handleArmMessage(Arm_MessageReceived* armMessage)
{
    // we probably want to make some verfications here
    LOG_F(INFO, "Send arming command");
    bool arm = armMessage->armDrone;
    if ((arm && m_drone->motors == Drone_Motors::ARM) || (!arm && m_drone->motors == Drone_Motors::UNARM))
    {
        LOG_F(INFO, "Arm : Drone motors already in wanted state");
        // Technically this is already done, so the operation is a success
        auto toSend = make_unique<Answer_MessageToSend>("ARM", true, "Drone already in wanted state");
        m_appMessagesHolder->add(move(toSend));
    }
    else
    {
        int command = arm ? 1 : 0;
        m_drone->command_arm(command);
    }
}

void DroneSender_ThreadClass::handleTakeOffMessage(TakeOff_MessageReceived* takeOffMessage)
{
    bool takeOff = takeOffMessage->takeOff;
    if (takeOff && m_drone->tookOff)
    {
        LOG_F(INFO, "Take off : Drone is already at wanted state");
        auto toSend = make_unique<Answer_MessageToSend>("TAKE_OFF", true, "Drone already in wanted state");
        m_appMessagesHolder->add(move(toSend));
    }
    // ! We probably want to check some conditions here (grounded, etc.)
    else if (takeOff)
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

void DroneSender_ThreadClass::handleManualControlMessage(Manual_MessageReceived* manualControlMessage)
{
    // Don't log here, we will have a LOT of commands here
    m_drone->command_directControl(
        // -1000 = back, 1000 = forward
        manualControlMessage->forwardMove * 1000.0,
        // -1000 = left, 1000 = right
        manualControlMessage->leftMove * -1000.0,
        // -1000 = min thrust, 1000 = max thrust
        manualControlMessage->motorPower * 1000.0,
        // -1000 = clockwise, 1000 = counter-clockwise
        manualControlMessage->leftRotation * 1000.0
    );
}

void DroneSender_ThreadClass::onMessageReceived(Abstract_AndroidReceivedMessage* androidMessage)
{
    switch (androidMessage->messageType)
    {
    case MESSAGE_TYPE::ARM_COMMAND:
        // * Brackets are made to avoid cross initialization error (var defined in all scopes)
        // see : https://stackoverflow.com/questions/11578936/getting-a-bunch-of-crosses-initialization-error#answer-11578973
    {
        Arm_MessageReceived* armMessage = static_cast<Arm_MessageReceived*>(androidMessage);
        handleArmMessage(armMessage);
    }
    break;
    case MESSAGE_TYPE::MANUAL_CONTROL:
    {
        Manual_MessageReceived* manualMessage = static_cast<Manual_MessageReceived*>(androidMessage);
        handleManualControlMessage(manualMessage);
    }
    break;
    case MESSAGE_TYPE::TAKE_OFF:
    {
        TakeOff_MessageReceived* takeOffMessage = static_cast<TakeOff_MessageReceived*>(androidMessage);
        handleTakeOffMessage(takeOffMessage);
    }
    break;
    case MESSAGE_TYPE::UNKNOWN:
    default:
        throw runtime_error("Unknown message type, or illegal one");
        break;
    }

}