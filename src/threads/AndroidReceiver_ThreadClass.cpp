#include "threads/AndroidReceiver_ThreadClass.h"

#include <loguru/loguru.hpp>

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(
    shared_ptr<AndroidUDPSocket> androidUdpSocket,
    shared_ptr<ToDroneMessagesHolder> messageHolder,
    shared_ptr<Abstract_AndroidMessageConverter> messageConverter)
    : Abstract_ThreadClass(1000, 200)
{
    m_UDPSocket = androidUdpSocket;
    m_messageHolder = messageHolder;
    m_messageConverter = messageConverter;
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
            // LOG_F(INFO, messageReceived->toString().c_str());
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
    char buffer[BUFFER_SIZE];
    int bytesRead = m_UDPSocket->receiveAndSaveSender(buffer, BUFFER_SIZE);
    if (bytesRead >= BUFFER_SIZE)
    {
        LOG_F(ERROR, "Received bigger message than buffer could handle, message truncated");
    }

    auto convertedMessage = m_messageConverter->convertMessageReceived(string(buffer));
    return unique_ptr<Abstract_AndroidReceivedMessage>(convertedMessage);
}
