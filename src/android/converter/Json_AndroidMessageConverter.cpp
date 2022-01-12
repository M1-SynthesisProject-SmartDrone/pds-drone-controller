#include "../../../include/android/converter/Json_AndroidMessageConverter.h"

#include "../../../lib/loguru/loguru.hpp"

#include "../../../include/android/message/Manual_MessageReceived.h"

#include <stdexcept>
#include <algorithm>
#include <string>

using namespace std;

Json_AndroidMessageConverter::Json_AndroidMessageConverter()
{}

Json_AndroidMessageConverter::~Json_AndroidMessageConverter()
{}

Abstract_AndroidReceivedMessage Json_AndroidMessageConverter::convertMessageReceived(std::string message)
{
    rapidjson::Document document;
    document.Parse<rapidjson::kParseStopWhenDoneFlag>(message.c_str());

    MESSAGE_TYPE messageType = findMessageType(document);

    Abstract_AndroidReceivedMessage converted;
    try
    {
        if (!document["content"].IsObject()) {
            throw invalid_argument("No content object found");
        }
        rapidjson::GenericObject<false, rapidjson::Value> content = document["content"].GetObject();
        switch (messageType)
        {
        case MESSAGE_TYPE::ARM_COMMAND:
            converted = tryParseArmCommand(content);
            break;
        case MESSAGE_TYPE::MANUAL_CONTROL:
            converted = tryParseManualCommand(content);
            break;
        case MESSAGE_TYPE::TAKE_OFF:
            converted = tryParseTakeOffCommand(content);
            break;
        default:
            throw invalid_argument("Message type unknown");
        }
    }
    catch (exception& e)
    {
        //remove newlines if exists
        message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
        LOG_F(ERROR, "Cannot parse the json %s : %s", message.c_str(), e.what());
        throw invalid_argument("Json value is not parsable");
    }

    return converted;
}

MESSAGE_TYPE Json_AndroidMessageConverter::findMessageType(rapidjson::Document& doc)
{
    if (doc.HasMember("type"))
    {
        const char* type = doc["type"].GetString();
        // if comparison slow, we may use hases here
        if (strcmp(type, "ARM") == 0)
        {
            return MESSAGE_TYPE::ARM_COMMAND;
        }
        else if (strcmp(type, "TAKEOFF") == 0)
        {
            return MESSAGE_TYPE::TAKE_OFF;
        }
        else if (strcmp(type, "MANUAL_CONTROL") == 0)
        {
            return MESSAGE_TYPE::MANUAL_CONTROL;
        }
    }
    return MESSAGE_TYPE::UNKNOWN;
}

Manual_MessageReceived Json_AndroidMessageConverter::tryParseManualCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj)
{
    double leftMove = obj["leftmove"].GetDouble();
    double leftRotation = obj["leftrotation"].GetDouble();
    double forwardMove = obj["forwardmove"].GetDouble();
    double motorPower = obj["motorpower"].GetDouble();
    return Manual_MessageReceived{
        leftMove,
        leftRotation,
        forwardMove,
        motorPower
    };
}

Arm_MessageReceived Json_AndroidMessageConverter::tryParseArmCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj)
{
    bool armDrone = obj["armDrone"].GetBool();
    return Arm_MessageReceived{
        armDrone
    };
}

TakeOff_MessageReceived Json_AndroidMessageConverter::tryParseTakeOffCommand(rapidjson::GenericObject<false, rapidjson::Value> &obj)
{
    bool takeOff = obj["takeoff"].GetBool();
    return TakeOff_MessageReceived{
        takeOff
    };
}