#include "../h/_sem.hpp"

int _sem::valid[MAX_NUMBER_OF_SEMAPHORES] = {0};
unsigned char _sem::semSlots[sizeof(_sem)*MAX_NUMBER_OF_SEMAPHORES];
int volatile _sem::flags[MAX_NUMBER_OF_SEMAPHORES] = {0};


_sem::~_sem()
{
    //deblokiranje svih niti koje cekaju na ovom semaforu
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if ((unsigned char*)this == &semSlots[i*sizeof(_sem)]) break;
        i++;
    }

    flags[i] = 1;
    int j = getValue();
    while (j < 0)
    {
        signal();
        j++;
    }
}

void* _sem::operator new(size_t n)
{
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if (valid[i] == 0) break;
        i++;
    }
    valid[i] = 1;
    flags[i] = 0;

    return &semSlots[i*sizeof(_sem)];
}

void _sem::operator delete(void* p)
{
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if ((unsigned char*)p == &semSlots[i*sizeof(_sem)]) break;
        i++;
    }
    valid[i] = 0;
}

_sem* _sem::createSemaphore(int init)
{
    return new _sem(init);
}

int _sem::getValue(){return value;}

void _sem::put(_thread* tcb)
{
    if (first == -1 && last == -1)
    {
        first = last = 0;
    }else
    {
        int last_p = (last + 1) % MAX_NUMBER_OF_THREADS;
        if (last_p == first) return; //nema vise mesta u scheduleru
        last = last_p;
    }

    waitQueue[last] = tcb;
}

int _sem::deleteSemaphore()
{
    //deblokiranje svih niti koje cekaju na ovom semaforu
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if ((unsigned char*)this == &semSlots[i*sizeof(_sem)]) break;
        i++;
    }
    if (i == MAX_NUMBER_OF_THREADS) return -1;

    flags[i] = 1;
    int j = getValue();
    while (j < 0)
    {
        signal();
        j++;
    }
    valid[i] = 0;
    return 0;
}

_thread* _sem::get()
{
    if (first == -1 && last == -1) return 0; //prazan scheduler

    _thread* p;
    if (first == last)
    {
        p = waitQueue[first];
        first = last = -1;
    }else
    {
        p = waitQueue[first];
        first = (first + 1) % MAX_NUMBER_OF_THREADS;
    }

    return p;
}

int _sem::wait()
{
    value--;
    if (value >= 0) return 0;

    _thread *waitThread = _thread::running;
    put(waitThread);

    _thread::running = Scheduler::getInstance()->get();
    _thread::contextSwitch(&waitThread->context, &_thread::running->context);

    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if ((unsigned char*)this == &semSlots[i*sizeof(_sem)]) break;
        i++;
    }
    if (i == MAX_NUMBER_OF_THREADS) return 1;
    if (flags[i] == 1) return 1;

    return 0;
}

int _sem::signal()
{
    value++;
    if (value > 0) return 0;

    _thread* t = get();
    if (t == 0) return 1;

    Scheduler::getInstance()->put(t);
    return 0;

}