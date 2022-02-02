#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 *
 * @author Aldric Vitali Silvestre
 */
 // Library used : https://github.com/simdjson/simdjson
 
#include <stdexcept>
#include <algorithm>
#include <string>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <loguru/loguru.hpp>

#include "Abstract_AndroidMessageConverter.h"
#include "android/message/Manual_MessageReceived.h"
#include "android/message/Arm_MessageReceived.h"
#include "android/message/TakeOff_MessageReceived.h"
#include "android/message/MessageType.h"
#include "android/message/Manual_MessageReceived.h"


// All message types are hashed here in order to avoid string comparaisons

class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message);
private:
    /* data */
    MESSAGE_TYPE findMessageType(rapidjson::Document& doc);

    Manual_MessageReceived* tryParseManualCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    Arm_MessageReceived* tryParseArmCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    TakeOff_MessageReceived* tryParseTakeOffCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__