#ifndef __ANDROIDMEDIATOR_H__
#define __ANDROIDMEDIATOR_H__

#include <memory>

#include "AndroidUDPSocket.h"
#include "android/converter/Abstract_AndroidMessageConverter.h"
#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/message/tosend/Abstract_AndroidToSendMessage.h"

/**
 * This class ensures the communication process between the ground station 
 * (this application) and the remote control one (the smartphone).
 */
class AndroidMediator
{
private:
    std::unique_ptr<AndroidUDPSocket> m_socket;
    std::unique_ptr<Abstract_AndroidMessageConverter> m_converter;

    const int BUFFER_SIZE = 4096;
public:
    AndroidMediator(uint16_t receivePort, uint16_t sendPort);
    ~AndroidMediator();

    void sendMessage(std::unique_ptr<Abstract_AndroidToSendMessage> messageToSend);

    std::unique_ptr<Abstract_AndroidReceivedMessage> receiveMessage();
};

#endif // __ANDROIDMEDIATOR_H__