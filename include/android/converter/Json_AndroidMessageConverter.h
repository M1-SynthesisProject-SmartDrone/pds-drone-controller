#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 * 
 * @author Aldric Vitali Silvestre
 */
// Library used : https://github.com/simdjson/simdjson
#include "../../../lib/rapidjson/rapidjson.h"
#include "../../../lib/rapidjson/document.h"

#include "Abstract_AndroidMessageConverter.h"
#include "../message/Manual_MessageReceived.h"
#include "../message/Arm_MessageReceived.h"
#include "../message/TakeOff_MessageReceived.h"
#include "../message/MessageType.h"

// All message types are hashed here in order to avoid string comparaisons

class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    Abstract_AndroidReceivedMessage convertMessageReceived(std::string message);
private:
    /* data */
    MESSAGE_TYPE findMessageType(rapidjson::Document &doc);

    Manual_MessageReceived tryParseManualCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj);
    Arm_MessageReceived tryParseArmCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj);
    TakeOff_MessageReceived tryParseTakeOffCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj);
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__