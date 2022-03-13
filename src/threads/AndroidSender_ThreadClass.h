#ifndef __ANDROIDSENDER_THREADCLASS_H__
#define __ANDROIDSENDER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to send
 * messages from the android application
 *
 * @author Aldric Vitali Silvestre
 */

#include "Abstract_ThreadClass.h"

#include "android/network/AndroidMediator.h"
#include "threads/bridges/ToAppMessagesHolder.h"

class AndroidSender_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<AndroidMediator> m_mediator;
    std::shared_ptr<ToAppMessagesHolder> m_appMsgHolder;

    // Private methods
    void sendQueueMessage(std::unique_ptr<Abstract_AndroidToSendMessage> toSendMessage);

public:
    AndroidSender_ThreadClass(
        std::shared_ptr<AndroidMediator> androidMediator,
        std::shared_ptr<ToAppMessagesHolder> appMsgHolder
    );
    ~AndroidSender_ThreadClass();

    void run();
};

#endif // __ANDROIDSENDER_THREADCLASS_H__