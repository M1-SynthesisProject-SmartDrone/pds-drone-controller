#include "../../../include/android/message/Json_AndroidMessageConverter.h"

// Library used : https://github.com/simdjson/simdjson
#include "../../../lib/simdjson/simdjson.h"
#include "../../../lib/loguru/loguru.hpp"

#include <stdexcept>

using namespace std;

Json_AndroidMessageConverter::Json_AndroidMessageConverter()
{
}

Json_AndroidMessageConverter::~Json_AndroidMessageConverter()
{
}

AndroidMessageReceived Json_AndroidMessageConverter::convertMessageReceived(std::string message)
{
    simdjson::ondemand::parser parser;
    auto json = simdjson::padded_string(message);

    auto doc = parser.iterate(json);
    // int not supported by lib

    // example message {"leftmove":"0.000","leftrotation":"0.560","forwardmove":"0.000","motorpower":"0.342"}

    double leftMove;
    double leftRotation;
    double forwardMove;
    double motorPower;

    // If any error occured while reading this message, stop all
    try
    {
        leftMove = doc["leftmove"].get_double_in_string();
        leftRotation = doc["leftrotation"].get_double_in_string();
        forwardMove = doc["forwardmove"].get_double_in_string();
        motorPower = doc["motorpower"].get_double_in_string();
    }
    catch (exception &e)
    {
        LOG_F(ERROR, "Cannot parse the json %s : %s", message.c_str(), e.what());
        throw invalid_argument("Json value is not parsable");
    }

    AndroidMessageReceived converted = {
        leftMove = leftMove,
        leftRotation = leftRotation,
        forwardMove = forwardMove,
        motorPower = motorPower};
    return converted;
}