#include "threads/bridges/ReceivedMessagesHolder.h"

using namespace std;

ReceivedMessagesHolder::ReceivedMessagesHolder()
{}

ReceivedMessagesHolder::~ReceivedMessagesHolder()
{}

unique_ptr<Abstract_AndroidReceivedMessage> ReceivedMessagesHolder::getLastMessage()
{
    unique_lock<mutex> lock(m_lock);
    // Make the thread wait until we have one message 
    m_condition_variable.wait(lock, [=] { return !this->m_isLastMessageTreated; });
    m_isLastMessageTreated = true;
    return move(m_message);
}

void ReceivedMessagesHolder::add(unique_ptr<Abstract_AndroidReceivedMessage> message)
{
    unique_lock<mutex> lock(m_lock);
    // If we have a message lying here, free it !
    if (m_message != nullptr)
    {
        m_message.release();
    }
    m_message = move(message);
    m_isLastMessageTreated = false;
    // If someone waits for a message to be here, we have to say to him to stop waiting
    m_condition_variable.notify_all();
}