/**
 * @author Sylvain Colomer
 * @date 17/08/18.
 */

#ifndef ABSTRACT_THREADCLASS_H_
#define ABSTRACT_THREADCLASS_H_

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <algorithm>

//#include <loguru.hpp>

#define THREAD_STATE_VOID 0 // Thread created and ready to be used
#define THREAD_STATE_READY 1 // Thread created and ready to be used
#define THREAD_STATE_WORK_TASK 2
#define THREAD_STATE_WORK_SLEEP 3
#define THREAD_STATE_WORK_DEADLINE 4



/**
 * Abstract class which convey to implement a thread brick
 * Use a real time context
 */
class Abstract_ThreadClass {
    
protected:

    // REAL TIME VAR
    /**
     * timeval that save the begin of the running method
     */
    struct timeval begin;

    /**
     * timeval use to control the period of the real time thread
     */
    struct timeval front_checkpoint;

    /**
     * timeval use to control the period of the real time thread
     */
    struct timeval end_checkpoint;

    /**
     * Represent the period between all task in ms
     */
    int task_period = 1000;
    
    /**
     * Var use to define when a task is overcome is execution delay
     */
    int task_deadline = 1000; //ms

    // RUN VAR
    /**
     * Mutex of the run flag
     */
    std::mutex runFlag_mutex;
    /**
     * Run flag : convey to loop the system
     */
    volatile bool runFlag = true;

    // MAIN THREAD VAR
    /**
     * The principal thread of the system
     */
    std::thread principalThread;
    /**
     * State flag use to know current state of the tread
     */
    int state = THREAD_STATE_VOID;

public :

    /**
     * Default constructor : take in parameters the task period time and the task deadline time
     * @param task_period period between all task execution
     * @param task_deadline alert limit use to send an alert message when the task are too long (see the notion of real time context)
     */
    Abstract_ThreadClass(int task_period, int task_deadline);

    /**
     * Default destructor
     */
    virtual ~Abstract_ThreadClass();

    /**
     * Method which convey to initialise the thread
     * Don't overwrite it without knowledge of the method goal
     */
    void init();

    /**
     * Start method : use it to first start the thread (only one time)
     * It create the thread associated to the run method
     */
    void start();

    /**
     * Convey to stop the thread.
     */
    void stop();

    /**
     * Warning : bad method
     */
    void stop_force();


    /**
     * Play method : convey to restart the thread before a first start and after a pause
     */
    void play();

    /**
     * Pause method : convey to pause the thread without quit it
     */
    void pause();

    /**
     * Method wich allow to join the thread
     * Warning : don't double join a thread
     */
    void join();

    /**
     * Method main loop. Implement a soft real time context
     * Overwrite it to create your real time thread
     */
    virtual void run() = 0;

    // GETTER AND SETTERS

    /**
     * Only a setter
     * @return
     */
    bool isRunFlag();


};

#endif
