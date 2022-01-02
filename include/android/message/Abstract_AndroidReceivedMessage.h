#ifndef __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__
#define __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__

#include "Abstract_AndroidMessage.h"

/**
 * All messages received from the android application will inherit this structure
 */
struct Abstract_AndroidReceivedMessage : Abstract_AndroidMessage
{

    std::string toString()
    {
        return this->str;
    }
};

#endif // __ABSTRACT_ANDROIDRECEIVEDMESSAGE_H__