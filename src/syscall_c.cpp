#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"

inline void syscall(uint64 code, uint64 arg0, uint64 arg1, uint64 arg2)
{

    //pakovanje koda sistemskog poziva i argumenata u ragistre
    __asm__ volatile ("mv a0, %0" : : "r"(code));
    __asm__ volatile ("mv a1, %0" : : "r"(arg0));
    __asm__ volatile ("mv a2, %0" : : "r"(arg1));
    __asm__ volatile ("mv a3, %0" : : "r"(arg2));

    //softverski prekid
    __asm__ volatile("ecall");

}

void* mem_alloc(size_t size)
{
    uint64 volatile code = 0x01;
    //broj blokova koji treba da se alocira; +1 je zbog zaglavlja
    uint64 volatile arg = (size + MEM_BLOCK_SIZE - 1)/ MEM_BLOCK_SIZE + 1;

    syscall(code, arg);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    return (void*)ret;
}

int mem_free(void* p)
{
    uint64 volatile code = 0x02;
    uint64 volatile arg = (uint64)p;

    //__putc('\n');
    syscall(code, arg);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    return (int)ret;
}

size_t mem_get_free_space()
{
    uint64 volatile code = 0x03;

    syscall(code);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    return ret;
}

size_t mem_get_largest_free_block()
{
    uint64 volatile code = 0x04;

    syscall(code);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    return ret;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)
{
    uint64 volatile code = 0x11;
    uint64 volatile arg0 = (uint64)handle;
    uint64 volatile arg1 = (uint64)start_routine;
    uint64 volatile arg2 = (uint64)arg;

    syscall(code, arg0, arg1, arg2);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

int thread_exit()
{
    uint64 volatile code = 0x12;

    syscall(code);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

void thread_dispatch()
{
    uint64 volatile code = 0x13;
    syscall(code);
}

int sem_open(sem_t* handle, unsigned init)
{
    uint64 volatile code = 0x21;
    uint64 volatile arg0 = (uint64)handle;
    uint64 volatile arg1 = init;

    syscall(code, arg0, arg1);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

int sem_close(sem_t handle)
{
    uint64 volatile code = 0x22;

    syscall(code);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

int sem_wait(sem_t handle)
{
    uint64 volatile code = 0x23;
    uint64 volatile arg0 = (uint64)handle;

    syscall(code, arg0);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

int sem_signal(sem_t handle)
{
    uint64 volatile code = 0x24;
    uint64 volatile arg0 = (uint64)handle;

    syscall(code, arg0);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

int time_sleep(time_t time)
{
    uint64 volatile code = 0x31;
    uint64 volatile arg0 = (uint64)time;

    syscall(code, arg0);

    //izvlacenje povratne vrednosti iz registra a0
    uint64 volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    if (ret == 0) return 0;
    else return -1;
}

char getc()
{
    uint64 volatile code = 0x41;

    syscall(code);

    //izvlacenje povratne vrednosti iz registra a0
    char volatile ret = 0;
    __asm__ volatile ("mv %0, a0" : "=r"(ret));

    return ret;
}

void putc(char c)
{
    uint64 volatile code = 0x42;
    uint64 volatile arg0 = (uint64)c;

    syscall(code, arg0);
}
