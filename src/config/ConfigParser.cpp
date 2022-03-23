#include "ConfigParser.h"

#include <libconfig.h++>

using namespace std;
namespace fs = std::filesystem;

string getConfigPath(char* argv[])
{
    std::filesystem::path exePath(argv[0]);
    std::filesystem::path exeDirectoryPath = exePath.parent_path();
    return exeDirectoryPath.string() + "/config.cfg";
}

ConfigParser::ConfigParser(int argc, char* argv[])
{
    // The file must be aside the executable normally (done at cmake step)
    // If there is an issue, we must stop the app anyways, so let the exception throw
    string filename = getConfigPath(argv);
    m_confFilePath = fs::path(filename);
    if (!fs::exists(m_confFilePath))
    {
        throw runtime_error("Config file not found: " + filename);
    }
}

ConfigParser::~ConfigParser() {}

ConfigParams ConfigParser::parse()
{
    libconfig::Config config;

    config.readFile(m_confFilePath.c_str());

    const libconfig::Setting& root = config.getRoot();
    auto globalConfig = ConfigGlobal{
        root.lookup("tmp_folder")
    };

    const libconfig::Setting& droneSettings = root["drone"];
    auto droneConfig = ConfigDrone{
        droneSettings["use_drone"],
        droneSettings["baudrate"],
        droneSettings["serial_path"],
    };

    const libconfig::Setting& appSettings = root["app"];
    auto appConfig = ConfigApp{
        appSettings["send_port"],
        appSettings["receive_port"]
    };

    const libconfig::Setting& exeSettings = root["exe_paths"];
    auto exesConfig = ConfigExes{
        exeSettings["check_presence"],
        exeSettings["path_saver"]
    };

    const libconfig::Setting& dataServerSettings = root["data_server"];
    auto dataServerConfig = ConfigDataServer{
        dataServerSettings["root_url"],
    };

    return ConfigParams(
        globalConfig,
        droneConfig,
        appConfig,
        exesConfig,
        dataServerConfig
    );
}

