#ifndef __ANDROIDRECEIVER_THREADCLASS_H__
#define __ANDROIDRECEIVER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to receive
 * messages from the android application
 *
 * @author Aldric Vitali Silvestre
 */

#include "Abstract_ThreadClass.h"
#include "threads/bridges/ReceivedMessagesHolder.h"
#include <memory>

#include "drone/Data_Drone.h"
#include "android/AndroidUDPSocket.h"
#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/converter/Json_AndroidMessageConverter.h"

class AndroidReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<ReceivedMessagesHolder> m_messageHolder;
    std::shared_ptr<AndroidUDPSocket> m_UDPSocket;
    
    std::unique_ptr<Abstract_AndroidMessageConverter> m_messageConverter;

    std::unique_ptr<Abstract_AndroidReceivedMessage> receiveMessage();
    
public:
    AndroidReceiver_ThreadClass(int task_period,
        int task_deadline, 
        std::shared_ptr<AndroidUDPSocket> AndroidUDPSocket,
        std::shared_ptr<ReceivedMessagesHolder> messageHolder);
    ~AndroidReceiver_ThreadClass();

    void run();
};
#endif // __ANDROIDRECEIVER_THREADCLASS_H__