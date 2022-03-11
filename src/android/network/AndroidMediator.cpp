#include "AndroidMediator.h"

#include "android/converter/Json_AndroidMessageConverter.h"

using namespace std;

AndroidMediator::AndroidMediator(uint16_t senderPort, uint16_t receiverPort)
{
    m_socket = make_unique<AndroidUDPSocket>(receiverPort, senderPort);
    m_converter = make_unique<Json_AndroidMessageConverter>();
}

AndroidMediator::~AndroidMediator()
{}

void AndroidMediator::sendMessage(std::unique_ptr<Abstract_AndroidToSendMessage> messageToSend)
{
    string toSend = m_converter->convertToSendMessage(messageToSend.get());
    m_socket->sendAsResponse(m_socket->getSendPort(), toSend.c_str(), toSend.length());
}

std::unique_ptr<Abstract_AndroidReceivedMessage> AndroidMediator::receiveMessage()
{
    char buffer[BUFFER_SIZE];
    int bytesRead = m_socket->receiveAndSaveSender(buffer, BUFFER_SIZE);
    if (bytesRead >= BUFFER_SIZE)
    {
        LOG_F(ERROR, "Received bigger message than buffer could handle, message truncated");
    }

    auto convertedMessage = m_converter->convertMessageReceived(string(buffer));
    return unique_ptr<Abstract_AndroidReceivedMessage>(convertedMessage);
}
