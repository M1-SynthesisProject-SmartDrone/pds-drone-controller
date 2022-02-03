#ifndef __ABSTRACT_ANDROIDMESSAGECONVERTER_H__
#define __ABSTRACT_ANDROIDMESSAGECONVERTER_H__
/**
 * Abstract helper class to convert messages received.
 * This class is declared inline as no real logic happens here. 
 * 
 * @author Aldric Vitali Silvestre
 */

#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include <string>

class Abstract_AndroidMessageConverter
{
public:
    virtual Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message) = 0;
    // TODO more later we will have the other side function

private:

};

#endif // __ABSTRACT_ANDROIDMESSAGECONVERTER_H__