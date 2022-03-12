#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 *
 * @author Aldric Vitali Silvestre
 */
 // Library used : https://rapidjson.org/

#include <memory>
#include <stdexcept>
#include <algorithm>
#include <string>

#include "Abstract_AndroidMessageConverter.h"

#include "android/message/tosend/Abstract_AndroidToSendMessage.h"
#include "android/message/received/Abstract_AndroidReceivedMessage.h"

#include "android/converter/Json_ReceivedMessagesConverter.h"
#include "android/converter/Json_ToSendMessagesConverter.h"

class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message);
    std::string convertToSendMessage(Abstract_AndroidToSendMessage* message);
private:
    std::unique_ptr<Json_ReceivedMessagesConverter> m_receivedMessagesConverter;
    std::unique_ptr<Json_ToSendMessagesConverter> m_toSendMessagesConverter;
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__