/**
 * Wait for messages sent by the android application
 * and store them into understandable objects.
 *  
 * @author Aldric Vitali Silvestre
 */
#ifndef ANDROID_RECEIVER_H
#define ANDROID_RECEIVER_H

#include "../network/UDPSocket.h"
#include "./message/Abstract_AndroidMessageConverter.h"
#include "./message/AndroidMessageReceived.h"

class AndroidReceiver
{
public:
    AndroidReceiver();
    ~AndroidReceiver();

    void init(uint16_t port);
    AndroidMessageReceived receiveMessage();

private:
    UDPSocket *m_UDPSocket;
    Abstract_AndroidMessageConverter *m_messageConverter;
};

#endif