#ifndef PROJECT_BASE_V1_1__CONSOLE_HPP
#define PROJECT_BASE_V1_1__CONSOLE_HPP

#include "_sem.hpp"
#include "consoleBuffer.hpp"

static const int BUF_CAP = 100;

void CharProducerThread (void*);

void CharConsumerThread (void*);

class ConsoleControl
{
public:

    static ConsoleControl* getInstance();

    void putc(char c);
    char getc();

    int initCharProducer();
    int initCharCosumer();

    _sem* plicReadyOut;
    _sem* plicReadyIn;

private:

    ConsoleControl() {
        buf_in = new consoleBuffer(BUF_CAP);
        buf_out = new consoleBuffer(BUF_CAP);
        plicReadyOut = _sem::createSemaphore(0);
        plicReadyIn = _sem::createSemaphore(0);
        CharProducerHandle = nullptr;
        CharConsumerHandle = nullptr;
    }
    ~ConsoleControl() = default;
    ConsoleControl(const ConsoleControl&) = delete;
    ConsoleControl& operator=(const ConsoleControl&) = delete;

    consoleBuffer* buf_in;
    consoleBuffer* buf_out;

    thread_t CharProducerHandle;
    thread_t CharConsumerHandle;

    friend void CharProducerThread (void*);
    friend void CharConsumerThread (void*);

};


#endif //PROJECT_BASE_V1_1__CONSOLE_HPP