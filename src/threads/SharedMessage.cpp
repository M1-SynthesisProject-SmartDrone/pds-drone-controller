/**
 * A multi-threded singleton that will receive the last message sent by the android application
 * in order to be send to the drone
 */

#include "../../include/threads/SharedMessage.h"

#include <stdexcept>

using namespace std;

SharedMessage::SharedMessage()
{
}

SharedMessage::~SharedMessage()
{
}

SharedMessage* SharedMessage::getInstance()
{
    // C++11 garanteed the thread-safe initialization
    // see : https://stackoverflow.com/questions/12248747/singleton-with-multithreads
    static SharedMessage instance;
    return &instance;
}

bool SharedMessage::isEmpty()
{
    return m_isMessageSent;
}

void SharedMessage::add(Abstract_AndroidReceivedMessage message)
{
    unique_lock<mutex> lock(m_lock);
    m_lastMessage = message;
    m_isMessageSent = false;
    // If someone waits for a message to be here, we have to say to him to stop waiting
    m_condition_variable.notify_all();
}

Abstract_AndroidReceivedMessage SharedMessage::pop()
{
    unique_lock<mutex> lock(m_lock);
    // Make the thread wait until we have one message 
    m_condition_variable.wait(lock, [=]{ return !this->isEmpty(); });
    m_isMessageSent = true;
    return m_lastMessage;
}