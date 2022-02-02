#include "../../include/threads/AndroidReceiver_ThreadClass.h"

#include "../../lib/loguru/loguru.hpp"
#include "../../include/android/AndroidReceiver.h"
#include "../../include/threads/SharedMessage.h"

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(int task_period, int task_deadline, uint16_t udpPort, std::shared_ptr<AndroidMessagesHolder> messageHolder)
    : Abstract_ThreadClass(task_period, task_deadline)
{
    m_udpPort = udpPort;
    m_UDPSocket = make_unique<UDPSocket>();
    m_messageHolder = messageHolder;
    m_messageConverter = make_unique<Json_AndroidMessageConverter>();
}

AndroidReceiver_ThreadClass::~AndroidReceiver_ThreadClass()
{}

void AndroidReceiver_ThreadClass::run()
{
    loguru::set_thread_name("android receiver thread");
    LOG_F(INFO, "Run android server thread on port %d", m_udpPort);
    AndroidReceiver androidReceiver;
    try
    {
        m_UDPSocket->bindPort(m_udpPort);
    }
    catch (std::exception const& exception)
    {
        LOG_F(ERROR, exception.what());
        // cannot do anything else, stop all
        exit(EXIT_FAILURE);
    }
    // wait messages
    LOG_F(INFO, "Start listening for messages");
    while (isRunFlag())
    {
        usleep(task_period);
        try
        {
            unique_ptr<Abstract_AndroidReceivedMessage> messageReceived = this->receiveMessage();
            LOG_F(INFO, messageReceived->toString().c_str());
            m_messageHolder->add(move(messageReceived));
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}

std::unique_ptr<Abstract_AndroidReceivedMessage> AndroidReceiver_ThreadClass::receiveMessage()
{
    // Must be a too big buffer to work with
    char buffer[512];
    int bytesRead = m_UDPSocket->receiveMessage(buffer, 512);

    auto convertedMessage = m_messageConverter->convertMessageReceived(string(buffer));
    return unique_ptr<Abstract_AndroidReceivedMessage>(convertedMessage);
}
