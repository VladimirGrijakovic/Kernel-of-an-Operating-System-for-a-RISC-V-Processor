#include "../h/_thread.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

/*void* __dso_handle = nullptr;
extern "C" int __cxa_atexit(void (*)(void*), void*, void*){return 0;}*/

_thread *_thread::running = nullptr;

int _thread::valid[MAX_NUMBER_OF_THREADS] = {0};
unsigned char _thread::threadSlots[sizeof(_thread)* MAX_NUMBER_OF_THREADS];

_thread *_thread::createThread(Body body, void* arg, int mode)
{
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if (valid[i] == 0) return new _thread(body, arg, mode);
        i++;
    }
    return nullptr;
}

void* _thread::operator new(size_t n)
{
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if (valid[i] == 0) break;
        i++;
    }
    valid[i] = 1;

    return &threadSlots[i * sizeof(_thread)];
}

void _thread::operator delete(void* p)
{
    int i = 0;
    while (i < MAX_NUMBER_OF_THREADS)
    {
        if ((unsigned char*)p == &threadSlots[i*sizeof(_thread)]) break;
        i++;
    }
    valid[i] = 0;
    dispatch();
}

void _thread::dispatch()
{
    _thread *old = running;
    if (!old->isFinished()) { Scheduler::getInstance()->put(old); }
    running = Scheduler::getInstance()->get();

    _thread::contextSwitch(&old->context, &running->context);
}

void _thread::threadWrapper()
{
    Riscv::popSppSpie(running->mode);
    running->body(running->argument);
    thread_exit();
}
