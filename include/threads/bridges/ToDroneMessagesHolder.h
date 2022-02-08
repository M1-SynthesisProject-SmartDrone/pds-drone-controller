#ifndef __ANDROIDMESSAGESHOLDER_H__
#define __ANDROIDMESSAGESHOLDER_H__

#include <memory>
#include <condition_variable>
#include <mutex>
#include "android/message/received/Abstract_AndroidReceivedMessage.h"

/**
 * Thread-safe class used to make the bridge between the android application and the ground station.
 */

class ToDroneMessagesHolder
{
private:
    std::unique_ptr<Abstract_AndroidReceivedMessage> m_message;
    bool m_isLastMessageTreated = false;
    std::mutex m_lock;
    std::condition_variable m_condition_variable;

public:
    ToDroneMessagesHolder();
    ~ToDroneMessagesHolder();

    std::unique_ptr<Abstract_AndroidReceivedMessage> getLastMessage();
    void add(std::unique_ptr<Abstract_AndroidReceivedMessage> message);
};

#endif // __ANDROIDMESSAGESHOLDER_H__