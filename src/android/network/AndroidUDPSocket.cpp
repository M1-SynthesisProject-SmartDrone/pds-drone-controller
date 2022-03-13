#include "AndroidUDPSocket.h"

using namespace std;

AndroidUDPSocket::AndroidUDPSocket(uint16_t listenPort, uint16_t sendPort) : UDPSocket() 
{
    m_sendPort = sendPort;
    if (m_sendPort == 0)
    {
        m_sendPort = listenPort;
    }
    // By default, the sender is no one (all zeros)
    m_senderAddr = {0};
    try
    {
        this->bindPort(listenPort);
        LOG_F(INFO, "Create UDP listener on port %d", listenPort);
    }
    catch (std::exception const& exception)
    {
        LOG_F(ERROR, "Error while opening socket : %s", exception.what());
        // cannot do anything else, stop all
        throw runtime_error(exception.what());
    }
}

AndroidUDPSocket::~AndroidUDPSocket()
{}

int AndroidUDPSocket::receiveAndSaveSender(char *buffer, int bufferLength, int flags)
{
    sockaddr_in sender;
    int nbBytesReceived = UDPSocket::receiveMessage(buffer, bufferLength, sender, flags);

    unique_lock<mutex> lock(m_lock);
    if (!areAddrEquals(sender, m_senderAddr))
    {
        m_senderAddr = sender;
    }
    return nbBytesReceived;
}

void AndroidUDPSocket::sendAsResponse(uint16_t port, const char *msg, int msgLength, int flags)
{
    // We only need the addr for constructing the destination address
    unique_lock<mutex> lock(m_lock);
    sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons(port);
    destination.sin_addr.s_addr = m_senderAddr.sin_addr.s_addr;
    lock.unlock();

    if(sendto(getSocket(), msg, msgLength, flags, (struct sockaddr *)&destination, sizeof(destination)) < 0)
    {
        throw system_error(errno, system_category(), "Cannot send message");
    }
}

uint16_t AndroidUDPSocket::getSendPort()
{
    return m_sendPort;
}

int AndroidUDPSocket::areAddrEquals(struct sockaddr_in x, struct sockaddr_in y)
{
    return (x.sin_addr.s_addr == y.sin_addr.s_addr) && (x.sin_port == y.sin_port);
}