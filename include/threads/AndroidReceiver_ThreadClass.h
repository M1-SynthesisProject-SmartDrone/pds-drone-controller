#ifndef __ANDROIDRECEIVER_THREADCLASS_H__
#define __ANDROIDRECEIVER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to receive
 * messages from the android application
 *
 * @author Aldric Vitali Silvestre
 */

#include "Abstract_ThreadClass.h"
#include "AndroidMessagesHolder.h"
#include <memory>

#include "drone/Data_Drone.h"
#include "network/UDPSocket.h"
#include "android/message/Abstract_AndroidReceivedMessage.h"
#include "android/converter/Json_AndroidMessageConverter.h"

class AndroidReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    uint16_t m_udpPort;
    std::shared_ptr<AndroidMessagesHolder> m_messageHolder;
    std::unique_ptr<UDPSocket> m_UDPSocket;
    std::unique_ptr<Abstract_AndroidMessageConverter> m_messageConverter;

    std::unique_ptr<Abstract_AndroidReceivedMessage> receiveMessage();
    
public:
    AndroidReceiver_ThreadClass(int task_period, int task_deadline, uint16_t udpPort, std::shared_ptr<AndroidMessagesHolder> messageHolder);
    ~AndroidReceiver_ThreadClass();

    void run();
};
#endif // __ANDROIDRECEIVER_THREADCLASS_H__