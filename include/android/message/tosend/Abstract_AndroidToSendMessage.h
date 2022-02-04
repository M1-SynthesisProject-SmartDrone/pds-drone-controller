#ifndef __ABSTRACT_ANDROIDTOSENDMESSAGE_H__
#define __ABSTRACT_ANDROIDTOSENDMESSAGE_H__

#include "android/message/Abstract_AndroidMessage.h"

struct Abstract_AndroidToSendMessage : Abstract_AndroidMessage
{
    virtual std::string toString()
    {
        return "Abstract_AndroidToSendMessage";
    }
};

#endif // __ABSTRACT_ANDROIDTOSENDMESSAGE_H__