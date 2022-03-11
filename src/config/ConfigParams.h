#ifndef __CONFIGPARAMS_H__
#define __CONFIGPARAMS_H__

struct ConfigGlobal
{
    std::string tmpFolderPath;
};

struct ConfigDrone
{
    bool useDrone;
    int serialBaudrate;
    std::string serialPath;
};

struct ConfigApp
{
    int sendPort;
    int receivePort;
};

struct ConfigExes
{
    bool checkExesPresence;
    std::string saverExePath;
};

/**
 * The struct containing all config params
 */
struct ConfigParams
{
    ConfigGlobal globalParams;
    ConfigDrone droneParams;
    ConfigApp appParams;
    ConfigExes exesParams;

    ConfigParams(ConfigGlobal global, ConfigDrone drone, ConfigApp app, ConfigExes exes)
    {
        this->globalParams = global;
        this->droneParams = drone;
        this->appParams = app;
        this->exesParams = exes;
    }
};

#endif // __CONFIGPARAMS_H__