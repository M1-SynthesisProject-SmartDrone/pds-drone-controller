#include <iostream>
#include <errno.h>
#include <thread>
#include <vector>
#include <random>
#include <stdexcept>
#include <stdlib.h>
#include <sstream>

#include <loguru/loguru.hpp>
#include <cxxopts/cxxopts.hpp>

#include "threads/Abstract_ThreadClass.h"
#include "threads/AndroidReceiver_ThreadClass.h"
#include "threads/AndroidSender_ThreadClass.h"
#include "threads/DroneSender_ThreadClass.h"
#include "threads/DroneReceiver_ThreadClass.h"
#include "network/Com_Serial.h"
#include "drone/Data_Drone.h"

#include "android/message/tosend/Answer_MessageToSend.h"

using namespace std;

const short ANDROID_UDP_PORT = 6869;
const short DRONE_TIMEOUT_LIMIT = 10000;

void initDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
{
    srand(time(0));
    LOG_F(INFO, "Try connecting to drone on %s with baudrate %d", serialPath, serialBaudrate);
    drone.get()->open(serialPath, serialBaudrate);
    if (drone->init_communication() != 0)
    {
        stringstream ss;
        ss << "Cannot init communication : " << strerror(errno);
        throw runtime_error(ss.str());
    }

    LOG_F(INFO, "Try init drone parameters");
    if (drone->init_parameters(DRONE_TIMEOUT_LIMIT) != 0)
    {
        stringstream ss;
        ss << "Cannot init parameters : " << strerror(errno);
        throw runtime_error(ss.str());
    }
    // TODO : create messages to handle this in a better way
    // If GPS enabled
    // drone->setMode_manual();
    // If GPS disabled
    drone->setMode_position();
    usleep(1000 * 10); // 10ms

    // drone->command_arm(1);
    // usleep(1000 * 10); // 10ms
}

int main(int argc, char* argv[])
{
    loguru::init(argc, argv);
    LOG_F(INFO, "Start the 'SmartDroneController' application");

    // Read command line arguments
    cxxopts::Options options("Test", "Test2");
    options.add_options()
        ("b,baudrate", "The baudrate to communicate with drone", cxxopts::value<int>()->default_value("57600"))
        ("s,serial", "The path on which communicate with drone", cxxopts::value<string>()->default_value("/dev/ttyUSB0"))
        ("p,port", "The port on which to listen the android application", cxxopts::value<uint16_t>()->default_value(to_string(ANDROID_UDP_PORT)))
        ("n,no_drone", "If this option is enabled, the app will not try to connect to the drone", cxxopts::value<bool>()->default_value("false"))
        ("f,folder_path", "Represent the folder where to put generated files", cxxopts::value<string>()->default_value("~/smart_drone_temp"));

    auto optionsParsed = options.parse(argc, argv);

    int serialBaudrate = optionsParsed["baudrate"].as<int>();
    string serialPath = optionsParsed["serial"].as<string>();
    uint16_t androidPort = optionsParsed["port"].as<uint16_t>();
    bool useDrone = !optionsParsed["no_drone"].as<bool>();
    string folderPath = optionsParsed["folder_path"].as<string>();

    auto drone = make_shared<Drone>();
    auto toDroneMessagesHolder = make_shared<ToDroneMessagesHolder>();
    auto toAppMessagesHolder = make_shared<ToAppMessagesHolder>();
    auto androidUdpSocket = make_shared<AndroidUDPSocket>(androidPort);
    auto messageConverter = make_shared<Json_AndroidMessageConverter>();
    auto pathRecorderHandler = make_shared<PathRecorderHandler>(folderPath);

    if (useDrone)
    {
        try
        {
            initDrone(drone, serialPath.data(), serialBaudrate);
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, "Error init drone. %s. Exit the app", e.what());
            sleep(1);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        LOG_F(INFO, "The drone connection is disabled for this run");
    }


    // The list of threads used by the app
    vector<unique_ptr<Abstract_ThreadClass>> threads;
    threads.push_back(
        make_unique<AndroidReceiver_ThreadClass>(
            androidUdpSocket,
            pathRecorderHandler,
            toAppMessagesHolder,
            toDroneMessagesHolder, 
            messageConverter));
    threads.push_back(
        make_unique<AndroidSender_ThreadClass>(
            drone, 
            androidUdpSocket, 
            toAppMessagesHolder, 
            messageConverter));
    if (useDrone)
    {
        threads.push_back(
            make_unique<DroneSender_ThreadClass>(
                drone, 
                toDroneMessagesHolder, 
                toAppMessagesHolder));
        threads.push_back(
            make_unique<DroneReceiver_ThreadClass>(
                drone,
                pathRecorderHandler,
                toAppMessagesHolder));
    }

    // start all threads
    LOG_F(INFO, "%ld threads stored", threads.size());
    for (auto& thread : threads)
    {
        thread->start();
    }

    for (auto& thread : threads)
    {
        thread->join();
    }

    return EXIT_SUCCESS;
}