#ifndef __JSON_TOSENDMESSAGESCONVERTER_H__
#define __JSON_TOSENDMESSAGESCONVERTER_H__

#include <string>
#include <functional>
#include <unordered_map>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

#include "android/message/tosend/Abstract_AndroidToSendMessage.h"
#include "android/message/tosend/Ack_MessageToSend.h"
#include "android/message/tosend/DroneInfos_MessageToSend.h"
#include "android/message/tosend/Record_MessageToSend.h"
#include "android/message/tosend/StartDrone_MessageToSend.h"

class Json_ToSendMessagesConverter
{
private:
    rapidjson::Document convertAck(Ack_MessageToSend* ackMessage);
    rapidjson::Document convertDroneInfos(DroneInfos_MessageToSend* droneInfosMessage);
    rapidjson::Document convertRecord(Record_MessageToSend* recordMessage);
    rapidjson::Document convertStartDrone(StartDrone_MessageToSend* startDroneMessage);
    
    std::function<rapidjson::Document(Abstract_AndroidToSendMessage*)> findConverter(Abstract_AndroidToSendMessage* message);
    /**
     * For each message that we can treat, assing a lambda that converts it to the wanted format
     */
    const std::unordered_map<MESSAGE_TYPE, std::function<rapidjson::Document(Abstract_AndroidToSendMessage*)>> CONVERTER_PER_TYPE {
        {MESSAGE_TYPE::RESP_ACK, [this](auto msg){return convertAck(static_cast<Ack_MessageToSend*>(msg));}},
        {MESSAGE_TYPE::RESP_DRONE_INFOS, [this](auto msg){return convertDroneInfos(static_cast<DroneInfos_MessageToSend*>(msg));}},
        {MESSAGE_TYPE::RESP_RECORD, [this](auto msg){return convertRecord(static_cast<Record_MessageToSend*>(msg));}},
        {MESSAGE_TYPE::RESP_START_DRONE, [this](auto msg){return convertStartDrone(static_cast<StartDrone_MessageToSend*>(msg));}}
    };
public:
    Json_ToSendMessagesConverter();
    ~Json_ToSendMessagesConverter();

    std::string convertToSendMessage(Abstract_AndroidToSendMessage* message);
};

#endif // __JSON_TOSENDMESSAGESCONVERTER_H__