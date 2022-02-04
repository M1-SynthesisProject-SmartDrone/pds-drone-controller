/**
 *
 * @author Aldric Vitali Silvestre
 */

#include <loguru/loguru.hpp>
#include <mavlink/common/mavlink.h>

#include "global_variables.h"
#include "threads/DroneReceiver_ThreadClass.h"
#include <stdio.h>

using namespace std;


DroneReceiver_ThreadClass::DroneReceiver_ThreadClass(std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass(1000, 200)
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
    // string buffer1 = "";

    while (isRunFlag()) {
        usleep(task_period);

        currentState = LifeCoreState::RUN;
        mavlink_message_t mavlinkMessage;
        if (m_drone->read_message(mavlinkMessage))
        {
            LOG_F(INFO, "Received message from px4 of id : %d ", mavlinkMessage.msgid);

            // ? Add a timestamp check ?
            switch (mavlinkMessage.msgid)
            {
            case MAVLINK_MSG_ID_ALTITUDE:
                mavlink_altitude_t altitude;
                mavlink_msg_altitude_decode(&mavlinkMessage, &altitude);
                updateDroneData(altitude);
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
                updateDroneData(global_position_int);
                break;

            case MAVLINK_MSG_ID_SCALED_IMU:
                mavlink_raw_imu_t scaled_imu;
                mavlink_msg_raw_imu_decode(&mavlinkMessage, &scaled_imu);
                updateDroneData(scaled_imu);
                break;

            case MAVLINK_MSG_ID_HIGHRES_IMU:
                mavlink_highres_imu_t highres_imu;
                mavlink_msg_highres_imu_decode(&mavlinkMessage, &highres_imu);
                updateDroneData(highres_imu);
                break;

            case MAVLINK_MSG_ID_BATTERY_STATUS:
                mavlink_battery_status_t battery_status;
                mavlink_msg_battery_status_decode(&mavlinkMessage, &battery_status);
                updateDroneData(battery_status);
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
    buffer = " Command :" + std::to_string(m_drone->ack.command) + "\n";
    buffer = buffer + " result: " + std::to_string(m_drone->ack.result);
    buffer = buffer + " result_param2: " + std::to_string(m_drone->ack.result_param2) + "\n";
    buffer = buffer + " progress: " + std::to_string(m_drone->ack.progress) + "\n";
    cout << buffer << endl;

    m_drone->ack = commandAck;

    bool isResultAccepted = commandAck.result == MAV_RESULT_ACCEPTED;
    // Multiple sub commands to handle
    switch (commandAck.command) 
    {
    // If arm / unarm, change the motor state
    case MAV_CMD_COMPONENT_ARM_DISARM:
        if (commandAck.result == MAV_RESULT_ACCEPTED)
        {
            m_drone->motors = m_drone->motors == ARM ? UNARM : ARM;
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Arm / Disarm : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    case MAVLINK_MSG_ID_MANUAL_CONTROL:
        if (commandAck.result != MAV_RESULT_IN_PROGRESS && commandAck.result != MAV_RESULT_ACCEPTED)
        {
            LOG_F(ERROR, "Manual control : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    // case MAV_CMD_NAV_TAKEOFF:
    //     if (commandAck.result == MAV_RESULT_ACCEPTED)
    //     {
    //         // We could update the drone state, if have one
    //     }
    }
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_altitude_t altitude)
{
    printf("Altitude : altitude_local %f altitude_relative %f altitude_terrain %f  bottom_clearance %f\n", altitude.altitude_local, altitude.altitude_relative, altitude.altitude_terrain, altitude.bottom_clearance);

    m_drone->altitude = altitude;
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_global_position_int_t globalPosition)
{
    string buffer1 = "\nGPS: " + std::to_string(globalPosition.lat) + " " + std::to_string(globalPosition.lon) + " " + std::to_string(globalPosition.alt) + "\n";
    cout << buffer1 << endl;
    m_drone->global_position_int = globalPosition;
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_raw_imu_t rawImu)
{
    // WARN : we don't store those structs, only for debugging
    string buffer1 = "mavlink_raw_imu_t : Acc: " + std::to_string(rawImu.xacc) + " " + std::to_string(rawImu.yacc) + " " + std::to_string(rawImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + std::to_string(rawImu.xgyro) + " " + std::to_string(rawImu.ygyro) + " " + std::to_string(rawImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + std::to_string(rawImu.xmag) + " " + std::to_string(rawImu.ymag) + " " + std::to_string(rawImu.zmag) + "\n";
    cout << buffer1 << endl;
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_highres_imu_t highresImu)
{

    string buffer1 = "highres : Acc: " + std::to_string(highresImu.xacc) + " " + std::to_string(highresImu.yacc) + " " + std::to_string(highresImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + std::to_string(highresImu.xgyro) + " " + std::to_string(highresImu.ygyro) + " " + std::to_string(highresImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + std::to_string(highresImu.xmag) + " " + std::to_string(highresImu.ymag) + " " + std::to_string(highresImu.zmag) + "\n";
    buffer1 = buffer1 + " Temperature: " + std::to_string(highresImu.temperature) + "\n\n";
    cout << buffer1 << endl;
    m_drone->highres_imu = highresImu;
}

void DroneReceiver_ThreadClass::updateDroneData(mavlink_battery_status_t batteryStatus)
{
    printf("Battery status : %d \n", batteryStatus.battery_remaining);
    m_drone->battery_status = batteryStatus;
}