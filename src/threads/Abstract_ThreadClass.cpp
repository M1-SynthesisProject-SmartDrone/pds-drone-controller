/**
 *  @author Sylvain Colomer, P. Gaussier
 *  @date 18/04/19.
 */

 
#include <iostream>
#include <string>

using namespace std;

#include <sys/time.h>
#include "../../include/threads/Abstract_ThreadClass.h"

//%%%%%%%%%%%%%%%%%%%%%%%%% begin/end phase function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

Abstract_ThreadClass::Abstract_ThreadClass(int task_period, int task_deadline)
{
    task_period=task_period;
    task_deadline=task_deadline;
    
    state = THREAD_STATE_READY;
}


Abstract_ThreadClass::~Abstract_ThreadClass(){
    
}

//%%%%%%%%%%%%%%%%%%%%%%%%% run function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Abstract_ThreadClass::run()
{
    long long currentThreadDelay;

    gettimeofday(&begin, 0);
    gettimeofday(&front_checkpoint, 0);

    while(isRunFlag())
    {
        state = THREAD_STATE_WORK_SLEEP;

        usleep(task_period);

        gettimeofday(&end_checkpoint, 0);
        currentThreadDelay=(end_checkpoint.tv_sec-front_checkpoint.tv_sec) * 1000000L + (end_checkpoint.tv_usec-front_checkpoint.tv_usec);

        if (currentThreadDelay > task_period )
        {
            gettimeofday(&front_checkpoint, 0);
            if (currentThreadDelay > task_period + task_deadline)
            {
                state = THREAD_STATE_WORK_DEADLINE;
            }
            else 
            {
                state = THREAD_STATE_WORK_TASK;
                //WORK
            }
        }
    }
}

//%%%%%%%%%%%%%%%%%%%%%%%%% control function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Abstract_ThreadClass::init()
{
}

void Abstract_ThreadClass::start()
{
    cout<< "Start one thread\n";
    principalThread= std::thread(&Abstract_ThreadClass::run, this);

    state = THREAD_STATE_WORK_SLEEP;
}


void Abstract_ThreadClass::stop(){
    cout<< "Stop one thread\n";

    runFlag_mutex.lock();
    runFlag = runFlag = false;
    runFlag_mutex.unlock();
}

void Abstract_ThreadClass::stop_force()
{
    cout<< "Force Stop one thread\n";
    runFlag = false;
}

void Abstract_ThreadClass::play()
{
}

void Abstract_ThreadClass::pause()
{
}

void Abstract_ThreadClass::join()
{
    cout<< "Join one thread\n";
    principalThread.join();
}

bool Abstract_ThreadClass::isRunFlag() 
{
    return runFlag;
}

