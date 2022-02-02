#include <iostream>
#include <errno.h>
#include <thread>
#include <vector>
#include <stdexcept>
#include <stdlib.h>
#include <sstream>

#include <loguru/loguru.hpp>
#include <cxxopts/cxxopts.hpp>

#include "threads/Abstract_ThreadClass.h"
#include "threads/AndroidReceiver_ThreadClass.h"
#include "threads/DroneSender_ThreadClass.h"
#include "threads/DroneReceiver_ThreadClass.h"
#include "threads/SharedMessage.h"
#include "network/Com_Serial.h"
#include "drone/Data_Drone.h"

using namespace std;

#define ANDROID_UDP_PORT 6969
#define DRONE_LIMIT 10000

void initDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
{
    LOG_F(INFO, "Try connecting to drone on %s with baudrate %d", serialPath, serialBaudrate);
    drone.get()->open(serialPath, serialBaudrate);
    if (drone.get()->init_communication() != 0)
    {
        stringstream ss;
        ss << "Cannot init communication : " << strerror(errno);
        throw runtime_error(ss.str());
    }

    LOG_F(INFO, "Try init drone parameters");
    if (drone.get()->init_parameters(DRONE_LIMIT) != 0)
    {
        stringstream ss;
        ss << "Cannot init parameters : " << strerror(errno);
        throw runtime_error(ss.str());
    }
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
        ("p,port", "The port on which to listen the android application", cxxopts::value<uint16_t>()->default_value("6969"))
        ("n,no_drone", "If this option is enabled, the app will not try to connect to the drone", cxxopts::value<bool>()->default_value("false"));

    auto optionsParsed = options.parse(argc, argv);

    int serialBaudrate = optionsParsed["baudrate"].as<int>();
    string serialPath = optionsParsed["serial"].as<string>();
    uint16_t androidPort = optionsParsed["port"].as<uint16_t>();
    bool useDrone = !optionsParsed["no_drone"].as<bool>();

    auto drone = make_shared<Drone>();
    char* serialPathChar = &serialPath[0];

    auto androidMessagesHolder = make_shared<AndroidMessagesHolder>();

    if (useDrone)
    {
        try
        {
            initDrone(drone, serialPathChar, serialBaudrate);
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
    threads.push_back(make_unique<AndroidReceiver_ThreadClass>(1000, 200, androidPort, androidMessagesHolder));
    if (useDrone)
    {
        threads.push_back(make_unique<DroneSender_ThreadClass>(1000, 200, drone, androidMessagesHolder));
        threads.push_back(make_unique<DroneReceiver_ThreadClass>(1000, 200, drone));
    }

    // start all threads
    LOG_F(INFO, "Stored %ld threads", threads.size());
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