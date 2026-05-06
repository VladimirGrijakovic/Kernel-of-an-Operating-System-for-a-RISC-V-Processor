#ifndef PROJECT_BASE_V1_1_SCB_HPP
#define PROJECT_BASE_V1_1_SCB_HPP

#include "syscall_c.hpp"
#include "_thread.hpp"

static const int MAX_NUMBER_OF_SEMAPHORES = 30;

class _sem
{

public:

    ~_sem();
    static _sem* createSemaphore(int init);

    int getValue();
    int wait();
    int signal();

    void* operator new(size_t n);
    void operator delete(void* p);

    int deleteSemaphore();

private:

    _sem(int init = 1): value(init), first(-1), last(-1){}

    void put(_thread* tcb);
    _thread* get();

    int value;
    int first;
    int last;
    _thread* waitQueue[MAX_NUMBER_OF_THREADS];

    static int valid[MAX_NUMBER_OF_SEMAPHORES];
    static unsigned char semSlots[];
    static int volatile flags[MAX_NUMBER_OF_SEMAPHORES];

};

#endif //PROJECT_BASE_V1_1_SCB_HPP