#include "threads/AndroidSender_ThreadClass.h"


AndroidSender_ThreadClass::AndroidSender_ThreadClass(std::shared_ptr<Drone> drone,
    std::shared_ptr<AndroidUDPSocket> udpSocket,
    std::shared_ptr<Abstract_AndroidMessageConverter> messageConverter
    ) :Abstract_ThreadClass(1000, 200)
{
    m_drone = drone;
    m_udpSocket = udpSocket;
    m_messageConverter = messageConverter;
}


void AndroidSender_ThreadClass::run()
{
    loguru::set_thread_name("android sender thread");
    // wait messages
    LOG_F(INFO, "Start waiting for messages to send");
    while (isRunFlag())
    {
        usleep(task_period);
        try
        {
            // TODO
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}

