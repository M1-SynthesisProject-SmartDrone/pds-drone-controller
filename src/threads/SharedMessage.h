#ifndef __SHAREDMESSAGELIST_H__
#define __SHAREDMESSAGELIST_H__


#include "../android/message/Abstract_AndroidReceivedMessage.h"

#include <queue>
#include <condition_variable>
#include <mutex>

/**
 * This Singleton is meant to be used by multiple threads.
 *
 * @deprecated
 * @author Aldric Vitali Silvestre
 */
class SharedMessage
{
private:
    SharedMessage();
    ~SharedMessage();

    Abstract_AndroidReceivedMessage m_lastMessage;
    bool m_isMessageSent = false;
    std::mutex m_lock;
    std::condition_variable m_condition_variable;

    bool isEmpty();

public:
    static SharedMessage* getInstance();

    Abstract_AndroidReceivedMessage pop();
    void add(Abstract_AndroidReceivedMessage message);
};

#endif // __SHAREDMESSAGELIST_H__