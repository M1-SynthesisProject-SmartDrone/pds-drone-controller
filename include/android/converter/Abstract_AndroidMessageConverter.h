#ifndef __ABSTRACT_ANDROIDMESSAGECONVERTER_H__
#define __ABSTRACT_ANDROIDMESSAGECONVERTER_H__
/**
 * Abstract helper class to convert messages received.
 * This class is declared inline as no real logic happens here. 
 * 
 * @author Aldric Vitali Silvestre
 */

#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/message/tosend/Abstract_AndroidToSendMessage.h"
#include <string>

class Abstract_AndroidMessageConverter
{
public:
    virtual Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message) = 0;
    
    virtual std::string convertToSendMessage(Abstract_AndroidToSendMessage* message) = 0;
    
private:

};

#endif // __ABSTRACT_ANDROIDMESSAGECONVERTER_H__