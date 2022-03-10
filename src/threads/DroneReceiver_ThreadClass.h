#ifndef __DRONERECEIVER_THREADCLASS_H__
#define __DRONERECEIVER_THREADCLASS_H__

/**
 * This thread will wait for messages comming from the drone and update
 * the drone stats depending on those.
 *
 * @author Aldric Vitali Silvestre
 */

#include <loguru/loguru.hpp>
#include <mavlink/common/mavlink.h>
#include <stdio.h>


#include "Abstract_ThreadClass.h"
#include "global_variables.h"
#include "drone/Data_Drone.h"
#include "threads/bridges/ToAppMessagesHolder.h"
#include "threads/bridges/PathRecorderHandler.h"
#include "android/message/tosend/Answer_MessageToSend.h"
#include "android/message/tosend/DroneStatus_MessageToSend.h"

class DroneReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
    std::shared_ptr<ToAppMessagesHolder> m_appMessagesHolder;
    std::shared_ptr<PathRecorderHandler> m_pathRecorder;

    void handleHeartbeat(mavlink_heartbeat_t heartbeat);
    void handleAltitude(mavlink_altitude_t altitude);
    void handleAck(mavlink_command_ack_t commandAck);
    void handleGlobalPosition(mavlink_global_position_int_t globalPosition);
    void handleRawImu(mavlink_raw_imu_t rawImu);
    void handleHighresImu(mavlink_highres_imu_t highresImu);
    void handleBatteryStatus(mavlink_battery_status_t batteryStatus);

public:
    DroneReceiver_ThreadClass(
        std::shared_ptr<Drone> drone,
        std::shared_ptr<PathRecorderHandler> pathRecorderHandler,
        std::shared_ptr<ToAppMessagesHolder> appMessagesHolder);
    ~DroneReceiver_ThreadClass();

    void run();

};
#endif // __DRONERECEIVER_THREADCLASS_H__