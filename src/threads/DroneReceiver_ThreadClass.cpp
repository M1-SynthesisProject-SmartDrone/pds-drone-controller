/**
 *
 * @author Aldric Vitali Silvestre
 */

#include "../../lib/loguru/loguru.hpp"
#include "../../lib/mavlink/common/mavlink.h"

#include "../../include/global_variables.h"
#include "../../include/threads/DroneReceiver_ThreadClass.h"
#include <stdio.h>

using namespace std;


DroneReceiver_ThreadClass::DroneReceiver_ThreadClass(int task_period, int task_deadline, std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass(task_period, task_deadline)
{
    m_drone = drone;
}

DroneReceiver_ThreadClass::~DroneReceiver_ThreadClass()
{
}


void DroneReceiver_ThreadClass::run()
{
    loguru::set_thread_name("drone receiver");
    LOG_F(INFO, "Start Drone receiver thread class");
    string buffer1 = "";

    while (isRunFlag()) {
        usleep(task_period);

        currentState = LifeCoreState::RUN;
        mavlink_message_t mavlinkMessage;
        if (m_drone.get()->read_message(mavlinkMessage))
        {
            LOG_F(INFO, "Received message from px4 of id : %d ", mavlinkMessage.msgid);

            switch (mavlinkMessage.msgid)
            {
            case MAVLINK_MSG_ID_ALTITUDE:
                mavlink_altitude_t altitude;
                mavlink_msg_altitude_decode(&mavlinkMessage, &altitude);
                printf("altitude_local  %f altitude_relative %f altitude_terrain %f  bottom_clearance %f\n", altitude.altitude_local, altitude.altitude_relative, altitude.altitude_terrain, altitude.bottom_clearance);
                break;

            case MAVLINK_MSG_ID_COMMAND_ACK:
                mavlink_command_ack_t commandAck;
                mavlink_msg_command_ack_decode(&mavlinkMessage, &commandAck);
                updateDroneData(commandAck);
                break;

            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:

                mavlink_global_position_int_t global_position_int;
                mavlink_msg_global_position_int_decode(&mavlinkMessage, &global_position_int);
                buffer1 = "\nGPS: " + std::to_string(global_position_int.lat) + " " + std::to_string(global_position_int.lon) + " " + std::to_string(global_position_int.alt) + "\n";
                cout << buffer1 << endl;

                break;

            case MAVLINK_MSG_ID_SCALED_IMU:
                mavlink_raw_imu_t scaled_imu;
                mavlink_msg_raw_imu_decode(&mavlinkMessage, &scaled_imu);
                buffer1 = buffer1 + " Acc: " + std::to_string(scaled_imu.xacc) + " " + std::to_string(scaled_imu.yacc) + " " + std::to_string(scaled_imu.zacc) + "\n";
                buffer1 = buffer1 + " Gyro: " + std::to_string(scaled_imu.xgyro) + " " + std::to_string(scaled_imu.ygyro) + " " + std::to_string(scaled_imu.zgyro) + "\n";
                buffer1 = buffer1 + " Mag: " + std::to_string(scaled_imu.xmag) + " " + std::to_string(scaled_imu.ymag) + " " + std::to_string(scaled_imu.zmag) + "\n";
                cout << buffer1 << endl;

            case MAVLINK_MSG_ID_HIGHRES_IMU:
                mavlink_highres_imu_t highres_imu;

                mavlink_msg_highres_imu_decode(&mavlinkMessage, &highres_imu);

                buffer1 = buffer1 + " Acc: " + std::to_string(highres_imu.xacc) + " " + std::to_string(highres_imu.yacc) + " " + std::to_string(highres_imu.zacc) + "\n";
                buffer1 = buffer1 + " Gyro: " + std::to_string(highres_imu.xgyro) + " " + std::to_string(highres_imu.ygyro) + " " + std::to_string(highres_imu.zgyro) + "\n";
                buffer1 = buffer1 + " Mag: " + std::to_string(highres_imu.xmag) + " " + std::to_string(highres_imu.ymag) + " " + std::to_string(highres_imu.zmag) + "\n";
                buffer1 = buffer1 + " Temperature: " + std::to_string(highres_imu.temperature) + "\n\n";
                cout << buffer1 << endl;

                break;

            case MAVLINK_MSG_ID_BATTERY_STATUS:
                mavlink_battery_status_t battery_status;
                mavlink_msg_battery_status_decode(&mavlinkMessage, &battery_status);
                printf("Battery status : %d \n", battery_status.battery_remaining);

                break;
            default:
                LOG_F(INFO, "Unknown message id");
                break;
            }


        }
    }
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_command_ack_t commandAck)
{
    string buffer;
    buffer = " Command :" + std::to_string(m_drone.get()->ack.command) + "\n";
    buffer = buffer + " result: " + std::to_string(m_drone.get()->ack.result);
    buffer = buffer + " result_param2: " + std::to_string(m_drone.get()->ack.result_param2) + "\n";
    buffer = buffer + " progress: " + std::to_string(m_drone.get()->ack.progress) + "\n";
    cout << buffer << endl;
    Drone *drone = m_drone.get();
    drone->ack = commandAck;

    bool isResultAccepted = commandAck.result == MAV_RESULT_ACCEPTED;
    // Multiple sub commands to handle
    switch (commandAck.command) 
    {
    // If arm / unarm, change the motor state
    case MAV_CMD_COMPONENT_ARM_DISARM:
        if (commandAck.result == MAV_RESULT_ACCEPTED)
        {
            drone->motors = drone->motors == ARM ? UNARM : ARM;
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Arm / Disarm : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    case MAVLINK_MSG_ID_MANUAL_CONTROL:
        break;
    }
}