#ifndef __CONFIGPARSER_H__
#define __CONFIGPARSER_H__

#include <string>
#include <filesystem>

#include "ConfigParams.h"

class ConfigParser
{
private:
    std::filesystem::path m_confFilePath;
public:
    ConfigParser(int argc, char *argv[]);
    ~ConfigParser();

    ConfigParams parse();
};

#endif // __CONFIGPARSER_H__