#include "Json_AndroidMessageConverter.h"

using namespace std;

Json_AndroidMessageConverter::Json_AndroidMessageConverter()
{
    m_receivedMessagesConverter = make_unique<Json_ReceivedMessagesConverter>();
    m_toSendMessagesConverter = make_unique<Json_ToSendMessagesConverter>();
}

Json_AndroidMessageConverter::~Json_AndroidMessageConverter()
{}

Abstract_AndroidReceivedMessage* Json_AndroidMessageConverter::convertMessageReceived(string message)
{
    return m_receivedMessagesConverter->convertMessageReceived(message);
}

string Json_AndroidMessageConverter::convertToSendMessage(Abstract_AndroidToSendMessage* message)
{
    return m_toSendMessagesConverter->convertToSendMessage(message);
}