#ifndef __SHAREDMESSAGELIST_H__
#define __SHAREDMESSAGELIST_H__

/**
 * This Singleton is meant to be used by multiple threads.
 * 
 * @author Aldric Vitali Silvestre
 */
#include "../android/message/AndroidMessageReceived.h"

#include <queue>
#include <condition_variable>
#include <mutex>

class SharedMessage
{
private:
    SharedMessage();
    ~SharedMessage();

    AndroidMessageReceived m_lastMessage;
    bool m_isMessageSent = false;
    std::mutex m_lock;
    std::condition_variable m_condition_variable;

    bool isEmpty();

public:
    static SharedMessage* getInstance(); 

    AndroidMessageReceived pop();
    void add(AndroidMessageReceived message);
};

#endif // __SHAREDMESSAGELIST_H__