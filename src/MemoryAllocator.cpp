#include "../h/MemoryAllocator.hpp"


MemoryAllocator* MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return &instance;
}

uint64* MemoryAllocator::getChunk(uint64 n) {
    uint64* ret = 0;
    uint64* addr = first;
    if (addr == 0) return 0;
    uint64 size = *addr;

    //pronalazenje dovoljno velikog slobodnog chunka
    uint64* prev_addr = 0;
    while (size < n*MEM_BLOCK_SIZE) {
        prev_addr = addr;
        addr = (uint64*)*(addr + 1);
        if (addr == 0) return 0;
        size = *addr;
    }


    ret = addr;


    uint64 next_addr = *(addr+1);
    uint64 new_size = size - n*MEM_BLOCK_SIZE;

    if (new_size >= 16) {
        addr = (uint64*)((uint64)addr + n*MEM_BLOCK_SIZE);
        if (prev_addr != 0) *(prev_addr + 1) = (uint64)addr;
        else first = addr;
        *addr = new_size;
        *(addr+1) = next_addr;
    }else {
        if (prev_addr != 0) *(prev_addr + 1) = (uint64)next_addr;
        else first = (uint64*)next_addr;
    }

    *ret = n*MEM_BLOCK_SIZE; //velicina alociranog prostora u prvi bajt zauzetog chunka
    return ret;
}

//funkcija za spajanje chunka sa sledecim ukoliko su jedan do drugog
int merge(uint64* addr) {
    if (addr == 0) return 0;
    if ((uint64)addr + *addr == *(addr + 1)) {
        uint64* next = (uint64*)*(addr + 1);
        if (next == 0) return 0;
        *(addr + 1) = *(next + 1);
        *(addr) += *(next);
        return 1;
    }
    return 0;
}

int MemoryAllocator::freeChunk(uint64* addr) {
    uint64* free_addr = first;
    uint64* free_prev = nullptr;
    uint64 size = *addr;

    //delimicna provera da li je addr dobar argument
    while (free_addr < addr) {
        free_prev = free_addr;
        free_addr = (uint64*)*(free_addr + 1);
    }
    if (size > (uint64)free_addr - (uint64)addr) return -1;
    if (free_prev != 0 && (*free_prev + (uint64)free_prev) > (uint64)addr) return -1;

    //dealociranje chunka i eventualno spajanje sa prethodnim ili sledecim slobodnim(ili oba)
    *(addr + 1) = (uint64)free_addr;
    if (free_prev != 0) *(free_prev + 1) = (uint64)addr;
    else first = addr;

    //potencijalno spajanje sa prethodnim ili(i) sledecim chunkom
    if (merge(free_prev) == 0) {
        merge(addr);
    }else {
        merge(free_prev);
    }

    return 0;
}

int MemoryAllocator::free_space_size()
{
    uint64* addr = first;

    uint64 sum = 0;
    while (addr != 0)
    {
        sum += *addr;
        addr = (uint64*)*(addr + 1);
    }

    return (int)sum;
}

int MemoryAllocator::largest_free_chunk()
{

    uint64* addr = first;

    uint64 max = 0;
    while (addr != 0)
    {
        if (*addr > max) max = *addr;
        addr = (uint64*)*(addr + 1);
    }

    return (int)max;

}


