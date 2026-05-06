#include "../h/print.hpp"
#include "../lib/hw.h"
#include "../h/riscv.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/_thread.hpp"
#include "../h/ConsoleControl.hpp"
#include "../h/userMain.hpp"

Semaphore* main_sem;

void userWrapper(void* arg)
{
    userMain();
}

void idle(void* user_t)
{
    thread_t thread_um = (thread_t)user_t;
    while (!thread_um->isFinished())
    {
        thread_dispatch();
    }
    main_sem->signal();
}

int main(){

    //postavljanje rutine na koju se skace prilikom prekida ili izuzetaka
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    main_sem = new Semaphore(0);

    //postavljanje main kao nit
    thread_t thread_m;
    thread_create(&thread_m, nullptr, 0);
    _thread::running = thread_m;

    //inicijalizacija niti za interakciju sa konzolom
    ConsoleControl::getInstance()->initCharCosumer();
    ConsoleControl::getInstance()->initCharProducer();


    //pravljenje niti nad funkcijom userMain
    thread_t thread_um;
    thread_create(&thread_um, userWrapper, 0);


    //pravljenje jedne besposlene korisnicke niti
    thread_t thread_idle;
    thread_create(&thread_idle, idle, thread_um);

    main_sem->wait();

    //zavrsetak rada kernela
    uint32* p = (uint32*)0x100000;
    *p = 0x5555;


    return 0;
}