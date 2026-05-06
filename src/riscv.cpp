#include "../h/riscv.hpp"
#include "../h/print.hpp"
#include "../h/_sem.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/SleepQueue.hpp"
#include "../h/_thread.hpp"
#include "../h/ConsoleControl.hpp"
#include "../lib/console.h"

void* Riscv::kmem_alloc(size_t size)
{
    return (void*)((uint64)MemoryAllocator::getInstance()->getChunk(size) + MEM_BLOCK_SIZE);
}

int Riscv::kmem_free(void* p)
{
    return MemoryAllocator::getInstance()->freeChunk((uint64*)((uint64)p - MEM_BLOCK_SIZE));
}


void Riscv::popSppSpie(int mode)
{
    if (mode == 0)
    {
        __asm__ volatile("csrc sstatus, %0" :: "r"(Riscv::SSTATUS_SPP));
    }else
    {
        __asm__ volatile("csrs sstatus, %0" :: "r"(Riscv::SSTATUS_SPP));
    }
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

enum syscalls
{
    MEM_ALLOC = 0x01,
    MEM_FREE = 0x02,
    MEM_GET_FREE_SPACE = 0X03,
    MEM_GET_LARGEST_FREE_BLOCK = 0X04,
    THREAD_CREATE = 0X11,
    THREAD_EXIT = 0X12,
    THREAD_DISPATCH = 0X13,
    SEM_OPEN = 0X21,
    SEM_CLOSE = 0X22,
    SEM_WAIT = 0X23,
    SEM_SIGNAL = 0X24,
    TIME_SLEEP = 0x31,
    GETC = 0x41,
    PUTC = 0x42
};

void Riscv::handleSupervisorTrap() {

    //volatile uint64 sepc = Riscv::r_sepc();
    //volatile uint64 sstatus = Riscv::r_sstatus();
    uint64 scause = Riscv::r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 code;
        __asm__ volatile ("ld %0, 10 * 8(fp)" : "=r"(code));

        uint64 arg0;
        __asm__ volatile ("ld %0, 11 * 8(fp)" : "=r"(arg0));

        uint64 arg1;
        __asm__ volatile ("ld %0, 12 * 8(fp)" : "=r"(arg1));

        uint64 arg2;
        __asm__ volatile ("ld %0, 13 * 8(fp)" : "=r"(arg2));


        uint64 volatile ret = 0;
        thread_t* p = 0;
        unsigned char* q = 0;
        sem_t* m = 0;
        sem_t r = 0;
        int i = 0;

        switch (code)
        {
        case MEM_ALLOC:
            ret = (uint64)MemoryAllocator::getInstance()->getChunk(arg0) + MEM_BLOCK_SIZE;
            break;
        case MEM_FREE:
            ret = MemoryAllocator::getInstance()->freeChunk((uint64*)(arg0 - MEM_BLOCK_SIZE));
            break;
        case MEM_GET_FREE_SPACE:
            ret = MemoryAllocator::getInstance()->free_space_size();
            break;
        case MEM_GET_LARGEST_FREE_BLOCK:
            ret = MemoryAllocator::getInstance()->largest_free_chunk();
            break;
        case THREAD_CREATE:
            p = (thread_t*)arg0;
            if (p == nullptr)
            {
                ret = 1;
                break;
            }
            *p = _thread::createThread((_thread::Body)arg1, (void*)arg2);
            if ((*p) != nullptr) ret = 0;
            else ret = 1;
            break;
        case THREAD_EXIT:
            _thread::running->setFinished(1);
            q = (unsigned char*)_thread::running;
            i = 0;
            while (i < MAX_NUMBER_OF_THREADS)
            {
                if (q == &_thread::threadSlots[i*sizeof(_thread)]) break;
                i++;
            }
            _thread::valid[i] = 0;
            _thread::dispatch();
            ret = 1;
            break;
        case THREAD_DISPATCH:
            _thread::dispatch();
            break;
        case SEM_OPEN:
            m = (sem_t*)arg0;
            if (m == nullptr)
            {
                ret = -1;
                break;
            }
            *m = _sem::createSemaphore((int)arg1);
            if (*m != 0) ret = 0;
            else ret = 1;
            break;
        case SEM_CLOSE:
            r = (sem_t)arg0;
            if (r == nullptr)
            {
                ret = 1;
                break;
            }
            ret = r->deleteSemaphore();
            break;
        case SEM_WAIT:
            r = (sem_t)arg0;
            ret = r->wait();
            break;
        case SEM_SIGNAL:
            r = (sem_t)arg0;
            ret = r->signal();
            break;
        case TIME_SLEEP:
            if (arg0 > 0) SleepQueue::getInstance()->put_and_dispatch(arg0);
            else ret = 1;
            break;
        case GETC:
            //ret = __getc();
            ret = ConsoleControl::getInstance()->getc();
            break;
        case PUTC:
            //__putc(arg0);
            ConsoleControl::getInstance()->putc(arg0);
        default:
            break;
        }

        //upis povratne vrednostiu registar a0
        __asm__ volatile ("sd %0, 10 * 8(fp)" : : "r"(ret));

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        volatile uint64 sepc = Riscv::r_sepc();
        volatile uint64 sstatus = Riscv::r_sstatus();
        Riscv::mc_sip(0x02);

        SleepQueue::getInstance()->update_and_wake();
        if ((--_thread::running->timeSlice) <= 0)
        {
            _thread::running->timeSlice = DEFAULT_TIME_SLICE;
            _thread::dispatch();
        }

        Riscv::w_sepc(sepc);
        Riscv::w_sstatus(sstatus);

    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        //console_handler();
        int plic_code = plic_claim();
        if (plic_code == 0x0a)
        {
            if (*(char*)CONSOLE_STATUS & 0x20) ConsoleControl::getInstance()->plicReadyOut->signal();
            if (*(char*)CONSOLE_STATUS & 0x01) ConsoleControl::getInstance()->plicReadyIn->signal();
            plic_complete(plic_code);
        }


    }
    else
    {
        /*printString("KRAJ\n");
        printString("Unhandled exception: SEPC = ");
        printInt(sepc);
        printString(", SCAUSE = ");
        printInt(scause);
        printString("\n");*/
        //neocekivani prekid - zavrsava se kernel
        uint32* p = (uint32*)0x100000;
        *p = 0x5555;
    }
}
