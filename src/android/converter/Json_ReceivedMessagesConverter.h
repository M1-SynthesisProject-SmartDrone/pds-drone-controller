#ifndef __JSON_RECIVEDMESSAGESCONVERTER_H__
#define __JSON_RECIVEDMESSAGESCONVERTER_H__

#include <string>
#include <unordered_map>
#include <functional>

#include <nlohmann/json.hpp>

#include "android/message/received/Abstract_AndroidReceivedMessage.h"
#include "android/message/received/Ack_MessageReceived.h"
#include "android/message/received/Start_MessageReceived.h"
#include "android/message/received/Record_MessageReceived.h"
#include "android/message/received/Manual_MessageReceived.h"
#include "android/message/received/DroneInfos_MessageReceived.h"

class Json_ReceivedMessagesConverter
{
private:
    Ack_MessageReceived* parseAckRequest(nlohmann::json& obj);
    Start_MessageReceived* parseStartRequest(nlohmann::json& obj);
    Record_MessageReceived* parseRecordRequest(nlohmann::json& obj);
    Manual_MessageReceived* parseManualRequest(nlohmann::json& obj);
    DroneInfos_MessageReceived* parseDroneInfosRequest(nlohmann::json& obj);

    std::function<Abstract_AndroidReceivedMessage*(nlohmann::json&)> 
        findMessageConverterFunc(nlohmann::json& document);

    /**
     * This map contains method references (as lambdas) to converters for each message type (key are strings).
     * Contains only request messages, responses messages does not have to be parsed
     */
    const std::unordered_map<
        std::string,
        // This is a bit long, but it is better not to throw aliases in header files
        // And yes, lambdas are a bit ugly in c++, but safier than raw function pointers (and std::bind)
        std::function<Abstract_AndroidReceivedMessage*(nlohmann::json&)>
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