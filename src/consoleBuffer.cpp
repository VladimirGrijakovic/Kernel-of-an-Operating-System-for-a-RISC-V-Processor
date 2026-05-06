#include "../h/consoleBuffer.hpp"

consoleBuffer::consoleBuffer(int _cap) : cap(_cap + 1), head(0), tail(0) {
    buffer = (char *)mem_alloc(sizeof(char) * cap);
    sem_open(&itemAvailable, 0);
    sem_open(&spaceAvailable, _cap);
    sem_open(&mutexHead, 1);
    sem_open(&mutexTail, 1);
}

consoleBuffer::~consoleBuffer() {
    while (getCnt() > 0) {
        char ch = buffer[head];
        putc(ch);
        head = (head + 1) % cap;
    }
    putc('!');
    putc('\n');

    mem_free(buffer);
    sem_close(itemAvailable);
    sem_close(spaceAvailable);
    sem_close(mutexTail);
    sem_close(mutexHead);
}

void consoleBuffer::put(char val) {
    sem_wait(spaceAvailable);

    sem_wait(mutexTail);
    buffer[tail] = val;
    tail = (tail + 1) % cap;
    sem_signal(mutexTail);

    sem_signal(itemAvailable);

}

char consoleBuffer::get() {
    sem_wait(itemAvailable);

    sem_wait(mutexHead);

    char ret = buffer[head];
    head = (head + 1) % cap;
    sem_signal(mutexHead);

    sem_signal(spaceAvailable);

    return ret;
}

int consoleBuffer::getCnt() {
    int ret;

    sem_wait(mutexHead);
    sem_wait(mutexTail);

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

    sem_signal(mutexTail);
    sem_signal(mutexHead);

    return ret;
}
