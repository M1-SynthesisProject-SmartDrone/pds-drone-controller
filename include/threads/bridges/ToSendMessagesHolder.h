#ifndef __TOSENDMESSAGESHOLDER_H__
#define __TOSENDMESSAGESHOLDER_H__

#include <memory>
#include <condition_variable>
#include <mutex>
#include <queue>

class ToSendMessagesHolder
{
private:
    std::mutex m_lock;
    std::condition_variable m_condition_variable;
    // TODO

public:
    ToSendMessagesHolder(/* args */);
    ~ToSendMessagesHolder();
};

ToSendMessagesHolder::ToSendMessagesHolder(/* args */)
{
}

ToSendMessagesHolder::~ToSendMessagesHolder()
{
}


#endif // __TOSENDMESSAGESHOLDER_H__