#include "process/ProcessExecutor.h"

#include <loguru/loguru.hpp>

using namespace std;
namespace fs = std::filesystem;

void throwIfInvalid(fs::path path)
{
    if (!fs::exists(path))
    {
        throw runtime_error("File does not exist or is not a file : " + path.string());
    }
}

ProcessExecutor::ProcessExecutor(std::string pathSaverFilename, bool checkFilePresence) : m_pathSaverPath(pathSaverFilename)
{
    LOG_F(INFO, "Check executables presence");
    if (checkFilePresence)
    {
        throwIfInvalid(m_pathSaverPath);
    }
}

ProcessExecutor::~ProcessExecutor()
{

}

void ProcessExecutor::launchSavePath(std::string inputFilename)
{
    string commandline = m_pathSaverPath.string() + " -i " + inputFilename;
    LOG_F(INFO, "Launch command '%s'", commandline.c_str());
    int returnCode = system(commandline.c_str());

    switch (returnCode)
    {
    case 1:
        throw runtime_error("An error happended during path saver process");
    case 0:
        return;
    default:
        throw runtime_error("Return code not handled");
    }
}
