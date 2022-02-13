#include "threads/AndroidSender_ThreadClass.h"

using namespace std;

AndroidSender_ThreadClass::AndroidSender_ThreadClass(std::shared_ptr<Drone> drone,
    std::shared_ptr<AndroidUDPSocket> udpSocket,
    std::shared_ptr<ToAppMessagesHolder> appMsgHolder,
    std::shared_ptr<Abstract_AndroidMessageConverter> messageConverter
) : Abstract_ThreadClass(200, 1000)
{
    m_drone = drone;
    m_udpSocket = udpSocket;
    m_messageConverter = messageConverter;
    m_appMsgHolder = appMsgHolder;
}

AndroidSender_ThreadClass::~AndroidSender_ThreadClass()
{}

void AndroidSender_ThreadClass::run()
{
    loguru::set_thread_name("android sender thread");
    LOG_F(INFO, "Android sender thread created");

    while (isRunFlag())
    {
        m_timeRemainingMs = TIME_BETWEEN_UPDATES_MS;

        // The first time, we want the drone to wait on connection with app, 
        // so we can wait indefinitely in the message holder
        m_appMsgHolder->waitForMessages();
        LOG_F(INFO, "Start waiting for messages to send");

        // We can enter in it with an unarmed drone (path request for example)
        // In this case, we enter only once in the loop. In the other case, we keep
        // looping until the drone is disarmed
        while (isRunFlag())
        {
            // COUNTER BEGIN
            auto timeBegin = chrono::steady_clock::now();
            usleep(task_period);
            m_timeRemainingMs -= task_period;
            try
            {
                // We want to send a message about the drone each 10 seconds
                // BUT, if we have a message in the appMsgHolder, we want to treat it first !
                // So : timeout wait on appMsgHolder (special method)

                auto message = m_appMsgHolder->pop(m_timeRemainingMs);
                if (message == nullptr)
                {
                    // LOG_F(INFO, "Send Drone update message");
                    sendUpdateMessage();
                    m_timeRemainingMs = TIME_BETWEEN_UPDATES_MS;
                    // We don't have to do anything now (and we don't want to update m_timeRemainingMs)
                    continue;
                }
                else
                {
                    // LOG_F(INFO, "Send message from queue");
                    sendQueueMessage(move(message));
                }
            }
            catch (const std::exception& e)
            {
                LOG_F(ERROR, e.what());
            }
            // COUNTER END
            auto timeEnd = chrono::steady_clock::now();
            int64_t timeExecMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();
            m_timeRemainingMs -= timeExecMs;

            // Check if we need to continue to send messages
            if (m_drone->motors == UNARM)
            {
                break;
            }
        }

        LOG_F(INFO, "Drone unarmed, stop sending messages");
    }
}

void AndroidSender_ThreadClass::sendUpdateMessage()
{
    auto droneUpdateMessage = fetchDroneData();
    string toSend = m_messageConverter->convertToSendMessage(droneUpdateMessage.get());
    m_udpSocket->sendAsResponse(m_udpSocket->getPort(), toSend.c_str(), toSend.length());
}

void AndroidSender_ThreadClass::sendQueueMessage(unique_ptr<Abstract_AndroidToSendMessage> toSendMessage)
{
    string toSend = m_messageConverter->convertToSendMessage(toSendMessage.get());
    m_udpSocket->sendAsResponse(m_udpSocket->getPort(), toSend.c_str(), toSend.length());
}

unique_ptr<DroneData_MessageToSend> AndroidSender_ThreadClass::fetchDroneData()
{
    auto data = make_unique<DroneData_MessageToSend>();

    // Copy drone important structs
    auto globalPosition = m_drone->global_position_int;
    auto batteryStatus = m_drone->battery_status;
    // auto highresImu = m_drone->highres_imu;

    data->batteryRemaining = batteryStatus.battery_remaining;
    data->lat = globalPosition.lat;
    data->lon = globalPosition.lon;
    data->alt = globalPosition.alt;
    data->relativeAlt = globalPosition.relative_alt;
    data->vx = globalPosition.vx;
    data->vy = globalPosition.vy;
    data->vz = globalPosition.vz;
    data->yawRotation = globalPosition.hdg;

    return data;
}

