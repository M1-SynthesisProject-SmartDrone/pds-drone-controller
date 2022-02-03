#include "threads/AndroidReceiver_ThreadClass.h"

#include <loguru/loguru.hpp>

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(int task_period, 
    int task_deadline, 
    shared_ptr<AndroidUDPSocket> androidUdpSocket,
    shared_ptr<ReceivedMessagesHolder> messageHolder)
    : Abstract_ThreadClass(task_period, task_deadline)
{
    m_UDPSocket = androidUdpSocket;
    m_messageHolder = messageHolder;
    m_messageConverter = make_unique<Json_AndroidMessageConverter>();
}

AndroidReceiver_ThreadClass::~AndroidReceiver_ThreadClass()
{}

void AndroidReceiver_ThreadClass::run()
{
    loguru::set_thread_name("android receiver thread");
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
    int bytesRead = m_UDPSocket->receiveAndSaveSender(buffer, 512);
    if (bytesRead > 512)
    {
        LOG_F(ERROR, "Received bigger message than buffer could handle, message truncated");
    }

    auto convertedMessage = m_messageConverter->convertMessageReceived(string(buffer));
    return unique_ptr<Abstract_AndroidReceivedMessage>(convertedMessage);
}
