#include "../../include/network/UDPSocket.h"

#include "../../lib/loguru/loguru.hpp"

#include <system_error>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

UDPSocket::UDPSocket()
{
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == -1)
    {
        throw system_error(errno, system_category(), "Error while creating socket");
    }
    LOG_F(INFO, "Socket created");
}

UDPSocket::~UDPSocket()
{
    close(m_socket);
}

void UDPSocket::bindPort(uint16_t port)
{
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // sockaddr struct is a global struct, and sockaddr_in is specialized in IP communications
    if (bind(m_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        throw system_error(errno, system_category(), "Biding of port failed");
    }
    m_port = port;
    LOG_F(INFO, "Binded on port %u", m_port);
}

void UDPSocket::sendMessage(const std::string &ipAddress, uint16_t port, const char *msg, int msgLength, int flags)
{
    sockaddr_in destination;
    destination.sin_family = AF_INET;
    destination.sin_port = htons(port);
    destination.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    if(sendto(m_socket, msg, msgLength, flags, (struct sockaddr *)&destination, sizeof(destination)) < 0)
    {
        throw system_error(errno, system_category(), "Cannot send message");
    }
    // LOG_F(INFO, "Message sent to %s", ipAddress.c_str());
}

int UDPSocket::receiveMessage(char *buffer, int bufferLength, int flags)
{
    sockaddr_in from;
    socklen_t size = sizeof(from);
    int bytesRead  = recvfrom(m_socket, buffer, bufferLength, flags, (struct sockaddr *)&from, &size) < 0;
    if(bytesRead  < 0)
    {
        throw system_error(errno, system_category(), "Cannot receive message");
    }
    // LOG_F(INFO, "Message received from %s", inet_ntoa(from.sin_addr));
    return bytesRead;
}

uint16_t UDPSocket::getPort()
{
    return m_port;
}

int UDPSocket::getSocket()
{
    return m_socket;
}
