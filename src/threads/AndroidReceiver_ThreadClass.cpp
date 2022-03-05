#include "threads/AndroidReceiver_ThreadClass.h"

#include <loguru/loguru.hpp>

using namespace std;

AndroidReceiver_ThreadClass::AndroidReceiver_ThreadClass(
    std::shared_ptr<AndroidUDPSocket> androidUDPSocket,
    std::shared_ptr<PathRecorderHandler> pathRecorderHandler,
    std::shared_ptr<ToAppMessagesHolder> appMessagesHolder,
    std::shared_ptr<ToDroneMessagesHolder> droneMessageHolder,
    std::shared_ptr<Abstract_AndroidMessageConverter> messageConverter)
    : Abstract_ThreadClass(1000, 200)
{
    m_UDPSocket = androidUDPSocket;
    m_droneMessageHolder = droneMessageHolder;
    m_appMessagesHolder = appMessagesHolder;
    m_pathRecorder = pathRecorderHandler;
    m_messageConverter = messageConverter;
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
            unique_ptr<Abstract_AndroidReceivedMessage> messageReceived = this->receiveMessage();
            // Special case, we want to handle record
            switch (messageReceived->messageType)
            {
            case MESSAGE_TYPE::RECORD:
            {
                auto recordMessage = static_cast<Record_MessageReceived*>(messageReceived.get());
                handleRecordMessage(recordMessage);
            }
            break;
            case MESSAGE_TYPE::ACKNOWLEDGEMENT:
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

std::unique_ptr<Abstract_AndroidReceivedMessage> AndroidReceiver_ThreadClass::receiveMessage()
{
    // Must be a too big buffer to work with
    char buffer[BUFFER_SIZE];
    int bytesRead = m_UDPSocket->receiveAndSaveSender(buffer, BUFFER_SIZE);
    if (bytesRead >= BUFFER_SIZE)
    {
        LOG_F(ERROR, "Received bigger message than buffer could handle, message truncated");
    }

    auto convertedMessage = m_messageConverter->convertMessageReceived(string(buffer));
    return unique_ptr<Abstract_AndroidReceivedMessage>(convertedMessage);
}

void AndroidReceiver_ThreadClass::handleStartRecording()
{
    unique_ptr<Answer_MessageToSend> answer;
    try
    {
        m_pathRecorder->startRecording();
        answer = make_unique<Answer_MessageToSend>("RECORD", true, "Record started");
    }
    catch(const std::exception& e)
    {
        answer = make_unique<Answer_MessageToSend>("RECORD", false, "Cannot start record : " + string(e.what()));
    }
    m_appMessagesHolder->add(move(answer));
}

void AndroidReceiver_ThreadClass::handleEndRecording()
{
    unique_ptr<Answer_MessageToSend> answer;
    try
    {
        string filename = m_pathRecorder->stopRecording();
        // TODO : launch an app to store to DB
        answer = make_unique<Answer_MessageToSend>("RECORD", true, "Record ended");
    }
    catch(const std::exception& e)
    {
        answer = make_unique<Answer_MessageToSend>("RECORD", false, "Cannot end record");
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
    auto answer = make_unique<Answer_MessageToSend>("ACK", true);
    m_appMessagesHolder->add(move(answer));
}