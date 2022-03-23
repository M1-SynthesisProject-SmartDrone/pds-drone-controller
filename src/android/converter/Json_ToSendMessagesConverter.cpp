#include "Json_ToSendMessagesConverter.h"

#include <loguru/loguru.hpp>
#include <algorithm>

using namespace std;

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
        nlohmann::json document = messageConverterFunc(message);
        return document.dump();
    }
    catch (const std::exception& e)
    {
        LOG_F(ERROR, "Cannot convert struct to json : %s", e.what());
        throw invalid_argument("Cannot convert struct to json");
    }
}

std::function<nlohmann::json(Abstract_AndroidToSendMessage*)>
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
nlohmann::json createBaseDocument(char* messageType)
{
    nlohmann::json document;
    document["type"] = messageType;
    return document;
}

// This is a shortcut for answer messages types, that have nearly the same shape
nlohmann::json convertAnswerMessage(char* messageType, Abstract_Answer_MessageToSend* answer)
{
    nlohmann::json document = createBaseDocument(messageType);

    document["content"]["validated"] = answer->validated;
    document["content"]["message"] = answer->message;

    return document;
}

nlohmann::json Json_ToSendMessagesConverter::convertAck(Ack_MessageToSend* ackMessage)
{
    return convertAnswerMessage("RESP_ACK", ackMessage);
}

nlohmann::json Json_ToSendMessagesConverter::convertRecord(Record_MessageToSend* recordMessage)
{
    return convertAnswerMessage("RESP_RECORD", recordMessage);
}

nlohmann::json Json_ToSendMessagesConverter::convertStartDrone(StartDrone_MessageToSend* startDroneMessage)
{
    return convertAnswerMessage("RESP_START_DRONE", startDroneMessage);
}

nlohmann::json Json_ToSendMessagesConverter::convertDroneInfos(DroneInfos_MessageToSend* droneInfos)
{
    nlohmann::json document = createBaseDocument("RESP_DRONE_INFOS");

    document["content"]["armed"] = droneInfos->isArmed;
    document["content"]["recording"] = droneInfos->isRecording;
    document["content"]["batteryRemaining"] = droneInfos->batteryRemaining;
    document["content"]["lat"] = droneInfos->lat;
    document["content"]["lon"] = droneInfos->lon;
    document["content"]["alt"] = droneInfos->alt;
    document["content"]["relativeAlt"] = droneInfos->relativeAlt;
    document["content"]["vx"] = droneInfos->vx;
    document["content"]["vy"] = droneInfos->vy;
    document["content"]["vz"] = droneInfos->vz;
    document["content"]["yawRotation"] = droneInfos->yawRotation;

    return document;
}