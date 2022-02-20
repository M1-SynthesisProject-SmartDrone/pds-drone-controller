#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 *
 * @author Aldric Vitali Silvestre
 */
 // Library used : https://rapidjson.org/
 
#include <stdexcept>
#include <algorithm>
#include <string>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <loguru/loguru.hpp>

#include "Abstract_AndroidMessageConverter.h"
#include "android/message/MessageType.h"
#include "android/message/received/Manual_MessageReceived.h"
#include "android/message/received/Arm_MessageReceived.h"
#include "android/message/received/TakeOff_MessageReceived.h"
#include "android/message/received/Manual_MessageReceived.h"

#include "android/message/tosend/Abstract_AndroidToSendMessage.h"
#include "android/message/tosend/DroneData_MessageToSend.h"
#include "android/message/tosend/DroneStatus_MessageToSend.h"
#include "android/message/tosend/Answer_MessageToSend.h"


class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    Abstract_AndroidReceivedMessage* convertMessageReceived(std::string message);
    std::string convertToSendMessage(Abstract_AndroidToSendMessage* message);
private:
    /* data */
    MESSAGE_TYPE findMessageType(rapidjson::Document& doc);

    // Convert recevied message
    Manual_MessageReceived* tryParseManualCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    Arm_MessageReceived* tryParseArmCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);
    TakeOff_MessageReceived* tryParseTakeOffCommand(rapidjson::GenericObject<false, rapidjson::Value>& obj);

    // Convert to send message
    rapidjson::Document convertDroneUpdateMessage(DroneData_MessageToSend* droneData);
    rapidjson::Document convertDroneStatusMessage(DroneStatus_MessageToSend* droneStatus);
    rapidjson::Document convertAnswerMessage(Answer_MessageToSend* answer);
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__