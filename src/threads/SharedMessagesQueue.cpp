#include "../../include/threads/SharedMessagesQueue.h"

#include <stdexcept>

using namespace std;

SharedMessagesQueue::SharedMessagesQueue()
{
}

SharedMessagesQueue::~SharedMessagesQueue()
{
}

SharedMessagesQueue* SharedMessagesQueue::getInstance()
{
    // C++11 garanteed the thread-safe initialization
    // see : https://stackoverflow.com/questions/12248747/singleton-with-multithreads
    static SharedMessagesQueue instance;
    return &instance;
}

bool SharedMessagesQueue::isEmpty()
{
    return m_messages.empty();
}

void SharedMessagesQueue::add(AndroidMessageReceived message)
{
    unique_lock<mutex> lock(m_lock);
    m_messages.push(message);
    // If someone waits for a message to be here, we have to say to him to stop waiting
    m_condition_variable.notify_all();
}

AndroidMessageReceived SharedMessagesQueue::pop()
{
    unique_lock<mutex> lock(m_lock);
    // Make the thread wait until we have one message in the 
    m_condition_variable.wait(lock, [=]{ return !this->isEmpty(); });
    AndroidMessageReceived message = m_messages.front();
    m_messages.pop();
    return message;
}