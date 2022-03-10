#ifndef __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__
#define __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__

#include "android/message/Abstract_AndroidMessage.h"

/**
 * All messages received from the android application will inherit this structure
 */
struct Abstract_AndroidReceivedMessage : Abstract_AndroidMessage
{

    virtual std::string toString()
    {
        return "Abstract_AndroidReceivedMessage";
    }
};

#endif // __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__