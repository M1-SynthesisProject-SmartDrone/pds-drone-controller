/**
 *
 * @author Aldric Vitali Silvestre
 */
#include "threads/DroneReceiver_ThreadClass.h"

// #define DEBUG_CMD

using namespace std;

DroneReceiver_ThreadClass::DroneReceiver_ThreadClass(
    std::shared_ptr<Drone> drone,
    std::shared_ptr<PathRecorderHandler> pathRecorderHandler,
    std::shared_ptr<ToAppMessagesHolder> appMessagesHolder)
    : Abstract_ThreadClass(1000, 200)
{
    m_drone = drone;
    m_appMessagesHolder = appMessagesHolder;
    m_pathRecorder = pathRecorderHandler;
}

DroneReceiver_ThreadClass::~DroneReceiver_ThreadClass()
{}

void DroneReceiver_ThreadClass::run()
{
    loguru::set_thread_name("drone receiver");
    LOG_F(INFO, "Start Drone receiver thread class");
    // string buffer1 = "";

    while (isRunFlag())
    {
        // usleep(task_period);
        currentState = LifeCoreState::RUN;
        mavlink_message_t mavlinkMessage;
        if (m_drone->read_message(mavlinkMessage))
        {
            // LOG_F(INFO, "Received message from px4 of id : %d ", mavlinkMessage.msgid);

            // ? Add a timestamp check ?
            switch (mavlinkMessage.msgid)
            {
            case MAVLINK_MSG_ID_HEARTBEAT:
            {
                mavlink_heartbeat_t heartbeat;
                mavlink_msg_heartbeat_decode(&mavlinkMessage, &heartbeat);
                handleHeartbeat(heartbeat);
                break;
            }
            case MAVLINK_MSG_ID_ALTITUDE:
            {
                mavlink_altitude_t altitude;
                mavlink_msg_altitude_decode(&mavlinkMessage, &altitude);
                handleAltitude(altitude);
                // printf("altitude_local  %f altitude_relative %f altitude_terrain %f  bottom_clearance %f\n", altitude.altitude_local, altitude.altitude_relative, altitude.altitude_terrain, altitude.bottom_clearance);
                break;
            }
            case MAVLINK_MSG_ID_COMMAND_ACK:
            {
                mavlink_command_ack_t commandAck;
                mavlink_msg_command_ack_decode(&mavlinkMessage, &commandAck);
                handleAck(commandAck);
                break;
            }
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            {
                mavlink_global_position_int_t global_position_int;
                mavlink_msg_global_position_int_decode(&mavlinkMessage, &global_position_int);
                handleGlobalPosition(global_position_int);
                break;
            }

            case MAVLINK_MSG_ID_SCALED_IMU:
            {
                mavlink_raw_imu_t scaled_imu;
                mavlink_msg_raw_imu_decode(&mavlinkMessage, &scaled_imu);
                handleRawImu(scaled_imu);
                break;
            }
            case MAVLINK_MSG_ID_HIGHRES_IMU:
            {
                mavlink_highres_imu_t highres_imu;
                mavlink_msg_highres_imu_decode(&mavlinkMessage, &highres_imu);
                handleHighresImu(highres_imu);
                break;
            }

            case MAVLINK_MSG_ID_BATTERY_STATUS:
            {
                mavlink_battery_status_t battery_status;
                mavlink_msg_battery_status_decode(&mavlinkMessage, &battery_status);
                handleBatteryStatus(battery_status);
                break;
            }
            default:
                // LOG_F(INFO, "Unknown message id");
                break;
            }
        }
    }
}

void DroneReceiver_ThreadClass::handleHeartbeat(mavlink_heartbeat_t heartbeat)
{
    // We have a lot of messages types that we could handle, but only the arm state is important here
    if (heartbeat.type == MAV_TYPE_QUADROTOR) // Check if it has a chance to be our drone
    {
        // If drone is armed, this flag must be set
        bool isArmed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;
        if (isArmed != (m_drone->motors == ARM))
        {
            LOG_F(INFO, "Drone motor state changed : Arm is now equals to %s", isArmed ? "true" : "false");
            m_drone->motors = isArmed ? ARM : UNARM;
        }
        auto toSend = make_unique<DroneStatus_MessageToSend>(isArmed);
        m_appMessagesHolder->add(move(toSend));
    }
}

void DroneReceiver_ThreadClass::handleAck(mavlink_command_ack_t commandAck)
{
#ifdef DEBUG_CMD
    string buffer;
    buffer = " Command :" + std::to_string(m_drone->ack.command) + "\n";
    buffer = buffer + " result: " + std::to_string(m_drone->ack.result);
    buffer = buffer + " result_param2: " + std::to_string(m_drone->ack.result_param2) + "\n";
    buffer = buffer + " progress: " + std::to_string(m_drone->ack.progress) + "\n";
    cout << buffer << endl;
#endif

    m_drone->ack = commandAck;

    bool isResultAccepted = commandAck.result == MAV_RESULT_ACCEPTED;
    // Multiple sub commands to handle (change drone data + send messages to app)
    switch (commandAck.command)
    {
        // If arm / unarm, change the motor state
    case MAV_CMD_COMPONENT_ARM_DISARM:
        if (commandAck.result == MAV_RESULT_ACCEPTED)
        {
            m_drone->motors = m_drone->motors == ARM ? UNARM : ARM;
            // Normally, if the drone is unarmed, it must be landed too
            if (m_drone->motors == UNARM)
            {
                m_drone->tookOff = false;
            }
            auto toSend = make_unique<Answer_MessageToSend>("START_DRONE", true);
            m_appMessagesHolder->add(move(toSend));
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Arm / Disarm : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
            auto toSend = make_unique<Answer_MessageToSend>("START_DRONE", false, "The drone send a negative answer");
            m_appMessagesHolder->add(move(toSend));
        }
        break;
    case MAVLINK_MSG_ID_MANUAL_CONTROL:
        if (commandAck.result != MAV_RESULT_IN_PROGRESS && commandAck.result != MAV_RESULT_ACCEPTED)
        {
            LOG_F(ERROR, "Manual control : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    case MAV_CMD_NAV_TAKEOFF:
        if (commandAck.result == MAV_RESULT_ACCEPTED)
        {
            m_drone->tookOff = true;
            auto toSend = make_unique<Answer_MessageToSend>("TAKE_OFF", true);
            m_appMessagesHolder->add(move(toSend));
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Take off : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
            auto toSend = make_unique<Answer_MessageToSend>("TAKE_OFF", false);
            m_appMessagesHolder->add(move(toSend));
        }
        break;
    case MAV_CMD_NAV_LAND:
        if (commandAck.result == MAV_RESULT_ACCEPTED)
        {
            m_drone->tookOff = false;
            auto toSend = make_unique<Answer_MessageToSend>("LAND", true);
            m_appMessagesHolder->add(move(toSend));
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Land : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
            auto toSend = make_unique<Answer_MessageToSend>("LAND", false);
            m_appMessagesHolder->add(move(toSend));
        }
    }
}

void DroneReceiver_ThreadClass::handleAltitude(mavlink_altitude_t altitude)
{
#ifdef DEBUG_CMD
    printf("Altitude : altitude_local %f altitude_relative %f altitude_terrain %f  bottom_clearance %f\n", 
        altitude.altitude_local, 
        altitude.altitude_relative, 
        altitude.altitude_terrain,
        altitude.bottom_clearance
    );
#endif
    m_drone->altitude = altitude;
}

void DroneReceiver_ThreadClass::handleGlobalPosition(mavlink_global_position_int_t globalPosition)
{
#ifdef DEBUG_CMD
    string buffer1 = "global_position : [lat:" + to_string(globalPosition.lat)
        + " lon: " + to_string(globalPosition.lon)
        + " alt: " + to_string(globalPosition.alt)
        + " alt_rel: " + to_string(globalPosition.relative_alt)
        + " v(x,y,z): (" + to_string(globalPosition.vx) + "," + to_string(globalPosition.vy) + "," + to_string(globalPosition.vz) + ")"
        + " hdg: " + to_string(globalPosition.hdg) 
        + "]";
    cout << buffer1 << endl;
#endif
    m_drone->global_position_int = globalPosition;
    m_pathRecorder->insertData(globalPosition.lat, globalPosition.lon, globalPosition.alt);
}

void DroneReceiver_ThreadClass::handleRawImu(mavlink_raw_imu_t rawImu)
{
    // WARN : we don't store those structs, only for debugging
#ifdef DEBUG_CMD
    string buffer1 = "mavlink_raw_imu_t : Acc: " + std::to_string(rawImu.xacc) + " " + std::to_string(rawImu.yacc) + " " + std::to_string(rawImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + std::to_string(rawImu.xgyro) + " " + std::to_string(rawImu.ygyro) + " " + std::to_string(rawImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + std::to_string(rawImu.xmag) + " " + std::to_string(rawImu.ymag) + " " + std::to_string(rawImu.zmag) + "\n";
    cout << buffer1 << endl;
#endif
}

void DroneReceiver_ThreadClass::handleHighresImu(mavlink_highres_imu_t highresImu)
{
#ifdef DEBUG_CMD
    string buffer1 = "highres : Acc: " + std::to_string(highresImu.xacc) + " " + std::to_string(highresImu.yacc) + " " + std::to_string(highresImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + std::to_string(highresImu.xgyro) + " " + std::to_string(highresImu.ygyro) + " " + std::to_string(highresImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + std::to_string(highresImu.xmag) + " " + std::to_string(highresImu.ymag) + " " + std::to_string(highresImu.zmag) + "\n";
    buffer1 = buffer1 + " Temperature: " + std::to_string(highresImu.temperature) + "\n\n";
    // cout << buffer1 << endl;
    m_drone->highres_imu = highresImu;
#endif
}

void DroneReceiver_ThreadClass::handleBatteryStatus(mavlink_battery_status_t batteryStatus)
{
#ifdef DEBUG_CMD
    printf("Battery status : %d \n", batteryStatus.battery_remaining);
#endif
    m_drone->battery_status = batteryStatus;
}