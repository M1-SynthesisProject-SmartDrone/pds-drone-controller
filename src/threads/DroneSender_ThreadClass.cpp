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
    m_currentMessageSent.forwardMove = 0;
    m_currentMessageSent.leftMove = 0;
    m_currentMessageSent.leftRotation = 0;
    m_currentMessageSent.motorPower = 0;
}

DroneSender_ThreadClass::~DroneSender_ThreadClass()
{
}

void DroneSender_ThreadClass::run()
{
    loguru::set_thread_name("drone sender");
    LOG_F(INFO, "Run drone sender thread");
    // Ask drone to arm, doesn't change anything for now
    LOG_F(INFO, "Arm drone");
    m_drone.get()->command_arm(1);


    // TODO : test the new command
    // see DroneManualCommand for infos
    short x = 0, y = 0, z = 10, r = 0;
    DroneManualCommand manualCommand = {x, y, z, r};
    // m_drone.get()->manual_control(manualCommand);

    while (isRunFlag())
    {
        
        // wait for message to come, then send it to the drone
        auto message = SharedMessage::getInstance()->pop();
        // LOG_F(INFO, "Process message : Action = %ld Value =%lf", message.action, message.value);

        // while (m_drone.get()->motors == Drone_Motors::UNARM);

        // depending on the action, do something
        try {
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

void DroneSender_ThreadClass::onMessageReceived(AndroidMessageReceived androidMessage)
{

    // TODO : if manual_control works, all below is useless and must be updated

    // We have multiple command values at once, we want to update the most important first
    // We will compare one by one and send an appropriate command
    if (m_currentMessageSent.motorPower != androidMessage.motorPower)
    {
        treatMotorPower(androidMessage.motorPower);
    }
    else if (m_currentMessageSent.forwardMove != androidMessage.forwardMove)
    {
        treatForwardMove(androidMessage.forwardMove);
    }
    else if (m_currentMessageSent.leftMove != androidMessage.leftMove)
    {
        treatLeftMove(androidMessage.leftMove);
    }
    else if (m_currentMessageSent.leftRotation != androidMessage.leftRotation)
    {
        treatLeftRotation(androidMessage.leftRotation);
    }
}

void DroneSender_ThreadClass::treatMotorPower(double motorPower)
{
    m_currentMessageSent.motorPower = motorPower;
}

void DroneSender_ThreadClass::treatForwardMove(double forwardMove)
{
    // if > 0, forward, if < 0, backwards
    if (forwardMove > 0)
    {
    }
    else if (forwardMove < 0)
    {
        
    }
    m_currentMessageSent.forwardMove = forwardMove;
}

void DroneSender_ThreadClass::treatLeftMove(double leftMove)
{
    m_currentMessageSent.leftMove = leftMove;
}

void DroneSender_ThreadClass::treatLeftRotation(double leftRotation)
{
    m_currentMessageSent.leftRotation = leftRotation;
}