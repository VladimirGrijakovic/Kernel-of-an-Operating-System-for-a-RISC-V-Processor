#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {

public:

    static MemoryAllocator* getInstance();
    uint64* getChunk(uint64 n); //n - broj blokova velicine MEM_BLOCK_SIZE
    int freeChunk(uint64* addr); //addr - pocetna dresa chunka koja se brise
    int free_space_size(); //vraca kolicinu slobodnog prostora u bajtovima
    int largest_free_chunk();

private:
    MemoryAllocator() {
        first = (uint64*)HEAP_START_ADDR;
        *first = (uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR; //velicina slobodnog bloka
        *(first+1) = 0; //adresa sledeceg slobodnog bloka
    }
    ~MemoryAllocator() = default;
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    uint64* first; //adresa prvog slobodnog chunka

};


#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP