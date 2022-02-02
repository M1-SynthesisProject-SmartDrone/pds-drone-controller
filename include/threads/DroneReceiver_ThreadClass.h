#ifndef __DRONERECEIVER_THREADCLASS_H__
#define __DRONERECEIVER_THREADCLASS_H__

/**
 * This thread will wait for messages comming from the drone and update
 * the drone stats depending on those.
 *
 * @author Aldric Vitali Silvestre
 */

#include "./Abstract_ThreadClass.h"
#include "drone/Data_Drone.h"

class DroneReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;

    void updateDroneData(mavlink_altitude_t altitude);
    void updateDroneData(mavlink_command_ack_t commandAck);
    void updateDroneData(mavlink_global_position_int_t globalPosition);
    void updateDroneData(mavlink_raw_imu_t rawImu);
    void updateDroneData(mavlink_highres_imu_t highresImu);
    void updateDroneData(mavlink_battery_status_t batteryStatus);

public:
    DroneReceiver_ThreadClass(int task_period, int task_deadline, std::shared_ptr<Drone> drone);
    ~DroneReceiver_ThreadClass();

    void run();

};
#endif // __DRONERECEIVER_THREADCLASS_H__