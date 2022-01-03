#ifndef __ANDROIDRECEIVER_THREADCLASS_H__
#define __ANDROIDRECEIVER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to receive 
 * messages from the android application
 * 
 * @author Aldric Vitali Silvestre
 */

#include "Abstract_ThreadClass.h"
#include "SharedMessage.h"

#include "../drone/Data_Drone.h"

class AndroidReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    uint16_t m_udpPort;

public:
    AndroidReceiver_ThreadClass(int task_period, int task_deadline, uint16_t udpPort);
    ~AndroidReceiver_ThreadClass();

    void run();
};
#endif // __ANDROIDRECEIVER_THREADCLASS_H__