#include "threads/bridges/ToAppMessagesHolder.h"

using namespace std;

ToAppMessagesHolder::ToAppMessagesHolder()
{}

ToAppMessagesHolder::~ToAppMessagesHolder()
{}

bool ToAppMessagesHolder::isQueueEmpty()
{
    return m_messagesQueue.empty();
}

std::unique_ptr<Abstract_AndroidToSendMessage> ToAppMessagesHolder::pop()
{
    unique_lock<mutex> lock(m_lock);
    // Make the thread wait until we have one message 
    m_condition_variable.wait(lock, lambdaQueueFilled);
    auto message = move(m_messagesQueue.front());
    m_messagesQueue.pop();
    return move(message);
}

std::unique_ptr<Abstract_AndroidToSendMessage> ToAppMessagesHolder::pop(int64_t timeoutMs)
{
    if (timeoutMs <= 0)
    {
        return nullptr;
    }
    unique_lock<mutex> lock(m_lock);
    if (m_condition_variable.wait_for(lock, chrono::milliseconds(timeoutMs), lambdaQueueFilled))
    {
        // Wake-up before timeout
        auto message = move(m_messagesQueue.front());
        m_messagesQueue.pop();
        return move(message);
    }
    else
    {
        // Timeout exceeded !
        return nullptr;
    }
}

std::unique_ptr<Abstract_AndroidToSendMessage> ToAppMessagesHolder::pop(int64_t timeoutMs, int64_t& timeRemainingMs)
{
    if (timeoutMs <= 0)
    {
        return nullptr;
    }
    unique_lock<mutex> lock(m_lock);
    auto timeBegin = chrono::steady_clock::now();
    m_condition_variable.wait_for(lock, chrono::milliseconds(timeoutMs), lambdaQueueFilled);
    auto message = move(m_messagesQueue.front());
    m_messagesQueue.pop();
    auto timeEnd = chrono::steady_clock::now();
    // Save time elapsed
    int64_t timeExecMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();
    timeRemainingMs = timeoutMs - timeExecMs;
    return move(message);
}

void ToAppMessagesHolder::add(unique_ptr<Abstract_AndroidToSendMessage> message)
{
    unique_lock<mutex> lock(m_lock);
    m_messagesQueue.push(move(message));
    m_condition_variable.notify_all();
}
