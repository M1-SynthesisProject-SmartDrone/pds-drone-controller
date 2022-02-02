# Root Cmake. Use to link the different libraries
#
# @author  sylvain Colomer, P. Gaussier
# @version 1.0


#########################################################################################################
###                                       GENERAL CONFIG                                               ##
#########################################################################################################

message("%%% SYNTHESIS PROJECT : SMART DRONE")
message("\n%% GENERAL CONFIGURATION")

# GENERAL SETTINGS
project(SmartDrone CXX)
cmake_minimum_required(VERSION 3.2)
set(VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(APP_NAME SmartDroneController)

# OUTPUT DIR SETTINGS
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# COMPILER OPTIONS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG   " -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE " -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Warning : option are important for some lib


########################################################################################################
###                                       LIBRARY CONFIG                                              ##
########################################################################################################

message("\n%% EXTERNALS libs configuration")

message("- Checking : pkg")
find_package(PkgConfig REQUIRED)
if(PkgConfig_FOUND)
    message("-> succes " )
endif()

# message("- Checking : mavlink")
# if(EXISTS ${CMAKE_SOURCE_DIR}/lib/mavlink)
#     message("-> succes " )
# else()
#     message("Fail: You must install mavlink")
# endif()
# set(MAVLINK_LIB -I/lib/mavlink)
# include_directories(./lib/mavlink)

# message("- Checking : cxxopts")
# if(EXISTS ${CMAKE_SOURCE_DIR}/lib/cxxopts)
#     message("-> succes " )
# else()
#     message("Fail: You must install cxxopts")
# endif()
# set(CXXOPTS_LIB -I/lib/cxxopts)
# include_directories(./lib/cxxopts)

# message("- Checking : Loguru")
# if(EXISTS ${CMAKE_SOURCE_DIR}/lib/loguru)
#    message("-> succes " )
# else()
#    message("Fail: You must add loguru log system in lib folder")
# endif()
# set(LOGURU_LIB -I/lib/loguru)
# include_directories(./lib/loguru)

message("- Checking : simdjson")
if(EXISTS ${CMAKE_SOURCE_DIR}/lib/rapidjson)
   message("-> succes " )
else()
   message("Fail: You must add rapidjson files in lib folder")
endif()
set(RAPIDJSON_LIB -I/lib/rapidjson)
include_directories(./lib/rapidjson)

message("- Checking : Blc channels")
find_package(blc_channel REQUIRED)
find_package(blc_program REQUIRED)
include_directories( ${BL_INCLUDE_DIRS} )
add_definitions( ${BL_DEFINITIONS} )


message("- Checking : Thread")
find_package(Threads)

message("- Checking : Curses")
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})


######################################################################################################
###                                            LINKS                                                ##
######################################################################################################

link_libraries(
    # ${CXXOPTS_LIB}
   #  ${RAPIDJSON_LIB}
    ${BL_LIBRARIES} 
    # ${MAVLINK_LIB}
   #  ${LOGURU_LIB}
    ${CURSES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    dl
    panel 
)

include_directories(lib)


######################################################################################################
###                                         APPLICATION                                             ##
######################################################################################################

message("\n%% APPLICATION GENERATION")

add_subdirectory(lib)

file(GLOB_RECURSE drone_SRC 
    "lib/**"
    "include/*.h"
     "src/*.cpp"
)

# %% App files
add_executable(${APP_NAME} ${drone_SRC})

target_link_libraries(${APP_NAME} 
    cxxopts 
    mavlink
    loguru
)