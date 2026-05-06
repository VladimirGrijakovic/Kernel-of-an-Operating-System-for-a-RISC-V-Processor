//
// Created by os on 10/2/25.
//

#include "../h/ConsoleControl.hpp"

void CharProducerThread (void*)
{
    while (1)
    {
        ConsoleControl::getInstance()->plicReadyIn->wait();
        while (*(char*)CONSOLE_STATUS & 0x01)
        {
            ConsoleControl::getInstance()->buf_in->put(*(char*)CONSOLE_RX_DATA);
        }
    }
}

void CharConsumerThread (void*)
{
    while (1)
    {
        ConsoleControl::getInstance()->plicReadyOut->wait();
        while (*(char*)CONSOLE_STATUS & 0x20)
        {
            *(char*)CONSOLE_TX_DATA = ConsoleControl::getInstance()->buf_out->get();
        }
    }
}

ConsoleControl* ConsoleControl::getInstance() {
    static ConsoleControl instance;
    return &instance;
}

void ConsoleControl::putc(char c)
{
    buf_out->put(c);
}

char ConsoleControl::getc()
{
    char c = buf_in->get();
    return c;
}

int ConsoleControl::initCharCosumer()
{
    CharConsumerHandle = _thread::createThread(CharConsumerThread, 0, 1);
    if (CharConsumerHandle == nullptr) return -1;
    return 0;
}

int ConsoleControl::initCharProducer()
{
    CharProducerHandle = _thread::createThread(CharProducerThread, 0, 1);
    if (CharProducerHandle == nullptr) return -1;
    return 0;
}
