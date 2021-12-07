#include "../../include/android/AndroidReceiver.h"

#include <stdexcept>
#include <string>
#include "../../lib/loguru/loguru.hpp"
#include "../../include/android/converter/Json_AndroidMessageConverter.h"

#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

AndroidReceiver::AndroidReceiver()
{
    m_messageConverter = new Json_AndroidMessageConverter();
}

void AndroidReceiver::init(uint16_t port)
{
    m_UDPSocket = new UDPSocket();
    m_UDPSocket->bindPort(port);
}

Abstract_AndroidReceivedMessage AndroidReceiver::receiveMessage()
{
    // Must be a too big buffer to work with
    char buffer[512];
    int bytesRead = m_UDPSocket->receiveMessage(buffer, 512);

    return m_messageConverter->convertMessageReceived(string(buffer));
}

AndroidReceiver::~AndroidReceiver()
{
    delete m_UDPSocket;
    delete m_messageConverter;
}