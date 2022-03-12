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

void DroneSender_ThreadClass::handleStartMessage(Start_MessageReceived* startMessage)
{
    LOG_F(INFO, "Send arming command");
    if (startMessage->startDrone)
    {
        if (m_drone->motors == Drone_Motors::ARM)
        {
            LOG_F(INFO, "Start : Drone motors already in wanted state");
            // Technically this is already done, so the operation is a success
            auto toSend = make_unique<StartDrone_MessageToSend>(true, "Drone already armed");
            m_appMessagesHolder->add(move(toSend));
        }
        else
        {
            m_drone->command_directControl(0, 0, 0, 0);
            m_drone->command_arm(1);
        }
    }
    else
    {
        auto toSend = make_unique<StartDrone_MessageToSend>(false, "Start drone does not accept false value (useless)");
        m_appMessagesHolder->add(move(toSend));
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
    case MESSAGE_TYPE::REQ_START_DRONE:
    {
        auto startMessageReceived = static_cast<Start_MessageReceived*>(androidMessage);
        handleStartMessage(startMessageReceived);
    }
    break;
    case MESSAGE_TYPE::REQ_MANUAL_CONTROL:
    {
        auto manualMessage = static_cast<Manual_MessageReceived*>(androidMessage);
        handleManualControlMessage(manualMessage);
    }
    break;
    case MESSAGE_TYPE::UNKNOWN:
    default:
        throw runtime_error("Unknown message type, or illegal one");
        break;
    }

}