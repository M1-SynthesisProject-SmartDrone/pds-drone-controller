#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 * 
 * @author Aldric Vitali Silvestre
 */
// Library used : https://github.com/simdjson/simdjson
#include "../../../lib/simdjson/simdjson.h"

#include "Abstract_AndroidMessageConverter.h"
#include "../message/Manual_MessageReceived.h"
#include "../message/Arm_MessageReceived.h"
#include "../message/MessageType.h"

class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    Abstract_AndroidReceivedMessage convertMessageReceived(std::string message);
private:
    /* data */
    MESSAGE_TYPE findMessageType(simdjson::padded_string &json);

    Manual_MessageReceived tryParseManualCommand(simdjson::padded_string &json);
    Arm_MessageReceived tryParseArmCommand(simdjson::padded_string &json);
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__