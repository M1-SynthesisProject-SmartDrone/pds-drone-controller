#include "../../../include/android/converter/Json_AndroidMessageConverter.h"

#include "../../../lib/loguru/loguru.hpp"

#include "../../../include/android/message/Manual_MessageReceived.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

Json_AndroidMessageConverter::Json_AndroidMessageConverter()
{}

Json_AndroidMessageConverter::~Json_AndroidMessageConverter()
{}

Abstract_AndroidReceivedMessage Json_AndroidMessageConverter::convertMessageReceived(std::string message)
{
    simdjson::ondemand::parser parser;
    auto json = simdjson::padded_string(message);

    MESSAGE_TYPE messageType = findMessageType(json);

    Abstract_AndroidReceivedMessage converted;
    try
    {
        switch (messageType)
        {
        case MESSAGE_TYPE::ARM_COMMAND:
            converted = tryParseArmCommand(json);
            break;
        case MESSAGE_TYPE::MANUAL_CONTROL:
            converted = tryParseManualCommand(json);
            break;
        case MESSAGE_TYPE::TAKE_OFF:
            converted = tryParseTakeOffCommand(json);
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

MESSAGE_TYPE Json_AndroidMessageConverter::findMessageType(simdjson::padded_string& json)
{
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);

    if (doc["leftmove"].error() == simdjson::SUCCESS
        && doc["leftrotation"].error() == simdjson::SUCCESS
        && doc["forwardmove"].error() == simdjson::SUCCESS
        && doc["motorpower"].error() == simdjson::SUCCESS
        )
    {
        return MESSAGE_TYPE::MANUAL_CONTROL;
    }

    if (doc["armDrone"].error() == simdjson::SUCCESS)
    {
        return MESSAGE_TYPE::ARM_COMMAND;
    }

    if (doc["takeoff"].error() == simdjson::SUCCESS)
    {
        return MESSAGE_TYPE::TAKE_OFF;
    }

    return MESSAGE_TYPE::UNKNOWN;
}

Manual_MessageReceived Json_AndroidMessageConverter::tryParseManualCommand(simdjson::padded_string& json)
{
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);
    double leftMove = doc["leftmove"].get_double_in_string();
    double leftRotation = doc["leftrotation"].get_double_in_string();
    double forwardMove = doc["forwardmove"].get_double_in_string();
    double motorPower = doc["motorpower"].get_double_in_string();
    return Manual_MessageReceived{
        leftMove,
        leftRotation,
        forwardMove,
        motorPower
    };
}

Arm_MessageReceived Json_AndroidMessageConverter::tryParseArmCommand(simdjson::padded_string& json)
{
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);
    bool armDrone = doc["armDrone"].get_bool();
    return Arm_MessageReceived{
        armDrone
    };
}

TakeOff_MessageReceived Json_AndroidMessageConverter::tryParseTakeOffCommand(simdjson::padded_string& json)
{
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json);
    if (doc["takeoff"].error() == simdjson::SUCCESS) {};
    bool takeOff = doc["takeoff"].get_bool();
    return TakeOff_MessageReceived{
        takeOff
    };
}