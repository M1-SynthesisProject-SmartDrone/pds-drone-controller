#include "../../include/threads/AndroidReceiver_ThreadClass.h"

#include "../../lib/loguru/loguru.hpp"
#include "../../include/android/AndroidReceiver.h"
#include "../../include/threads/SharedMessage.h"

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(int task_period, int task_deadline, uint16_t udpPort)
    : Abstract_ThreadClass(task_period, task_deadline)
{
    m_udpPort = udpPort;
}

AndroidReceiver_ThreadClass::~AndroidReceiver_ThreadClass() 
{
}

void AndroidReceiver_ThreadClass::run() 
{
    loguru::set_thread_name("android receiver thread");
    LOG_F(INFO, "Run android server thread on port %d", m_udpPort);
    AndroidReceiver androidReceiver;
    try
    {
        androidReceiver.init(m_udpPort);
    }
    catch (std::exception const &exception)
    {
        LOG_F(ERROR, exception.what());
        // cannot do anything else, stop all
        exit(EXIT_FAILURE);
    }
    // wait messages
    AndroidMessageReceived messageReceived;
    LOG_F(INFO, "Start listening for messages");
    while (isRunFlag())
    {
        usleep(task_period);
        try
        {
            messageReceived = androidReceiver.receiveMessage();
            LOG_F(INFO, "Message received ! LeftMove = %lf, LeftRotation = %lf, ForwardMove = %lf, MotorPower = %lf", 
                messageReceived.leftMove, messageReceived.leftRotation, messageReceived.forwardMove, messageReceived.motorPower);
            SharedMessage::getInstance()->add(messageReceived);
        }
        catch (const std::exception &e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}
