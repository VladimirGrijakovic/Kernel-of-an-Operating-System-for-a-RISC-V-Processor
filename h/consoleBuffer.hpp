//
// Created by zika on 2/26/22.
//

#ifndef OS1_BUFFER_CPP_H
#define OS1_BUFFER_CPP_H

#include "syscall_c.hpp"

class consoleBuffer {
private:
    int cap;
    char *buffer;
    int head, tail;

    sem_t spaceAvailable;
    sem_t itemAvailable;
    sem_t mutexHead;
    sem_t mutexTail;

public:
    consoleBuffer(int _cap);
    ~consoleBuffer();

    void put(char val);
    char get();

    int getCnt();

};


#endif //OS1_BUFFER_CPP_H

