/**
 * @brief Class that model a drone
 * 
 * @author Sylvain Colomer
 * @date 23/04/19
 * @version 1.0
 */

#include <chrono>
#include <ctime>    

#include <iostream>
#include <string>

using namespace std;


#include "../../include/drone/Data_Drone.h"


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONTRUCTOR %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * Default constructor. Initialize var to first value
 */
Drone::Drone()
{

}

Drone::Drone(std::string serialPort_path, int serialPort_baudrate)
{
    communication=DRONE_SERIAL;
    cout<<"Data_Drone  "<< serialPort_path<<"\n";
    serial1 = std::shared_ptr<Serial_Port>(new Serial_Port(serialPort_path, serialPort_baudrate));
}

void Drone::open(char  *serialPort_path, int serialPort_baudrate)
{
    communication=DRONE_SERIAL;
    cout<<"Data_Drone  "<< serialPort_path<<"\n";
    serial1 = std::shared_ptr<Serial_Port>(new Serial_Port(serialPort_path, serialPort_baudrate));
}



Drone::~Drone()
{
    serial1.get()->close_serial();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% COMMUNICATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Drone::read_message(mavlink_message_t &message)
{
    if(communication == DRONE_SERIAL)
    {
    return serial1.get()->read_message(message);
    }
    return 1;
}

int Drone::write_message(mavlink_message_t message)
{
    if(communication == DRONE_SERIAL)
    {
        return serial1.get()->write_message(message);
    }
    else if(communication == DRONE_WIFI)
    {
        return 1;
    }
    return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% INITIALISATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Drone::init_communication()
{
    if(communication==DRONE_SERIAL)
    {
        return Drone::serial1.get()->open_serial();
    }

    return 0;
}
/*
void Drone::send_setpoint_velocity(float vx, float vy, float vz) {

		/* Documentation start from bit 1 instead 0,
		 * but implementation PX4 Firmware #1151 starts from 0
		 */
	//	uint16_t ignore_all_except_v_xyz = (7<<6)|(7<<0);

		// ENU->NED. Issue #49.
	/*	set_position_target_local_ned(std::chrono::system_clock::now(),
				MAV_FRAME_LOCAL_NED,
				ignore_all_except_v_xyz,
				0.0, 0.0, 0.0,
				vy, vx, -vz,
				0.0, 0.0, 0.0);
	} 
	
*/
/**
 * Initialisation of a drone 
 */
int Drone::init_parameters(uint limit)
{
    mavlink_message_t mavlink_message;
    bool flag_test = true;
    bool flag_success = true;
    uint counter=0;
    std::string buffer1="";

    while(flag_test)
    {
        
        flag_success = serial1.get()->read_message(mavlink_message);
        if(flag_success)
        {
            system_id = mavlink_message.sysid;
            component_id = mavlink_message.compid;
            
            flag_test=false;
        }
        
        if(counter >= limit) //Computation limit test
        {
            //Display_IHM::getInstanceOf().printLog("-> Error, no drone detected");
            return 1;
        }
        counter++;
        usleep(1000);
    }   

    return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%% MANUAL_CONTROL API %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Drone::manual_control(DroneManualCommand manualCommand)
{
    // From the implementation of the message sent by joystick : 
    // https://github.com/mavlink/qgroundcontrol/blob/master/src/Vehicle/Vehicle.cc (method "sendJoystickDataThreadSafe")
    mavlink_message_t message;
    short msgLength = mavlink_msg_manual_control_pack(
        1, // system id
        1, // component id
        &message,
        1, //target id
        manualCommand.x,
        manualCommand.y,
        manualCommand.z,
        manualCommand.r,
        0 // buttons, none taken account for now
    );
    return write_message(message);
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONTROL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// int Drone::disable_gps()

/**
 * Arm with 1, un arm with 0
 */
int Drone::command_arm(int param1)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.confirmation = false;
    command.param1           = param1;
	command.param2           = 21196;     //21196;  Force Arm/disarm  0 // ????

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }

    return write_message(message);
}


int Drone::make_mode(int param1)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_SET_MODE;
 //   command.confirmation = false;
    command.param1           = 193;
	command.param2           = 1;     //21196;   // ????
	command.param3 = 0;
    mavlink_msg_command_long_encode(255, 1, &message, &command);

    if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }

    return write_message(message);
}



int Drone::make_command_msg_rate(int param1)
{	
	
	float interval_us = 0.0f;


        
  
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_SET_MESSAGE_INTERVAL;
    command.param1           = 1; //rate
	command.param2           = 1; //intervall en us


    mavlink_msg_command_long_encode(255, 1, &message, &command);

    if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }

    return write_message(message);
}



int Drone::waypoint(float param5, float param6, float param7)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_WAYPOINT;
    command.confirmation = true;
    command.param1           = 100;  //Hold time. (ignored by fixed wing, time to stay at waypoint for rotary wing) min : 0 (seconds)
	command.param2           = 0;	//Acceptance radius (if the sphere with this radius is hit, the waypoint counts as reached)  min 0 (m)
	command.param3           = 0;	//0 to pass through the WP, if > 0 radius to pass by WP. Positive value for clockwise orbit, negative value for counter-clockwise orbit. Allows trajectory control. (m)
	command.param4           = 0;	//Desired yaw angle at waypoint (rotary wing). NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.). (deg)
	command.param5           = param5;	//Latitude (m)
	command.param6           = param6;	//Longitude
	command.param7           = param7;	//Altitude
    mavlink_msg_command_long_encode(255, 1, &message, &command);
    return write_message(message);
}

int Drone::waypointforback(float param6)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_WAYPOINT;
    command.confirmation = false;
    command.param1           = 0;  //Hold time. (ignored by fixed wing, time to stay at waypoint for rotary wing) min : 0 (seconds)
	command.param2           = 0;	//Acceptance radius (if the sphere with this radius is hit, the waypoint counts as reached)  min 0 (m)
	command.param3           = 0;	//0 to pass through the WP, if > 0 radius to pass by WP. Positive value for clockwise orbit, negative value for counter-clockwise orbit. Allows trajectory control. (m)
	command.param4           = 0;	//Desired yaw angle at waypoint (rotary wing). NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.). (deg)
//	command.param5           = 0;	//Latitude (m)
	command.param6           = 0;	//Longitude
//	command.param7           = 0;	//Altitude
    mavlink_msg_command_long_encode(255, 1, &message, &command);
    return write_message(message);
}

int Drone::waypointupdown(float param7)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_WAYPOINT;
    command.confirmation = false;
    command.param1           = 0;  //Hold time. (ignored by fixed wing, time to stay at waypoint for rotary wing) min : 0 (seconds)
	command.param2           = 0;	//Acceptance radius (if the sphere with this radius is hit, the waypoint counts as reached)  min 0 (m)
	command.param3           = 0;	//0 to pass through the WP, if > 0 radius to pass by WP. Positive value for clockwise orbit, negative value for counter-clockwise orbit. Allows trajectory control. (m)
	command.param4           = 0;	//Desired yaw angle at waypoint (rotary wing). NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.). (deg)
//	command.param5           = 0;	//Latitude (m)
//	command.param6           = 0;	//Longitude
	command.param7           = 0;	//Altitude
    mavlink_msg_command_long_encode(255, 1, &message, &command);
    return write_message(message);
}

int Drone::go_to()
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_REPOSITION;
    command.confirmation = false;
    command.param1           = -1;
	command.param4           = NAN;
	command.param5           = 0;
	command.param6           = 0;
	command.param7           = 1;


    mavlink_msg_command_long_encode(255, 1, &message, &command);

   
    return write_message(message);
}



int Drone::return_to_launch(int param1)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_RETURN_TO_LAUNCH;
  //  command.param1           = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }

    return write_message(message);
}

int Drone::take_off() 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	printf("===========================  take_off Demande au drone de monter en altitude \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_TAKEOFF;  
    command.confirmation = true ;
  //  command.param1 = 245;  //Minimum pitch (if airspeed sensor present), desired pitch without sensor
	command.param2 = 1e+06;
	command.param3 = NAN; 
	command.param4 = NAN; //Yaw angle (if magnetometer present), ignored without magnetometer. NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.).
	command.param5 =NAN ;  //global_position_int.lat; // local_position_ned.x;	//Latitude
	command.param6 =NAN; //global_position_int.lon; //local_position_ned.y;    //longitude 
	printf("departure alt = %d \n",departure_alt);
	command.param7 = 1 ; // 10m altitude
	
	
    mavlink_msg_command_long_encode(255, 1, &message, &command);
	
	cout <<"param1"<<command.param1<<"param2"<<command.param2<<"param3"<<command.param3<<"param4 "<<command.param4<<"param5 "<<command.param5<<"param6 "<<command.param6<<"param7 "<<command.param7<<endl;



    return  write_message(message);
}
int Drone::take_off_vtol() 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	printf("===========================  take_off Demande au drone de monter en altitude \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_NAV_VTOL_TAKEOFF;  
    command.confirmation = true ;
   // command.param1 = 0;  //Pitch
	//command.param2 = VTOL_TRANSITION_HEADING;
	//command.param3 = 0;
	command.param4 = NAN; //YAW
	command.param5 = gps_raw_int.lat;  //global_position_int.lat; // local_position_ned.x;	//Latitude
	command.param6 = gps_raw_int.lon; //global_position_int.lon; //local_position_ned.y;    //longitude 
	printf("departure alt = %d \n",departure_alt);
	command.param7 = departure_alt+1000; // 10m altitude

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}

int Drone::testThrottle(int motorId)
{
    mavlink_message_t message;
    mavlink_command_long_t command;
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_MOTOR_TEST;  
    command.confirmation = true ;
    command.param1 = motorId;  //Hold mode
	command.param2 = MOTOR_TEST_THROTTLE_PERCENT;
	command.param3 = 10;
	command.param4 = 0;
	command.param5 = 0;
	command.param6 = MOTOR_TEST_ORDER_DEFAULT;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}

int Drone::test()
{
    mavlink_message_t message;
    mavlink_command_long_t command;
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_MOTOR_TEST;  
    command.confirmation = true ;
    command.param1 = 1;  //Hold mode
	command.param2 = MOTOR_TEST_THROTTLE_PERCENT;
	command.param3 = 50;
	command.param4 = 0;
	command.param5 = 0;
	command.param6 = MOTOR_TEST_ORDER_DEFAULT;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}

int Drone::followhold() 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	printf("===========================  take_off Demande au drone de monter en altitude \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_FOLLOW;  
    command.confirmation = true ;
    command.param1 = 0;  //Hold mode
	//command.param2 = VTOL_TRANSITION_HEADING;
	//command.param3 = 0;
	command.param4 = 2; //Altitude mode: 0: Keep current altitude, 1: keep altitude difference to target, 2: go to a fixed altitude above home.
	command.param5 = departure_alt+1000; // 10m altitudeAltitude above home. (used if mode=2)
	//command.param6 = gps_raw_int.lon; //global_position_int.lon; //local_position_ned.y;    //longitude 
	printf("departure alt = %d \n",departure_alt);
	command.param7 = 1; //Time to land in which the MAV should go to the default position hold mode after a message RX timeout.

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}




int Drone::landing() 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	printf("=============================  landing Demande au drone de faire un landing \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
     command.command = MAV_CMD_NAV_LAND;  
    command.confirmation = true ;
    command.param1 = 1;			//Minimum target altitude if landing is aborted (0 = undefined/use system default).
	command.param2 = 0;			//Precision land mode. //PRECISION_LAND_MODE
	command.param3 = 0;			//Empty
	command.param4 = 1.5;		//Desired yaw angle. NaN to use the current system yaw heading mode (e.g. yaw towards next waypoint, yaw to home, etc.).
	command.param5 = 1;	//Latitude
	command.param6 = 1;	//Longitude
	command.param7 = 10; 	//Landing altitude (ground level in current frame).

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}

//============================================TEST=============================================================



int Drone::move_to(float param1, float param2, float param3) 
{
	int16_t xx,yy,zz;
	xx=(int16_t)param1; yy=(int16_t)param2; zz=(int16_t)param3;
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	  // printf("Demande au drone de tourner vers la droite \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAVLINK_MSG_ID_POSITION_TARGET_LOCAL_NED;
    command.confirmation = false;
    command.param5 = 1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

   if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }
    return write_message(message);
}

//===============================================================================================================


int Drone::command_right(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	  // printf("Demande au drone de tourner vers la droite \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_FRAME_BODY_FRD;
    command.confirmation = false;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

   if(param1 >0)
    {
        //Display_IHM::getInstanceOf().printLog("ARMING");
    }else
    {
        //Display_IHM::getInstanceOf().printLog("STOP ARMING");
    }

    return write_message(message);
}




int Drone::command_kill(int param1)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;

    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_FLIGHTTERMINATION;
    command.confirmation = true;
    command.param1           = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);
    if(param1>0)
    {
        //Display_IHM::getInstanceOf().printLog("KILL MODE");
    }else{
        //Display_IHM::getInstanceOf().printLog("OUT KILL MODE");
    }

    return write_message(message);
}

/*int Drone::command_setModeGuided(float param1)
{
    //Message buffer
	mavlink_message_t message;
    mavlink_command_long_t command;
    command=mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command          = MAV_CMD_NAV_GUIDED_ENABLE;
    command.confirmation     = true; //false;
    command.param1           = param1; // flag >0.5 => start, <0.5 => stop

    mavlink_msg_command_long_encode(255, 1, &message, &command);
    if(param1>0)
    {
        //Display_IHM::getInstanceOf().printLog("GUIDED MODE");
    }else{
        //Display_IHM::getInstanceOf().printLog("OUT GUIDED MODE");
    }

    return write_message(message);
}
/*
 * set_landing()
{
	// Prepare command for landing
	mavlink_command_long_t com = { 0 };
	com.target_system    = system_id;
	com.target_component = autopilot_id;
	com.command          = MAV_CMD_NAV_LAND;
	com.confirmation     = true;
	com.param1           = (float) flag; // flag >0.5 => start, <0.5 => stop

	// Encode
	mavlink_message_t message;
	mavlink_msg_command_long_encode(system_id, companion_id, &message, &com);

	// Send the message
	int len = serial_port->write_message(message);

	// Done!
	return;
}*/

int Drone::command_directControl(float x, float y, float z, float r)
{
	mavlink_message_t message;
	int16_t xx,yy,zz,rr;
	xx=(int16_t)x; yy=(int16_t)y; zz=(int16_t)z; rr=(int16_t)r;
  //  mavlink_msg_manual_control_pack(255, 1, &message, 1, 0, xx, yy, zz, rr);
        mavlink_msg_manual_control_pack(255, 1, &message, 1, xx, yy, zz, rr,1); //xx :  Generally corresponds to forward(1000)-backward(-1000) movement on a joystick and the pitch of a vehicle. 
															//yy : Generally corresponds to left(-1000)-right(1000) movement on a joystick and the roll of a vehicle. 
															//zz : Generally corresponds to a separate slider movement with maximum being 1000 and minimum being -1000 on a joystick and the thrust of a vehicle.
															//rr : Generally corresponds to a twisting of the joystick, with counter-clockwise being 1000 and clockwise being -1000, and the yaw of a vehicle. 

  cout<<"command_directControl  : "<<xx<<" "<<yy<<" "<<zz<<" "<<rr<<" "<<endl;
  return write_message(message);
// return 1;
}

int Drone::battery_check(float x)
{
	cout<<"current_consumed :"<<battery_status.current_consumed<<endl;
	cout<<"=========================================="<<endl;
	cout<<"energy_consumed :"<<battery_status.energy_consumed<<endl;
	cout<<"=========================================="<<endl;
	cout<<"temperature :"<<battery_status.temperature<<endl;
	cout<<"=========================================="<<endl;
	cout<<"voltages :"<<battery_status.voltages[10]<<endl;
	cout<<"=========================================="<<endl;
	cout<<"current_battery :"<<battery_status.current_battery<<endl;
	cout<<"=========================================="<<endl;
	cout<<"battery_function :"<<battery_status.battery_function<<endl;
	cout<<"=========================================="<<endl;
	cout<<"type :"<<battery_status.type<<endl;
	cout<<"=========================================="<<endl;
	cout<<"battery_remaining :"<<battery_status.battery_remaining<<endl;
	cout<<"=========================================="<<endl;
	cout<<"time_remaining :"<<battery_status.time_remaining<<endl;
	cout<<"=========================================="<<endl;
	cout<<"charge_state :"<<battery_status.charge_state<<endl;

}
int Drone::move_drone_to(float vx, float vy, float vz)
{
	mavlink_message_t message;
	uint8_t system_id = 255;
	uint8_t component_id = 1;
	
	uint32_t time_boot_ms = 1000;
	uint8_t target_system = 1;
	uint8_t target_component = 1;
	uint8_t coordinate_frame = MAV_FRAME_LOCAL_NED; //Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9 
	uint16_t mask = 0b0000111111000111; // only speed enabled //itmask to indicate which dimensions should be ignored by the vehicle: a value of 0b0000000000000000 or 0b0000001000000000 indicates that none of the setpoint dimensions should be ignored.
	float x =0 ; //	X Position in NED frame in meters 
	float y = 0; // 	Y Position in NED frame in meters 
	float z =0; // 	Z Position in NED frame in meters   (note, altitude is negative in NED) 
	float vvx = vx; //	X velocity in NED frame in meter / s	
	float vvy = vy; //Y velocity in NED frame in meter / s	
	float vvz = vz; //Z velocity in NED frame in meter / s 
	float ax = 0; //X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N 
	float ay = 0; //Y  acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N 
	float az = 0; // Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N 
	float yaw = 1.5; //yaw setpoint in rad 
	float  yaw_rate = 0; //yaw rate setpoint in rad/s 
   mavlink_msg_set_position_target_local_ned_pack(system_id, component_id, &message, time_boot_ms,target_system,target_component, coordinate_frame , mask, x,y,z, vvx, vvy, vvz, ax,ay,az, yaw, yaw_rate);
 //uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                  //             uint32_t time_boot_ms, uint8_t target_system, uint8_t target_component, uint8_t coordinate_frame, uint16_t type_mask,
                        //        float x, float y, float z, float vx, float vy, float vz, float afx, float afy, float afz, float yaw, float yaw_rate)
 // cout<<"command_directControl  : "<<xx<<" "<<yy<<" "<<zz<<" "<<rr<<" "<<endl;
  return write_message(message);
// return 1;
}

int Drone::request_home_position(float x, float y, float z)
{
	mavlink_message_t message;
    mavlink_command_long_t command;
	printf("===========================  REQUEST_HOME_POSITION \n");
    command = mavlink_newCommand();
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_GET_HOME_POSITION;  
    command.confirmation = true ;
   
   
   	printf("===========================  HOME_POSITION_OBTAINED \n");

	
	
}
int Drone::get_home_x(float x)
{
	printf("===========================  X_POSITION \n");
	mavlink_message_t message;
	mavlink_msg_home_position_get_x(&message);
	printf("===========================  X_POSITION_OBTAINED \n");

}




//static uint8_t mavlink_msg_manual_control_get_target individual autopilot specifications for details.| Empty| Empty| Empty| Empty|  */
//MANUAL_CONTROL
//RC_CHANNELS_OVERRIDE


//   MAV_CMD_SET_GUIDED_SUBMODE_STANDARD=4000, /* This command sets the submode to standard guided when vehicle is in guided mode. The vehicle holds position and altitude and the user can input the desired velocites along all three axes.
//   MAV_CMD_CONDITION_CHANGE_ALT=113, /* Ascend/descend at rate.  Delay mission state machine until desired altitude reached. |Descent / Ascend rate (m/s)| Empty| Empty| Empty| Empty| Empty| Finish Altitude|  */
 //  MAV_CMD_DO_SET_MODE=176, /* Set system mode. |Mode, as defined by ENUM MAV_MODE| Custom mode - this is system specific, please refer to the individual autopilot specifications for details.| Custom sub mode - this is system specific, please refer to the individual autopilot specifications for details.| Empty| Empty| Empty| Empty|  */
//ds.command = 213; //MAV_CMD_DO_SET_POSITION_YAW_THRUST;
//   MAV_CMD_NAV_LAND=21, /* Land at location |Abort Alt| Empty| Empty| Desired yaw angle| Latitude| Longitude| Altitude|  */
//   MAV_CMD_CONDITION_YAW=115, /* Reach a certain target angle. |target angle: [0-360], 0 is north| speed during yaw change:[deg per second]| direction: negative: counter clockwise, positive: clockwise [-1,1]| relative offset or absolute angle: [ 1,0]| Empty| Empty| Empty|  */

int Drone::command_left(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	// printf("Demande au drone de tourner vers la gauche \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
  //  command.command = MAV_CMD_DO_ORBIT;
    command.confirmation = (uint8_t) 0;
    command.param1 = (int16_t) param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}



int Drone::command_down(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	// printf("Demande au drone de descendre \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    //command.command = MAV_CMD_NAV_CONTINUE_AND_CHANGE_ALT;
    command.confirmation = (uint8_t) 0;
    command.param1 = (int16_t) param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return  write_message(message);
}

int Drone::command_pow(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	// printf("Demande augmentation de la puissance \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    //command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.confirmation = (uint8_t) 0;
    command.param1 = (int16_t) param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);
	
    return  write_message(message);
}

//void MockLink::_sendRCChannels(void) 
//{
   // mavlink_message_t   msg;

    // mavlink_msg_rc_channels_pack_chan(_vehicleSystemId,
       //                               _vehicleComponentId,
       //                               _mavlinkChannel,
       //                               &msg,
       //                               0,                     // time_boot_ms
       //                               16,                    // chancount
       //                               1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500,   // channel 1-8
       //                               1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500,   // channel 9-16
       //                               UINT16_MAX, UINT16_MAX,
       //                               0);                    // rssi

    //respondWithMavlinkMessage(msg);
// }




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ? %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mavlink_command_long_t Drone::mavlink_newCommand()
{
    mavlink_command_long_t com;
    com.target_system = 0;
    com.target_component = 0.;
    com.command = 0;
    com.confirmation = false;
    com.param1=0; 
    com.param2=0; 
    com.param3=0; 
    com.param4=0; 
    com.param5=0; 
    com.param6=0; 
    com.param7=0;
    return com;
}

/*
PARAM_REQUEST_LIST 
Request all parameters of this component. After this request, all parameters are emitted. The parameter microservice is documented at https://mavlink.io/en/services/parameter.html

mavlink_param_union_t param;
int32_t integer = 20000;
param.param_int32 = integer;
param.type = MAV_PARAM_TYPE_INT32;

// Then send the param by providing the float bytes to the send function
mavlink_msg_param_set_send(xxx, xxx, param.param_float, param.type, xxx);

??????   mavlink_msg_param_set_send ( 255, 1,  20000, MAV_PARAM_TYPE_INT32, ????)

static inline void mavlink_msg_param_set_send( mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, const char *param_id, float param_value, uint8_t param_type)
*/
/**
 * @brief Send a param_set message
 * @param chan MAVLink channel to send the message   : 255
 *
 * @param target_system  System ID  : 1 
 * @param target_component  Component ID
 * @param param_id  Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string
 * @param param_value  Onboard parameter value
 * @param param_type  Onboard parameter type.
 */


/*
 You need to assign base_mode, main_mode, and sub_mode.
For instance, in order to trigger ‘position’ flight mode, you can use the following pymavlink code. I think that it will be same in dronekit code.

master.mav.command_long_send ( master.target_system, master.target_component, mavutil.mavlink.MAV_CMD_DO_SET_MODE, 0, base_mode, main_mode, sub_mode, 0, 0, 0, 0)

sub_mode is only defined for the AUTO_MOD

master.mav.command_long_send( master.target_system, master.target_component, mavutil.mavlink.MAV_CMD_DO_SET_MODE, 0, 217, 3, 0, 0, 0, 0, 0)
 
    Manual: base_mode:217, main_mode:1, sub_mode:0
    Stabilized: base_mode:209, main_mode:7, sub_mode:0
    Acro: base_mode:209, main_mode:5, sub_mode:0
    Rattitude: base_mode:193, main_mode:8, sub_mode:0
    Altitude: base_mode:193, main_mode:2, sub_mode:0
    Offboard: base_mode:209, main_mode:6, sub_mode:0
    Position: base_mode:209, main_mode:3, sub_mode:0
    Hold: base_mode:217, main_mode:4, sub_mode:3
    Missition: base_mode:157, main_mode:4, sub_mode:4
    Return: base_mode:157, main_mode:4, sub_mode:5
    Follow me: base_mode:157, main_mode:4, sub_mode:8
*/

/*
std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
SystemImpl::make_command_flight_mode(FlightMode flight_mode, uint8_t component_id)
{
    const uint8_t flag_safety_armed = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed | flag_hitl_enabled;

    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //       but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;

    switch (flight_mode) {
        case FlightMode::Hold:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::ReturnToLaunch:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::Takeoff:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::Land:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::Mission:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FollowMe:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::Offboard:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        case FlightMode::Manual:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
            break;
        case FlightMode::Posctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
            break;
        case FlightMode::Altctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
            break;
        case FlightMode::Rattitude:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE;
            break;
        case FlightMode::Acro:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ACRO;
            break;
        case FlightMode::Stabilized:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
            break;
        default:
            LogErr() << "Unknown Flight mode.";
            MavlinkCommandSender::CommandLong empty_command{};
            return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
    }

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_SET_MODE;
    command.params.param1 = float(mode);
    command.params.param2 = float(custom_mode);
    command.params.param3 = float(custom_sub_mode);
    command.target_component_id = component_id;

    return std::make_pair<>(MavlinkCommandSender::Result::Success, command);
}
*/
//int Drone::command_setMode(Drone_mode my_mode)

 int Drone::command_setMode(Drone_mode my_mode)
{
    const uint8_t flag_safety_armed = MAV_MODE_FLAG_SAFETY_ARMED; // = is_armed() ? MAV_MODE_FLAG_SAFETY_ARMED : 0;
    const uint8_t flag_hitl_enabled = MAV_MODE_FLAG_HIL_ENABLED; // = _hitl_enabled ? MAV_MODE_FLAG_HIL_ENABLED : 0;

    const uint8_t mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | flag_safety_armed | flag_hitl_enabled;
//	Drone_mode my_mode;
    // Note: the safety flag is not needed in future versions of the PX4 Firmware
    //  but want to be rather safe than sorry.
    uint8_t custom_mode = px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    uint8_t custom_sub_mode = 0;	
    
   FlightMode flight_mode = FlightMode::Hold;
   custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
   
   /*  switch(my_mode)
    {
        case DRONE_OFF: 
        //We must reset all value to 0			
            Drone::mode=flight_mode;
        break;

        case DRONE_MANUAL_DIRECT: 
            if(remote_x!=0 && remote_y!=0 && remote_z!=0 && remote_r!=0){
                ////Display_IHM::getInstanceOf().printLog("Server: command refused");
                //beep();
            }else{
                //Display_IHM::getInstanceOf().printLog("Server: mode manual_direct");
                Drone::mode=flight_mode;
            }
        break;

        default:
        break;
    }
         return 0;
*/
	
//	FlightMode flight_mode = FlightMode::Hold;

    switch (flight_mode) {
        case FlightMode::Hold:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
            break;
        case FlightMode::ReturnToLaunch:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
            break;
        case FlightMode::Takeoff:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
            break;
        case FlightMode::Land:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
            break;
        case FlightMode::Mission:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
            break;
        case FlightMode::FollowMe:
            custom_sub_mode = px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
            break;
        case FlightMode::Offboard:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
            break;
        case FlightMode::Manual:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
            break;
        case FlightMode::Posctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
            break;
        case FlightMode::Altctl:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
            break;
        case FlightMode::Rattitude:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE;
            break;
        case FlightMode::Acro:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_ACRO;
            break;
        case FlightMode::Stabilized:
            custom_mode = px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
            break;
        default:
            cout << "Unknown Flight mode.";
        //    MavlinkCommandSender::CommandLong empty_command{};
        //    return std::make_pair<>(MavlinkCommandSender::Result::UnknownError, empty_command);
    }
	mavlink_message_t message;
    mavlink_command_long_t command;
    command = mavlink_newCommand();    
    command.target_system = 1;
    command.target_component = 1;
    command.command = MAV_CMD_DO_SET_MODE;
    command.param1 = float(mode);
    command.param2 = float(custom_mode);
    command.param3 = float(custom_sub_mode);
    //command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    mavlink_msg_command_long_encode(255, 1, &message, &command);
    return  write_message(message);
}


 //   MavlinkCommandSender::CommandLong command{};

    //Message buffer





  //  return std::make_pair<>(MavlinkCommandSender::Result::Success, command);

///==========================================================================

	
   


/*
toggle_offboard_control( bool flag )
{
	// Prepare command for off-board mode
	mavlink_command_long_t com;
	com.target_system    = system_id;
	com.target_component = autopilot_id;
	com.command          = MAV_CMD_NAV_GUIDED_ENABLE;
	com.confirmation     = true;
	com.param1           = (float) flag; // flag >0.5 => start, <0.5 => stop

	// Encode
	mavlink_message_t message;
	mavlink_msg_command_long_encode(system_id, companion_id, &message, &com);

	// Send the message
	int len = serial_port->write_message(message);

	// Done!
	return len;
}*/
