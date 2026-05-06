#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "Scheduler.hpp"
#include "riscv.hpp"


class _thread
{
public:
    ~_thread()
    {
        Riscv::kmem_free(stack);
        setFinished(true);
    }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    using Body = void (*)(void* arg);

    static _thread *createThread(Body body, void* arg, int mode = 0);

    static _thread *running;

    void* operator new(size_t n);
    void operator delete(void* p);

private:

    //niz pregradaka za smestanje TCB-ova
    static int valid[MAX_NUMBER_OF_THREADS];
    static unsigned char threadSlots[];


    _thread(Body body = nullptr, void* arg = 0, int m = 0) :
            body(body),
            argument(arg),
            stack(body != nullptr ? (uint64*)Riscv::kmem_alloc((DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1)/ MEM_BLOCK_SIZE + 1) : nullptr),
            context({(uint64) &threadWrapper,stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0}),
            finished(false),
            timeSlice(DEFAULT_TIME_SLICE),
            mode(m)
    {
        if (body != nullptr) { Scheduler::getInstance()->put(this); }
        sleepTime = 0;
    }

    _thread& operator=(const _thread& other)
    {
        if (this != &other)
        {
            body = other.body;
            argument = other.argument;
            context = other.context;
            //stack = other.stack;
            finished = other.finished;
        }
        return *this;
    }

    //TCB(){}

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body; //funkcija koju nit obavlja
    void* argument; //argument te funkcija

    uint64 *stack;
    Context context;
    bool finished;

    size_t timeSlice;

    unsigned long sleepTime;

    friend class Riscv;
    friend class _sem;
    friend class SleepQueue;

    static void threadWrapper();
    int mode;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 1024;
};


#endif //PROJECT_BASE_V1_1_TCB_HPP