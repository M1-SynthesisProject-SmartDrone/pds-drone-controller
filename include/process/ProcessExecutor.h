#ifndef __PROCESSEXECUTOR_H__
#define __PROCESSEXECUTOR_H__

/**
 * This class is the bridge permitting to run all programs linked to the project
 * 
 * @author Aldric Vitali Silvestre
 */ 

#include <string>
#include <filesystem>
#include <loguru/loguru.hpp>

class ProcessExecutor
{
private:
    std::filesystem::path m_pathSaverPath;
public:
    ProcessExecutor(std::string pathSaverFilename);
    ~ProcessExecutor();

    void launchSavePath(std::string inputFilename);
};
#endif // __PROCESSEXECUTOR_H__