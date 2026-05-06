
#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP
#include "../lib/hw.h"

inline void syscall(uint64 code, uint64 arg0 = 0, uint64 arg1 = 0, uint64 arg2 = 0);

void* mem_alloc(size_t size);

int mem_free(void* p);

size_t mem_get_free_space();

size_t mem_get_largest_free_block();

class _thread;
typedef _thread* thread_t;
int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

class _sem;
typedef _sem* sem_t;
int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t handle);

int sem_signal(sem_t handle);

typedef  unsigned long time_t;
int time_sleep(time_t time);

const int EOF = -1;
char getc();

void putc(char c);



#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP