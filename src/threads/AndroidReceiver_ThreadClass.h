#ifndef __ANDROIDRECEIVER_THREADCLASS_H__
#define __ANDROIDRECEIVER_THREADCLASS_H__

/**
 * Implementation of the Abstract_ThreadClass used to receive
 * messages from the android application
 *
 * @author Aldric Vitali Silvestre
 */
#include <memory>

#include "Abstract_ThreadClass.h"
#include "threads/bridges/ToDroneMessagesHolder.h"
#include "threads/bridges/ToAppMessagesHolder.h"
#include "threads/bridges/PathRecorderHandler.h"
#include "business/drone/Data_Drone.h"
#include "process/ProcessExecutor.h"
#include "android/network/AndroidMediator.h"
#include "android/converter/Json_AndroidMessageConverter.h"
#include "data_server/ApiHandler/ApiHandler.h"

class AndroidReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
    std::shared_ptr<ToDroneMessagesHolder> m_droneMessageHolder;
    std::shared_ptr<ToAppMessagesHolder> m_appMessagesHolder;
    std::shared_ptr<AndroidMediator> m_mediator;
    std::shared_ptr<PathRecorderHandler> m_pathRecorder;
    std::shared_ptr<ProcessExecutor> m_processExecutor;
    std::shared_ptr<ApiHandler> m_apiHandler;

    void handleRecordMessage(Record_MessageReceived* recordMessage);
    void handleStartRecording();
    void handleEndRecording();
    void handleAckMessage(Ack_MessageReceived* ackMessage);

    std::unique_ptr<DroneInfos_MessageToSend> createDroneInfos();
    void sendDroneInfos();

public:
    AndroidReceiver_ThreadClass(
        std::shared_ptr<Drone> drone,
        std::shared_ptr<AndroidMediator> androidMediator,
        std::shared_ptr<PathRecorderHandler> pathRecorderHandler,
        std::shared_ptr<ToAppMessagesHolder> appMessagesHolder,
        std::shared_ptr<ToDroneMessagesHolder> droneMessageHolder,
        std::shared_ptr<ProcessExecutor> processExecutor,
        std::shared_ptr<ApiHandler> apiHandler
    );
    ~AndroidReceiver_ThreadClass();

    void run();
};
#endif // __ANDROIDRECEIVER_THREADCLASS_H__