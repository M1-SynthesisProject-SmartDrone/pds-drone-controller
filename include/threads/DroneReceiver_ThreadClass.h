#ifndef __DRONERECEIVER_THREADCLASS_H__
#define __DRONERECEIVER_THREADCLASS_H__

/**
 * This thread will wait for messages comming from the drone and update 
 * the drone stats depending on those.
 * 
 * @author Aldric Vitali Silvestre
 */

#include "./Abstract_ThreadClass.h"
#include "../drone/Data_Drone.h"

class DroneReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
   

public:
    DroneReceiver_ThreadClass(int task_period, int task_deadline, std::shared_ptr<Drone> drone);
    ~DroneReceiver_ThreadClass();

    void run();

};
#endif // __DRONERECEIVER_THREADCLASS_H__