#include <iostream>
#include <errno.h>
#include <thread>
#include <vector>
#include <random>
#include <stdexcept>
#include <filesystem>
#include <stdlib.h>
#include <sstream>

#include <loguru/loguru.hpp>
#include <cxxopts/cxxopts.hpp>

#include "config/ConfigParams.h"
#include "config/ConfigParser.h"

#include "threads/Abstract_ThreadClass.h"
#include "threads/AndroidReceiver_ThreadClass.h"
#include "threads/AndroidSender_ThreadClass.h"
#include "threads/DroneSender_ThreadClass.h"
#include "threads/DroneReceiver_ThreadClass.h"
#include "network/Com_Serial.h"
#include "business/drone/Data_Drone.h"
#include "process/ProcessExecutor.h"
#include "data_server/ApiHandler/ApiHandler.h"

using namespace std;

const short DRONE_TIMEOUT_LIMIT = 10;

void handleDrone(bool useDrone, shared_ptr<Drone> drone, char* serialPath, int serialBaudrate);
void checkDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate);

int main(int argc, char* argv[])
{
    loguru::init(argc, argv);
    LOG_F(INFO, "Start the 'SmartDroneController' application");

    ConfigParser configParser(argc, argv);
    ConfigParams params = configParser.parse();

    auto drone = make_shared<Drone>();
    auto toDroneMessagesHolder = make_shared<ToDroneMessagesHolder>();
    auto toAppMessagesHolder = make_shared<ToAppMessagesHolder>();
    auto androidMediator = make_shared<AndroidMediator>(params.appParams.receivePort, params.appParams.sendPort);
    auto messageConverter = make_shared<Json_AndroidMessageConverter>();
    auto pathRecorderHandler = make_shared<PathRecorderHandler>(params.globalParams.tmpFolderPath);
    auto processExecutor = make_shared<ProcessExecutor>(params.exesParams.saverExePath, params.exesParams.checkExesPresence);
    auto apiHandler = make_shared<ApiHandler>(params.dataServerParams.rootUrl);

    bool useDrone = params.droneParams.useDrone;
    handleDrone(useDrone, drone, params.droneParams.serialPath.data(), params.droneParams.serialBaudrate);

    // The list of threads used by the app
    vector<unique_ptr<Abstract_ThreadClass>> threads;
    threads.push_back(
        make_unique<AndroidReceiver_ThreadClass>(
            drone,
            androidMediator,
            pathRecorderHandler,
            toAppMessagesHolder,
            toDroneMessagesHolder,
            processExecutor,
            apiHandler
        ));
    threads.push_back(
        make_unique<AndroidSender_ThreadClass>(
            androidMediator, 
            toAppMessagesHolder
        ));
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
    else
    {
        LOG_F(WARNING, "No connection made to the drone");
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

void handleDrone(bool useDrone, shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
{
    if (useDrone)
    {
        try
        {
            checkDrone(drone, serialPath, serialBaudrate);
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
}

void checkDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
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
    // If GPS disabled
    // drone->setMode_manual();
    // If GPS enabled
    drone->setMode_position();
    usleep(1000 * 10); // 10ms
}