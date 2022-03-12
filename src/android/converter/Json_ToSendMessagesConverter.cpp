#include "Json_ToSendMessagesConverter.h"

#include <loguru/loguru.hpp>
#include <algorithm>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace std;
using namespace rapidjson;

Json_ToSendMessagesConverter::Json_ToSendMessagesConverter()
{
    
}

Json_ToSendMessagesConverter::~Json_ToSendMessagesConverter()
{
    
}

std::string Json_ToSendMessagesConverter::convertToSendMessage(Abstract_AndroidToSendMessage* message)
{
    try
    {
        auto messageConverterFunc = findConverter(message);
        Document document = messageConverterFunc(message);
        StringBuffer buffer;
        buffer.Clear();
        Writer<StringBuffer> writer(buffer);
        document.Accept(writer);
        return string(buffer.GetString());
    }
    catch (const std::exception& e)
    {
        LOG_F(ERROR, "Cannot convert struct to json : %s", e.what());
        throw invalid_argument("Cannot convert struct to json");
    }
}

std::function<rapidjson::Document(Abstract_AndroidToSendMessage*)> 
    Json_ToSendMessagesConverter::findConverter(Abstract_AndroidToSendMessage* message)
{
    const auto messageTypeIterator = CONVERTER_PER_TYPE.find(message->messageType);
    if (messageTypeIterator != CONVERTER_PER_TYPE.end())
    {
        auto messageConverterFunc = messageTypeIterator->second;
        return messageConverterFunc;
    }
    else
    {
        throw runtime_error("Cannot find converter for message " + message->toString());
    }
}

// ==== CONVERTERS ====
Document createBaseDocument(string messageType)
{
    Document document;
    document.SetObject();
    rapidjson::GenericStringRef<char> value(messageType.c_str());

    Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("type", value, allocator);
    return document;
}

// This is a shortcut for answer messages types, that have nearly the same shape
Document convertAnswerMessage(string messageType, Abstract_Answer_MessageToSend* answer)
{
    Document document = createBaseDocument(messageType);
    Document::AllocatorType& allocator = document.GetAllocator();
    Value content(kObjectType);
    {
        content.AddMember("validated", answer->validated, allocator);
        content.AddMember("message", StringRef(answer->message.c_str()), allocator);
    }
    document.AddMember("content", content, allocator);

    return document;
}

Document Json_ToSendMessagesConverter::convertAck(Ack_MessageToSend* ackMessage)
{
    return convertAnswerMessage("RESP_ACK", ackMessage);
}

Document Json_ToSendMessagesConverter::convertRecord(Record_MessageToSend* recordMessage)
{
    return convertAnswerMessage("RESP_RECORD", recordMessage);
}

Document Json_ToSendMessagesConverter::convertStartDrone(StartDrone_MessageToSend* startDroneMessage)
{
    return convertAnswerMessage("RESP_START_DRONE", startDroneMessage);
}

Document Json_ToSendMessagesConverter::convertDroneInfos(DroneInfos_MessageToSend* droneInfos)
{
    Document document = createBaseDocument("RESP_DRONE_INFOS");
    Document::AllocatorType& allocator = document.GetAllocator();
    Value content(kObjectType);
    {
        content.AddMember("armed", droneInfos->isArmed, allocator);
        content.AddMember("recording", droneInfos->isRecording, allocator);
        content.AddMember("batteryRemaining", droneInfos->batteryRemaining, allocator);
        content.AddMember("lat", droneInfos->lat, allocator);
        content.AddMember("lon", droneInfos->lon, allocator);
        content.AddMember("alt", droneInfos->alt, allocator);
        content.AddMember("relativeAlt", droneInfos->relativeAlt, allocator);
        content.AddMember("vx", droneInfos->vx, allocator);
        content.AddMember("vy", droneInfos->vy, allocator);
        content.AddMember("vz", droneInfos->vz, allocator);
        content.AddMember("yawRotation", droneInfos->yawRotation, allocator);
    }
    document.AddMember("content", content, allocator);

    return document;
}