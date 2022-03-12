#include "Json_ReceivedMessagesConverter.h"

#include <loguru/loguru.hpp>
#include <algorithm>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace rapidjson;
using namespace std;

Json_ReceivedMessagesConverter::Json_ReceivedMessagesConverter() {}

Json_ReceivedMessagesConverter::~Json_ReceivedMessagesConverter() {}

Abstract_AndroidReceivedMessage* Json_ReceivedMessagesConverter::convertMessageReceived(std::string message)
{
    Document document;
    document.Parse<kParseStopWhenDoneFlag>(message.c_str());

    auto messageConverterFunc = findMessageConverterFunc(document);

    Abstract_AndroidReceivedMessage* converted;
    try
    {
        if (!document["content"].IsObject())
        {
            throw invalid_argument("No content object found");
        }
        GenericObject<false, Value> content = document["content"].GetObject();
        return messageConverterFunc(content);
    }
    catch (exception& e)
    {
        //remove newlines if exists
        message.erase(remove(message.begin(), message.end(), '\n'), message.end());
        LOG_F(ERROR, "Cannot parse the json %s : %s", message.c_str(), e.what());
        throw invalid_argument("Json value is not parsable");
    }

    return converted;
}

Ack_MessageReceived* Json_ReceivedMessagesConverter::parseAckRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj)
{
    return new Ack_MessageReceived();
}

Start_MessageReceived* Json_ReceivedMessagesConverter::parseStartRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj)
{
    bool start = obj["startDrone"].GetBool();
    return new Start_MessageReceived{
        start
    };
}

Record_MessageReceived* Json_ReceivedMessagesConverter::parseRecordRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj)
{
    bool record = obj["record"].GetBool();
    return new Record_MessageReceived{
        record
    };
}

Manual_MessageReceived* Json_ReceivedMessagesConverter::parseManualRequest(rapidjson::GenericObject<false, rapidjson::Value>& obj)
{
    double leftMove = obj["y"].GetDouble();
    double leftRotation = obj["r"].GetDouble();
    double forwardMove = obj["x"].GetDouble();
    double motorPower = obj["z"].GetDouble();
    return new Manual_MessageReceived{
        leftMove,
        leftRotation,
        forwardMove,
        motorPower
    };
}

std::function<Abstract_AndroidReceivedMessage*(rapidjson::GenericObject<false, rapidjson::Value>&)> 
    Json_ReceivedMessagesConverter::findMessageConverterFunc(rapidjson::Document &document)
{
    if (document.HasMember("type"))
    {
        const string typeStr(document["type"].GetString());
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