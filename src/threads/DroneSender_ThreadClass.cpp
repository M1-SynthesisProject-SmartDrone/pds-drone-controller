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

void DroneSender_ThreadClass::onMessageReceived(Abstract_AndroidReceivedMessage androidMessage)
{

    // TODO : if manual_control works, all below is useless and must be updated
    
}