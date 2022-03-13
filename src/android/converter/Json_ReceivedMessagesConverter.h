#ifndef __JSON_RECIVEDMESSAGESCONVERTER_H__
#define __JSON_RECIVEDMESSAGESCONVERTER_H__

#include <string>
#include <unordered_map>
#include <functional>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/message/received/Ack_MessageReceived.h"
#include "android/message/received/Start_MessageReceived.h"
#include "android/message/received/Record_MessageReceived.h"
#include "android/message/received/Manual_MessageReceived.h"
#include "android/message/received/DroneInfos_MessageReceived.h"

class Json_ReceivedMessagesConverter
{
private:
    Ack_MessageReceived* parseAckRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    Start_MessageReceived* parseStartRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    Record_MessageReceived* parseRecordRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    Manual_MessageReceived* parseManualRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    DroneInfos_MessageReceived* parseDroneInfosRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj);

    std::function<Abstract_AndroidReceivedMessage*(rapidjson::GenericObject<false, rapidjson::Value>&)> 
        findMessageConverterFunc(rapidjson::Document &document);

    /**
     * This map contains method references (as lambdas) to converters for each message type (key are strings).
     * Contains only request messages, responses messages does not have to be parsed
     */
    const std::unordered_map<
        std::string,
        // This is a bit long, but it is better not to throw aliases in header files
        // And yes, lambdas are a bit ugly in c++, but safier than raw function pointers (and std::bind)
        std::function<Abstract_AndroidReceivedMessage*(rapidjson::GenericObject<false, rapidjson::Value>&)>
        > CONVERTER_FROM_STR {
        {"ACK", [this](auto content) {return parseAckRequest(content);}},
        {"START_DRONE", [this](auto content) {return parseStartRequest(content);}},
        {"MANUAL_CONTROL", [this](auto content) {return parseManualRequest(content);}},
        {"RECORD", [this](auto content) {return parseRecordRequest(content);}},
        {"DRONE_INFOS", [this](auto content) {return parseDroneInfosRequest(content);}}
    };
public:
    Json_ReceivedMessagesConverter();
    ~Json_ReceivedMessagesConverter();

    Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message);
};

#endif // __JSON_RECIVEDMESSAGESCONVERTER_H__