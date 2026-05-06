#include "../h/Scheduler.hpp"


//klasa Scheduler je singleton
Scheduler* Scheduler::getInstance() {
    static Scheduler instance;
    return &instance;
}


_thread *Scheduler::get()
{
    if (first == -1 && last == -1) return 0; //prazan scheduler

    _thread* p;
    if (first == last)
    {
        p = red[first];
        first = last = -1;
    }else
    {
        p = red[first];
        first = (first + 1) % 30;
    }

    return p;
}

void Scheduler::put(_thread *tcb)
{
    if (first == -1 && last == -1)
    {
        first = last = 0;
    }else
    {
        int last_p = (last + 1) % 30;
        if (last_p == first) return; //nema vise mesta u scheduleru
        last = last_p;
    }

    red[last] = tcb;
}