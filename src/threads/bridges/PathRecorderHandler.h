#ifndef __PATHRECORDERHANDLER_H__
#define __PATHRECORDERHANDLER_H__

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <mutex>
#include <functional>
#include <exception>

#include <mavlink/common/mavlink.h>

/**
 * This class is used to create and handle path registration processes.
 * 
 * @author Aldric Vitali Silvestre
 */

class PathRecorderHandler
{
private:
    static inline const char CHARSET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static const size_t MAX_CHARSET_IDX = 63;
    static const char CSV_SEPARATOR = ';';

    std::mutex m_lock;
    std::filesystem::path m_baseFolderPath;
    std::filesystem::path m_currentFilePath;
    std::ofstream m_currentStream;

    bool m_isRecording = false;

    const uint8_t NAME_LENGTH = 8;
    std::function<char(void)> randomChar = [] {return CHARSET[rand() % MAX_CHARSET_IDX];};
    
    std::string createRandomString();
    std::string intToCoordinate(int32_t n);
public:

    /**
     * Create a new file with a random name ready for insertions.
     * Can throw an exception if the file cannot be created or there is already a recording
     */
    void startRecording();

    /**
     * Stop a currently started recording. This will throw an exception if no recording occured or 
     * an uknown IO error occurs.
     * 
     * @return The entire file name where the data was stored (absolute path)
     */
    std::string stopRecording();

    bool isRecording();

    /**
     * Convert the data and insert it in the file's record session.
     * This will do nothing if no record is currently running.
     */
    void insertData(int32_t latitude, int32_t longitude, int32_t altitude);

    PathRecorderHandler(std::string baseFolderName);
    ~PathRecorderHandler();
};

#endif // __PATHRECORDERHANDLER_H__