#include "../h/syscall_cpp.hpp"

Thread::Thread(void (*body)(void *), void *arg)
{
    this->body = body;
    this->arg = arg;
    this->myHandle = 0;
}

Thread::~Thread()
{
    if (!myHandle) return;
    //thread_exit();
}

int Thread::start()
{
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch()
{
    thread_dispatch();
}


int Thread::sleep(time_t time)
{
    return time_sleep(time);
}

void Thread::runWrapper(void* arg)
{
    Thread* t = (Thread*)arg;
    t->run();
}

Thread::Thread()
{
    body = runWrapper;
    arg = this;
    myHandle = 0;
}

Semaphore::Semaphore(unsigned init)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore()
{
    if (!myHandle) return;
    sem_close(myHandle);
    myHandle = 0;
}

int Semaphore::wait()
{
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    return sem_signal(myHandle);
}

void PeriodicThread::terminate ()
{
    period = (time_t)-1;
}

PeriodicThread::~PeriodicThread ()
{
    terminate();
}


PeriodicThread::PeriodicThread (time_t period)
{
    this->period = period;
}

void PeriodicThread::run()
{
    do
    {
        periodicActivation();
        time_sleep(period);
    }while (period != (time_t)-1);
}


char Console::getc ()
{
    char c = ::getc();
    return c;
}
void Console::putc (char c)
{
    ::putc(c);
}
