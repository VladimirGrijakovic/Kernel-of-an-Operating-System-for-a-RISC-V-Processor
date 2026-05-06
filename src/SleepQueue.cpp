#include "../h/SleepQueue.hpp"

SleepQueue* SleepQueue::getInstance() {
    static SleepQueue instance;
    return &instance;
}

void SleepQueue::update_and_wake()
{
    int i = sleepNum;
    for (int j = 0; j < MAX_NUMBER_OF_THREADS; j++)
    {
        if (valid[j])
        {
            if (--(red[j]->sleepTime) == 0)
            {
                Scheduler::getInstance()->put(red[j]);
                valid[j] = 0;
                sleepNum--;
            }
            if (--i == 0) break;
        }
    }
}



void SleepQueue::put_and_dispatch(unsigned long time)
{
    _thread *sleepThread = _thread::running;

    //staviti u niz uspavanih niti
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if (valid[i] == 0) break;
        i++;
    }
    sleepThread->sleepTime = time;
    valid[i] = 1;
    red[i] = sleepThread;
    sleepNum++;

    //dispatch bez stavljanja u scheduler

    _thread::running = Scheduler::getInstance()->get();
    _thread::contextSwitch(&sleepThread->context, &_thread::running->context);
}