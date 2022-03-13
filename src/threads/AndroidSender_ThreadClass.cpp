#include "threads/AndroidSender_ThreadClass.h"

using namespace std;

AndroidSender_ThreadClass::AndroidSender_ThreadClass(
    std::shared_ptr<AndroidMediator> androidMediator,
    std::shared_ptr<ToAppMessagesHolder> appMsgHolder
) : Abstract_ThreadClass(200, 1000)
{
    m_mediator = androidMediator;
    m_appMsgHolder = appMsgHolder;
}

AndroidSender_ThreadClass::~AndroidSender_ThreadClass()
{}

void AndroidSender_ThreadClass::run()
{
    loguru::set_thread_name("android sender thread");
    LOG_F(INFO, "Android sender thread created");

    while (isRunFlag())
    {
        try
        {
            auto message = m_appMsgHolder->pop();
            m_mediator->sendMessage(move(message));
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}
