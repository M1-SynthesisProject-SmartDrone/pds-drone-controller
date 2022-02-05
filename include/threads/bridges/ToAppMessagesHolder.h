#ifndef __TOSENDMESSAGESHOLDER_H__
#define __TOSENDMESSAGESHOLDER_H__

#include <memory>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>

#include <loguru/loguru.hpp>

#include "android/message/tosend/Abstract_AndroidToSendMessage.h"

/**
 * Thread-safe class containing a queue of messages to send to the application.
 * @author Aldric Vitali Silvestre
 */
class ToAppMessagesHolder
{
private:
    std::mutex m_lock;
    std::condition_variable m_condition_variable;
    std::queue<std::unique_ptr<Abstract_AndroidToSendMessage>> m_messagesQueue;

    // The predicate (lambda function) used in condition_variables "wait" and "wait_for" functions.
    const std::function<bool()> lambdaQueueFilled = [=] { return !isQueueEmpty(); };
    
    bool isQueueEmpty();

public:
    ToAppMessagesHolder(/* args */);
    ~ToAppMessagesHolder();

    std::unique_ptr<Abstract_AndroidToSendMessage> pop();
    /**
     * Can return nullptr (if timeout exceeded)
     */
    std::unique_ptr<Abstract_AndroidToSendMessage> pop(int64_t timeoutMs);
    /**
     * Can return nullptr (if timeout exceeded)
     */
    std::unique_ptr<Abstract_AndroidToSendMessage> pop(int64_t timeoutMs, int64_t& timeRemainingMs);
    void add(std::unique_ptr<Abstract_AndroidToSendMessage> message);
};

#endif // __TOSENDMESSAGESHOLDER_H__