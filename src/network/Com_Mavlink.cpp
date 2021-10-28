/**
 * @brief Tool class which convey to open a serial port
 * @file serial_port.cpp
 * 
 * @author Sylvain Colomer
 * @date 19/04/19
 * @version 1.1 
 */

#include "../../include/network/Com_Mavlink.h"

MavlinkTools::MavlinkTools()
{
}

// //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% PRIMITIVE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int MavlinkTools::read_message_serial(mavlink_message_t &message, std::shared_ptr<Serial_Port> serial1) 
{
    return serial1.get()->read_message(message);
}

int MavlinkTools::write_message_serial(mavlink_message_t message, std::shared_ptr<Serial_Port> serial1) {
    return serial1.get()->write_message(message);
}

// //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% CONTROL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mavlink_message_t MavlinkTools::command_arm(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	 printf("Demande armement drone \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.confirmation = (uint8_t) 0;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);
	
    return message;
}

mavlink_message_t MavlinkTools::command_disarm(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	 printf("Demande desarmement drone \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    command.command = MAV_CMD_COMPONENT_ARM_DISARM;
    command.confirmation = (uint8_t) 0;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);
	
    return message;
}

mavlink_message_t MavlinkTools::command_right(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	  // printf("Demande au drone de tourner vers la droite \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    //command.command = MAV_CMD_DO_ORBIT;
    command.confirmation = (uint8_t) 0;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return message;
}

mavlink_message_t MavlinkTools::command_left(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	// printf("Demande au drone de tourner vers la gauche \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    //command.command = MAV_CMD_DO_ORBIT;
    command.confirmation = (uint8_t) 0;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return message;
}

mavlink_message_t MavlinkTools::command_up(float param1) 
{
    //Message buffer
    mavlink_message_t message;
    mavlink_command_long_t command;
	// printf("Demande au drone de monter en altitude \n");
    command = mavlink_newCommand();
    command.target_system = (uint8_t) 1;
    command.target_component = (uint8_t) 1;
    command.command = MAV_CMD_NAV_TAKEOFF;
    command.confirmation = (uint8_t) 0;
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return message;
}

mavlink_message_t MavlinkTools::command_down(float param1) 
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
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);

    return message;
}

mavlink_message_t MavlinkTools::command_pow(float param1) 
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
    command.param1 = param1;

    mavlink_msg_command_long_encode(255, 1, &message, &command);
	
    return message;
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


mavlink_command_long_t MavlinkTools::mavlink_newCommand() 
{
    mavlink_command_long_t com;
    com.target_system = 0;
    com.target_component = 0.;
    com.command = 0;
    com.confirmation = false;
    com.param1 = 0;
    com.param2 = 0;
    com.param3 = 0;
    com.param4 = 0;
    com.param5 = 0;
    com.param6 = 0;
    com.param7 = 0;
    return com;
}

//SNIPPET

// int MavlinkTools::command_kill(float param1) {
//     //Message buffer
//     mavlink_message_t message;
//     mavlink_command_long_t command;

//     command = mavlink_newCommand();
//     command.target_system = 1;
//     command.target_component = 1;
//     command.command = MAV_CMD_DO_FLIGHTTERMINATION;
//     command.confirmation = false;
//     command.param1 = param1;

//     mavlink_msg_command_long_encode(255, 1, &message, &command);
//     if (param1 > 0) {
//         //Display_IHM::getInstanceOf().printLog("KILL MODE");
//     } else {
//         //Display_IHM::getInstanceOf().printLog("OUT KILL MODE");
//     }

//     return (message);
// }

//  int MavlinkTools::command_setModeGuided(float param1) {
//     //Message buffer
//     mavlink_message_t message;
//     mavlink_command_long_t command;

//     command = mavlink_newCommand();
//     command.target_system = 1;
//     command.target_component = 1;
//     command.command = MAV_CMD_NAV_GUIDED_ENABLE;
//     command.confirmation = false;
//     command.param1 = param1; // flag >0.5 => start, <0.5 => stop

//     mavlink_msg_command_long_encode(255, 1, &message, &command);
//     if (param1 > 0) {
//         //Display_IHM::getInstanceOf().printLog("GUIDED MODE");
//     } else {
//         //Display_IHM::getInstanceOf().printLog("OUT GUIDED MODE");
//     }

//     return write_message(message);
// }

// // FIXME check here for modifications
// int MavlinkTools::command_directControl(int16_t x, int16_t y, int16_t z, int16_t r) {
//     mavlink_message_t message;
//     mavlink_msg_manual_control_pack(255, 1, &message, 1, x, y, z, r, 0);
//     return write_message(message);
// }


// // FIXME not functional
// int MavlinkTools::command_stabilize_control(uint32_t time_boot_ms, float x, float y, float z, float r) {
//     mavlink_message_t message;

//     uint16_t mask = 0b0000001111111111 - 0b111;
//     mavlink_msg_set_position_target_local_ned_pack(255, 1, &message, time_boot_ms, 0, 0, 1, mask, x, y, z, 0.0, 0.0, 0.0,
//                                                    0.0, 0.0, 0.0, 0.0, 0.0);
//     return write_message(message);
// }


// int MavlinkTools::command_setMode(MavlinkTools_mode mode) {

//     switch (mode) {
//         case MavlinkTools_OFF:
//             //We must reset all value to 0
//             MavlinkTools::mode = mode;
//             break;

//         case MavlinkTools_MANUAL_DIRECT:
//             if (remote_x != 0 && remote_y != 0 && remote_z != 0 && remote_r != 0) {
//                 ////Display_IHM::getInstanceOf().printLog("Server: command refused");
//                 //beep();
//             } else {
//                 //Display_IHM::getInstanceOf().printLog("Server: mode manual_direct");
//                 MavlinkTools::mode = mode;
//             }
//             break;

//         default:

//             break;
//     }

//     return 0;
// }
