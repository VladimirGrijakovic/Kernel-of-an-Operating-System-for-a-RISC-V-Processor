#ifndef PROJECT_BASE_V1_1_SLEEPQUEUE_HPP
#define PROJECT_BASE_V1_1_SLEEPQUEUE_HPP

#include "_thread.hpp"

class SleepQueue
{
private:
    _thread* red[MAX_NUMBER_OF_THREADS];
    int valid[MAX_NUMBER_OF_THREADS] = {0};
    int sleepNum = 0;

    ~SleepQueue() = default;
    SleepQueue(const SleepQueue&) = delete;
    SleepQueue& operator=(const SleepQueue&) = delete;

public:

    SleepQueue(){}

    static SleepQueue* getInstance();

    void update_and_wake();

    void put_and_dispatch(unsigned long time);
};


#endif //PROJECT_BASE_V1_1_SLEEPQUEUE_HPP