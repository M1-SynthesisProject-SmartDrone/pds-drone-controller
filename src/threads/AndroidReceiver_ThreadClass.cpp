#include "threads/AndroidReceiver_ThreadClass.h"

#include <loguru/loguru.hpp>

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(
    std::shared_ptr<AndroidMediator> androidMediator,
    std::shared_ptr<PathRecorderHandler> pathRecorderHandler,
    std::shared_ptr<ToAppMessagesHolder> appMessagesHolder,
    std::shared_ptr<ToDroneMessagesHolder> droneMessageHolder
): Abstract_ThreadClass(1000, 200)
{
    m_mediator = androidMediator;
    m_droneMessageHolder = droneMessageHolder;
    m_appMessagesHolder = appMessagesHolder;
    m_pathRecorder = pathRecorderHandler;
}

AndroidReceiver_ThreadClass::~AndroidReceiver_ThreadClass()
{}

void AndroidReceiver_ThreadClass::run()
{
    loguru::set_thread_name("android receiver thread");
    // wait messages
    LOG_F(INFO, "Start listening for messages");
    while (isRunFlag())
    {
        usleep(task_period);
        try
        {
            auto messageReceived = m_mediator->receiveMessage();
            // Special case, we want to handle record
            switch (messageReceived->messageType)
            {
            case MESSAGE_TYPE::REQ_RECORD:
            {
                auto recordMessage = static_cast<Record_MessageReceived*>(messageReceived.get());
                handleRecordMessage(recordMessage);
            }
            break;
            case MESSAGE_TYPE::REQ_ACK:
            {
                auto ackMessage = static_cast<Ack_MessageReceived*>(messageReceived.get());
                handleAckMessage(ackMessage);
            }
            break;
            default:
                // No special case, send it to drone sender
                m_droneMessageHolder->add(move(messageReceived));
                break;
            }
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}

void AndroidReceiver_ThreadClass::handleStartRecording()
{
    unique_ptr<Record_MessageToSend> answer;
    try
    {
        m_pathRecorder->startRecording();
        answer = make_unique<Record_MessageToSend>(true, "Record started");
    }
    catch(const std::exception& e)
    {
        answer = make_unique<Record_MessageToSend>(false, "Cannot start record : " + string(e.what()));
    }
    m_appMessagesHolder->add(move(answer));
}

void AndroidReceiver_ThreadClass::handleEndRecording()
{
    unique_ptr<Record_MessageToSend> answer;
    try
    {
        string filename = m_pathRecorder->stopRecording();
        // TODO : launch an app to store to DB
        answer = make_unique<Record_MessageToSend>(true, "Record ended");
    }
    catch(const std::exception& e)
    {
        answer = make_unique<Record_MessageToSend>(false, "Cannot end record");
    }
    m_appMessagesHolder->add(move(answer));
}

void AndroidReceiver_ThreadClass::handleRecordMessage(Record_MessageReceived* recordMessage)
{
    if (recordMessage->record)
    {
        handleStartRecording();
    }
    else
    {
        handleEndRecording();
        string command = "";
    }
}

void AndroidReceiver_ThreadClass::handleAckMessage(Ack_MessageReceived* ackMessage)
{
    // Only thing to do is to respond here
    auto answer = make_unique<Ack_MessageToSend>(true);
    m_appMessagesHolder->add(move(answer));
}