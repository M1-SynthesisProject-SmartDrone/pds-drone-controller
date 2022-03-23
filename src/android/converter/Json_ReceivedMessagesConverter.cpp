#include "Json_ReceivedMessagesConverter.h"

#include <loguru/loguru.hpp>
#include <algorithm>

using namespace std;

Json_ReceivedMessagesConverter::Json_ReceivedMessagesConverter() {}

Json_ReceivedMessagesConverter::~Json_ReceivedMessagesConverter() {}

Abstract_AndroidReceivedMessage* Json_ReceivedMessagesConverter::convertMessageReceived(std::string message)
{
    try
    {
        auto document = nlohmann::json::parse(message);

        auto messageConverterFunc = findMessageConverterFunc(document);

        Abstract_AndroidReceivedMessage* converted;
        if (!document["content"].is_object())
        {
            throw invalid_argument("No content object found");
        }
        auto content = document["content"];
        return messageConverterFunc(content);
        return converted;
    }
    catch (exception& e)
    {
        //remove newlines if exists
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        LOG_F(ERROR, "Cannot parse the json %s : %s", message.c_str(), e.what());
        throw invalid_argument("Json value is not parsable");
    }
}

Ack_MessageReceived* Json_ReceivedMessagesConverter::parseAckRequest(nlohmann::json& obj)
{
    return new Ack_MessageReceived();
}

Start_MessageReceived* Json_ReceivedMessagesConverter::parseStartRequest(nlohmann::json& obj)
{
    bool start = obj["startDrone"];
    return new Start_MessageReceived{
        start
    };
}

Record_MessageReceived* Json_ReceivedMessagesConverter::parseRecordRequest(nlohmann::json& obj)
{
    bool record = obj["record"];
    return new Record_MessageReceived{
        record
    };
}


DroneInfos_MessageReceived* Json_ReceivedMessagesConverter::parseDroneInfosRequest(nlohmann::json& obj)
{
    return new DroneInfos_MessageReceived();
}

Manual_MessageReceived* Json_ReceivedMessagesConverter::parseManualRequest(nlohmann::json& obj)
{
    double leftMove = obj["y"];
    double leftRotation = obj["r"];
    double forwardMove = obj["x"];
    double motorPower = obj["z"];
    return new Manual_MessageReceived{
        leftMove,
        leftRotation,
        forwardMove,
        motorPower
    };
}

std::function<Abstract_AndroidReceivedMessage* (nlohmann::json&)>
Json_ReceivedMessagesConverter::findMessageConverterFunc(nlohmann::json& document)
{
    if (document.contains("type"))
    {
        const string typeStr(document["type"]);
        const auto messageTypeIterator = CONVERTER_FROM_STR.find(typeStr);
        if (messageTypeIterator != CONVERTER_FROM_STR.end())
        {
            auto messageConverterFunc = messageTypeIterator->second;
            return messageConverterFunc;
        }
        else
        {
            throw runtime_error("Cannot find converter for message type " + typeStr);
        }
    }
    else
    {
        throw runtime_error("Cannot find type in request object");
    }
}