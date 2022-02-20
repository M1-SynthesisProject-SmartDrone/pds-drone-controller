#include "android/converter/Json_AndroidMessageConverter.h"

using namespace std;
using namespace rapidjson;

Json_AndroidMessageConverter::Json_AndroidMessageConverter()
{}

Json_AndroidMessageConverter::~Json_AndroidMessageConverter()
{}

Abstract_AndroidReceivedMessage* Json_AndroidMessageConverter::convertMessageReceived(string message)
{
    Document document;
    document.Parse<kParseStopWhenDoneFlag>(message.c_str());

    MESSAGE_TYPE messageType = findMessageType(document);

    Abstract_AndroidReceivedMessage* converted;
    try
    {
        if (!document["content"].IsObject())
        {
            throw invalid_argument("No content object found");
        }
        GenericObject<false, Value> content = document["content"].GetObject();
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

string Json_AndroidMessageConverter::convertToSendMessage(Abstract_AndroidToSendMessage* message)
{
    Document document;
    try
    {
        switch (message->messageType)
        {
        case MESSAGE_TYPE::ANSWER:
        {
            auto answer = static_cast<Answer_MessageToSend*>(message);
            document = convertAnswerMessage(answer);
        }
        break;
        case MESSAGE_TYPE::DRONE_DATA:
        {
            auto droneData = static_cast<DroneData_MessageToSend*>(message);
            document = convertDroneUpdateMessage(droneData);
        }
        break;
        default:
            throw invalid_argument("Message to send type unknown");
        }
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

MESSAGE_TYPE Json_AndroidMessageConverter::findMessageType(Document& doc)
{
    if (doc.HasMember("type"))
    {
        const char* type = doc["type"].GetString();
        // if comparison slow, we may use hases here
        if (strcmp(type, "MANUAL_CONTROL") == 0)
        {
            return MESSAGE_TYPE::MANUAL_CONTROL;
        }
        else if (strcmp(type, "START") == 0)
        {
            return MESSAGE_TYPE::START_COMMAND;
        }
        else if (strcmp(type, "ARM") == 0)
        {
            return MESSAGE_TYPE::ARM_COMMAND;
        }
        else if (strcmp(type, "TAKEOFF") == 0)
        {
            return MESSAGE_TYPE::TAKE_OFF;
        }
    }
    return MESSAGE_TYPE::UNKNOWN;
}

// ==== SPECIFIC MESSAGES ====
// ---- Received ----
Manual_MessageReceived* Json_AndroidMessageConverter::tryParseManualCommand(GenericObject<false, Value>& obj)
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

Start_MessageReceived* Json_AndroidMessageConverter::tryParseStartCommand(GenericObject<false, Value>& obj)
{
    bool start = obj["startDrone"].GetBool();
    return new Start_MessageReceived{
        start
    };
}

Record_MessageReceived* Json_AndroidMessageConverter::tryParseRecordCommand(GenericObject<false, Value>& obj)
{
    bool record = obj["record"].GetBool();
    return new Record_MessageReceived{
        record
    };
}

Arm_MessageReceived* Json_AndroidMessageConverter::tryParseArmCommand(GenericObject<false, Value>& obj)
{
    bool armDrone = obj["armDrone"].GetBool();
    return new Arm_MessageReceived{
        armDrone
    };
}

TakeOff_MessageReceived* Json_AndroidMessageConverter::tryParseTakeOffCommand(GenericObject<false, Value>& obj)
{
    bool takeOff = obj["takeOff"].GetBool();
    return new TakeOff_MessageReceived{
        takeOff
    };
}

// ---- To Send ----
Document createBaseDocument(string messageType)
{
    Document document;
    document.SetObject();

    Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember("type", messageType, allocator);
    return document;
}

Document Json_AndroidMessageConverter::convertAnswerMessage(Answer_MessageToSend* answer)
{
    Document document = createBaseDocument("ANSWER");
    Document::AllocatorType& allocator = document.GetAllocator();
    Value content(kObjectType);
    {
        content.AddMember("name", StringRef(answer->name.c_str()), allocator);
        content.AddMember("validated", answer->validated ? true : false, allocator);
        content.AddMember("message", StringRef(answer->message.c_str()), allocator);
    }
    document.AddMember("content", content, allocator);

    return document;
}

Document Json_AndroidMessageConverter::convertDroneUpdateMessage(DroneData_MessageToSend* droneData)
{
    Document document = createBaseDocument("DRONE_DATA");
    Document::AllocatorType& allocator = document.GetAllocator();
    Value content(kObjectType);
    {
        content.AddMember("batteryRemaining", droneData->batteryRemaining, allocator);
        content.AddMember("lat", droneData->lat, allocator);
        content.AddMember("lon", droneData->lon, allocator);
        content.AddMember("alt", droneData->alt, allocator);
        content.AddMember("relativeAlt", droneData->relativeAlt, allocator);
        content.AddMember("vx", droneData->vx, allocator);
        content.AddMember("vy", droneData->vy, allocator);
        content.AddMember("vz", droneData->vz, allocator);
        content.AddMember("yawRotation", droneData->yawRotation, allocator);
    }
    document.AddMember("content", content, allocator);

    return document;
}

Document convertDroneStatusMessage(DroneStatus_MessageToSend* droneStatus)
{
    Document document = createBaseDocument("DRONE_STATUS");
    Document::AllocatorType& allocator = document.GetAllocator();
    Value content(kObjectType);
    {
        content.AddMember("isArmed", droneStatus->isArmed, allocator);
    }
    document.AddMember("content", content, allocator);

    return document;
}