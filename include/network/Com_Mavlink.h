/**
 * @brief Tool class which convey to open a serial port
 * @file serial_port.cpp
 * 
 * @author Sylvain Colomer
 * @date 19/04/19
 * @version 1.1 
 */

#ifndef COMM_MAVLIN_H_
#define COMM_MAVLIN_H_

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>

//#include <loguru.hpp>

#include "../../lib/mavlink/common/mavlink.h"

#include "../Constants.h"
#include "Com_Serial.h"


/*
 * Mavlink library class
 *
 * Convey to make group of command with a better understand of how work the command (bc I comment them and test them...)
 */
class MavlinkTools
{

public:

	MavlinkTools();
	virtual ~MavlinkTools() = default;

	//HIGH LEVEL FUNCTION
	/**
	 * Arm function -> if param is >0.5 = arm. If param is <0.5 = unarm
	 */
	mavlink_message_t command_arm(float param1);
	mavlink_message_t command_disarm(float param1);
	mavlink_message_t command_right(float param1);
	mavlink_message_t command_left(float param1);
	mavlink_message_t command_up(float param1);
	mavlink_message_t command_down(float param1);
	mavlink_message_t command_pow(float param1);

	//PRIMITIVE
	/**
	 * Function which convey to get message from selected connection
	 * By defalt, we use serial port communication (radio or usb)
	 */
	int read_message_serial(mavlink_message_t &message, std::shared_ptr<Serial_Port> serial1); //int communicationWay

	/**
	 * Method that allow to write message with a specific communication protocol
	 */
	int write_message_serial(mavlink_message_t message, std::shared_ptr<Serial_Port> serial1);

	/**
	 * Function that allow to initialize a mavlink command
	 */
	mavlink_command_long_t mavlink_newCommand();
};



#endif // COMM_MAVLIN_H_


