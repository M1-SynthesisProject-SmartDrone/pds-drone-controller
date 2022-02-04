#ifndef __ANDROIDSENDER_THREADCLASS_H__
#define __ANDROIDSENDER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to send
 * messages from the android application
 *
 * @author Aldric Vitali Silvestre
 */

#include "Abstract_ThreadClass.h"

#include "drone/Data_Drone.h"
#include "android/AndroidUDPSocket.h"
#include "android/converter/Abstract_AndroidMessageConverter.h"

class AndroidSender_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
    std::shared_ptr<AndroidUDPSocket> m_udpSocket;
    std::shared_ptr<Abstract_AndroidMessageConverter> m_messageConverter;

public:
    AndroidSender_ThreadClass(std::shared_ptr<Drone> drone, 
        std::shared_ptr<AndroidUDPSocket> udpSocket,
        std::shared_ptr<Abstract_AndroidMessageConverter> messageConverter);
    ~AndroidSender_ThreadClass();

    void run();
};

#endif // __ANDROIDSENDER_THREADCLASS_H__