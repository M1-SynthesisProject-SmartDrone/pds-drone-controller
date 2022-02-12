/**
 * @brief Tool class which convey to open a serial port
 * @file serial_port.cpp
 *
 * @author Sylvain ColomerJoystickModeAttitude
 * @date 19/04/19
 * @version 1.1
 */

#ifndef DRONE_H
#define DRONE_H


#include <mavlink/common/mavlink.h>

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <math.h>
#include <sys/time.h>

#include "network/Com_Serial.h"
#include "network/Com_Wifi.h"

#include "drone/DroneManualCommand.h"

 //#include "common/mavlink_msg_compassmot_status.h"

 /**
  * Define about drone communication protocol. it's use to automatic open specific communication
  */
enum Drone_Communication
{
	DRONE_WIFI,
	DRONE_SERIAL,
	DRONE_DUAL
};

/**
 * define about drone mode
 */
enum Drone_Motors
{
	ARM,
	UNARM
};

/**
 * define about drone mode
 */
enum Drone_mode
{
	DRONE_OFF,
	DRONE_MANUAL_DIRECT

	// Mode manual with direct control of the motors
};


enum class FlightMode
{
	Unknown,
	Ready,
	Takeoff,
	Hold,
	Mission,
	ReturnToLaunch,
	Land,
	Offboard,
	FollowMe,
	Manual,
	Altctl,
	Posctl,
	Acro,
	Rattitude,
	Stabilized,
};


namespace px4
{

	enum PX4_CUSTOM_MAIN_MODE
	{
		PX4_CUSTOM_MAIN_MODE_MANUAL = 1,
		PX4_CUSTOM_MAIN_MODE_ALTCTL,
		PX4_CUSTOM_MAIN_MODE_POSCTL,
		PX4_CUSTOM_MAIN_MODE_AUTO,
		PX4_CUSTOM_MAIN_MODE_ACRO,
		PX4_CUSTOM_MAIN_MODE_OFFBOARD,
		PX4_CUSTOM_MAIN_MODE_STABILIZED,
		PX4_CUSTOM_MAIN_MODE_RATTITUDE
	};

	enum PX4_CUSTOM_SUB_MODE_AUTO
	{
		PX4_CUSTOM_SUB_MODE_AUTO_READY = 1,
		PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF,
		PX4_CUSTOM_SUB_MODE_AUTO_LOITER,
		PX4_CUSTOM_SUB_MODE_AUTO_MISSION,
		PX4_CUSTOM_SUB_MODE_AUTO_RTL,
		PX4_CUSTOM_SUB_MODE_AUTO_LAND,
		PX4_CUSTOM_SUB_MODE_AUTO_RTGS,
		PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET
	};

	union px4_custom_mode
	{
		struct
		{
			uint16_t reserved;
			uint8_t main_mode;
			uint8_t sub_mode;
		};
		uint32_t data;
		float data_float;
	};

} // namespace px4


/**
 * Drone classes
 */
class Drone
{

public:

	Drone_Communication drone_communication;

	Drone_Motors motors = UNARM;
	// We at least want to know if drone is flying
	bool tookOff = false;

	//	Drone_mode mode=DRONE_OFF;


	//Communication part
	Drone_Communication communication = DRONE_SERIAL;
	std::shared_ptr<Serial_Port> serial1;
	std::shared_ptr<Wifi_Port> wifi1;


	//Drone system var
	uint8_t system_id = -1;
	uint8_t component_id = -1;
	uint8_t autopilot_id = -1;

	// Var use in remote mode to control the drone. 
	// Warning: if the value are highter than 0 before, it will be dangerous because the drone will make an hight jump
	int remote_x = 0;
	int remote_y = 0;
	int remote_z = 0;
	int remote_r = 0;

	// Timestamp use for relative navigation
	uint64_t timestamps;

	//Drone buffers corresponding to input var. Very usefull for GPS navigation 
	mavlink_heartbeat_t heartbeat; //Heartbeat. gather Id and others
	mavlink_command_ack_t ack; //Ack command: one of the most usefull parameters. it convey to read return of command
	mavlink_sys_status_t sys_status;
	mavlink_ping_t ping;
	mavlink_param_value_t param_value;
	mavlink_servo_output_raw_t servo_output_raw;
	mavlink_battery_status_t battery_status;
	mavlink_autopilot_version_t autopilot_version;
	mavlink_estimator_status_t estimator_status;
	mavlink_vibration_t vibration;
	mavlink_extended_sys_state_t extended_sys_state;
	mavlink_radio_status_t radio_status;

	mavlink_highres_imu_t highres_imu; //Accelerometers of the system
	mavlink_altitude_t altitude; //Altitude data
	mavlink_home_position_t home_position;

	mavlink_attitude_t attitude; //Attitude of the drone use to manual control and target attitude
	mavlink_attitude_target_t attitude_target; //Attitude of the drone use to manual control and target attitude

	mavlink_global_position_int_t global_position_int;

	mavlink_local_position_ned_t local_position_ned;
	mavlink_position_target_local_ned_t position_target_local_ned;
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_command_long_t command_long;

	int departure_alt;

public:

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONTRUCTOR %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	Drone();

	/**
	 * Default constructor use for serial port system
	 */
	Drone(std::string serialPort_path, int serialPort_baudrate);

	~Drone();

	void open(char* serialPort_path, int serialPort_baudrate);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% COMMUNICATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		/**
	 * Function which convey to get message from selected connection
	 * By defalt, we use serial port communication (radio or usb)
	 */
	int read_message(mavlink_message_t& message); //int communicationWay

	/**
	 * Method that allow to write message with a specific communication protocol
	 */
	int write_message(mavlink_message_t message);

	/**
	 *
	 */
	int write_message(mavlink_message_t message, Drone_Communication protocol);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% INITIALISATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	int testThrottle(int motorId);

	int test();

	/**
	 * Init communication system
	 */
	int init_communication();

	/**
	 * Function use to automatic initialize the drone with time limit
	 * Need to have before intialize the communication
	 */
	int init_parameters(uint limit);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%% CHANGE MODES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/**
	 * This mode need the GPS to be active and the drone to be in a stabilized state
	 */
	int setMode_position();

	/**
	 * This mode does not require the GPS to be active, but the drone will not react to wind blows affecting its position
	 */
	int setMode_altitude();

	int setMode_manual();

	int setMode_stabilized();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%% MANUAL_CONTROL API %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/**
	 * Send a command to control the drone with the same api as with a joystick.
	 */
	int manual_control(DroneManualCommand manualCommand);

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONTROL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/**
	 * Convey to change the mode of the drone
	 */
	int command_setMode(Drone_mode my_mode);


	int command_arm(int param1);

	//int command_float(int param1);


	int command_kill(int param1);

	//	int command_setModeGuided(float param1);
	int make_command_msg_rate(int param1);
	int command_directControl(float x, float y, float z, float r);
	int move_drone_to(float x, float y, float z);
	int return_to_launch(int param1);
	int take(int param1);
	int take_off();
	int landing();
	int command_right(float param1);
	int command_left(float param1);
	int go_to();
	int waypoint(float param5, float param6, float param7);
	int battery_check(float x);


	int waypointforback(float param6);
	int waypointupdown(float param7);
	int take_off_vtol();

	int followhold();
	int make_mode(int param1);
	int get_home_x(float x);

	int request_home_position(float x, float y, float z);
	void send_setpoint_velocity(float vx, float vy, float vz);

	int move_to(float param1, float param2, float param3);

	int command_up(float param1);
	int command_down(float param1);
	int command_pow(float param1);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ? %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	mavlink_command_long_t mavlink_newCommand();
};



#endif // SERIAL_PORT_H_


