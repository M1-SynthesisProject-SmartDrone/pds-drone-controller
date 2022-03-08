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
#include <libconfig.h++>

#include "threads/Abstract_ThreadClass.h"
#include "threads/AndroidReceiver_ThreadClass.h"
#include "threads/AndroidSender_ThreadClass.h"
#include "threads/DroneSender_ThreadClass.h"
#include "threads/DroneReceiver_ThreadClass.h"
#include "network/Com_Serial.h"
#include "drone/Data_Drone.h"
#include "process/ProcessExecutor.h"

#include "android/message/tosend/Answer_MessageToSend.h"

using namespace std;

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
}

string getConfigPath(char *argv[])
{
    std::filesystem::path exePath(argv[0]);
    std::filesystem::path exeDirectoryPath = exePath.parent_path();
    return exeDirectoryPath.string() + "/config.cfg";
}

int main(int argc, char* argv[])
{
    loguru::init(argc, argv);
    LOG_F(INFO, "Start the 'SmartDroneController' application");

    // ==== READ CONFIG FILE ====
    libconfig::Config config;
    // The file must be aside the executable normally (done at cmake step)
    // If there is an issue, we must stop the app anyways, so let the exception throw
    config.readFile(getConfigPath(argv).c_str());

    const libconfig::Setting& root = config.getRoot();
    string tmpFolderPath = root.lookup("tmp_folder");

    const libconfig::Setting& droneSettings = root["drone"];
    int serialBaudrate = droneSettings["baudrate"];
    string serialPath = droneSettings["serial_path"];
    bool useDrone = droneSettings["use_drone"];
    
    const libconfig::Setting& appSettings = root["app"];
    int appSendPort = appSettings["send_port"];
    int appReceivePort = appSettings["receive_port"];

    const libconfig::Setting& exeSettings = root["exe_paths"];
    bool checkExesPresence = exeSettings["check_presence"];
    string saverExePath = exeSettings["path_saver"];

    auto drone = make_shared<Drone>();
    auto toDroneMessagesHolder = make_shared<ToDroneMessagesHolder>();
    auto toAppMessagesHolder = make_shared<ToAppMessagesHolder>();
    auto androidUdpSocket = make_shared<AndroidUDPSocket>(appReceivePort, appSendPort);
    auto messageConverter = make_shared<Json_AndroidMessageConverter>();
    auto pathRecorderHandler = make_shared<PathRecorderHandler>(tmpFolderPath);
    auto processExecutor = make_shared<ProcessExecutor>(saverExePath, checkExesPresence);

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