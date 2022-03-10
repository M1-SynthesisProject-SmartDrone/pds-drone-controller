#ifndef __ANDROIDUDPSOCKET_H__
#define __ANDROIDUDPSOCKET_H__

#include <mutex>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <loguru/loguru.hpp>

#include "network/UDPSocket.h"

/**
 * This is a refined version of the UDP socket class that have more functionalities,
 * such as a response system.
 */
class AndroidUDPSocket : public UDPSocket
{
private:
    // In certain cases the port where t send can be different from the port where to receive
    uint16_t m_sendPort;

    std::mutex m_lock;
    // Accessed only in a thread-safe way
    sockaddr_in m_senderAddr;

    int areAddrEquals(struct sockaddr_in x, struct sockaddr_in y);

public:
    /**
     * If send port is equals to "0", the value will be set to the listen port
     */
    AndroidUDPSocket(uint16_t listenPort, uint16_t sendPort = 0);
    ~AndroidUDPSocket();

    int receiveAndSaveSender(char* buffer, int bufferLength, int flags = 0);

    /**
     * Send the message to the last ip address that send a message on this port.
     * If no one sent any message before, it will be sent by default to localhost
     */
    void sendAsResponse(uint16_t port, const char* msg, int msgLength, int flags = 0);

    uint16_t getSendPort();
};

#endif // __ANDROIDUDPSOCKET_H__